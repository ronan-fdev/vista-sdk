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
 * @file TESTS_VISVersions.cpp
 * @brief Unit tests for VISVersion and VISVersions utilities
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VISVersions.h>

namespace dnv::vista::sdk::test
{
	//=====================================================================
	// VISVersions tests
	//=====================================================================

	TEST( VISVersionsTests, ToString )
	{
		auto visVersion = VISVersion::v3_9a;
		auto visVersionString = VISVersions::toString( visVersion );

		EXPECT_EQ( "3-9a", visVersionString );
	}

	TEST( VISVersionsTests, FromString_OptionalOverload )
	{
		// Test valid versions
		auto v34a = VISVersions::fromString( "3-4a" );
		ASSERT_TRUE( v34a.has_value() );
		EXPECT_EQ( VISVersion::v3_4a, *v34a );

		auto v39a = VISVersions::fromString( "3-9a" );
		ASSERT_TRUE( v39a.has_value() );
		EXPECT_EQ( VISVersion::v3_9a, *v39a );

		// Test invalid version
		auto invalid = VISVersions::fromString( "invalid" );
		EXPECT_FALSE( invalid.has_value() );
	}

	TEST( VISVersionsTests, FromString_OutParameterOverload )
	{
		// Test valid versions
		VISVersion result;
		bool success = VISVersions::fromString( "3-4a", result );
		EXPECT_TRUE( success );
		EXPECT_EQ( VISVersion::v3_4a, result );

		success = VISVersions::fromString( "3-9a", result );
		EXPECT_TRUE( success );
		EXPECT_EQ( VISVersion::v3_9a, result );

		// Test invalid version
		success = VISVersions::fromString( "invalid", result );
		EXPECT_FALSE( success );
	}

	TEST( VISVersionsTests, Ordered )
	{
		const auto versions = VISVersions::all();

		// Find indices of 3-4a and 3-10a
		auto it34 = std::find( versions.begin(), versions.end(), VISVersion::v3_4a );
		auto it310 = std::find( versions.begin(), versions.end(), VISVersion::v3_10a );

		ASSERT_NE( it34, versions.end() );
		ASSERT_NE( it310, versions.end() );

		// v3_4a should come before v3_10a
		EXPECT_LT( std::distance( versions.begin(), it34 ), std::distance( versions.begin(), it310 ) );
	}
} // namespace dnv::vista::sdk::test
