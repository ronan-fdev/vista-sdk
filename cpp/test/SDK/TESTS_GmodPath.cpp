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
 * @file TESTS_GmodPath.cpp
 * @brief Unit tests for GmodPath functionality
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

#include <EmbeddedTestData.h>

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace dnv::vista::sdk::test
{
	using nfx::serialization::json::Document;

	//=====================================================================
	// Test data loading utilities
	//=====================================================================

	//----------------------------------------------
	// GMOD path test data
	//----------------------------------------------

	/**
	 * @brief Test data item from GmodPaths.json
	 */
	struct GmodPathTestItem
	{
		std::string path;
		std::string visVersion;
	};

	/**
	 * @brief Loads valid GMOD paths from embedded test data
	 * @return Vector of valid path test items
	 */
	static std::vector<GmodPathTestItem> LoadValidGmodPaths()
	{
		auto doc = EmbeddedTestData::load<Document>( "GmodPaths.json" ).value();
		auto array = doc.get<Document::Array>( "Valid" ).value();

		std::vector<GmodPathTestItem> result;
		result.reserve( array.size() );

		for ( size_t i = 0; i < array.size(); ++i )
		{
			auto item = array.get<Document::Object>( i ).value();

			GmodPathTestItem data;
			data.path = item.get<std::string>( "path" ).value();
			data.visVersion = item.get<std::string>( "visVersion" ).value();

			result.push_back( data );
		}
		return result;
	}

	/**
	 * @brief Loads invalid GMOD paths from embedded test data
	 * @return Vector of invalid path test items
	 */
	static std::vector<GmodPathTestItem> LoadInvalidGmodPaths()
	{
		auto doc = EmbeddedTestData::load<Document>( "GmodPaths.json" ).value();
		auto array = doc.get<Document::Array>( "Invalid" ).value();

		std::vector<GmodPathTestItem> result;
		result.reserve( array.size() );

		for ( size_t i = 0; i < array.size(); ++i )
		{
			auto item = array.get<Document::Object>( i ).value();

			GmodPathTestItem data;
			data.path = item.get<std::string>( "path" ).value();
			data.visVersion = item.get<std::string>( "visVersion" ).value();

			result.push_back( data );
		}
		return result;
	}

	//----------------------------------------------
	// Individualizable sets test data
	//----------------------------------------------

	/**
	 * @brief Test data item for individualizable sets
	 */
	struct IndividualizableSetTestItem
	{
		bool isFullPath;
		std::string visVersion;
		std::string path;
		std::optional<std::vector<std::vector<std::string>>> expected;
	};

	/**
	 * @brief Loads individualizable sets test data from embedded test data
	 * @return Vector of individualizable set test items
	 */
	static std::vector<IndividualizableSetTestItem> LoadIndividualizableSetsData()
	{
		auto doc = EmbeddedTestData::load<Document>( "IndividualizableSets.json" ).value();

		std::vector<IndividualizableSetTestItem> result;

		Document wrappedDoc;
		wrappedDoc.set( "data", std::move( doc ) );

		auto arrayOpt = wrappedDoc.get<Document::Array>( "data" );
		if ( !arrayOpt.has_value() )
		{
			return result;
		}

		auto array = arrayOpt.value();
		result.reserve( array.size() );

		for ( size_t i = 0; i < array.size(); ++i )
		{
			auto item = array.get<Document::Object>( i ).value();

			IndividualizableSetTestItem data;
			data.isFullPath = item.get<bool>( "isFullPath" ).value();
			data.visVersion = item.get<std::string>( "visVersion" ).value();
			data.path = item.get<std::string>( "path" ).value();

			auto expectedOpt = item.get<Document::Array>( "expected" );
			if ( expectedOpt.has_value() )
			{
				auto expectedArray = expectedOpt.value();
				std::vector<std::vector<std::string>> expected;
				expected.reserve( expectedArray.size() );

				for ( size_t j = 0; j < expectedArray.size(); ++j )
				{
					auto setArray = expectedArray.get<Document::Array>( j ).value();
					std::vector<std::string> set;
					set.reserve( setArray.size() );

					for ( size_t k = 0; k < setArray.size(); ++k )
					{
						set.push_back( setArray.get<std::string>( k ).value() );
					}
					expected.push_back( set );
				}
				data.expected = expected;
			}

			result.push_back( data );
		}
		return result;
	}

	//=====================================================================
	// GmodPath tests
	//=====================================================================

	//----------------------------------------------
	// Parse valid paths
	//----------------------------------------------

	class GmodPathParseTest : public ::testing::TestWithParam<GmodPathTestItem>
	{
	};

	TEST_P( GmodPathParseTest, Parse_ValidPaths )
	{
		auto testData = GetParam();

		auto visVersion = VisVersions::fromString( testData.visVersion ).value();
		const auto& gmod = VIS::instance().gmod( visVersion );
		const auto& locations = VIS::instance().locations( visVersion );

		auto pathOpt = GmodPath::fromString( testData.path, gmod, locations );

		ASSERT_TRUE( pathOpt.has_value() );
		EXPECT_EQ( testData.path, pathOpt->toString() );
	}

	INSTANTIATE_TEST_SUITE_P(
		GmodPathTests,
		GmodPathParseTest,
		::testing::ValuesIn( LoadValidGmodPaths() ) );

	//----------------------------------------------
	// Parse invalid paths
	//----------------------------------------------

	class GmodPathParseInvalidTest : public ::testing::TestWithParam<GmodPathTestItem>
	{
	};

	TEST_P( GmodPathParseInvalidTest, Parse_InvalidPaths )
	{
		auto testData = GetParam();

		auto visVersion = VisVersions::fromString( testData.visVersion ).value();
		const auto& gmod = VIS::instance().gmod( visVersion );
		const auto& locations = VIS::instance().locations( visVersion );

		auto pathOpt = GmodPath::fromString( testData.path, gmod, locations );

		EXPECT_FALSE( pathOpt.has_value() );
	}

	INSTANTIATE_TEST_SUITE_P(
		GmodPathTests,
		GmodPathParseInvalidTest,
		::testing::ValuesIn( LoadInvalidGmodPaths() ) );

	//----------------------------------------------
	// Full path iteration
	//----------------------------------------------

	TEST( GmodPathTests, GetFullPath )
	{
		const auto& gmod = VIS::instance().gmod( VisVersion::v3_4a );
		const auto& locations = VIS::instance().locations( VisVersion::v3_4a );

		auto pathStr = "411.1/C101.72/I101";
		std::map<int, std::string> expectation = {
			{ 0, "VE" },
			{ 1, "400a" },
			{ 2, "410" },
			{ 3, "411" },
			{ 4, "411i" },
			{ 5, "411.1" },
			{ 6, "CS1" },
			{ 7, "C101" },
			{ 8, "C101.7" },
			{ 9, "C101.72" },
			{ 10, "I101" },
		};

		auto pathOpt = GmodPath::fromString( pathStr, gmod, locations );
		ASSERT_TRUE( pathOpt.has_value() );

		std::set<int> seen;
		for ( auto [depth, node] : pathOpt->fullPath() )
		{
			if ( !seen.insert( static_cast<int>( depth ) ).second )
			{
				FAIL() << "Duplicate depth: " << depth;
			}

			if ( seen.size() == 1 )
			{
				EXPECT_EQ( 0, depth );
			}

			auto it = expectation.find( static_cast<int>( depth ) );
			ASSERT_NE( expectation.end(), it ) << "Unexpected depth: " << depth;
			EXPECT_EQ( it->second, node.code() );
		}

		std::vector<int> expectedKeys;
		std::vector<int> seenKeys( seen.begin(), seen.end() );
		for ( const auto& [key, _] : expectation )
		{
			expectedKeys.push_back( key );
		}

		std::sort( expectedKeys.begin(), expectedKeys.end() );
		std::sort( seenKeys.begin(), seenKeys.end() );

		EXPECT_EQ( expectedKeys, seenKeys );
	}

	//----------------------------------------------
	// Full path iteration from depth
	//----------------------------------------------

	TEST( GmodPathTests, GetFullPathFrom )
	{
		const auto& gmod = VIS::instance().gmod( VisVersion::v3_4a );
		const auto& locations = VIS::instance().locations( VisVersion::v3_4a );

		auto pathStr = "411.1/C101.72/I101";
		std::map<int, std::string> expectation = {
			{ 4, "411i" },
			{ 5, "411.1" },
			{ 6, "CS1" },
			{ 7, "C101" },
			{ 8, "C101.7" },
			{ 9, "C101.72" },
			{ 10, "I101" },
		};

		auto pathOpt = GmodPath::fromString( pathStr, gmod, locations );
		ASSERT_TRUE( pathOpt.has_value() );

		std::set<int> seen;
		for ( auto [depth, node] : pathOpt->fullPathFrom( 4 ) )
		{
			if ( !seen.insert( static_cast<int>( depth ) ).second )
			{
				FAIL() << "Duplicate depth: " << depth;
			}

			if ( seen.size() == 1 )
			{
				EXPECT_EQ( 4, depth );
			}

			auto it = expectation.find( static_cast<int>( depth ) );
			ASSERT_NE( expectation.end(), it ) << "Unexpected depth: " << depth;
			EXPECT_EQ( it->second, node.code() );
		}

		std::vector<int> expectedKeys;
		std::vector<int> seenKeys( seen.begin(), seen.end() );
		for ( const auto& [key, _] : expectation )
		{
			expectedKeys.push_back( key );
		}

		std::sort( expectedKeys.begin(), expectedKeys.end() );
		std::sort( seenKeys.begin(), seenKeys.end() );

		EXPECT_EQ( expectedKeys, seenKeys );
	}

	//----------------------------------------------
	// Full path parsing
	//----------------------------------------------

	TEST( GmodPathTests, FullPathParsing )
	{
		const auto& gmod = VIS::instance().gmod( VisVersion::v3_4a );
		const auto& locations = VIS::instance().locations( VisVersion::v3_4a );

		struct TestCase
		{
			std::string shortPath;
			std::string expectedFullPath;
		};

		std::vector<TestCase> testCases = {
			{ "411.1/C101.72/I101", "VE/400a/410/411/411i/411.1/CS1/C101/C101.7/C101.72/I101" },
			{ "612.21-1/C701.13/S93", "VE/600a/610/612/612.2/612.2i-1/612.21-1/CS10/C701/C701.1/C701.13/S93" } };

		for ( const auto& testCase : testCases )
		{
			auto pathOpt = GmodPath::fromString( testCase.shortPath, gmod, locations );
			ASSERT_TRUE( pathOpt.has_value() ) << "Failed to parse: " << testCase.shortPath;

			auto fullString = pathOpt->toFullPathString();
			EXPECT_EQ( testCase.expectedFullPath, fullString );

			auto parsedOpt = GmodPath::fromFullPathString( fullString, gmod, locations );
			ASSERT_TRUE( parsedOpt.has_value() ) << "Failed to parse full path: " << fullString;

			EXPECT_EQ( pathOpt->toString(), parsedOpt->toString() );
			EXPECT_EQ( pathOpt->toFullPathString(), parsedOpt->toFullPathString() );
		}
	}

	//----------------------------------------------
	// Individualizable sets (short paths)
	//----------------------------------------------

	class IndividualizableSetsTest : public ::testing::TestWithParam<IndividualizableSetTestItem>
	{
	};

	TEST_P( IndividualizableSetsTest, IndividualizableSets_ShortPaths )
	{
		auto testData = GetParam();

		// Skip full path tests
		if ( testData.isFullPath )
		{
			return;
		}

		auto visVersion = VisVersions::fromString( testData.visVersion ).value();
		const auto& gmod = VIS::instance().gmod( visVersion );
		const auto& locations = VIS::instance().locations( visVersion );

		auto pathOpt = GmodPath::fromString( testData.path, gmod, locations );

		if ( !testData.expected.has_value() )
		{
			// Path should not parse or should have no individualizable sets
			if ( pathOpt.has_value() )
			{
				auto sets = pathOpt->individualizableSets();
				EXPECT_TRUE( sets.empty() ) << "Expected no individualizable sets for: " << testData.path;
			}
			return;
		}

		ASSERT_TRUE( pathOpt.has_value() ) << "Failed to parse: " << testData.path;

		auto sets = pathOpt->individualizableSets();
		const auto& expected = testData.expected.value();

		ASSERT_EQ( expected.size(), sets.size() ) << "Wrong number of individualizable sets for: " << testData.path;

		for ( size_t i = 0; i < expected.size(); ++i )
		{
			const auto& expectedSet = expected[i];
			auto nodes = sets[i].nodes();

			ASSERT_EQ( expectedSet.size(), nodes.size() )
				<< "Wrong number of nodes in set " << i << " for: " << testData.path;

			for ( size_t j = 0; j < expectedSet.size(); ++j )
			{
				EXPECT_EQ( expectedSet[j], nodes[j].code() )
					<< "Wrong node at position " << j << " in set " << i << " for: " << testData.path;
			}
		}
	}

	INSTANTIATE_TEST_SUITE_P(
		GmodPathTests,
		IndividualizableSetsTest,
		::testing::ValuesIn( LoadIndividualizableSetsData() ) );

	//----------------------------------------------
	// Individualizable sets (full paths)
	//----------------------------------------------

	class IndividualizableSetsFullPathTest : public ::testing::TestWithParam<IndividualizableSetTestItem>
	{
	};

	TEST_P( IndividualizableSetsFullPathTest, IndividualizableSets_FullPaths )
	{
		auto testData = GetParam();

		// Skip short path tests
		if ( !testData.isFullPath )
		{
			return;
		}

		auto visVersion = VisVersions::fromString( testData.visVersion ).value();
		const auto& gmod = VIS::instance().gmod( visVersion );
		const auto& locations = VIS::instance().locations( visVersion );

		auto pathOpt = GmodPath::fromFullPathString( testData.path, gmod, locations );

		if ( !testData.expected.has_value() )
		{
			// Path should not parse or should have no individualizable sets
			if ( pathOpt.has_value() )
			{
				auto sets = pathOpt->individualizableSets();
				EXPECT_TRUE( sets.empty() ) << "Expected no individualizable sets for: " << testData.path;
			}
			return;
		}

		ASSERT_TRUE( pathOpt.has_value() ) << "Failed to parse: " << testData.path;

		auto sets = pathOpt->individualizableSets();
		const auto& expected = testData.expected.value();

		ASSERT_EQ( expected.size(), sets.size() ) << "Wrong number of individualizable sets for: " << testData.path;

		for ( size_t i = 0; i < expected.size(); ++i )
		{
			const auto& expectedSet = expected[i];
			auto nodes = sets[i].nodes();

			ASSERT_EQ( expectedSet.size(), nodes.size() )
				<< "Wrong number of nodes in set " << i << " for: " << testData.path;

			for ( size_t j = 0; j < expectedSet.size(); ++j )
			{
				EXPECT_EQ( expectedSet[j], nodes[j].code() )
					<< "Wrong node at position " << j << " in set " << i << " for: " << testData.path;
			}
		}
	}

	INSTANTIATE_TEST_SUITE_P(
		GmodPathTests,
		IndividualizableSetsFullPathTest,
		::testing::ValuesIn( LoadIndividualizableSetsData() ) );

	//----------------------------------------------
	// Non-individualizable path
	//----------------------------------------------

	TEST( GmodPathTests, GmodPath_DoesNotIndividualize )
	{
		const auto& gmod = VIS::instance().gmod( VisVersion::v3_4a );
		const auto& locations = VIS::instance().locations( VisVersion::v3_4a );

		auto pathOpt = GmodPath::fromString( "500a-1", gmod, locations );

		// This path should not parse (invalid individualization)
		EXPECT_FALSE( pathOpt.has_value() );
	}

	//----------------------------------------------
	// toFullPathString
	//----------------------------------------------

	TEST( GmodPathTests, ToFullPathString )
	{
		const auto& gmod = VIS::instance().gmod( VisVersion::v3_4a );
		const auto& locations = VIS::instance().locations( VisVersion::v3_4a );

		struct TestCase
		{
			std::string shortPath;
			std::string expectedFullPath;
		};

		std::vector<TestCase> testCases = {
			{ "411.1/C101.72/I101", "VE/400a/410/411/411i/411.1/CS1/C101/C101.7/C101.72/I101" },
			{ "612.21-1/C701.13/S93", "VE/600a/610/612/612.2/612.2i-1/612.21-1/CS10/C701/C701.1/C701.13/S93" } };

		for ( const auto& testCase : testCases )
		{
			auto pathOpt = GmodPath::fromString( testCase.shortPath, gmod, locations );
			ASSERT_TRUE( pathOpt.has_value() );

			EXPECT_EQ( testCase.expectedFullPath, pathOpt->toFullPathString() );
		}
	}

	//----------------------------------------------
	// Valid paths individualizable sets
	//----------------------------------------------

	class ValidGmodPathIndividualizableSetsTest : public ::testing::TestWithParam<GmodPathTestItem>
	{
	};

	TEST_P( ValidGmodPathIndividualizableSetsTest, IndividualizableSets_ValidPaths )
	{
		auto testData = GetParam();

		auto visVersion = VisVersions::fromString( testData.visVersion ).value();
		const auto& gmod = VIS::instance().gmod( visVersion );
		const auto& locations = VIS::instance().locations( visVersion );

		auto pathOpt = GmodPath::fromString( testData.path, gmod, locations );
		ASSERT_TRUE( pathOpt.has_value() );

		// Just verify that individualizableSets() doesn't throw
		auto sets = pathOpt->individualizableSets();

		// Verify that each set can be converted to string
		for ( const auto& set : sets )
		{
			EXPECT_NO_THROW( { auto str = set.toString(); } );
		}
	}

	INSTANTIATE_TEST_SUITE_P(
		GmodPathTests,
		ValidGmodPathIndividualizableSetsTest,
		::testing::ValuesIn( LoadValidGmodPaths() ) );

	//----------------------------------------------
	// Valid paths individualizable sets (full path)
	//----------------------------------------------

	class ValidGmodPathIndividualizableSetsFullPathTest : public ::testing::TestWithParam<GmodPathTestItem>
	{
	};

	TEST_P( ValidGmodPathIndividualizableSetsFullPathTest, IndividualizableSets_ValidPaths_FullPath )
	{
		auto testData = GetParam();

		auto visVersion = VisVersions::fromString( testData.visVersion ).value();
		const auto& gmod = VIS::instance().gmod( visVersion );
		const auto& locations = VIS::instance().locations( visVersion );

		auto pathOpt = GmodPath::fromString( testData.path, gmod, locations );
		ASSERT_TRUE( pathOpt.has_value() );

		auto fullPathStr = pathOpt->toFullPathString();
		auto fullPathOpt = GmodPath::fromFullPathString( fullPathStr, gmod, locations );
		ASSERT_TRUE( fullPathOpt.has_value() );

		// Just verify that individualizableSets() doesn't throw
		auto sets = fullPathOpt->individualizableSets();

		// Verify that each set can be converted to string
		for ( const auto& set : sets )
		{
			EXPECT_NO_THROW( { auto str = set.toString(); } );
		}
	}

	INSTANTIATE_TEST_SUITE_P(
		GmodPathTests,
		ValidGmodPathIndividualizableSetsFullPathTest,
		::testing::ValuesIn( LoadValidGmodPaths() ) );
} // namespace dnv::vista::sdk::test
