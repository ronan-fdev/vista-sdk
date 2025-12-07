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
 * @file Tests_StringBuilder.cpp
 * @brief Unit tests for StringBuilder wrapper
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/StringBuilder.h>

namespace dnv::vista::sdk
{
	//=====================================================================
	// StringBuilder Tests
	//=====================================================================

	TEST( StringBuilderTests, Append_Methods )
	{
		auto sb = StringBuilder();

		sb.append( std::string_view( "view" ) );
		sb.append( std::string( " string" ) );
		sb.append( " cstr" );
		sb.append( '!' );

		EXPECT_EQ( "view string cstr!", sb.toString() );
	}

	TEST( StringBuilderTests, Stream_Operator_Overloads )
	{
		auto sb = StringBuilder();

		sb << std::string_view( "view" );
		sb << std::string( " string" );
		sb << " cstr";
		sb << '!';

		EXPECT_EQ( "view string cstr!", sb.toString() );
	}

	TEST( StringBuilderTests, Fluent_Interface )
	{
		auto sb = StringBuilder();

		sb.append( "a" ).append( "b" ) << "c" << "d";

		EXPECT_EQ( "abcd", sb.toString() );
	}

	TEST( StringBuilderTests, Very_Long_String )
	{
		auto sb = StringBuilder( 10000 );

		for ( int i = 0; i < 1000; ++i )
		{
			sb << "abcdefghij"; // 10 chars * 1000 = 10,000 chars
		}

		std::string result = sb.toString();
		EXPECT_EQ( 10000, result.length() );
	}

	TEST( StringBuilderTests, Special_Characters )
	{
		auto sb = StringBuilder();

		sb << "path/with/slashes";
		sb << '\n';
		sb << "tab\ttab";
		sb << '\0'; // Null character
		sb << "after_null";

		std::string result = sb.toString();
		EXPECT_TRUE( result.find( "path/with/slashes" ) != std::string::npos );
		EXPECT_TRUE( result.find( '\n' ) != std::string::npos );
		EXPECT_TRUE( result.find( '\t' ) != std::string::npos );
	}

	TEST( StringBuilderTests, UTF8_Strings )
	{
		auto sb = StringBuilder();

		sb << "Hello ";
		sb << "世界"; // Chinese "World"
		sb << " ";
		sb << "🌍"; // Earth emoji

		std::string result = sb.toString();
		EXPECT_TRUE( result.find( "世界" ) != std::string::npos );
		EXPECT_TRUE( result.find( "🌍" ) != std::string::npos );
	}

	TEST( StringBuilderTests, Size_And_IsEmpty )
	{
		auto sb = StringBuilder();

		EXPECT_EQ( 0, sb.size() );
		EXPECT_TRUE( sb.isEmpty() );

		sb << "Hello";
		EXPECT_EQ( 5, sb.size() );
		EXPECT_FALSE( sb.isEmpty() );

		sb << " World";
		EXPECT_EQ( 11, sb.size() );
		EXPECT_FALSE( sb.isEmpty() );
	}

	TEST( StringBuilderTests, Clear_And_Reuse )
	{
		auto sb = StringBuilder();

		sb << "First";
		EXPECT_EQ( "First", sb.toString() );
		EXPECT_EQ( 5, sb.size() );

		sb.clear();
		EXPECT_EQ( 0, sb.size() );
		EXPECT_TRUE( sb.isEmpty() );

		sb << "Second";
		EXPECT_EQ( "Second", sb.toString() );
		EXPECT_EQ( 6, sb.size() );
	}

	TEST( StringBuilderTests, ToStringView )
	{
		auto sb = StringBuilder();

		sb << "qty" << "-" << "temperature" << "/";

		auto view = sb.toStringView();
		EXPECT_EQ( "qty-temperature/", view );
		EXPECT_EQ( 16, view.size() );

		// Verify it's zero-copy (same pointer)
		auto str = sb.toString();
		EXPECT_EQ( str, std::string( view ) );
	}

	TEST( StringBuilderTests, ToStringView_Empty )
	{
		auto sb = StringBuilder();

		auto view = sb.toStringView();
		EXPECT_TRUE( view.empty() );
		EXPECT_EQ( 0, view.size() );
	}
} // namespace dnv::vista::sdk
