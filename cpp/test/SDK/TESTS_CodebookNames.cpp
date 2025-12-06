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
 * @file TESTS_CodebookNames.cpp
 * @brief Unit tests for CodebookName and CodebookNames utilities
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::test
{
	//=====================================================================
	// CodebookNames tests
	//=====================================================================

	//----------------------------------------------
	// Enum value
	//----------------------------------------------

	TEST( CodebookNamesTests, EnumValues_Sequential )
	{
		EXPECT_EQ( 1, static_cast<std::uint8_t>( CodebookName::Quantity ) );
		EXPECT_EQ( 2, static_cast<std::uint8_t>( CodebookName::Content ) );
		EXPECT_EQ( 3, static_cast<std::uint8_t>( CodebookName::Calculation ) );
		EXPECT_EQ( 4, static_cast<std::uint8_t>( CodebookName::State ) );
		EXPECT_EQ( 5, static_cast<std::uint8_t>( CodebookName::Command ) );
		EXPECT_EQ( 6, static_cast<std::uint8_t>( CodebookName::Type ) );
		EXPECT_EQ( 7, static_cast<std::uint8_t>( CodebookName::FunctionalServices ) );
		EXPECT_EQ( 8, static_cast<std::uint8_t>( CodebookName::MaintenanceCategory ) );
		EXPECT_EQ( 9, static_cast<std::uint8_t>( CodebookName::ActivityType ) );
		EXPECT_EQ( 10, static_cast<std::uint8_t>( CodebookName::Position ) );
		EXPECT_EQ( 11, static_cast<std::uint8_t>( CodebookName::Detail ) );
	}

	//----------------------------------------------
	// fromPrefix valid
	//----------------------------------------------

	TEST( CodebookNamesTests, FromPrefix_Quantity )
	{
		auto result = CodebookNames::fromPrefix( "qty" );
		EXPECT_EQ( CodebookName::Quantity, result );
	}

	TEST( CodebookNamesTests, FromPrefix_Content )
	{
		auto result = CodebookNames::fromPrefix( "cnt" );
		EXPECT_EQ( CodebookName::Content, result );
	}

	TEST( CodebookNamesTests, FromPrefix_Calculation )
	{
		auto result = CodebookNames::fromPrefix( "calc" );
		EXPECT_EQ( CodebookName::Calculation, result );
	}

	TEST( CodebookNamesTests, FromPrefix_State )
	{
		auto result = CodebookNames::fromPrefix( "state" );
		EXPECT_EQ( CodebookName::State, result );
	}

	TEST( CodebookNamesTests, FromPrefix_Command )
	{
		auto result = CodebookNames::fromPrefix( "cmd" );
		EXPECT_EQ( CodebookName::Command, result );
	}

	TEST( CodebookNamesTests, FromPrefix_Type )
	{
		auto result = CodebookNames::fromPrefix( "type" );
		EXPECT_EQ( CodebookName::Type, result );
	}

	TEST( CodebookNamesTests, FromPrefix_FunctionalServices )
	{
		auto result = CodebookNames::fromPrefix( "funct.svc" );
		EXPECT_EQ( CodebookName::FunctionalServices, result );
	}

	TEST( CodebookNamesTests, FromPrefix_MaintenanceCategory )
	{
		auto result = CodebookNames::fromPrefix( "maint.cat" );
		EXPECT_EQ( CodebookName::MaintenanceCategory, result );
	}

	TEST( CodebookNamesTests, FromPrefix_ActivityType )
	{
		auto result = CodebookNames::fromPrefix( "act.type" );
		EXPECT_EQ( CodebookName::ActivityType, result );
	}

	TEST( CodebookNamesTests, FromPrefix_Position )
	{
		auto result = CodebookNames::fromPrefix( "pos" );
		EXPECT_EQ( CodebookName::Position, result );
	}

	TEST( CodebookNamesTests, FromPrefix_Detail )
	{
		auto result = CodebookNames::fromPrefix( "detail" );
		EXPECT_EQ( CodebookName::Detail, result );
	}

	//----------------------------------------------
	// fromPrefix invalid
	//----------------------------------------------

	TEST( CodebookNamesTests, FromPrefix_InvalidPrefix_ThrowsException )
	{
		EXPECT_THROW( CodebookNames::fromPrefix( "invalid" ), std::invalid_argument );
	}

	TEST( CodebookNamesTests, FromPrefix_EmptyString_ThrowsException )
	{
		EXPECT_THROW( CodebookNames::fromPrefix( "" ), std::invalid_argument );
	}

	TEST( CodebookNamesTests, FromPrefix_CaseSensitive )
	{
		// Should be case-sensitive
		EXPECT_THROW( CodebookNames::fromPrefix( "QTY" ), std::invalid_argument );
		EXPECT_THROW( CodebookNames::fromPrefix( "Qty" ), std::invalid_argument );
		EXPECT_THROW( CodebookNames::fromPrefix( "CNT" ), std::invalid_argument );
	}

	TEST( CodebookNamesTests, FromPrefix_WhitespaceNotTrimmed )
	{
		// Whitespace should not be accepted
		EXPECT_THROW( CodebookNames::fromPrefix( " qty" ), std::invalid_argument );
		EXPECT_THROW( CodebookNames::fromPrefix( "qty " ), std::invalid_argument );
		EXPECT_THROW( CodebookNames::fromPrefix( " qty " ), std::invalid_argument );
	}

	TEST( CodebookNamesTests, FromPrefix_SimilarButInvalid )
	{
		// Test similar but incorrect prefixes
		EXPECT_THROW( CodebookNames::fromPrefix( "quantity" ), std::invalid_argument );
		EXPECT_THROW( CodebookNames::fromPrefix( "content" ), std::invalid_argument );
	}

	//----------------------------------------------
	// toPrefix valid
	//----------------------------------------------

	TEST( CodebookNamesTests, ToPrefix_Quantity )
	{
		auto result = CodebookNames::toPrefix( CodebookName::Quantity );
		EXPECT_EQ( "qty", result );
	}

	TEST( CodebookNamesTests, ToPrefix_Content )
	{
		auto result = CodebookNames::toPrefix( CodebookName::Content );
		EXPECT_EQ( "cnt", result );
	}

	TEST( CodebookNamesTests, ToPrefix_Calculation )
	{
		auto result = CodebookNames::toPrefix( CodebookName::Calculation );
		EXPECT_EQ( "calc", result );
	}

	TEST( CodebookNamesTests, ToPrefix_State )
	{
		auto result = CodebookNames::toPrefix( CodebookName::State );
		EXPECT_EQ( "state", result );
	}

	TEST( CodebookNamesTests, ToPrefix_Command )
	{
		auto result = CodebookNames::toPrefix( CodebookName::Command );
		EXPECT_EQ( "cmd", result );
	}

	TEST( CodebookNamesTests, ToPrefix_Type )
	{
		auto result = CodebookNames::toPrefix( CodebookName::Type );
		EXPECT_EQ( "type", result );
	}

	TEST( CodebookNamesTests, ToPrefix_FunctionalServices )
	{
		auto result = CodebookNames::toPrefix( CodebookName::FunctionalServices );
		EXPECT_EQ( "funct.svc", result );
	}

	TEST( CodebookNamesTests, ToPrefix_MaintenanceCategory )
	{
		auto result = CodebookNames::toPrefix( CodebookName::MaintenanceCategory );
		EXPECT_EQ( "maint.cat", result );
	}

	TEST( CodebookNamesTests, ToPrefix_ActivityType )
	{
		auto result = CodebookNames::toPrefix( CodebookName::ActivityType );
		EXPECT_EQ( "act.type", result );
	}

	TEST( CodebookNamesTests, ToPrefix_Position )
	{
		auto result = CodebookNames::toPrefix( CodebookName::Position );
		EXPECT_EQ( "pos", result );
	}

	TEST( CodebookNamesTests, ToPrefix_Detail )
	{
		auto result = CodebookNames::toPrefix( CodebookName::Detail );
		EXPECT_EQ( "detail", result );
	}
} // namespace dnv::vista::sdk::test
