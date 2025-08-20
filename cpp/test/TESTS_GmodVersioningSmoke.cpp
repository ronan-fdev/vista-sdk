/**
 * @file TESTS_GmodVersioningSmoke.cpp
 * @brief Unit tests for the GmodVersioningSmokeTests class.
 */

#include "pch.h"

#include "dnv/vista/sdk/GmodVersioning.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk::tests
{
	class GmodVersioningTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			try
			{
				m_vis = &VIS::instance();
				m_setupSuccess = true;
			}
			catch ( [[maybe_unused]] const std::exception& e )
			{
				std::cerr << "Failed to initialize VIS: " << e.what() << std::endl;
				m_setupSuccess = false;
			}
		}

		void TearDown() override { m_vis = nullptr; }

		VIS* m_vis = nullptr;
		bool m_setupSuccess = false;
	};

	template <typename T>
	class UnboundedChannel
	{
	private:
		std::queue<T> m_queue;
		mutable std::mutex m_mutex;
		std::condition_variable m_condition;
		bool m_completed = false;

	public:
		class Writer
		{
		private:
			UnboundedChannel* m_channel;

		public:
			Writer( UnboundedChannel* channel ) : m_channel( channel ) {}

			bool tryWrite( const T& item )
			{
				std::lock_guard<std::mutex> lock( m_channel->m_mutex );
				if ( m_channel->m_completed )
					return false;
				m_channel->m_queue.push( item );
				m_channel->m_condition.notify_one();
				return true;
			}

			void complete()
			{
				std::lock_guard<std::mutex> lock( m_channel->m_mutex );
				m_channel->m_completed = true;
				m_channel->m_condition.notify_all();
			}
		};

		class Reader
		{
		private:
			UnboundedChannel* m_channel;

		public:
			Reader( UnboundedChannel* channel ) : m_channel( channel ) {}

			bool waitToReadAsync()
			{
				std::unique_lock<std::mutex> lock( m_channel->m_mutex );
				m_channel->m_condition.wait( lock, [this] { return !m_channel->m_queue.empty() || m_channel->m_completed; } );
				return !m_channel->m_queue.empty() || !m_channel->m_completed;
			}

			bool tryRead( T& item )
			{
				std::lock_guard<std::mutex> lock( m_channel->m_mutex );
				if ( m_channel->m_queue.empty() )
					return false;
				item = std::move( m_channel->m_queue.front() );
				m_channel->m_queue.pop();
				return true;
			}
		};

		Writer writer() { return Writer( this ); }
		Reader reader() { return Reader( this ); }
	};

	struct SmokeTestContext
	{
		const Gmod& targetGmod;
		UnboundedChannel<GmodPath> channel;
		std::atomic<long> counter{ 0 };
		std::mutex failedConversionsMutex;
		std::vector<std::tuple<GmodPath, std::optional<GmodPath>, std::exception_ptr>> failedConversions;
		std::mutex changedPathsMutex;
		std::vector<std::pair<std::string, std::string>> changedPaths;

		SmokeTestContext( const Gmod& gmod ) : targetGmod( gmod ) {}
	};

	TEST_F( GmodVersioningTest, SmokeVersioningTest )
	{
		ASSERT_TRUE( m_setupSuccess ) << "Test setup failed";

		const auto& sourceGmod = m_vis->gmod( VisVersion::v3_4a );
		const auto& targetGmod = m_vis->gmod( VisVersion::v3_5a );

		SmokeTestContext context( targetGmod );
		const int pad = 7;

		auto producer = std::async( std::launch::async, [&context, &sourceGmod]() {
			struct TraversalState
			{
				SmokeTestContext& context;
				const Gmod& sourceGmod;
				TraversalState( SmokeTestContext& ctx, const Gmod& gmod ) : context( ctx ), sourceGmod( gmod ) {}
			};

			TraversalState state( context, sourceGmod );

			TraverseHandlerWithState<TraversalState> handler = []( TraversalState& state, const std::vector<const GmodNode*>& parents,
																   const GmodNode& node ) -> TraversalHandlerResult {
				if ( parents.empty() )
					return TraversalHandlerResult::Continue;

				std::vector<GmodNode> parentValues;
				parentValues.reserve( parents.size() );
				for ( const GmodNode* p : parents )
				{
					if ( p )
					{
						parentValues.emplace_back( *p );
					}
				}

				GmodPath path( state.sourceGmod, node, std::move( parentValues ) );
				state.context.channel.writer().tryWrite( path );
				return TraversalHandlerResult::Continue;
			};

			bool completed = sourceGmod.traverse( state, handler );
			ASSERT_TRUE( completed );
			context.channel.writer().complete();
			auto counter = context.counter.load();
			std::cout << "[" << std::setw( pad ) << counter << "] Done traversing "
					  << dnv::vista::sdk::VisVersionExtensions::toVersionString( sourceGmod.visVersion() ) << " gmod" << std::endl;
		} );

		unsigned int numThreads = std::thread::hardware_concurrency();
		std::vector<std::future<void>> consumers;
		consumers.reserve( numThreads );

		for ( unsigned int i = 0; i < numThreads; ++i )
		{
			consumers.emplace_back( std::async( std::launch::async, [&context, i]() {
				auto thread = i;
				auto reader = context.channel.reader();

				while ( reader.waitToReadAsync() )
				{
					GmodPath sourcePath;
					while ( reader.tryRead( sourcePath ) )
					{
						std::optional<GmodPath> targetPath;
						long counter = 0;
						try
						{
							targetPath = VIS::instance().convertPath( VisVersion::v3_4a, sourcePath, VisVersion::v3_5a );
							ASSERT_TRUE( targetPath.has_value() );

							std::optional<GmodPath> parsedTargetPath;
							bool parsedPath = context.targetGmod.tryParsePath( targetPath->toString(), parsedTargetPath );
							ASSERT_TRUE( parsedPath );
							ASSERT_EQ( parsedTargetPath->toString(), targetPath->toString() );

							counter = ++context.counter;

							auto sourcePathStr = sourcePath.toString();
							auto targetPathStr = targetPath->toString();
							if ( sourcePathStr != targetPathStr )
							{
								std::lock_guard<std::mutex> lock( context.changedPathsMutex );
								context.changedPaths.emplace_back( sourcePathStr, targetPathStr );
							}
						}
						catch ( const std::exception& e )
						{
							counter = ++context.counter;
							std::cout << "[" << std::setw( pad ) << counter << "][" << std::setw( 2 ) << thread
									  << "] Failed to create valid path from: " << sourcePath.toString() << " -> "
									  << ( targetPath ? targetPath->toString() : "N/A" ) << " - " << e.what() << std::endl;

							std::lock_guard<std::mutex> lock( context.failedConversionsMutex );
							context.failedConversions.emplace_back( sourcePath, targetPath, std::current_exception() );
						}

						if ( counter % 10000 == 0 )
						{
							std::cout << "[" << std::setw( pad ) << counter << "][" << std::setw( 2 ) << thread << "] Paths processed" << std::endl;
						}
					}
				}
			} ) );
		}

		producer.wait();
		for ( auto& consumer : consumers )
		{
			consumer.wait();
		}

		std::cout << "[" << std::setw( pad ) << context.counter.load() << "] Done converting from "
				  << dnv::vista::sdk::VisVersionExtensions::toVersionString( sourceGmod.visVersion() ) << " to "
				  << dnv::vista::sdk::VisVersionExtensions::toVersionString( targetGmod.visVersion() ) << std::endl;

		auto success = context.counter.load() - context.failedConversions.size();
		auto failed = context.failedConversions.size();
		auto successRate = static_cast<double>( success ) / static_cast<double>( context.counter.load() ) * 100.0;

		std::cout << "Success/failed - " << success << "/" << failed << " (" << std::fixed << std::setprecision( 2 ) << successRate << "% success)"
				  << std::endl;

		/* Group errors by exception message */
		std::map<std::string, int> errorGroups;
		for ( const auto& [sourcePath, targetPath, exceptionPtr] : context.failedConversions )
		{
			try
			{
				std::rethrow_exception( exceptionPtr );
			}
			catch ( const std::exception& e )
			{
				errorGroups[e.what()]++;
			}
		}

		std::cout << "Errors: " << std::endl;
		std::vector<std::pair<std::string, int>> sortedErrors( errorGroups.begin(), errorGroups.end() );
		std::sort( sortedErrors.begin(), sortedErrors.end(), []( const auto& a, const auto& b ) { return a.second > b.second; } );

		for ( const auto& [errorMsg, count] : sortedErrors )
		{
			std::cout << "[" << std::setw( 4 ) << count << "] " << errorMsg << std::endl;
			std::cout << "--------------------------------------------------------" << std::endl;
		}

		/* Write output files */
		std::string sourceVersionStr{ dnv::vista::sdk::VisVersionExtensions::toVersionString( sourceGmod.visVersion() ) };
		std::string targetVersionStr{ dnv::vista::sdk::VisVersionExtensions::toVersionString( targetGmod.visVersion() ) };

		std::string changedSourcesFileName = "changed-source-paths-" + sourceVersionStr + "-" + targetVersionStr + ".txt";
		std::string changedTargetsFileName = "changed-target-paths-" + sourceVersionStr + "-" + targetVersionStr + ".txt";

		std::ofstream changedSources( changedSourcesFileName );
		std::ofstream changedTargets( changedTargetsFileName );

		for ( const auto& [sourcePath, targetPath] : context.changedPaths )
		{
			changedSources << sourcePath << std::endl;
			changedTargets << targetPath << std::endl;
		}

		changedSources.close();
		changedTargets.close();

		ASSERT_TRUE( context.failedConversions.empty() ) << "Found " << context.failedConversions.size() << " failed conversions";
	}
}
