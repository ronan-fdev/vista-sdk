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
 * @file TESTS_Codebooks.cpp
 * @brief Unit tests for Codebooks container functionality
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

#include <algorithm>
#include <set>
#include <vector>

namespace dnv::vista::sdk::test
{
	//=====================================================================
	// Codebooks tests
	//=====================================================================

	//----------------------------------------------
	// Codebooks loading
	//----------------------------------------------

	TEST( CodebooksTests, LoadsAllVersions )
	{
		const auto& vis = VIS::instance();
		const auto allVersions = vis.versions();

		for ( const auto& version : allVersions )
		{
			const Codebooks* codebooksPtr = nullptr;
			ASSERT_NO_THROW( { codebooksPtr = &vis.codebooks( version ); } );
			ASSERT_NE( nullptr, codebooksPtr );

			const Codebook* positionCodebookPtr = nullptr;

			ASSERT_NO_THROW( { positionCodebookPtr = &( *codebooksPtr )[CodebookName::Position]; } );

			ASSERT_NE( nullptr, positionCodebookPtr );
		}
	}

	//----------------------------------------------
	// Tag creation
	//----------------------------------------------

	TEST( CodebooksTests, CreateTag_OutputParam )
	{
		const auto& vis = VIS::instance();
		auto codebooks = vis.codebooks( VisVersion::v3_4a );

		const auto& positions = codebooks[CodebookName::Position];

		// Test creating valid tag
		auto tagOpt = positions.createTag( "upper" );
		ASSERT_TRUE( tagOpt.has_value() );
		EXPECT_EQ( tagOpt->value(), "upper" );
		EXPECT_FALSE( tagOpt->isCustom() );

		// Test creating invalid tag
		auto invalidTag = positions.createTag( "invalid value" );
		EXPECT_FALSE( invalidTag.has_value() );
	}

	//----------------------------------------------
	// Equality
	//----------------------------------------------

	TEST( CodebooksTests, HasStandardValue )
	{
		const auto& vis = VIS::instance();

		const auto& codebooks = vis.codebooks( VisVersion::v3_4a );

		ASSERT_TRUE( codebooks[CodebookName::Position].hasStandardValue( "centre" ) );
	}

	//----------------------------------------------
	// CodebookName enum properties
	//----------------------------------------------

	TEST( CodebooksTests, CodebookNameEnumProperties )
	{
		const std::vector<CodebookName> allCodebookNames{
			CodebookName::Quantity,
			CodebookName::Content,
			CodebookName::Calculation,
			CodebookName::State,
			CodebookName::Command,
			CodebookName::Type,
			CodebookName::FunctionalServices,
			CodebookName::MaintenanceCategory,
			CodebookName::ActivityType,
			CodebookName::Position,
			CodebookName::Detail };

		std::vector<int> actualEnumValues;
		actualEnumValues.reserve( allCodebookNames.size() );
		for ( const auto& cn : allCodebookNames )
		{
			actualEnumValues.push_back( static_cast<int>( cn ) );
		}

		// Verify uniqueness of underlying integer values
		std::set<int> uniqueValues( actualEnumValues.begin(), actualEnumValues.end() );
		ASSERT_EQ( actualEnumValues.size(), uniqueValues.size() );

		/*
			Verify that the enum values are sequential starting from 1
			This assumes the 'allCodebookNames' vector lists them in their natural enum
			order or that their integer values are inherently sequential as expected.
		*/
		std::vector<int> sortedValues = actualEnumValues;
		std::sort( sortedValues.begin(), sortedValues.end() );

		for ( size_t i = 0; i < sortedValues.size(); ++i )
		{
			ASSERT_EQ( static_cast<int>( i + 1 ), sortedValues[i] );
		}
	}
} // namespace dnv::vista::sdk::test
