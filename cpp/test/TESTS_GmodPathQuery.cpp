/**
 * @file TESTS_GmodPathQuery.cpp
 * @brief Unit tests for the GmodPathQuery class functionality
 * @details Tests GmodPathQuery path-based and node-based query building,
 *          location filtering, and path matching capabilities.
 */

#include <gtest/gtest.h>

#include "dnv/vista/sdk/GmodPathQuery.h"
#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/Locations.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk::tests
{
	namespace GmodPathQueryTests
	{
		//=====================================================================
		// Test Data Structure
		//=====================================================================

		struct InputData
		{
			std::string path;
			VisVersion visVersion;
			std::vector<std::pair<std::string, std::optional<std::vector<std::string>>>> parameters;
			bool expectedMatch;
		};

		//=====================================================================
		// Test Fixture
		//=====================================================================

		class GmodPathQueryTest : public ::testing::Test
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
		// Test Data for Path Builder Tests
		//=====================================================================

		static std::vector<InputData> pathBuilderTestData()
		{
			return {
				{ "411.1-1/C101", VisVersion::v3_4a, {}, true },
				{ "411.1-1/C101", VisVersion::v3_4a, { { "411.1", std::vector<std::string>{ "1" } } }, true },
				{ "411.1-1/C101", VisVersion::v3_4a, { { "411.1", std::vector<std::string>{ "A" } } }, false },
				{ "433.1-P/C322.31/C173", VisVersion::v3_4a, { { "C322.31", std::nullopt } }, true },
				{ "433.1-P/C322.31-2/C173", VisVersion::v3_4a, { { "433.1", std::vector<std::string>{ "P" } }, { "C322.31", std::nullopt } }, true },
				{ "433.1-P/C322.31-2/C173", VisVersion::v3_4a, { { "433.1", std::vector<std::string>{ "A" } }, { "C322.31", std::nullopt } }, false },
				{ "433.1-P/C322.31-2/C173", VisVersion::v3_4a, { { "433.1", std::vector<std::string>{ "P" } }, { "C322.31", std::vector<std::string>{ "1" } } }, false },
				{ "433.1-A/C322.31-2/C173", VisVersion::v3_4a, { { "433.1", std::vector<std::string>{ "P" } }, { "C322.31", std::vector<std::string>{ "1" } } }, false },
				{ "433.1-A/C322.31-2/C173", VisVersion::v3_4a, { { "433.1", std::nullopt }, { "C322.31", std::nullopt } }, true },
				{ "433.1/C322.31-2/C173", VisVersion::v3_4a, { { "433.1", std::vector<std::string>{ "A" } } }, false },
				{ "433.1/C322.31-2/C173", VisVersion::v3_4a, { { "433.1", std::vector<std::string>{} } }, true } };
		}

		//=====================================================================
		// Test Data for Nodes Builder Tests
		//=====================================================================

		static std::vector<InputData> nodesBuilderTestData()
		{
			return {
				{ "411.1-1/C101", VisVersion::v3_4a, { { "411.1", std::vector<std::string>{ "1" } } }, true },
				{ "411.1-1/C101.61/S203.3/S110.2/C101", VisVersion::v3_7a, { { "411.1", std::vector<std::string>{ "1" } } }, true },
				{ "411.1/C101.61-1/S203.3/S110.2/C101", VisVersion::v3_7a, { { "C101.61", std::vector<std::string>{ "1" } } }, true },
				{ "511.11/C101.61-1/S203.3/S110.2/C101", VisVersion::v3_7a, { { "C101.61", std::vector<std::string>{ "1" } } }, true },
				{ "411.1/C101.61-1/S203.3/S110.2/C101", VisVersion::v3_7a, { { "C101.61", std::nullopt } }, true },
				{ "511.11/C101.61-1/S203.3/S110.2/C101", VisVersion::v3_7a, { { "C101.61", std::nullopt } }, true },
				{ "221.11/C1141.421/C1051.7/C101.61-2/S203", VisVersion::v3_7a, { { "C101.61", std::nullopt } }, true },
				{ "411.1/C101.61-1/S203.3/S110.2/C101", VisVersion::v3_7a, { { "411.1", std::nullopt }, { "C101.61", std::nullopt } }, true },
				{ "511.11/C101.61-1/S203.3/S110.2/C101", VisVersion::v3_7a, { { "411.1", std::nullopt }, { "C101.61", std::nullopt } }, false },
				{ "411.1/C101.61/S203.3-1/S110.2/C101", VisVersion::v3_7a, { { "S203.3", std::vector<std::string>{ "1" } } }, true },
				{ "411.1/C101.61/S203.3-1/S110.2/C101", VisVersion::v3_7a, { { "S203.3", std::vector<std::string>{ "1" } } }, true } };
		}

		//=====================================================================
		// Parameterized Test Classes
		//=====================================================================

		class PathBuilderTest : public ::testing::TestWithParam<InputData>
		{
		protected:
			static std::pair<VIS&, const Gmod&> visAndGmod( VisVersion visVersion )
			{
				VIS& vis = VIS::instance();
				const auto& gmod = vis.gmod( visVersion );
				return { vis, gmod };
			}
		};

		class NodesBuilderTest : public ::testing::TestWithParam<InputData>
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
		// Path Builder Tests
		//=====================================================================

		TEST_P( PathBuilderTest, Test_Path_Builder )
		{
			const InputData& data = GetParam();
			const VisVersion visVersion = data.visVersion;

			auto [vis, gmod] = visAndGmod( visVersion );
			const auto& locations = vis.locations( visVersion );

			const std::string& pathStr = data.path;
			auto path = gmod.parsePath( pathStr );

			auto query = GmodPathQuery::fromPath( path );

			// For consistency, the query should always match itself
			EXPECT_TRUE( query.match( &path ) );

			// Apply parameters
			for ( const auto& [nodeCode, locationStrings] : data.parameters )
			{
				if ( !locationStrings.has_value() || locationStrings->empty() )
				{
					// Match all locations for this node
					query = query.withNode( [&nodeCode]( const auto& setNodes ) -> const GmodNode& {
						auto it = setNodes.find( nodeCode );
						if ( it == setNodes.end() )
						{
							throw std::runtime_error{ "Node not found in set nodes: " + nodeCode };
						}

						return it->second;
					},
						true );
				}
				else
				{
					// Parse locations and apply them
					std::vector<Location> parsedLocations;
					for ( const std::string& locStr : *locationStrings )
					{
						auto location = locations.parse( locStr );
						parsedLocations.push_back( location );
					}
					query = query.withNode( [&nodeCode]( const auto& setNodes ) -> const GmodNode& {
						auto it = setNodes.find( nodeCode );
						if ( it == setNodes.end() )
						{
							throw std::runtime_error{ "Node not found in set nodes: " + nodeCode };
						}

						return it->second;
					},
						parsedLocations );
				}
			}

			bool match = query.match( &path );
			EXPECT_EQ( data.expectedMatch, match );
		}

		//=====================================================================
		// Nodes Builder Tests
		//=====================================================================

		TEST_P( NodesBuilderTest, Test_Nodes_Builder )
		{
			const InputData& data = GetParam();
			const VisVersion visVersion = data.visVersion;

			auto [vis, gmod] = visAndGmod( visVersion );
			const auto& locations = vis.locations( visVersion );

			const std::string& pathStr = data.path;
			auto path = gmod.parsePath( pathStr );

			auto query = GmodPathQuery::empty();

			// Apply parameters
			for ( const auto& [nodeCode, locationStrings] : data.parameters )
			{
				const auto& node = gmod[nodeCode];

				if ( !locationStrings.has_value() || locationStrings->empty() )
				{
					// Match all locations for this node
					query = query.withNode( node, true );
				}
				else
				{
					// Parse locations and apply them
					std::vector<Location> parsedLocations;
					for ( const std::string& locStr : *locationStrings )
					{
						auto location = locations.parse( locStr );
						parsedLocations.push_back( location );
					}
					query = query.withNode( node, parsedLocations );
				}
			}

			bool match = query.match( &path );
			EXPECT_EQ( data.expectedMatch, match );
		}

		//=====================================================================
		// Basic Functionality Tests
		//=====================================================================

		TEST_F( GmodPathQueryTest, Test_Empty_Query_Creation )
		{
			auto query = GmodPathQuery::empty();

			auto [vis, gmod] = visAndGmod( VisVersion::v3_4a );
			auto path = gmod.parsePath( "411.1/C101" );

			// An empty query should not match any path (no constraints = no match)
			EXPECT_TRUE( query.match( &path ) ); // Empty query matches everything
		}

		TEST_F( GmodPathQueryTest, Test_FromPath_Query_Creation )
		{
			auto [vis, gmod] = visAndGmod( VisVersion::v3_4a );
			auto path = gmod.parsePath( "411.1-1/C101" );

			auto query = GmodPathQuery::fromPath( path );

			// A query created from a path should always match that path
			EXPECT_TRUE( query.match( &path ) );

			// Verify source path is stored
			EXPECT_TRUE( query.sourcePath().has_value() );
		}

		TEST_F( GmodPathQueryTest, Test_WithoutLocations_Method )
		{
			auto [vis, gmod] = visAndGmod( VisVersion::v3_4a );
			auto path = gmod.parsePath( "411.1-1/C101" );

			auto query = GmodPathQuery::fromPath( path );
			auto queryWithoutLocations = query.withoutLocations();

			// Both queries should match the original path
			EXPECT_TRUE( query.match( &path ) );
			EXPECT_TRUE( queryWithoutLocations.match( &path ) );

			// The query without locations should also match a path without locations
			auto pathWithoutLocation = gmod.parsePath( "411.1/C101" );
			EXPECT_TRUE( queryWithoutLocations.match( &pathWithoutLocation ) );
		}

		TEST_F( GmodPathQueryTest, Test_Null_Path_Matching )
		{
			auto query = GmodPathQuery::empty();

			// Null path should never match
			EXPECT_FALSE( query.match( nullptr ) );
		}

		//=====================================================================
		// Test Instance Registration
		//=====================================================================

		INSTANTIATE_TEST_SUITE_P(
			PathBuilderTestSuite,
			PathBuilderTest,
			::testing::ValuesIn( pathBuilderTestData() ) );

		INSTANTIATE_TEST_SUITE_P(
			NodesBuilderTestSuite,
			NodesBuilderTest,
			::testing::ValuesIn( nodesBuilderTestData() ) );
	}
}
