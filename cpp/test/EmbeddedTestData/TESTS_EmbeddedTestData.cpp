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
 * @file TESTS_EmbeddedTestData.cpp
 * @brief Tests for EmbeddedTestData functionality
 */

#include <gtest/gtest.h>

#include <EmbeddedTestData.h>

using namespace dnv::vista::sdk;

//=====================================================================
// EmbeddedTestData tests
//=====================================================================

TEST( EmbeddedTestDataTests, ListFiles )
{
	// Should list all embedded testdata files
	const auto files = EmbeddedTestData::listFiles();

	EXPECT_FALSE( files.empty() );
	EXPECT_GT( files.size(), 0 );

	// Should include known test files
	bool hasCodebookJson = false;
	bool hasLocalIdsTxt = false;

	for ( const auto& file : files )
	{
		if ( file == "Codebook.json" )
			hasCodebookJson = true;
		if ( file == "LocalIds.txt" )
			hasLocalIdsTxt = true;
	}

	EXPECT_TRUE( hasCodebookJson ) << "Should have Codebook.json";
	EXPECT_TRUE( hasLocalIdsTxt ) << "Should have LocalIds.txt";
}

TEST( EmbeddedTestDataTests, Load_ReturnsDocument )
{
	// Load returns Document which can be used with nfx::serialization::json API
	const auto doc = EmbeddedTestData::load<nfx::serialization::json::Document>( "Codebook.json" );

	ASSERT_TRUE( doc.has_value() );
	EXPECT_TRUE( doc->contains( "ValidPosition" ) );
	EXPECT_TRUE( doc->contains( "States" ) );
	EXPECT_TRUE( doc->is<nfx::serialization::json::Document::Array>( "ValidPosition" ) );
	EXPECT_TRUE( doc->is<nfx::serialization::json::Document::Array>( "States" ) );
}

TEST( EmbeddedTestDataTests, Load_NotFound )
{
	const auto doc = EmbeddedTestData::load<nfx::serialization::json::Document>( "NonExistent.json" );
	EXPECT_FALSE( doc.has_value() );
}
