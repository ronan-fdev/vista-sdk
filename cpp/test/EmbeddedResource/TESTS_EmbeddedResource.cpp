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
 * @file TESTS_EmbeddedResource.cpp
 * @brief Unit tests for EmbeddedResource library
 */

#include <EmbeddedResource/EmbeddedResource.h>

#include <gtest/gtest.h>

#include <SDK/dto/CodebooksDto.h>
#include <SDK/dto/GmodDto.h>
#include <SDK/dto/LocationsDto.h>
#include <SDK/dto/GmodVersioningDto.h>
#include <SDK/dto/ISO19848Dtos.h>

#include <algorithm>

namespace dnv::vista::sdk::test
{
	//=====================================================================
	// EmbeddedResource tests
	//=====================================================================

	//----------------------------------------------
	// VISVersions
	//----------------------------------------------

	TEST( EmbeddedResourceTest, VISVersions_ContainsExpectedVersions )
	{
		auto versions = EmbeddedResource::visVersions();

		std::vector<std::string> expectedVersions = { "3-4a", "3-5a", "3-6a", "3-7a", "3-8a", "3-9a" };

		for ( const auto& expected : expectedVersions )
		{
			bool found = std::find( versions.begin(), versions.end(), expected ) != versions.end();
			EXPECT_TRUE( found ) << "Expected version not found: " << expected;
		}
	}

	TEST( EmbeddedResourceTest, Ordered )
	{
		const auto versions = EmbeddedResource::visVersions();

		// Find indices of 3-4a and 3-10a
		auto it34 = std::find( versions.begin(), versions.end(), "3-4a" );
		auto it310 = std::find( versions.begin(), versions.end(), "3-10a" );

		ASSERT_NE( it34, versions.end() );
		ASSERT_NE( it310, versions.end() );

		// v3_4a should come before v3_10a
		EXPECT_LT( std::distance( versions.begin(), it34 ), std::distance( versions.begin(), it310 ) );
	}

	//----------------------------------------------
	// Gmod
	//----------------------------------------------

	TEST( EmbeddedResourceTest, Gmod_LoadValidVersion )
	{
		auto gmod = EmbeddedResource::gmod( "3-9a" );

		ASSERT_TRUE( gmod.has_value() );
		EXPECT_EQ( "3-9a", gmod->visVersion );
		EXPECT_FALSE( gmod->items.empty() );
		EXPECT_FALSE( gmod->relations.empty() );
	}

	TEST( EmbeddedResourceTest, Gmod_InvalidVersion_ReturnsNullopt )
	{
		auto gmod = EmbeddedResource::gmod( "invalid-version" );

		EXPECT_FALSE( gmod.has_value() );
	}

	//----------------------------------------------
	// Codebooks
	//----------------------------------------------

	TEST( EmbeddedResourceTest, Codebooks_LoadValidVersion )
	{
		auto codebooks = EmbeddedResource::codebooks( "3-9a" );

		ASSERT_TRUE( codebooks.has_value() );
		EXPECT_EQ( "3-9a", codebooks->visVersion );
		EXPECT_FALSE( codebooks->items.empty() );

		// Check first codebook has expected structure
		if ( !codebooks->items.empty() )
		{
			const auto& firstCodebook = codebooks->items[0];
			EXPECT_FALSE( firstCodebook.name.empty() );
			EXPECT_FALSE( firstCodebook.values.empty() );
		}
	}

	TEST( EmbeddedResourceTest, Codebooks_InvalidVersion_ReturnsNullopt )
	{
		auto codebooks = EmbeddedResource::codebooks( "invalid-version" );

		EXPECT_FALSE( codebooks.has_value() );
	}

	//----------------------------------------------
	// Locations
	//----------------------------------------------

	TEST( EmbeddedResourceTest, Locations_LoadValidVersion )
	{
		auto locations = EmbeddedResource::locations( "3-9a" );

		ASSERT_TRUE( locations.has_value() );
		EXPECT_EQ( "3-9a", locations->visVersion );
		EXPECT_FALSE( locations->items.empty() );

		// Check first location has expected structure
		if ( !locations->items.empty() )
		{
			const auto& firstLocation = locations->items[0];
			EXPECT_NE( '\0', firstLocation.code );
			EXPECT_FALSE( firstLocation.name.empty() );
		}
	}

