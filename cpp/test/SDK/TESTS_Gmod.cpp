/*
 * MIT License
 *
 * Copyright (c) 2025 DNV
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file TESTS_Gmod.cpp
 * @brief Unit tests for Gmod
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::test
{
	//=====================================================================
	// Gmod tests - Expected values per VIS version
	//=====================================================================

	struct ExpectedGmodValues
	{
		std::string_view maxCode;
		size_t nodeCount;
	};

	static const std::unordered_map<VisVersion, ExpectedGmodValues> expectedMaxes = {
		{ VisVersion::v3_4a, { "C1053.3114", 6420 } },
		{ VisVersion::v3_5a, { "C1053.3114", 6557 } },
		{ VisVersion::v3_6a, { "C1053.3114", 6557 } },
		{ VisVersion::v3_7a, { "H346.11113", 6672 } },
		{ VisVersion::v3_8a, { "H346.11113", 6335 } },
		{ VisVersion::v3_9a, { "H346.11113", 6553 } },
	};

	//=====================================================================
	// Gmod tests
	//=====================================================================

	TEST( GmodTests, GmodLoads )
	{
		const auto& gmod = VIS::instance().gmod( VisVersion::v3_4a );
		EXPECT_TRUE( gmod.node( "400a" ).has_value() );
	}

	class GmodPropertiesTest : public ::testing::TestWithParam<VisVersion>
	{
	};

	TEST_P( GmodPropertiesTest, ValidatesGmodProperties )
	{
		VisVersion version = GetParam();
		const auto& gmod = VIS::instance().gmod( version );

		const GmodNode* minNode = nullptr;
		const GmodNode* maxNode = nullptr;
		size_t currentMinLength = std::numeric_limits<size_t>::max();
		size_t currentMaxLength = 0;
		size_t nodeCount = 0;

		for ( const auto& [code, node] : gmod )
		{
			nodeCount++;
			size_t len = code.length();

			if ( minNode == nullptr || len < currentMinLength )
			{
				currentMinLength = len;
				minNode = &node;
			}
			else if ( len == currentMinLength && code < minNode->code() )
			{
				minNode = &node;
			}

			if ( maxNode == nullptr || len > currentMaxLength )
			{
				currentMaxLength = len;
				maxNode = &node;
			}
			else if ( len == currentMaxLength && code > maxNode->code() )
			{
				maxNode = &node;
			}
		}

		ASSERT_NE( nullptr, minNode );
		ASSERT_NE( nullptr, maxNode );

		// Min node should always be VE with length 2
		EXPECT_EQ( 2u, minNode->code().length() );
		EXPECT_EQ( "VE", minNode->code() );

		// Max node and count should match expected values
		auto it = expectedMaxes.find( version );
		ASSERT_NE( expectedMaxes.end(), it ) << "Missing expected values for version " << static_cast<int>( version );

		EXPECT_EQ( 10u, maxNode->code().length() );
		EXPECT_EQ( it->second.maxCode, maxNode->code() );
		EXPECT_EQ( it->second.nodeCount, nodeCount );
	}

	INSTANTIATE_TEST_SUITE_P(
		AllVisVersions,
		GmodPropertiesTest,
		::testing::Values(
			VisVersion::v3_4a,
			VisVersion::v3_5a,
			VisVersion::v3_6a,
			VisVersion::v3_7a,
			VisVersion::v3_8a,
			VisVersion::v3_9a ) );

	TEST( GmodTests, GmodLookup )
	{
		const auto& gmod = VIS::instance().gmod( VisVersion::v3_4a );

		std::unordered_set<std::string> seen;
		size_t counter = 0;

		for ( const auto& [code, node] : gmod )
		{
			ASSERT_TRUE( seen.insert( code ).second ) << "Duplicate code: " << code;

			auto foundNodeOpt = gmod.node( code );
			ASSERT_TRUE( foundNodeOpt.has_value() );
			EXPECT_EQ( code, ( *foundNodeOpt )->code() );
			EXPECT_EQ( &node, *foundNodeOpt );
			counter++;
		}

		EXPECT_GT( counter, 0u );

		EXPECT_FALSE( gmod.node( "ABC" ).has_value() );
		EXPECT_FALSE( gmod.node( "" ).has_value() );
		EXPECT_FALSE( gmod.node( "SDFASDFSDAFb" ).has_value() );
		EXPECT_FALSE( gmod.node( "✅" ).has_value() );
		EXPECT_FALSE( gmod.node( "a✅b" ).has_value() );
		EXPECT_FALSE( gmod.node( "ac✅bc" ).has_value() );
		EXPECT_FALSE( gmod.node( "✅bc" ).has_value() );
		EXPECT_FALSE( gmod.node( "a✅" ).has_value() );
		EXPECT_FALSE( gmod.node( "ag✅" ).has_value() );
	}

	TEST( GmodTests, NodeEquality )
	{
		const auto& gmod = VIS::instance().gmod( VisVersion::v3_4a );
		const auto& node1 = gmod["400a"];
		const auto& node2 = gmod["400a"];

		// Same node accessed twice should be same instance
		EXPECT_EQ( &node1, &node2 );

		// Different nodes should be different instances
		const auto& node3 = gmod["411"];
		EXPECT_NE( &node1, &node3 );
		EXPECT_NE( node1.code(), node3.code() );
	}

	TEST( GmodTests, NodeTypes )
	{
		const auto& gmod = VIS::instance().gmod( VisVersion::v3_4a );
		std::unordered_set<std::string> typeSet;

		for ( const auto& [code, node] : gmod )
		{
			std::string combined = std::string( node.metadata().category() ) + " | " + std::string( node.metadata().type() );
			typeSet.insert( combined );
		}

		EXPECT_FALSE( typeSet.empty() );
	}

	TEST( GmodTests, GmodRootNodeChildren )
	{
		const auto& gmod = VIS::instance().gmod( VisVersion::v3_4a );

		const auto& node = gmod.rootNode();
		EXPECT_FALSE( node.children().empty() );
	}

	TEST( GmodTests, NormalAssignments )
	{
		const auto& gmod = VIS::instance().gmod( VisVersion::v3_4a );

		const auto& node1 = gmod["411.3"];
		EXPECT_TRUE( node1.productType().has_value() );
		EXPECT_FALSE( node1.productSelection().has_value() );

		const auto& node2 = gmod["H601"];
		EXPECT_FALSE( node2.productType().has_value() );
	}

	TEST( GmodTests, NodeWithProductSelection )
	{
		const auto& gmod = VIS::instance().gmod( VisVersion::v3_4a );

		auto node1Opt = gmod.node( "411.2" );
		ASSERT_TRUE( node1Opt.has_value() );
		EXPECT_TRUE( ( *node1Opt )->productSelection().has_value() );
		EXPECT_FALSE( ( *node1Opt )->productType().has_value() );

		auto node2Opt = gmod.node( "H601" );
		ASSERT_TRUE( node2Opt.has_value() );
		EXPECT_FALSE( ( *node2Opt )->productSelection().has_value() );
	}

	TEST( GmodTests, ProductSelection )
	{
		const auto& gmod = VIS::instance().gmod( VisVersion::v3_4a );

		auto nodeOpt = gmod.node( "CS1" );
		ASSERT_TRUE( nodeOpt.has_value() );
		EXPECT_TRUE( ( *nodeOpt )->isProductSelection() );
	}

	TEST( GmodTests, Mappability )
	{
		const auto& gmod = VIS::instance().gmod( VisVersion::v3_4a );

		EXPECT_FALSE( gmod["VE"].isMappable() );
		EXPECT_FALSE( gmod["300a"].isMappable() );
		EXPECT_TRUE( gmod["300"].isMappable() );
		EXPECT_TRUE( gmod["411"].isMappable() );
		EXPECT_TRUE( gmod["410"].isMappable() );
		EXPECT_FALSE( gmod["651.21s"].isMappable() );
		EXPECT_TRUE( gmod["924.2"].isMappable() );
		EXPECT_FALSE( gmod["411.1"].isMappable() );
		EXPECT_TRUE( gmod["C101"].isMappable() );
		EXPECT_FALSE( gmod["CS1"].isMappable() );
		EXPECT_TRUE( gmod["C101.663"].isMappable() );
		EXPECT_TRUE( gmod["C101.4"].isMappable() );
		EXPECT_FALSE( gmod["C101.21s"].isMappable() );
		EXPECT_TRUE( gmod["F201.11"].isMappable() );
		EXPECT_FALSE( gmod["C101.211"].isMappable() );
	}
} // namespace dnv::vista::sdk::test
