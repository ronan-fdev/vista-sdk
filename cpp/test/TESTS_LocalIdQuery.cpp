/**
 * @file TESTS_LocalIdQuery.cpp
 * @brief Unit tests for the LocalIdQuery and LocalIdQueryBuilder class functionality
 * @details Tests LocalIdQuery building, matching, and filtering capabilities based on
 *          primary item, secondary item, and metadata tag constraints.
 */

/*
 * TODO: Test_DataChannelList_Filter not yet implemented
 */

#include "dnv/vista/sdk/Codebooks.h"
#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/LocalId.h"
#include "dnv/vista/sdk/LocalIdBuilder.h"
#include "dnv/vista/sdk/LocalIdQuery.h"
#include "dnv/vista/sdk/Locations.h"
#include "dnv/vista/sdk/MetadataTagsQuery.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk::tests
{
	namespace LocalIdQueryTests
	{
		//=====================================================================
		// Test Data Structure
		//=====================================================================

		struct InputData
		{
			std::string localIdStr;
			std::function<LocalIdQuery()> queryFactory;
			bool expectedMatch;
		};

		//=====================================================================
		// Test Fixture
		//=====================================================================

		class LocalIdQueryTest : public ::testing::Test
		{
		protected:
			static std::pair<VIS&, const Gmod&> visAndGmod( VisVersion visVersion )
			{
				VIS& vis = VIS::instance();
				const auto& gmod = vis.gmod( visVersion );
				return { vis, gmod };
			}
		};

		//=====================================================================
		// Test Data
		//=====================================================================

		static std::vector<InputData> testData()
		{
			std::vector<InputData> data;
			data.reserve( 7 );

			// Test case 1
			data.emplace_back( InputData{
				"/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage",
				[]() {
					auto path = GmodPath::parse( "1021.1i-6P/H123", VisVersion::v3_4a );
					auto pathQuery = GmodPathQuery::fromPath( path ).withoutLocations();
					return LocalIdQueryBuilder::empty().withPrimaryItem( pathQuery ).build();
				},
				true } );

			// Test case 2
			data.emplace_back( InputData{
				"/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened",
				[]() {
					auto& vis = VIS::instance();
					auto& codebooks = vis.codebooks( VisVersion::v3_4a );
					auto tag = codebooks.createTag( CodebookName::Content, "sea.water" );
					auto tagsQuery = MetadataTagsQuery::empty().withTag( tag );
					return LocalIdQueryBuilder::empty().withTags( tagsQuery ).build();
				},
				true } );

			// Test case 3
			data.emplace_back( InputData{
				"/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
				[]() {
					auto path = GmodPath::parse( "411.1/C101.31-1", VisVersion::v3_4a );
					return LocalIdQueryBuilder::empty().withPrimaryItem( path ).build();
				},
				false } );

			// Test case 4
			data.emplace_back( InputData{
				"/dnv-v2/vis-3-4a/411.1-1/C101.63/S206/~propulsion.engine/~cooling.system/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
				[]() {
					auto path = GmodPath::parse( "411.1-2/C101.63/S206", VisVersion::v3_4a );
					auto pathQuery = GmodPathQuery::fromPath( path ).withoutLocations();
					return LocalIdQueryBuilder::empty().withPrimaryItem( pathQuery ).build();
				},
				true } );

			// Test case 5
			data.emplace_back( InputData{
				"/dnv-v2/vis-3-4a/411.1/C101.63/S206/sec/411.1/C101.31-5/~propulsion.engine/~cooling.system/~for.propulsion.engine/~cylinder.5/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
				[]() {
					auto path = GmodPath::parse( "/411.1/C101.31-2", VisVersion::v3_4a );
					auto pathQuery = GmodPathQuery::fromPath( path ).withoutLocations();
					return LocalIdQueryBuilder::empty().withSecondaryItem( pathQuery ).build();
				},
				true } );

			// Test case 6
			data.emplace_back( InputData{
				"/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low",
				[]() {
					auto path = GmodPath::parse( "511.11", VisVersion::v3_4a );
					auto pathQuery = GmodPathQuery::fromPath( path ).withoutLocations();
					return LocalIdQueryBuilder::empty().withPrimaryItem( pathQuery ).build();
				},
				true } );

			// Test case 7
			data.emplace_back( InputData{
				"/dnv-v2/vis-3-7a/433.1-S/C322.91/S205/meta/qty-conductivity/detail-relative",
				[]() {
					auto localId = LocalIdBuilder::parse( "/dnv-v2/vis-3-7a/433.1-S/C322.91/S205/meta/qty-conductivity" ).build();
					auto& vis = VIS::instance();
					auto& gmod = vis.gmod( VisVersion::v3_7a );
					const GmodNode* node433 = nullptr;
					if ( !gmod.tryGetNode( "433.1", node433 ) || !node433 )
						throw std::runtime_error( "Node 433.1 not found" );
					auto primaryPath = localId.primaryItem();
					if ( !primaryPath.has_value() )
						throw std::runtime_error( "No primary path found" );
					auto pathQuery = GmodPathQuery::fromPath( *primaryPath ).withNode( *node433, true );
					return LocalIdQueryBuilder::from( localId ).withPrimaryItem( pathQuery ).build();
				},
				true } );

			return data;
		}

		//=====================================================================
		// Parameterized Test Class
		//=====================================================================

		class LocalIdQueryParameterizedTest : public ::testing::TestWithParam<InputData>
		{
		protected:
			static std::pair<VIS&, const Gmod&> visAndGmod( VisVersion visVersion )
			{
				VIS& vis = VIS::instance();
				const auto& gmod = vis.gmod( visVersion );
				return { vis, gmod };
			}
		};

		//=====================================================================
		// Parameterized Tests
		//=====================================================================

		TEST_P( LocalIdQueryParameterizedTest, Test_Matches )
		{
			const InputData& data = GetParam();

			std::optional<LocalIdBuilder> builderOpt;
			bool parsed = LocalIdBuilder::tryParse( data.localIdStr, builderOpt );
			ASSERT_TRUE( parsed ) << "Failed to parse LocalId: " << data.localIdStr;
			ASSERT_TRUE( builderOpt.has_value() );

			auto localId = builderOpt->build();
			auto query = data.queryFactory();
			bool match = query.match( localId );

			EXPECT_EQ( data.expectedMatch, match );
		}

		//=====================================================================
		// Basic Tests
		//=====================================================================

		TEST_F( LocalIdQueryTest, Test_Happy_Path )
		{
			auto localIdStr = "/dnv-v2/vis-3-4a/1036.13i-1/C662/sec/411.1-2/C101/meta/qty-pressure/cnt-cargo/state-high.high/pos-stage-3/detail-discharge";

			std::optional<LocalIdBuilder> builderOpt;
			bool parsed = LocalIdBuilder::tryParse( localIdStr, builderOpt );
			ASSERT_TRUE( parsed );
			ASSERT_TRUE( builderOpt.has_value() );

			auto localId = builderOpt->build();

			// Match exact
			auto builder = LocalIdQueryBuilder::from( localId );
			auto query = builder.build();
			EXPECT_TRUE( query.match( localId ) );
		}

		TEST_F( LocalIdQueryTest, Test_Empty_Query_Creation )
		{
			auto query = LocalIdQueryBuilder::empty().build();

			std::optional<LocalIdBuilder> builderOpt;
			bool parsed = LocalIdBuilder::tryParse( "/dnv-v2/vis-3-4a/411.1/C101/meta/qty-temperature", builderOpt );
			ASSERT_TRUE( parsed );
			ASSERT_TRUE( builderOpt.has_value() );

			auto localId = builderOpt->build();
			// Empty query should match everything
			EXPECT_TRUE( query.match( localId ) );
		}

		TEST_F( LocalIdQueryTest, Test_Comprehensive_Happy_Path )
		{
			auto localIdStr = "/dnv-v2/vis-3-4a/1036.13i-1/C662/sec/411.1-2/C101/meta/qty-pressure/cnt-cargo/state-high.high/pos-stage-3/detail-discharge";

			std::optional<LocalIdBuilder> builderOpt;
			bool parsed = LocalIdBuilder::tryParse( localIdStr, builderOpt );
			ASSERT_TRUE( parsed );
			ASSERT_TRUE( builderOpt.has_value() );

			auto localId = builderOpt->build();
			auto primaryItem = localId.primaryItem();
			auto secondaryItem = localId.secondaryItem();
			ASSERT_TRUE( primaryItem.has_value() );
			ASSERT_TRUE( secondaryItem.has_value() );

			// Match exact
			auto builder = LocalIdQueryBuilder::from( localId );
			auto query = builder.build();
			EXPECT_TRUE( query.match( localId ) );

			// Test with and without individualization
			for ( bool individualized : { true, false } )
			{
				ASSERT_TRUE( primaryItem.has_value() );
				auto primaryQueryBuilder = GmodPathQuery::fromPath( *primaryItem );
				auto secondaryQueryBuilder = GmodPathQuery::fromPath( *secondaryItem );

				if ( !individualized )
				{
					primaryQueryBuilder = primaryQueryBuilder.withoutLocations();
					secondaryQueryBuilder = secondaryQueryBuilder.withoutLocations();
				}

				// Match primary
				builder = LocalIdQueryBuilder::empty().withPrimaryItem( primaryQueryBuilder );
				query = builder.build();
				EXPECT_TRUE( query.match( localId ) );

				// Match secondary
				builder = LocalIdQueryBuilder::empty().withSecondaryItem( secondaryQueryBuilder );
				query = builder.build();
				EXPECT_TRUE( query.match( localId ) );

				// Match tags
				builder = LocalIdQueryBuilder::empty();
				for ( const auto& tag : localId.metadataTags() )
				{
					auto tagsQuery = MetadataTagsQuery::empty().withTag( tag );
					builder = builder.withTags( tagsQuery );
				}
				query = builder.build();
				EXPECT_TRUE( query.match( localId ) );

				// Match primary and secondary
				builder = LocalIdQueryBuilder::empty().withPrimaryItem( primaryQueryBuilder ).withSecondaryItem( secondaryQueryBuilder );
				query = builder.build();
				EXPECT_TRUE( query.match( localId ) );

				// Match primary and tags
				builder = LocalIdQueryBuilder::empty().withPrimaryItem( primaryQueryBuilder );
				for ( const auto& tag : localId.metadataTags() )
				{
					auto tagsQuery = MetadataTagsQuery::empty().withTag( tag );
					builder = builder.withTags( tagsQuery );
				}
				query = builder.build();
				EXPECT_TRUE( query.match( localId ) );

				// Match secondary and tags
				builder = LocalIdQueryBuilder::empty().withSecondaryItem( secondaryQueryBuilder );
				for ( const auto& tag : localId.metadataTags() )
				{
					auto tagsQuery = MetadataTagsQuery::empty().withTag( tag );
					builder = builder.withTags( tagsQuery );
				}
				query = builder.build();
				EXPECT_TRUE( query.match( localId ) );

				// Match primary, secondary, and tags
				builder = LocalIdQueryBuilder::empty().withPrimaryItem( primaryQueryBuilder ).withSecondaryItem( secondaryQueryBuilder );
				for ( const auto& tag : localId.metadataTags() )
				{
					auto tagsQuery = MetadataTagsQuery::empty().withTag( tag );
					builder = builder.withTags( tagsQuery );
				}
				query = builder.build();
				EXPECT_TRUE( query.match( localId ) );
			}
		}

		TEST_F( LocalIdQueryTest, Test_Variations )
		{
			auto localIdStr = "/dnv-v2/vis-3-4a/1036.13i-1/C662/sec/411.1-2/C101/meta/qty-pressure/cnt-cargo/state-high.high/pos-stage-3/detail-discharge";

			std::optional<LocalIdBuilder> builderOpt;
			bool parsed = LocalIdBuilder::tryParse( localIdStr, builderOpt );
			ASSERT_TRUE( parsed );
			ASSERT_TRUE( builderOpt.has_value() );

			auto localId = builderOpt->build();
			auto primaryItem = GmodPath::parse( "1036.13i-2/C662", VisVersion::v3_4a );

			auto builder = LocalIdQueryBuilder::empty().withPrimaryItem( primaryItem );
			auto query = builder.build();
			EXPECT_FALSE( query.match( localId ) );

			// With withoutLocations should match
			auto pathQuery = GmodPathQuery::fromPath( primaryItem ).withoutLocations();
			builder = LocalIdQueryBuilder::empty().withPrimaryItem( pathQuery );
			query = builder.build();
			EXPECT_TRUE( query.match( localId ) );

			// Test path without locations directly
			primaryItem = primaryItem.withoutLocations();
			builder = LocalIdQueryBuilder::empty().withPrimaryItem( primaryItem );
			query = builder.build();
			EXPECT_FALSE( query.match( localId ) );

			// But with explicit withoutLocations query should match
			pathQuery = GmodPathQuery::fromPath( primaryItem ).withoutLocations();
			builder = LocalIdQueryBuilder::empty().withPrimaryItem( pathQuery );
			query = builder.build();
			EXPECT_TRUE( query.match( localId ) );
		}

		TEST_F( LocalIdQueryTest, Test_Generic_Without_Locations )
		{
			std::optional<LocalIdBuilder> builderOpt;
			bool parsed = LocalIdBuilder::tryParse( "/dnv-v2/vis-3-7a/511.11/C101/meta/qty-pressure/cnt-lubricating.oil", builderOpt );
			ASSERT_TRUE( parsed );
			ASSERT_TRUE( builderOpt.has_value() );

			auto sourceLocalId = builderOpt->build();
			auto primaryPath = sourceLocalId.primaryItem();
			ASSERT_TRUE( primaryPath.has_value() );
			auto pathQuery = GmodPathQuery::fromPath( *primaryPath ).withoutLocations();
			auto query = LocalIdQueryBuilder::from( sourceLocalId ).withPrimaryItem( pathQuery ).build();

			// Should match variant with location
			bool parsedOther = LocalIdBuilder::tryParse( "/dnv-v2/vis-3-7a/511.11-1/C101/meta/qty-pressure/cnt-lubricating.oil", builderOpt );
			ASSERT_TRUE( parsedOther );
			ASSERT_TRUE( builderOpt.has_value() );

			auto otherLocalId = builderOpt->build();
			EXPECT_TRUE( query.match( otherLocalId ) );
		}

		TEST_F( LocalIdQueryTest, Test_Use_Case_1 )
		{
			auto& vis = VIS::instance();
			auto latestVersion = VisVersionExtensions::latestVersion();
			auto& locations = vis.locations( latestVersion );

			std::optional<LocalIdBuilder> builderOpt;
			bool parsed = LocalIdBuilder::tryParse( "/dnv-v2/vis-3-7a/433.1-P/C322/meta/qty-linear.vibration.amplitude/pos-driving.end/detail-iso.10816", builderOpt );
			ASSERT_TRUE( parsed );
			ASSERT_TRUE( builderOpt.has_value() );

			auto localId = builderOpt->build();

			// Match both 433.1-P and 433.1-S
			auto& gmod = vis.gmod( VisVersion::v3_7a );
			const GmodNode* node433 = nullptr;
			ASSERT_TRUE( gmod.tryGetNode( "433.1", node433 ) );
			ASSERT_NE( node433, nullptr );

			auto locationP = locations.parse( "P" );
			auto locationS = locations.parse( "S" );

			std::vector<Location> locs = { locationP, locationS };
			auto primaryPath = localId.primaryItem();
			ASSERT_TRUE( primaryPath.has_value() );
			auto pathQuery = GmodPathQuery::fromPath( *primaryPath ).withNode( *node433, locs );
			auto query = LocalIdQueryBuilder::from( localId ).withPrimaryItem( pathQuery ).build();

			EXPECT_TRUE( query.match( localId ) );

			// Should also match the S variant
			bool parsedS = LocalIdBuilder::tryParse( "/dnv-v2/vis-3-7a/433.1-S/C322/meta/qty-linear.vibration.amplitude/pos-driving.end/detail-iso.10816", builderOpt );
			ASSERT_TRUE( parsedS );
			ASSERT_TRUE( builderOpt.has_value() );

			auto localIdS = builderOpt->build();
			EXPECT_TRUE( query.match( localIdS ) );
		}

		TEST_F( LocalIdQueryTest, Test_Use_Case_2 )
		{
			std::optional<LocalIdBuilder> builderOpt;
			bool parsed = LocalIdBuilder::tryParse( "/dnv-v2/vis-3-7a/511.31/C121/meta/qty-linear.vibration.amplitude/pos-driving.end/detail-iso.10816", builderOpt );
			ASSERT_TRUE( parsed );
			ASSERT_TRUE( builderOpt.has_value() );

			auto localId = builderOpt->build();
			auto& vis = VIS::instance();
			auto& gmod = vis.gmod( localId.visVersion() );

			// Match all Wind turbine arrangements (511.3)
			const GmodNode* node5113 = nullptr;
			ASSERT_TRUE( gmod.tryGetNode( "511.3", node5113 ) );
			ASSERT_NE( node5113, nullptr );

			auto primaryPath = localId.primaryItem();
			ASSERT_TRUE( primaryPath.has_value() );
			auto pathQuery = GmodPathQuery::fromPath( *primaryPath ).withNode( *node5113, true );
			auto query = LocalIdQueryBuilder::from( localId ).withPrimaryItem( pathQuery ).build();
			EXPECT_TRUE( query.match( localId ) );

			// Should not match Solar panel arrangements (511.4)
			const GmodNode* node5114 = nullptr;
			ASSERT_TRUE( gmod.tryGetNode( "511.4", node5114 ) );
			ASSERT_NE( node5114, nullptr );

			pathQuery = GmodPathQuery::fromPath( *primaryPath ).withNode( *node5114, true );
			query = LocalIdQueryBuilder::empty().withPrimaryItem( pathQuery ).build();
			EXPECT_FALSE( query.match( localId ) );
		}

		TEST_F( LocalIdQueryTest, Test_Use_Case_3 )
		{
			// Only match 100% matches
			auto localId = LocalIdBuilder::parse( "/dnv-v2/vis-3-7a/433.1-S/C322.91/S205/meta/qty-conductivity/detail-relative" ).build();
			auto& vis = VIS::instance();
			auto visVersion = localId.visVersion();

			// Create query with strict tag matching (no other tags allowed)
			auto tagsQuery = MetadataTagsQuery::from( localId, false ); // allowOtherTags = false
			auto query = LocalIdQueryBuilder::from( localId ).withTags( tagsQuery ).build();
			EXPECT_TRUE( query.match( localId ) );

			auto& gmod = vis.gmod( visVersion );
			auto& codebooks = vis.codebooks( visVersion );

			// Test with additional tag - should fail
			auto extraTag = codebooks.createTag( CodebookName::Content, "random" );
			auto builderCopy1 = localId.builder();
			auto l1 = builderCopy1.withMetadataTag( extraTag ).build();
			EXPECT_FALSE( query.match( l1 ) );

			// Test with different primary item - should fail
			auto newPath = gmod.parsePath( "433.1-1S" );
			auto builderCopy2 = localId.builder();
			auto l2 = builderCopy2.withPrimaryItem( std::move( newPath ) ).build();
			EXPECT_FALSE( query.match( l2 ) );
		}

		TEST_F( LocalIdQueryTest, Test_Use_Case_4 )
		{

			auto localId = LocalIdBuilder::parse( "/dnv-v2/vis-3-7a/511.11/C101/meta/qty-pressure/cnt-lubricating.oil" ).build();
			auto primaryPath = localId.primaryItem();
			ASSERT_TRUE( primaryPath.has_value() );
			auto pathQuery = GmodPathQuery::fromPath( *primaryPath ).withoutLocations();
			auto query = LocalIdQueryBuilder::from( localId ).withPrimaryItem( pathQuery ).build();

			auto other = LocalIdBuilder::parse( "/dnv-v2/vis-3-7a/511.11-1/C101/meta/qty-pressure/cnt-lubricating.oil" ).build();
			EXPECT_TRUE( query.match( other ) );
		}

		//=====================================================================
		// Match All Test Class
		//=====================================================================

		class MatchAllTest : public ::testing::TestWithParam<InputData>
		{
		};

		TEST_P( MatchAllTest, Test_Empty_Query_Matches_All )
		{
			const InputData& data = GetParam();

			auto query = LocalIdQueryBuilder::empty().build();

			std::optional<LocalIdBuilder> builderOpt;
			bool parsed = LocalIdBuilder::tryParse( data.localIdStr, builderOpt );
			ASSERT_TRUE( parsed ) << "Failed to parse LocalId: " << data.localIdStr;
			ASSERT_TRUE( builderOpt.has_value() );

			auto localId = builderOpt->build();
			// Empty query should match everything
			EXPECT_TRUE( query.match( localId ) );
		}

		//=====================================================================
		// Sample Test Class
		//=====================================================================

		class SampleTest : public ::testing::TestWithParam<std::string>
		{
		};

		TEST_P( SampleTest, Test_Samples )
		{
			const std::string& localIdStr = GetParam();

			std::optional<LocalIdBuilder> builderOpt;
			bool parsed = LocalIdBuilder::tryParse( localIdStr, builderOpt );
			ASSERT_TRUE( parsed ) << "Failed to parse: " << localIdStr;
			ASSERT_TRUE( builderOpt.has_value() );

			auto localId = builderOpt->build();
			auto builder = LocalIdQueryBuilder::from( localId );
			auto query = builder.build();
			EXPECT_TRUE( query.match( localId ) );

			// Empty query should match everything
			query = LocalIdQueryBuilder::empty().build();
			EXPECT_TRUE( query.match( localId ) );
		}

		//=====================================================================
		// Consistency smoke tests
		//=====================================================================

		TEST_F( LocalIdQueryTest, Test_Consistency_Smoke_Test )
		{
			std::ifstream file( "testdata/LocalIds.txt" );
			if ( !file.is_open() )
			{
				FAIL() << "Failed to open testdata/LocalIds.txt";
				return;
			}

			std::vector<std::pair<std::string, std::optional<std::string>>> errored;

			std::string localIdStr;
			while ( std::getline( file, localIdStr ) )
			{
				try
				{
					auto localId = LocalIdBuilder::parse( localIdStr ).build();
					auto builder = LocalIdQueryBuilder::from( localId );
					auto query = builder.build();

					bool match = query.match( localId );
					if ( !match )
					{
						errored.push_back( std::make_pair( localIdStr, std::nullopt ) );
					}
				}
				catch ( const std::exception& ex )
				{
					errored.push_back( std::make_pair( localIdStr, std::string( ex.what() ) ) );
				}
			}

			file.close();

			if ( !errored.empty() )
			{
				for ( const auto& errorInfo : errored )
				{
					std::cout << "Failed on " << errorInfo.first << std::endl;
					if ( errorInfo.second.has_value() )
					{
						std::cout << errorInfo.second.value() << std::endl;
					}
				}
			}

			EXPECT_TRUE( errored.empty() );
		}

		//=====================================================================
		// Test Instance Registration
		//=====================================================================

		INSTANTIATE_TEST_SUITE_P(
			LocalIdQueryTestSuite,
			LocalIdQueryParameterizedTest,
			::testing::ValuesIn( testData() ) );

		INSTANTIATE_TEST_SUITE_P(
			MatchAllTestSuite,
			MatchAllTest,
			::testing::ValuesIn( testData() ) );

		INSTANTIATE_TEST_SUITE_P(
			SampleTestSuite,
			SampleTest,
			::testing::Values(
				"/dnv-v2/vis-3-4a/623.121/H201/sec/412.722-F/C542/meta/qty-level/cnt-lubricating.oil/state-low",
				"/dnv-v2/vis-3-4a/412.723-F/C261/meta/qty-temperature/state-high",
				"/dnv-v2/vis-3-4a/412.723-A/C261/meta/qty-temperature/state-high",
				"/dnv-v2/vis-3-4a/412.723-A/C261/sec/411.1/C101/meta/qty-temperature/state-high/cmd-slow.down",
				"/dnv-v2/vis-3-4a/623.1/sec/412.722-F/CS5/meta/qty-level/cnt-lubricating.oil/state-high",
				"/dnv-v2/vis-3-4a/623.1/sec/412.722-F/CS5/meta/qty-level/cnt-lubricating.oil/state-low",
				"/dnv-v2/vis-3-4a/623.22i-1/S110/sec/412.722-F/C542/meta/state-running",
				"/dnv-v2/vis-3-4a/623.22i-1/S110/sec/412.722-F/C542/meta/state-failure",
				"/dnv-v2/vis-3-4a/623.22i-1/S110/sec/412.722-F/C542/meta/cmd-start",
				"/dnv-v2/vis-3-4a/623.22i-1/S110/sec/412.722-F/C542/meta/cmd-stop",
				"/dnv-v2/vis-3-4a/623.22i-1/S110.2/E31/sec/412.722-F/C542/meta/qty-electric.current/cnt-lubricating.oil",
				"/dnv-v2/vis-3-4a/623.22i-1/S110/sec/412.722-F/C542/meta/state-remote.control",
				"/dnv-v2/vis-3-4a/623.22i-2/S110/sec/412.722-F/C542/meta/state-running",
				"/dnv-v2/vis-3-4a/623.22i-2/S110/sec/412.722-F/C542/meta/state-failure",
				"/dnv-v2/vis-3-4a/623.22i-2/S110/sec/412.722-F/C542/meta/cmd-start",
				"/dnv-v2/vis-3-4a/623.22i-2/S110/sec/412.722-F/C542/meta/cmd-stop",
				"/dnv-v2/vis-3-4a/623.22i-2/S110.2/E31/sec/412.722-F/C542/meta/qty-electric.current/cnt-lubricating.oil",
				"/dnv-v2/vis-3-4a/623.22i-2/S110/sec/412.722-F/C542/meta/state-remote.control",
				"/dnv-v2/vis-3-4a/623.22i/S110/sec/412.722-F/C542/meta/state-stand.by/cmd-start",
				"/dnv-v2/vis-3-4a/623.1/sec/412.722-F/C542/meta/qty-level/cnt-lubricating.oil/state-low",
				"/dnv-v2/vis-3-4a/623.22i/S110/sec/412.722-F/C542/meta/state-control.location",
				"/dnv-v2/vis-3-4a/623.22i/S110/sec/412.722-F/C542/meta/detail-stand.by.start.or.power.failure",
				"/dnv-v2/vis-3-4a/623.1/sec/412.722-F/C542/meta/qty-level/cnt-lubricating.oil/state-high",
				"/dnv-v2/vis-3-4a/412.723-F/C261/meta/qty-temperature",
				"/dnv-v2/vis-3-4a/412.723-A/C261/meta/qty-temperature",
				"/dnv-v2/vis-3-4a/623.121/H201/sec/412.722-A/C542/meta/qty-level/cnt-lubricating.oil/state-high",
				"/dnv-v2/vis-3-4a/623.121/H201/sec/412.722-A/C542/meta/qty-level/cnt-lubricating.oil/state-low",
				"/dnv-v2/vis-3-4a/412.723-A/CS6d/meta/qty-temperature",
				"/dnv-v2/vis-3-4a/411.1/C101.64i-1/S201.1/C151.2/S110/meta/cnt-hydraulic.oil/state-running" ) );

	}
}
