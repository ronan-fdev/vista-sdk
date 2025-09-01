/**
 * @file TESTS_LocalIdQuery.cpp
 * @brief Unit tests for the LocalIdQuery and LocalIdQueryBuilder class functionality
 * @details Tests LocalIdQuery building, matching, and filtering capabilities based on
 *          primary item, secondary item, and metadata tag constraints.
 */

#include <gtest/gtest.h>

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

		static std::vector<InputData> getTestData()
		{
			return {
				{ "/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage",
					[]() {
						auto path = GmodPath::parse( "1021.1i-6P/H123", VisVersion::v3_4a );
						auto pathQuery = GmodPathQuery::fromPath( path ).withoutLocations();
						return LocalIdQueryBuilder::empty().withPrimaryItem( pathQuery ).build();
					},
					true },
				{ "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened",
					[]() {
						auto& vis = VIS::instance();
						auto& codebooks = vis.codebooks( VisVersion::v3_4a );
						auto tag = codebooks.createTag( CodebookName::Content, "sea.water" );
						auto tagsQuery = MetadataTagsQuery::empty().withTag( tag );
						return LocalIdQueryBuilder::empty().withTags( tagsQuery ).build();
					},
					true },
				{ "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
					[]() {
						auto path = GmodPath::parse( "411.1/C101.31-1", VisVersion::v3_4a );
						return LocalIdQueryBuilder::empty().withPrimaryItem( path ).build();
					},
					false } };
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
		// Test Instance Registration
		//=====================================================================

		INSTANTIATE_TEST_SUITE_P(
			LocalIdQueryTestSuite,
			LocalIdQueryParameterizedTest,
			::testing::ValuesIn( getTestData() ) );

		INSTANTIATE_TEST_SUITE_P(
			SampleTestSuite,
			SampleTest,
			::testing::Values(
				"/dnv-v2/vis-3-4a/623.121/H201/sec/412.722-F/C542/meta/qty-level/cnt-lubricating.oil/state-low",
				"/dnv-v2/vis-3-4a/412.723-F/C261/meta/qty-temperature/state-high",
				"/dnv-v2/vis-3-4a/412.723-A/C261/meta/qty-temperature/state-high" ) );
	}
}
