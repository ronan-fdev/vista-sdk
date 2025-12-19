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
 * @file TESTS_Locations.cpp
 * @brief Unit tests for Locations functionality
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

#include <EmbeddedTestData.h>

#include <string>
#include <vector>

namespace dnv::vista::sdk::test
{
	using nfx::serialization::json::Document;

	//=====================================================================
	// Test data loading utilities
	//=====================================================================

	/**
	 * @brief Test data structure for location parsing tests
	 */
	struct LocationTestCase
	{
		std::string value;
		bool success;
		std::string output;
		std::vector<std::string> expectedErrorMessages;
	};

	/**
	 * @brief Loads Location test cases from embedded test data
	 * @return Vector of location test cases
	 */
	static std::vector<LocationTestCase> LoadLocationTestData()
	{
		auto doc = EmbeddedTestData::load<Document>( "Locations.json" ).value();
		auto array = doc.get<Document::Array>( "locations" ).value();

		std::vector<LocationTestCase> result;
		result.reserve( array.size() );

		for ( size_t i = 0; i < array.size(); ++i )
		{
			auto item = array.get<Document::Object>( i ).value();

			LocationTestCase testCase;

			auto valueOpt = item.get<std::string>( "value" );
			if ( !valueOpt.has_value() )
				continue;
			testCase.value = valueOpt.value();

			auto successOpt = item.get<bool>( "success" );
			if ( !successOpt.has_value() )
				continue;
			testCase.success = successOpt.value();

			if ( auto output = item.get<std::string>( "output" ) )
				testCase.output = output.value();

			if ( auto errArray = item.get<Document::Array>( "expectedErrorMessages" ) )
			{
				for ( size_t j = 0; j < errArray->size(); ++j )
				{
					auto errOpt = errArray->get<std::string>( j );
					if ( errOpt.has_value() )
						testCase.expectedErrorMessages.push_back( errOpt.value() );
				}
			}

			result.push_back( std::move( testCase ) );
		}
		return result;
	}

	//=====================================================================
	// Locations tests
	//=====================================================================

	TEST( LocationsTests, LoadsForAllVersions )
	{
		const auto& vis = VIS::instance();
		const auto& allVersions = vis.versions();

		for ( const auto& visVersion : allVersions )
		{
			SCOPED_TRACE( "Testing VIS version: " + std::string( VisVersions::toString( visVersion ) ) );

			const auto& locations = vis.locations( visVersion );
			EXPECT_FALSE( locations.groups().empty() );
			EXPECT_FALSE( locations.relativeLocations().empty() );
		}
	}

	TEST( LocationsTests, EnumProperties )
	{
		std::vector<int> values = {
			static_cast<int>( LocationGroup::Number ),
			static_cast<int>( LocationGroup::Side ),
			static_cast<int>( LocationGroup::Vertical ),
			static_cast<int>( LocationGroup::Transverse ),
			static_cast<int>( LocationGroup::Longitudinal ) };

		std::set<int> uniqueValues( values.begin(), values.end() );
		EXPECT_EQ( values.size(), uniqueValues.size() );
		EXPECT_EQ( 5, values.size() );
		EXPECT_EQ( 0, static_cast<int>( LocationGroup::Number ) );

		for ( size_t i = 0; i < values.size() - 1; i++ )
		{
			EXPECT_EQ( static_cast<int>( i ), values[i + 1] - 1 );
		}
	}

	TEST( LocationsTests, Locations )
	{
		const auto& locations = VIS::instance().locations( VisVersion::v3_4a );
		auto testCases = LoadLocationTestData();

		ASSERT_FALSE( testCases.empty() ) << "Failed to load test cases";

		for ( const auto& testCase : testCases )
		{
			SCOPED_TRACE( "Testing location: '" + testCase.value + "'" );

			auto result = locations.fromString( testCase.value );

			ParsingErrors errors;
			auto resultWithErrors = locations.fromString( testCase.value, errors );

			if ( !testCase.success )
			{
				EXPECT_FALSE( result.has_value() );
				EXPECT_FALSE( resultWithErrors.has_value() );
				EXPECT_TRUE( errors.hasErrors() );

				if ( !testCase.expectedErrorMessages.empty() && errors.hasErrors() )
				{
					std::vector<std::string> actualErrors;
					actualErrors.reserve( errors.count() );

					for ( const auto& error : errors )
					{
						actualErrors.push_back( error.message );
					}

					EXPECT_EQ( testCase.expectedErrorMessages.size(), actualErrors.size() );

					for ( size_t i = 0; i < std::min( testCase.expectedErrorMessages.size(), actualErrors.size() ); ++i )
					{
						EXPECT_EQ( testCase.expectedErrorMessages[i], actualErrors[i] );
					}
				}
			}
			else
			{
				ASSERT_TRUE( result.has_value() );
				ASSERT_TRUE( resultWithErrors.has_value() );
				EXPECT_FALSE( errors.hasErrors() );

				EXPECT_EQ( testCase.output, result->value() );
				EXPECT_EQ( testCase.output, resultWithErrors->value() );
			}
		}
	}

	TEST( LocationsTests, ParseThrowsOnNullOrEmpty )
	{
		const auto& locations = VIS::instance().locations( VisVersion::v3_4a );

		auto result1 = locations.fromString( "" );
		EXPECT_FALSE( result1.has_value() );

		auto result2 = locations.fromString( std::string_view{} );
		EXPECT_FALSE( result2.has_value() );
	}

} // namespace dnv::vista::sdk::test
