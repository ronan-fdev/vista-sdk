/**
 * @file TESTS_ImoNumber.cpp
 * @brief Unit tests for the ImoNumber class.
 */

#include <fstream>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "dnv/vista/sdk/ImoNumber.h"

namespace dnv::vista::sdk::test
{
	class ImoNumberTests : public ::testing::Test
	{
	protected:
		struct TestDataItem
		{
			std::string value;
			bool success;
			std::optional<std::string> output;
		};

		std::vector<TestDataItem> testData;

		virtual void SetUp() override
		{
			std::vector<std::string> possiblePaths{
				"testdata/ImoNumbers.json",
				"../testdata/ImoNumbers.json",
				"../../testdata/ImoNumbers.json",
				"../../../testdata/ImoNumbers.json",
				"./ImoNumbers.json" };

			std::ifstream file;
			std::string attemptedPaths;
			bool fileOpened = false;

			for ( const auto& path : possiblePaths )
			{
				file.open( path );
				if ( file.is_open() )
				{
					fileOpened = true;
					break;
				}

				attemptedPaths += path + ", ";
				file.clear();
			}

			if ( !fileOpened )
			{
				ASSERT_TRUE( false ) << "Failed to open ImoNumbers.json. Attempted paths: " << attemptedPaths;
				return;
			}

			nlohmann::json data;
			try
			{
				data = nlohmann::json::parse( file );
			}
			catch ( [[maybe_unused]] const nlohmann::json::parse_error& ex )
			{
				ASSERT_TRUE( false ) << "Failed to parse ImoNumbers.json: " << ex.what();
				return;
			}

			ASSERT_TRUE( data.is_object() || data.is_array() ) << "JSON data is not a valid object or array";

			const nlohmann::json& testCases = data.is_array()
												  ? data
												  : ( data.contains( "imoNumbers" ) ? data.at( "imoNumbers" ) : data );

			ASSERT_TRUE( testCases.is_array() ) << "Test cases must be an array";

			for ( const auto& item : testCases )
			{
				ASSERT_TRUE( item.contains( "value" ) && item.at( "value" ).is_string() ) << "Item missing 'value' field or not a string";
				ASSERT_TRUE( item.contains( "success" ) && item.at( "success" ).is_boolean() ) << "Item missing 'success' field or not a boolean";

				std::optional<std::string> output;
				if ( item.contains( "output" ) && item.at( "output" ).is_string() )
				{
					output = item.at( "output" ).get<std::string>();
				}

				testData.push_back( { item.at( "value" ).get<std::string>(), item.at( "success" ).get<bool>(), output } );
			}
		}
	};

	TEST_F( ImoNumberTests, Test_Validation )
	{
		for ( const auto& item : testData )
		{
			auto parsedImo = ImoNumber::tryParse( item.value );
			bool parsedOk = parsedImo.has_value();

			if ( item.success )
			{
				EXPECT_TRUE( parsedOk );
			}
			else
			{
				EXPECT_FALSE( parsedOk );
			}

			if ( item.output.has_value() && parsedOk )
			{
				EXPECT_EQ( parsedImo->toString(), item.output.value() );
			}
		}
	}

	TEST_F( ImoNumberTests, Test_ConstCharInputs )
	{
		const char* validImo = "IMO9074729";
		const char* invalidImo = "IMO123";
		const char* numericOnly = "9074729";

		auto parsed1 = ImoNumber::tryParse( validImo );
		EXPECT_TRUE( parsed1.has_value() );
		EXPECT_EQ( parsed1->toString(), "IMO9074729" );

		auto parsed2 = ImoNumber::tryParse( invalidImo );
		EXPECT_FALSE( parsed2.has_value() );

		auto parsed3 = ImoNumber::tryParse( numericOnly );
		EXPECT_TRUE( parsed3.has_value() );
		EXPECT_EQ( parsed3->toString(), "IMO9074729" );

		EXPECT_NO_THROW( ImoNumber{ validImo } );
		EXPECT_THROW( ImoNumber{ invalidImo }, std::invalid_argument );

		auto parsed4 = ImoNumber::parse( validImo );
		EXPECT_EQ( parsed4.toString(), "IMO9074729" );

		EXPECT_THROW( ImoNumber::parse( invalidImo ), std::invalid_argument );
	}

	TEST_F( ImoNumberTests, Test_StdStringInputs )
	{
		std::string validImo = "IMO9074729";
		std::string invalidImo = "IMO123";
		std::string numericOnly = "9074729";
		std::string caseVariant = "imo9074729";

		auto parsed1 = ImoNumber::tryParse( validImo );
		EXPECT_TRUE( parsed1.has_value() );
		EXPECT_EQ( parsed1->toString(), "IMO9074729" );

		auto parsed2 = ImoNumber::tryParse( invalidImo );
		EXPECT_FALSE( parsed2.has_value() );

		auto parsed3 = ImoNumber::tryParse( numericOnly );
		EXPECT_TRUE( parsed3.has_value() );
		EXPECT_EQ( parsed3->toString(), "IMO9074729" );

		auto parsed4 = ImoNumber::tryParse( caseVariant );
		EXPECT_TRUE( parsed4.has_value() );
		EXPECT_EQ( parsed4->toString(), "IMO9074729" );

		EXPECT_NO_THROW( ImoNumber{ validImo } );
		EXPECT_THROW( ImoNumber{ invalidImo }, std::invalid_argument );

		auto parsed5 = ImoNumber::parse( validImo );
		EXPECT_EQ( parsed5.toString(), "IMO9074729" );

		EXPECT_THROW( ImoNumber::parse( invalidImo ), std::invalid_argument );
	}

	TEST_F( ImoNumberTests, Test_StringLiteralInputs )
	{
		auto parsed1 = ImoNumber::tryParse( "IMO9074729" );
		EXPECT_TRUE( parsed1.has_value() );
		EXPECT_EQ( parsed1->toString(), "IMO9074729" );

		auto parsed2 = ImoNumber::tryParse( "9074729" );
		EXPECT_TRUE( parsed2.has_value() );
		EXPECT_EQ( parsed2->toString(), "IMO9074729" );

		EXPECT_NO_THROW( ImoNumber{ "IMO9074729" } );
		EXPECT_THROW( ImoNumber{ "invalid" }, std::invalid_argument );

		auto parsed3 = ImoNumber::parse( "IMO9074729" );
		EXPECT_EQ( parsed3.toString(), "IMO9074729" );
	}
}