	TEST( EmbeddedResourceTest, Locations_InvalidVersion_ReturnsNullopt )
	{
		auto locations = EmbeddedResource::locations( "invalid-version" );

		EXPECT_FALSE( locations.has_value() );
	}

	//----------------------------------------------
	// GmodVersioning
	//----------------------------------------------

	TEST( EmbeddedResourceTest, GmodVersioning_LoadsAllVersions )
	{
		auto versioningMap = EmbeddedResource::gmodVersioning();

		ASSERT_TRUE( versioningMap.has_value() );
		EXPECT_FALSE( versioningMap->empty() );

		// Check if at least one version has non-empty items
		bool hasNonEmptyItems = false;
		for ( const auto& [visVersion, versioningDto] : *versioningMap )
		{
			EXPECT_FALSE( visVersion.empty() );
			EXPECT_EQ( visVersion, versioningDto.visVersion );

			if ( !versioningDto.items.empty() )
			{
				hasNonEmptyItems = true;
			}
		}

		// At least one version should have conversion data
		EXPECT_TRUE( hasNonEmptyItems ) << "All versioning files have empty items - check deserialization";
	}

	TEST( EmbeddedResourceTest, GmodVersioning_ContainsExpectedVersions )
	{
		auto versioningMap = EmbeddedResource::gmodVersioning();

		ASSERT_TRUE( versioningMap.has_value() );

		// Check for at least one expected version
		EXPECT_TRUE( versioningMap->find( "3-4a" ) != versioningMap->end() ||
					 versioningMap->find( "3-5a" ) != versioningMap->end() ||
					 versioningMap->find( "3-6a" ) != versioningMap->end() ||
					 versioningMap->find( "3-7a" ) != versioningMap->end() ||
					 versioningMap->find( "3-8a" ) != versioningMap->end() ||
					 versioningMap->find( "3-9a" ) != versioningMap->end() )
			<< "Expected to find at least one versioning file";
	}

	//----------------------------------------------
	// DataChannelTypeNames (ISO19848)
	//----------------------------------------------

	TEST( EmbeddedResourceTest, DataChannelTypeNames_LoadValidVersion )
	{
		auto dataChannelTypeNames = EmbeddedResource::dataChannelTypeNames( "v2024" );

		ASSERT_TRUE( dataChannelTypeNames.has_value() );
		EXPECT_FALSE( dataChannelTypeNames->values.empty() );

		// Check first data channel type name has expected structure
		if ( !dataChannelTypeNames->values.empty() )
		{
			const auto& firstType = dataChannelTypeNames->values[0];
			EXPECT_FALSE( firstType.type.empty() );
			EXPECT_FALSE( firstType.description.empty() );
		}
	}

	TEST( EmbeddedResourceTest, DataChannelTypeNames_InvalidVersion_ReturnsNullopt )
	{
		auto dataChannelTypeNames = EmbeddedResource::dataChannelTypeNames( "invalid-version" );

		EXPECT_FALSE( dataChannelTypeNames.has_value() );
	}

	//----------------------------------------------
	// FormatDataTypes (ISO19848)
	//----------------------------------------------

	TEST( EmbeddedResourceTest, FormatDataTypes_LoadValidVersion )
	{
		auto formatDataTypes = EmbeddedResource::formatDataTypes( "v2024" );

		ASSERT_TRUE( formatDataTypes.has_value() );
		EXPECT_FALSE( formatDataTypes->values.empty() );

		// Check first format data type has expected structure
		if ( !formatDataTypes->values.empty() )
		{
			const auto& firstType = formatDataTypes->values[0];
			EXPECT_FALSE( firstType.type.empty() );
			EXPECT_FALSE( firstType.description.empty() );
		}
	}

	TEST( EmbeddedResourceTest, FormatDataTypes_InvalidVersion_ReturnsNullopt )
	{
		auto formatDataTypes = EmbeddedResource::formatDataTypes( "invalid-version" );

		EXPECT_FALSE( formatDataTypes.has_value() );
	}
} // namespace dnv::vista::sdk::test
