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
 * @file TESTS_Codebook.cpp
 * @brief Unit tests for Codebook functionality
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

#include <EmbeddedTestData.h>

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
	// Position validation data
	//----------------------------------------------

	/**
	 * @brief Loads ValidPosition test cases from embedded test data
	 * @return Vector of (input, expectedResult) pairs
	 */
	static std::vector<std::pair<std::string, std::string>> LoadValidPositionData()
	{
		auto doc = EmbeddedTestData::load<Document>( "Codebook.json" ).value();
		auto array = doc.get<Document::Array>( "ValidPosition" ).value();

		std::vector<std::pair<std::string, std::string>> result;
		result.reserve( array.size() );

		for ( size_t i = 0; i < array.size(); ++i )
		{
			auto item = array.get<Document::Array>( i ).value();
			result.emplace_back(
				item.get<std::string>( 0 ).value(),
				item.get<std::string>( 1 ).value() );
		}
		return result;
	}

	//----------------------------------------------
	// Position standard values data
	//----------------------------------------------

	/**
	 * @brief Loads Positions test cases from embedded test data
	 * @return Vector of (invalidStandardValue, validStandardValue) pairs
	 */
	static std::vector<std::pair<std::string, std::string>> LoadPositionsData()
	{
		auto doc = EmbeddedTestData::load<Document>( "Codebook.json" ).value();
		auto array = doc.get<Document::Array>( "Positions" ).value();

		std::vector<std::pair<std::string, std::string>> result;
		result.reserve( array.size() );

		for ( size_t i = 0; i < array.size(); ++i )
		{
			auto item = array.get<Document::Array>( i ).value();
			result.emplace_back(
				item.get<std::string>( 0 ).value(),
				item.get<std::string>( 1 ).value() );
		}
		return result;
	}

	//----------------------------------------------
	// State codebook data
	//----------------------------------------------

	/**
	 * @brief Loads States test cases from embedded test data
	 * @return Vector of (invalidGroup, validValue, validGroup, secondValidValue) tuples
	 */
	static std::vector<std::tuple<std::string, std::string, std::string, std::string>> LoadStatesData()
	{
		auto doc = EmbeddedTestData::load<Document>( "Codebook.json" ).value();
		auto array = doc.get<Document::Array>( "States" ).value();

		std::vector<std::tuple<std::string, std::string, std::string, std::string>> result;
		result.reserve( array.size() );

		for ( size_t i = 0; i < array.size(); ++i )
		{
			auto item = array.get<Document::Array>( i ).value();
			result.emplace_back(
				item.get<std::string>( 0 ).value(),
				item.get<std::string>( 1 ).value(),
				item.get<std::string>( 2 ).value(),
				item.get<std::string>( 3 ).value() );
		}
		return result;
	}

	//----------------------------------------------
	// Metadata tag data
	//----------------------------------------------

	/**
	 * @brief Loads Tag test cases from embedded test data
	 * @return Vector of tuples with 8 parameters for tag validation
	 */
	static std::vector<std::tuple<std::string, std::string, std::string, char, std::string, char, std::string, std::string>>
	LoadTagData()
	{
		auto doc = EmbeddedTestData::load<Document>( "Codebook.json" ).value();
		auto array = doc.get<Document::Array>( "Tag" ).value();

		std::vector<std::tuple<std::string, std::string, std::string, char, std::string, char, std::string, std::string>>
			result;
		result.reserve( array.size() );

		for ( size_t i = 0; i < array.size(); ++i )
		{
			auto item = array.get<Document::Array>( i ).value();
			result.emplace_back(
				item.get<std::string>( 0 ).value(),	   // firstTag
				item.get<std::string>( 1 ).value(),	   // secondTag
				item.get<std::string>( 2 ).value(),	   // thirdTag
				item.get<std::string>( 3 ).value()[0], // thirdTagPrefix
				item.get<std::string>( 4 ).value(),	   // customTag
				item.get<std::string>( 5 ).value()[0], // customTagPrefix
				item.get<std::string>( 6 ).value(),	   // firstInvalidTag
				item.get<std::string>( 7 ).value() );  // secondInvalidTag
		}
		return result;
	}

	//----------------------------------------------
	// Detail codebook data
	//----------------------------------------------

	/**
	 * @brief Loads DetailTag test cases from embedded test data
	 * @return Vector of (validCustomTag, firstInvalidCustomTag, secondInvalidCustomTag) tuples
	 */
	static std::vector<std::tuple<std::string, std::string, std::string>> LoadDetailTagData()
	{
		auto doc = EmbeddedTestData::load<Document>( "Codebook.json" ).value();
		auto array = doc.get<Document::Array>( "DetailTag" ).value();

		std::vector<std::tuple<std::string, std::string, std::string>> result;
		result.reserve( array.size() );

		for ( size_t i = 0; i < array.size(); ++i )
		{
			auto item = array.get<Document::Array>( i ).value();
			result.emplace_back(
				item.get<std::string>( 0 ).value(),
				item.get<std::string>( 1 ).value(),
				item.get<std::string>( 2 ).value() );
		}
		return result;
	}

	//----------------------------------------------
	// Validation result parsing
	//----------------------------------------------

	/**
	 * @brief Converts string representation to PositionValidationResult enum
	 * @param str String representation of validation result
	 * @return Corresponding PositionValidationResult enum value
	 * @throws std::invalid_argument if string is not recognized
	 */
	static PositionValidationResult ParsePositionValidationResult( const std::string& str )
	{
		if ( str == "Valid" )
			return PositionValidationResult::Valid;
		if ( str == "Custom" )
			return PositionValidationResult::Custom;
		if ( str == "Invalid" )
			return PositionValidationResult::Invalid;
		if ( str == "InvalidOrder" )
			return PositionValidationResult::InvalidOrder;
		if ( str == "InvalidGrouping" )
			return PositionValidationResult::InvalidGrouping;

		throw std::invalid_argument( "Unknown PositionValidationResult: " + str );
	}

	//=====================================================================
	// Codebook tests
	//=====================================================================

	//----------------------------------------------
	// Position validation
	//----------------------------------------------

	class PositionValidationTest : public ::testing::TestWithParam<std::pair<std::string, std::string>>
	{
	};

	TEST_P( PositionValidationTest, ValidatePosition_TestCases )
	{
		auto [input, expectedOutput] = GetParam();

		const auto& vis = VIS::instance();
		auto codebooks = vis.codebooks( VisVersion::v3_4a );
		const auto& codebookType = codebooks[CodebookName::Position];

		auto validPosition = codebookType.validatePosition( input );
		auto parsedExpectedOutput = ParsePositionValidationResult( expectedOutput );

		EXPECT_EQ( validPosition, parsedExpectedOutput )
			<< "Input: \"" << input << "\" Expected: " << expectedOutput;
	}

	INSTANTIATE_TEST_SUITE_P(
		CodebookTests,
		PositionValidationTest,
		::testing::ValuesIn( LoadValidPositionData() ) );

	//----------------------------------------------
	// Position standard values
	//----------------------------------------------

	class PositionsTest : public ::testing::TestWithParam<std::pair<std::string, std::string>>
	{
	};

	TEST_P( PositionsTest, HasStandardValue_ValidatesCorrectly )
	{
		auto [invalidStandardValue, validStandardValue] = GetParam();

		const auto& vis = VIS::instance();
		auto codebooks = vis.codebooks( VisVersion::v3_4a );
		const auto& positions = codebooks[CodebookName::Position];

		EXPECT_FALSE( positions.hasStandardValue( invalidStandardValue ) );
		EXPECT_TRUE( positions.hasStandardValue( validStandardValue ) );
	}

	INSTANTIATE_TEST_SUITE_P(
		CodebookTests,
		PositionsTest,
		::testing::ValuesIn( LoadPositionsData() ) );

	//----------------------------------------------
	// Standard values and groups
	//----------------------------------------------

	TEST( CodebookTests, StandardValues_UpperExists )
	{
		const auto& vis = VIS::instance();
		auto codebooks = vis.codebooks( VisVersion::v3_4a );
		const auto& positions = codebooks[CodebookName::Position];

		EXPECT_TRUE( positions.hasStandardValue( "upper" ) );

		const auto& groups = positions.groups();
		EXPECT_TRUE( groups.find( "Vertical" ) != groups.end() );

		const auto& standardValues = positions.standardValues();
		EXPECT_TRUE( standardValues.find( "upper" ) != standardValues.end() );
	}

	//----------------------------------------------
	// State codebook
	//----------------------------------------------

	class StatesTest : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string, std::string>>
	{
	};

	TEST_P( StatesTest, StateCodebook_GroupsAndValues )
	{
		auto [invalidGroup, validValue, validGroup, secondValidValue] = GetParam();

		const auto& vis = VIS::instance();
		auto codebooks = vis.codebooks( VisVersion::v3_4a );
		const auto& states = codebooks[CodebookName::State];

		ASSERT_NE( &states, nullptr );

		EXPECT_FALSE( states.hasGroup( invalidGroup ) );
		EXPECT_TRUE( states.hasStandardValue( validValue ) );
		EXPECT_TRUE( states.hasGroup( validGroup ) );
		EXPECT_TRUE( states.hasStandardValue( secondValidValue ) );
	}

	INSTANTIATE_TEST_SUITE_P(
		CodebookTests,
		StatesTest,
		::testing::ValuesIn( LoadStatesData() ) );

	//----------------------------------------------
	// Metadata tag creation
	//----------------------------------------------

	class CreateTagTest
		: public ::testing::TestWithParam<
			  std::tuple<std::string, std::string, std::string, char, std::string, char, std::string, std::string>>
	{
	};

	TEST_P( CreateTagTest, CreateTag_StandardAndCustom )
	{
		auto [firstTag, secondTag, thirdTag, thirdTagPrefix, customTag, customTagPrefix, firstInvalidTag, secondInvalidTag] =
			GetParam();

		const auto& vis = VIS::instance();
		auto codebooks = vis.codebooks( VisVersion::v3_4a );
		const auto& codebookType = codebooks[CodebookName::Position];

		// Test firstTag
		auto metadataTag1 = codebookType.createTag( firstTag );
		ASSERT_TRUE( metadataTag1.has_value() );
		EXPECT_EQ( metadataTag1->value(), firstTag );
		EXPECT_FALSE( metadataTag1->isCustom() );

		// Test secondTag
		auto metadataTag2 = codebookType.createTag( secondTag );
		ASSERT_TRUE( metadataTag2.has_value() );
		EXPECT_EQ( metadataTag2->value(), secondTag );
		EXPECT_FALSE( metadataTag2->isCustom() );

		// Test thirdTag with prefix
		auto metadataTag3 = codebookType.createTag( thirdTag );
		ASSERT_TRUE( metadataTag3.has_value() );
		EXPECT_EQ( metadataTag3->value(), thirdTag );
		EXPECT_FALSE( metadataTag3->isCustom() );
		EXPECT_EQ( metadataTag3->prefix(), thirdTagPrefix );

		// Test customTag
		auto metadataTag4 = codebookType.createTag( customTag );
		ASSERT_TRUE( metadataTag4.has_value() );
		EXPECT_EQ( metadataTag4->value(), customTag );
		EXPECT_TRUE( metadataTag4->isCustom() );
		EXPECT_EQ( metadataTag4->prefix(), customTagPrefix );

		// Test invalid tags
		EXPECT_FALSE( codebookType.createTag( firstInvalidTag ).has_value() );
		EXPECT_FALSE( codebookType.createTag( secondInvalidTag ).has_value() );
	}

	INSTANTIATE_TEST_SUITE_P(
		CodebookTests,
		CreateTagTest,
		::testing::ValuesIn( LoadTagData() ) );

	//----------------------------------------------
	// Groups iteration
	//----------------------------------------------

	TEST( CodebookTests, Groups_ContainsVertical )
	{
		const auto& vis = VIS::instance();
		auto codebooks = vis.codebooks( VisVersion::v3_4a );
		const auto& positions = codebooks[CodebookName::Position];

		const auto& groups = positions.groups();
		EXPECT_GT( groups.size(), 1 );
		EXPECT_TRUE( groups.find( "Vertical" ) != groups.end() );
	}

	TEST( CodebookTests, Groups_IteratesAllElements )
	{
		const auto& vis = VIS::instance();
		auto codebooks = vis.codebooks( VisVersion::v3_4a );
		const auto& positions = codebooks[CodebookName::Position];

		const auto& groups = positions.groups();
		size_t count = 0;
		for ( [[maybe_unused]] const auto& group : groups )
		{
			++count;
		}

		EXPECT_EQ( count, 11 );
	}

	//----------------------------------------------
	// Standard values iteration
	//----------------------------------------------

	TEST( CodebookTests, StandardValues_IteratesAllElements )
	{
		const auto& vis = VIS::instance();
		auto codebooks = vis.codebooks( VisVersion::v3_4a );
		const auto& positions = codebooks[CodebookName::Position];

		const auto& values = positions.standardValues();
		size_t count = 0;
		for ( [[maybe_unused]] const auto& value : values )
		{
			++count;
		}

		EXPECT_EQ( count, 28 );
	}

	//----------------------------------------------
	// Detail codebook
	//----------------------------------------------

	class DetailTagTest : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string>>
	{
	};

	TEST_P( DetailTagTest, DetailCodebook_TagValidation )
	{
		auto [validCustomTag, firstInvalidCustomTag, secondInvalidCustomTag] = GetParam();

		const auto& vis = VIS::instance();
		auto codebooks = vis.codebooks( VisVersion::v3_4a );
		const auto& codebook = codebooks[CodebookName::Detail];

		ASSERT_NE( &codebook, nullptr );

		// Valid tag should succeed
		auto validTag = codebook.createTag( validCustomTag );
		EXPECT_TRUE( validTag.has_value() );

		// Invalid tags should fail
		EXPECT_FALSE( codebook.createTag( firstInvalidCustomTag ).has_value() );
		EXPECT_FALSE( codebook.createTag( secondInvalidCustomTag ).has_value() );
	}

	INSTANTIATE_TEST_SUITE_P(
		CodebookTests,
		DetailTagTest,
		::testing::ValuesIn( LoadDetailTagData() ) );
} // namespace dnv::vista::sdk::test
