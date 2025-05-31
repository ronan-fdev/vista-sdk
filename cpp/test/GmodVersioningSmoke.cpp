/**
 * @file GmodVersioningSmokeTests.cpp
 * @brief Unit tests for the GmodVersioningSmokeTests class.
 */

#include "pch.h"

#include "dnv/vista/sdk/GmodTraversal.h"
#include "dnv/vista/sdk/GmodVersioning.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISversion.h"

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

		void TearDown() override
		{
			m_vis = nullptr;
		}

		VIS* m_vis = nullptr;
		bool m_setupSuccess = false;
	};

	class ThreadSafeLogger
	{
	private:
		std::mutex m_mutex;

	public:
		template <typename... Args>
		void log( Args&&... args )
		{
			std::lock_guard<std::mutex> lock( m_mutex );
			( std::cout << ... << args );
		}

		template <typename... Args>
		void logError( Args&&... args )
		{
			std::lock_guard<std::mutex> lock( m_mutex );
			( std::cerr << ... << args );
		}
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
		size_t size() const
		{
			std::lock_guard<std::mutex> lock( m_mutex );
			return m_queue.size();
		}

		bool isCompleted() const
		{
			std::lock_guard<std::mutex> lock( m_mutex );
			return m_completed;
		}

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

			bool waitToRead()
			{
				std::unique_lock<std::mutex> lock( m_channel->m_mutex );
				m_channel->m_condition.wait( lock, [this] {
					return !m_channel->m_queue.empty() || m_channel->m_completed;
				} );
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

	struct SmokeTestMetrics
	{
		/* Timing metrics */
		std::chrono::steady_clock::time_point startTime;
		std::chrono::steady_clock::time_point producerEndTime;
		std::chrono::steady_clock::time_point testEndTime;

		/* Processing metrics */
		std::atomic<long> totalPaths{ 0 };
		std::atomic<long> successfulConversions{ 0 };
		std::atomic<long> failedConversions{ 0 };
		std::atomic<long> unchangedPaths{ 0 };
		std::atomic<long> changedPaths{ 0 };

		/* Performance metrics */
		std::atomic<long> totalProcessingTimeMs{ 0 };
		std::atomic<long> minProcessingTimeUs{ LONG_MAX };
		std::atomic<long> maxProcessingTimeUs{ 0 };

		/* Thread metrics */
		std::vector<std::atomic<long>> threadProcessedCounts;
		std::vector<std::atomic<long>> threadProcessingTimeMs;

		/* Path complexity metrics */
		std::atomic<long> totalPathLength{ 0 };
		std::atomic<long> maxPathLength{ 0 };
		std::atomic<long> totalNodeCount{ 0 };

		/* Conversion metrics */
		std::atomic<long> simpleCodeChanges{ 0 };
		std::atomic<long> structuralChanges{ 0 };
		std::atomic<long> pathExpansions{ 0 };
		std::atomic<long> pathContractions{ 0 };

		/* Error categorization */
		std::unordered_map<std::string, std::atomic<int>> errorCategories;
		std::mutex errorCategoriesMutex;

		SmokeTestMetrics( const uint64_t numThreads ) : threadProcessedCounts( numThreads ), threadProcessingTimeMs( numThreads )
		{
			startTime = std::chrono::steady_clock::now();
			for ( size_t i = 0; i < numThreads; ++i )
			{
				threadProcessedCounts[i] = 0;
				threadProcessingTimeMs[i] = 0;
			}
		}
	};

	struct TestContext
	{
		const Gmod& targetGmod;
		UnboundedChannel<GmodPath> channel;
		std::vector<std::tuple<GmodPath, std::optional<GmodPath>, std::string>> failedConversions;
		std::vector<std::pair<std::string, std::string>> changedPaths;
		std::mutex failedConversionsMutex;
		std::mutex changedPathsMutex;

		SmokeTestMetrics metrics;
		ThreadSafeLogger logger;

		TestContext( const Gmod& gmod, int numThreads ) : targetGmod( gmod ), metrics( static_cast<uint64_t>( numThreads ) ) {}
	};

	std::vector<std::string> split( const std::string& str, char delimiter )
	{
		std::vector<std::string> parts;
		std::stringstream ss( str );
		std::string part;
		while ( std::getline( ss, part, delimiter ) )
		{
			parts.push_back( part );
		}
		return parts;
	}

	std::string categorizeError( const std::string& error )
	{
		if ( error.find( "not found" ) != std::string::npos )
		{
			return "Node Not Found";
		}
		if ( error.find( "invalid" ) != std::string::npos )
		{
			return "Invalid Reference";
		}
		if ( error.find( "conversion" ) != std::string::npos )
		{
			return "Conversion Failed";
		}
		if ( error.find( "parse" ) != std::string::npos )
		{
			return "Parse Error";
		}

		return "Other";
	}

	double calculateLoadBalance( const std::vector<std::atomic<long>>& threadCounts )
	{
		if ( threadCounts.empty() )
		{
			return 0.0;
		}

		long total = 0;
		long min_val = LONG_MAX;
		long max_val = 0;

		for ( const auto& count : threadCounts )
		{
			long val = count.load();
			total += val;
			min_val = std::min( min_val, val );
			max_val = std::max( max_val, val );
		}

		if ( max_val == 0 )
		{
			return 100.0;
		}

		return ( static_cast<double>( min_val ) / max_val ) * 100.0;
	}

	std::string currentTimestamp()
	{
		auto now = std::chrono::system_clock::now();
		auto time_t = std::chrono::system_clock::to_time_t( now );

		std::stringstream dateTimeStream;
		std::tm timeStruct;
		localtime_s( &timeStruct, &time_t );
		dateTimeStream << std::put_time( &timeStruct, "%Y-%m-%d %H:%M:%S" );
		return dateTimeStream.str();
	}

	void writeOutputFiles( const Gmod& sourceGmod, const Gmod& targetGmod, const TestContext& context )
	{
		auto sourceVersionStr = VisVersionExtensions::toVersionString( sourceGmod.visVersion() );
		auto targetVersionStr = VisVersionExtensions::toVersionString( targetGmod.visVersion() );
		auto timestamp = currentTimestamp();

		std::string metricsFileName = "smoke-test-metrics-" + sourceVersionStr + "-" + targetVersionStr + "-" + timestamp + ".json";
		std::ofstream metricsFile( metricsFileName );

		auto totalDurationMs = std::chrono::duration_cast<std::chrono::milliseconds>(
			context.metrics.testEndTime - context.metrics.startTime )
								   .count();

		auto successRate = ( static_cast<double>( context.metrics.successfulConversions.load() ) /
							   context.metrics.totalPaths.load() ) *
						   100.0;

		double pathsPerSecond = ( static_cast<double>( context.metrics.totalPaths.load() ) * 1000.0 ) / static_cast<double>( totalDurationMs );

		metricsFile << "{\n";
		metricsFile << "  \"timestamp\": \"" << timestamp << "\",\n";
		metricsFile << "  \"sourceVersion\": \"" << sourceVersionStr << "\",\n";
		metricsFile << "  \"targetVersion\": \"" << targetVersionStr << "\",\n";
		metricsFile << "  \"totalPaths\": " << context.metrics.totalPaths.load() << ",\n";
		metricsFile << "  \"successfulConversions\": " << context.metrics.successfulConversions.load() << ",\n";
		metricsFile << "  \"failedConversions\": " << context.metrics.failedConversions.load() << ",\n";
		metricsFile << "  \"successRate\": " << std::fixed << std::setprecision( 3 ) << successRate << ",\n";
		metricsFile << "  \"testDurationMs\": " << totalDurationMs << ",\n";
		metricsFile << "  \"throughputPathsPerSecond\": " << std::fixed << std::setprecision( 2 ) << pathsPerSecond << ",\n";
		metricsFile << "  \"changedPaths\": " << context.metrics.changedPaths.load() << ",\n";
		metricsFile << "  \"unchangedPaths\": " << context.metrics.unchangedPaths.load() << ",\n";
		metricsFile << "  \"averagePathLength\": " << std::fixed << std::setprecision( 1 ) << ( static_cast<double>( context.metrics.totalPathLength.load() ) / context.metrics.totalPaths.load() ) << ",\n";
		metricsFile << "  \"maxPathLength\": " << context.metrics.maxPathLength.load() << ",\n";
		metricsFile << "  \"simpleCodeChanges\": " << context.metrics.simpleCodeChanges.load() << ",\n";
		metricsFile << "  \"structuralChanges\": " << context.metrics.structuralChanges.load() << ",\n";
		metricsFile << "  \"pathExpansions\": " << context.metrics.pathExpansions.load() << ",\n";
		metricsFile << "  \"pathContractions\": " << context.metrics.pathContractions.load() << ",\n";
		metricsFile << "  \"loadBalanceScore\": " << std::fixed << std::setprecision( 2 ) << calculateLoadBalance( context.metrics.threadProcessedCounts ) << "\n";
		metricsFile << "}\n";
		metricsFile.close();

		std::cout << "Metrics written to: " << metricsFileName << std::endl;

		std::string sourceFileName = "changed-source-paths-" + sourceVersionStr + "-" + targetVersionStr + "-" + timestamp + ".txt";
		std::string targetFileName = "changed-target-paths-" + sourceVersionStr + "-" + targetVersionStr + "-" + timestamp + ".txt";

		std::ofstream changedSources( sourceFileName );
		std::ofstream changedTargets( targetFileName );

		for ( const auto& [sourcePath, targetPath] : context.changedPaths )
		{
			changedSources << sourcePath << "\n";
			changedTargets << targetPath << "\n";
		}

		changedSources.close();
		changedTargets.close();

		std::cout << "Changed paths written to: " << sourceFileName << " and " << targetFileName << std::endl;
	}

	TEST_F( GmodVersioningTest, SmokeVersioningTest )
	{
		ASSERT_TRUE( m_setupSuccess ) << "Test setup failed";

		const auto& sourceGmod = m_vis->gmod( VisVersion::v3_4a );
		const auto& targetGmod = m_vis->gmod( VisVersion::v3_5a );

		const unsigned int numConsumers = std::thread::hardware_concurrency();
		TestContext context( targetGmod, static_cast<int>( numConsumers ) );
		const int pad = 7;

		auto timestamp = currentTimestamp();

		std::cout << "=== GMOD VERSIONING SMOKE TEST STARTED ===" << std::endl;
		std::cout << "Source version: " << VisVersionExtensions::toVersionString( sourceGmod.visVersion() ) << std::endl;
		std::cout << "Target version: " << VisVersionExtensions::toVersionString( targetGmod.visVersion() ) << std::endl;
		std::cout << "Number of consumer threads: " << numConsumers << std::endl;
		std::cout << "Test started at: " << timestamp << std::endl;

		auto producer = std::async( std::launch::async, [&context, &sourceGmod]() {
			long nodesTraversed = 0;
			long pathsGenerated = 0;
			auto producerStart = std::chrono::steady_clock::now();

			bool completed = GmodTraversal::traverse( sourceGmod,
				[&context, &sourceGmod, &nodesTraversed, &pathsGenerated]( const std::vector<const GmodNode*>& parents, const GmodNode& node ) -> TraversalHandlerResult {
					nodesTraversed++;

					if ( parents.empty() )
					{
						return TraversalHandlerResult::Continue;
					}

					std::vector<GmodNode*> nonConstParents;
					nonConstParents.reserve( parents.size() );
					for ( const GmodNode* p : parents )
					{
						nonConstParents.push_back( const_cast<GmodNode*>( p ) );
					}

					GmodPath path( sourceGmod, const_cast<GmodNode*>( &node ), nonConstParents );

					long pathLength = static_cast<long>( path.toString().length() );
					context.metrics.totalPathLength += pathLength;
					long currentMax = context.metrics.maxPathLength.load();
					while ( pathLength > currentMax && !context.metrics.maxPathLength.compare_exchange_weak( currentMax, pathLength ) )
					{
					}

					context.metrics.totalNodeCount += static_cast<long>( parents.size() + 1 );

					context.channel.writer().tryWrite( path );
					pathsGenerated++;

					if ( pathsGenerated % 5000 == 0 )
					{
						context.logger.log( "Producer: ", nodesTraversed, " nodes traversed, ", pathsGenerated, " paths generated\n" );
					}

					return TraversalHandlerResult::Continue;
				} );

			EXPECT_TRUE( completed );
			context.channel.writer().complete();
			context.metrics.producerEndTime = std::chrono::steady_clock::now();

			auto producerDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
				context.metrics.producerEndTime - context.metrics.startTime )
										.count();

			std::cout << "Producer completed: " << nodesTraversed << " nodes traversed, " << pathsGenerated << " paths generated in " << producerDuration << "ms" << std::endl;
			std::cout << "Average paths per node: " << std::fixed << std::setprecision( 2 ) << static_cast<double>( pathsGenerated ) / nodesTraversed << std::endl;
		} );

		std::vector<std::future<void>> consumers;
		consumers.reserve( numConsumers );

		for ( size_t i = 0; i < numConsumers; ++i )
		{
			consumers.emplace_back( std::async( std::launch::async, [&context, i, pad]() {
				size_t thread = i;
				auto reader = context.channel.reader();
				long threadCounter = 0;
				auto threadStart = std::chrono::steady_clock::now();

				while ( reader.waitToRead() )
				{
					GmodPath sourcePath;
					while ( reader.tryRead( sourcePath ) )
					{
						auto conversionStart = std::chrono::high_resolution_clock::now();
						std::optional<GmodPath> targetPath;

						try
						{
							targetPath = VIS::instance().convertPath( VisVersion::v3_4a, sourcePath, VisVersion::v3_5a );
							ASSERT_TRUE( targetPath.has_value() );

							std::optional<GmodPath> parsedTargetPath;
							bool parsedPath = context.targetGmod.tryParsePath( targetPath->toString(), parsedTargetPath );
							ASSERT_TRUE( parsedPath );
							ASSERT_EQ( parsedTargetPath->toString(), targetPath->toString() );

							auto conversionEnd = std::chrono::high_resolution_clock::now();
							auto conversionTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(
								conversionEnd - conversionStart )
														.count();

							context.metrics.totalProcessingTimeMs += static_cast<long>( conversionTimeUs ) / 1000;

							long conversionTimeLong = static_cast<long>( conversionTimeUs );
							long currentMin = context.metrics.minProcessingTimeUs.load();
							while ( conversionTimeLong < currentMin &&
									!context.metrics.minProcessingTimeUs.compare_exchange_weak( currentMin, conversionTimeLong ) )
							{
							}

							long currentMax = context.metrics.maxProcessingTimeUs.load();
							while ( conversionTimeLong > currentMax &&
									!context.metrics.maxProcessingTimeUs.compare_exchange_weak( currentMax, conversionTimeLong ) )
							{
							}

							context.metrics.successfulConversions++;
							threadCounter++;

							auto sourcePathStr = sourcePath.toString();
							auto targetPathStr = targetPath->toString();

							if ( sourcePathStr != targetPathStr )
							{
								context.metrics.changedPaths++;

								auto sourceParts = split( sourcePathStr, '/' );
								auto targetParts = split( targetPathStr, '/' );

								if ( sourceParts.size() == targetParts.size() )
								{
									context.metrics.simpleCodeChanges++;
								}
								else
								{
									context.metrics.structuralChanges++;
									if ( targetParts.size() > sourceParts.size() )
									{
										context.metrics.pathExpansions++;
									}
									else
									{
										context.metrics.pathContractions++;
									}
								}

								std::lock_guard<std::mutex> lock( context.changedPathsMutex );
								context.changedPaths.emplace_back( sourcePathStr, targetPathStr );
							}
							else
							{
								context.metrics.unchangedPaths++;
							}
						}
						catch ( const std::exception& e )
						{
							context.metrics.failedConversions++;
							threadCounter++;

							std::string errorType = categorizeError( e.what() );
							{
								std::lock_guard<std::mutex> lock( context.metrics.errorCategoriesMutex );
								context.metrics.errorCategories[errorType]++;
							}

							context.logger.logError( "[", std::setw( pad ), context.metrics.totalPaths.load(), "][",
								std::setw( 2 ), thread, "] Failed conversion: ", sourcePath.toString(), " -> ",
								( targetPath ? targetPath->toString() : "N/A" ), " - ", e.what(), "\n" );

							std::lock_guard<std::mutex> lock( context.failedConversionsMutex );
							context.failedConversions.emplace_back( sourcePath, targetPath, e.what() );
						}

						context.metrics.totalPaths++;
						context.metrics.threadProcessedCounts[thread]++;

						if ( context.metrics.totalPaths.load() % 10000 == 0 )
						{
							auto currentTime = std::chrono::steady_clock::now();
							auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
								currentTime - context.metrics.startTime )
												 .count();

							auto pathsPerSecond = ( context.metrics.totalPaths.load() * 1000 ) / std::max( 1LL, elapsedMs );
							size_t queueBacklog = context.channel.size();
							bool producerDone = context.channel.isCompleted();

							context.logger.log( "[", std::setw( pad ), context.metrics.totalPaths.load(), "][",
								std::setw( 2 ), thread, "] Processed | ", std::fixed, std::setprecision( 0 ),
								static_cast<double>( pathsPerSecond ), " paths/sec | ", std::fixed,
								std::setprecision( 1 ), ( static_cast<double>( context.metrics.successfulConversions.load() ) / context.metrics.totalPaths.load() ) * 100.0,
								"% success | Queue: ", queueBacklog,
								( producerDone ? " (producer done)" : "" ), "\n" );
						}
					}
				}

				auto threadEnd = std::chrono::steady_clock::now();
				context.metrics.threadProcessingTimeMs[thread] =
					static_cast<long>( std::chrono::duration_cast<std::chrono::milliseconds>( threadEnd - threadStart ).count() );

				context.logger.log( "Thread ", thread, " completed: ", threadCounter, " paths processed\n" );
			} ) );
		}

		producer.wait();
		for ( auto& consumer : consumers )
		{
			consumer.wait();
		}

		context.metrics.testEndTime = std::chrono::steady_clock::now();

		/* === METRICS SUMMARY === */

		auto totalDurationMs = std::chrono::duration_cast<std::chrono::milliseconds>( context.metrics.testEndTime - context.metrics.startTime ).count();
		auto producerDurationMs = std::chrono::duration_cast<std::chrono::milliseconds>( context.metrics.producerEndTime - context.metrics.startTime ).count();
		auto consumerDurationMs = std::chrono::duration_cast<std::chrono::milliseconds>( context.metrics.testEndTime - context.metrics.producerEndTime ).count();

		std::cout << "=== GMOD VERSIONING SMOKE TEST METRICS ===" << std::endl;

		std::cout << "TIMING METRICS" << std::endl;
		std::cout << "  Total test duration: " << std::fixed << std::setprecision( 2 ) << static_cast<double>( totalDurationMs ) / 1000.0 << "s" << std::endl;
		std::cout << "  Producer duration: " << std::fixed << std::setprecision( 2 ) << static_cast<double>( producerDurationMs ) / 1000.0 << "s" << std::endl;
		std::cout << "  Consumer duration: " << std::fixed << std::setprecision( 2 ) << static_cast<double>( consumerDurationMs ) / 1000.0 << "s" << std::endl;
		std::cout << "  Overlap efficiency: " << std::fixed << std::setprecision( 1 ) << ( ( static_cast<double>( producerDurationMs ) / static_cast<double>( consumerDurationMs ) ) * 100.0 ) << "%" << std::endl;

		long totalPaths = context.metrics.totalPaths.load();
		long successful = context.metrics.successfulConversions.load();
		long failed = context.metrics.failedConversions.load();
		double successRate = ( static_cast<double>( successful ) / totalPaths ) * 100.0;

		std::cout << "PROCESSING METRICS" << std::endl;
		std::cout << "  Total paths processed: " << totalPaths << std::endl;
		std::cout << "  Successful conversions: " << successful << " (" << std::fixed << std::setprecision( 2 ) << successRate << "%)" << std::endl;
		std::cout << "  Failed conversions: " << failed << " (" << std::fixed << std::setprecision( 2 ) << ( static_cast<double>( failed ) / totalPaths ) * 100.0 << "%)" << std::endl;
		std::cout << "  Unchanged paths: " << context.metrics.unchangedPaths.load() << " (" << std::fixed << std::setprecision( 2 ) << ( static_cast<double>( context.metrics.unchangedPaths.load() ) / totalPaths ) * 100.0 << "%)" << std::endl;
		std::cout << "  Changed paths: " << context.metrics.changedPaths.load() << " (" << std::fixed << std::setprecision( 2 ) << ( static_cast<double>( context.metrics.changedPaths.load() ) / totalPaths ) * 100.0 << "%)" << std::endl;

		double pathsPerSecond = ( static_cast<double>( totalPaths ) * 1000.0 ) / static_cast<double>( totalDurationMs );
		double avgProcessingTimeUs = static_cast<double>( context.metrics.totalProcessingTimeMs.load() * 1000 ) / totalPaths;

		std::cout << "PERFORMANCE METRICS" << std::endl;
		std::cout << "  Throughput: " << std::fixed << std::setprecision( 0 ) << pathsPerSecond << " paths/second" << std::endl;
		std::cout << "  Average processing time: " << std::fixed << std::setprecision( 2 ) << avgProcessingTimeUs << "us per path" << std::endl;
		std::cout << "  Min processing time: " << context.metrics.minProcessingTimeUs.load() << "us" << std::endl;
		std::cout << "  Max processing time: " << context.metrics.maxProcessingTimeUs.load() << "us" << std::endl;

		std::cout << "THREAD EFFICIENCY" << std::endl;
		for ( size_t i = 0; i < numConsumers; ++i )
		{
			long threadPaths = context.metrics.threadProcessedCounts[i].load();
			double threadEfficiency = ( static_cast<double>( threadPaths ) / totalPaths ) * 100.0;
			std::cout << "  Thread " << std::setw( 2 ) << i << ": " << threadPaths << " paths (" << std::fixed << std::setprecision( 1 ) << threadEfficiency << "%) in " << context.metrics.threadProcessingTimeMs[i].load() << "ms" << std::endl;
		}

		double loadBalance = calculateLoadBalance( context.metrics.threadProcessedCounts );
		std::cout << "  Load balance score: " << std::fixed << std::setprecision( 1 ) << loadBalance << "% (100% = perfect)" << std::endl;

		std::cout << "CONVERSION ANALYSIS" << std::endl;
		std::cout << "  Simple code changes: " << context.metrics.simpleCodeChanges.load() << std::endl;
		std::cout << "  Structural changes: " << context.metrics.structuralChanges.load() << std::endl;
		std::cout << "  Path expansions: " << context.metrics.pathExpansions.load() << std::endl;
		std::cout << "  Path contractions: " << context.metrics.pathContractions.load() << std::endl;

		double avgPathLength = static_cast<double>( context.metrics.totalPathLength.load() ) / totalPaths;
		double avgNodesPerPath = static_cast<double>( context.metrics.totalNodeCount.load() ) / totalPaths;

		std::cout << "PATH COMPLEXITY" << std::endl;
		std::cout << "  Average path length: " << std::fixed << std::setprecision( 1 ) << avgPathLength << " characters" << std::endl;
		std::cout << "  Max path length: " << context.metrics.maxPathLength.load() << " characters" << std::endl;
		std::cout << "  Average nodes per path: " << std::fixed << std::setprecision( 1 ) << avgNodesPerPath << std::endl;

		if ( !context.metrics.errorCategories.empty() )
		{
			std::cout << "ERROR ANALYSIS" << std::endl;
			std::vector<std::pair<std::string, int>> sortedErrors;
			for ( const auto& [error, count] : context.metrics.errorCategories )
			{
				sortedErrors.emplace_back( error, count.load() );
			}
			std::sort( sortedErrors.begin(), sortedErrors.end(),
				[]( const auto& a, const auto& b ) { return a.second > b.second; } );

			for ( const auto& [error, count] : sortedErrors )
			{
				double errorPercent = ( static_cast<double>( count ) / failed ) * 100.0;
				std::cout << "  [" << std::setw( 4 ) << count << "] " << std::fixed << std::setprecision( 1 ) << errorPercent << "% - " << error << std::endl;
			}
		}

		writeOutputFiles( sourceGmod, targetGmod, context );

		std::cout << "=== GMOD VERSIONING SMOKE TEST COMPLETED ===" << std::endl;

		ASSERT_TRUE( context.failedConversions.empty() )
			<< "Found " << context.failedConversions.size() << " failed conversions";
	}
}
