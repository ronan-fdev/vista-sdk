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

#include <gtest/gtest.h>

#include <EmbeddedResource.h>

#include <CodebooksDto.h>
#include <GmodDto.h>
#include <LocationsDto.h>
#include <GmodVersioningDto.h>
#include <ISO19848Dtos.h>

#include <algorithm>

namespace dnv::vista::sdk::test
{
	//=====================================================================
	// EmbeddedResource tests
	//=====================================================================

	//----------------------------------------------
	// VisVersions
	//----------------------------------------------

	TEST( EmbeddedResourceTest, VisVersions_ContainsExpectedVersions )
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

		auto it34 = std::find( versions.begin(), versions.end(), "3-4a" );
		auto it39 = std::find( versions.begin(), versions.end(), "3-9a" );

		ASSERT_NE( it34, versions.end() );
		ASSERT_NE( it39, versions.end() );

		EXPECT_LT( std::distance( versions.begin(), it34 ), std::distance( versions.begin(), it39 ) );
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

	TEST( EmbeddedResourceTest, Gmod_AllVersionsLoad )
	{
		auto allVersions = EmbeddedResource::visVersions();

		for ( const auto& version : allVersions )
		{
			auto gmod = EmbeddedResource::gmod( version );

			ASSERT_TRUE( gmod.has_value() ) << "Failed to load Gmod for version: " << version;
			EXPECT_EQ( version, gmod->visVersion );
			EXPECT_FALSE( gmod->items.empty() ) << "Gmod items empty for version: " << version;
			EXPECT_FALSE( gmod->relations.empty() ) << "Gmod relations empty for version: " << version;
		}
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

	TEST( EmbeddedResourceTest, Codebooks_AllVersionsLoad )
	{
		auto allVersions = EmbeddedResource::visVersions();

		for ( const auto& version : allVersions )
		{
			auto codebooks = EmbeddedResource::codebooks( version );

			ASSERT_TRUE( codebooks.has_value() ) << "Failed to load Codebooks for version: " << version;
			EXPECT_EQ( version, codebooks->visVersion );
			EXPECT_FALSE( codebooks->items.empty() ) << "Codebooks items empty for version: " << version;

			// Validate first codebook structure
			const auto& firstCodebook = codebooks->items[0];
			EXPECT_FALSE( firstCodebook.name.empty() ) << "Codebook name empty for version: " << version;
			EXPECT_FALSE( firstCodebook.values.empty() ) << "Codebook values empty for version: " << version;
		}
	}

	TEST( EmbeddedResourceTest, Codebooks_EqualityOperators )
	{
		// Load same version twice
		auto codebooks1 = EmbeddedResource::codebooks( "3-9a" );
		auto codebooks2 = EmbeddedResource::codebooks( "3-9a" );

		ASSERT_TRUE( codebooks1.has_value() );
		ASSERT_TRUE( codebooks2.has_value() );

		// Test CodebooksDto equality
		EXPECT_EQ( *codebooks1, *codebooks2 );
		EXPECT_FALSE( *codebooks1 != *codebooks2 );

		// Test individual CodebookDto equality
		if ( !codebooks1->items.empty() && !codebooks2->items.empty() )
		{
			EXPECT_EQ( codebooks1->items[0], codebooks2->items[0] );
			EXPECT_FALSE( codebooks1->items[0] != codebooks2->items[0] );
		}

		// Load different version
		auto codebooks3 = EmbeddedResource::codebooks( "3-4a" );
		ASSERT_TRUE( codebooks3.has_value() );

		EXPECT_NE( *codebooks1, *codebooks3 );
		EXPECT_FALSE( *codebooks1 == *codebooks3 );

		// Test inequality
		EXPECT_NE( codebooks1->visVersion, codebooks3->visVersion );
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

	TEST( EmbeddedResourceTest, Locations_AllVersionsLoad )
	{
		auto allVersions = EmbeddedResource::visVersions();

		for ( const auto& version : allVersions )
		{
			auto locations = EmbeddedResource::locations( version );

			ASSERT_TRUE( locations.has_value() ) << "Failed to load Locations for version: " << version;
			EXPECT_EQ( version, locations->visVersion );
			EXPECT_FALSE( locations->items.empty() ) << "Locations items empty for version: " << version;

			// Validate first location structure
			const auto& firstLocation = locations->items[0];
			EXPECT_NE( '\0', firstLocation.code ) << "Location code empty for version: " << version;
			EXPECT_FALSE( firstLocation.name.empty() ) << "Location name empty for version: " << version;
		}
	}

	TEST( EmbeddedResourceTest, Locations_EqualityOperators )
	{
		// Load same version twice
		auto locations1 = EmbeddedResource::locations( "3-9a" );
		auto locations2 = EmbeddedResource::locations( "3-9a" );

		ASSERT_TRUE( locations1.has_value() );
		ASSERT_TRUE( locations2.has_value() );

		// Test LocationsDto equality
		EXPECT_EQ( *locations1, *locations2 );
		EXPECT_FALSE( *locations1 != *locations2 );

		// Test individual RelativeLocationsDto equality
		if ( !locations1->items.empty() && !locations2->items.empty() )
		{
			EXPECT_EQ( locations1->items[0], locations2->items[0] );
			EXPECT_FALSE( locations1->items[0] != locations2->items[0] );
		}

		// Load different version
		auto locations3 = EmbeddedResource::locations( "3-4a" );
		ASSERT_TRUE( locations3.has_value() );

		EXPECT_NE( *locations1, *locations3 );
		EXPECT_FALSE( *locations1 == *locations3 );
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

	TEST( EmbeddedResourceTest, GmodVersioning_EqualityOperators )
	{
		auto versioningMap = EmbeddedResource::gmodVersioning();

		ASSERT_TRUE( versioningMap.has_value() );
		ASSERT_FALSE( versioningMap->empty() );

		// Get two versions to compare
		auto it35 = versioningMap->find( "3-5a" );
		auto it35_2 = versioningMap->find( "3-5a" );
		auto it39 = versioningMap->find( "3-9a" );

		ASSERT_TRUE( it35 != versioningMap->end() );
		ASSERT_TRUE( it39 != versioningMap->end() );

		// Test GmodVersioningDto equality
		EXPECT_EQ( it35->second, it35_2->second );
		EXPECT_FALSE( it35->second != it35_2->second );

		// Test GmodVersioningDto inequality
		EXPECT_NE( it35->second, it39->second );
		EXPECT_FALSE( it35->second == it39->second );

		// Test individual GmodNodeConversionDto equality if items exist
		if ( !it35->second.items.empty() )
		{
			const auto& firstItem = it35->second.items.begin()->second;
			EXPECT_EQ( firstItem, firstItem );
			EXPECT_FALSE( firstItem != firstItem );
		}
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

	TEST( EmbeddedResourceTest, DataChannelTypeNames_EqualityOperators )
	{
		// Load same version twice
		auto dataChannelTypeNames1 = EmbeddedResource::dataChannelTypeNames( "v2024" );
		auto dataChannelTypeNames2 = EmbeddedResource::dataChannelTypeNames( "v2024" );

		ASSERT_TRUE( dataChannelTypeNames1.has_value() );
		ASSERT_TRUE( dataChannelTypeNames2.has_value() );

		// Test DataChannelTypeNamesDto equality
		EXPECT_EQ( *dataChannelTypeNames1, *dataChannelTypeNames2 );
		EXPECT_FALSE( *dataChannelTypeNames1 != *dataChannelTypeNames2 );

		// Test individual DataChannelTypeNameDto equality
		if ( !dataChannelTypeNames1->values.empty() && !dataChannelTypeNames2->values.empty() )
		{
			EXPECT_EQ( dataChannelTypeNames1->values[0], dataChannelTypeNames2->values[0] );
			EXPECT_FALSE( dataChannelTypeNames1->values[0] != dataChannelTypeNames2->values[0] );
		}

		// Load different version
		auto dataChannelTypeNames3 = EmbeddedResource::dataChannelTypeNames( "v2018" );
		ASSERT_TRUE( dataChannelTypeNames3.has_value() );

		EXPECT_NE( *dataChannelTypeNames1, *dataChannelTypeNames3 );
		EXPECT_FALSE( *dataChannelTypeNames1 == *dataChannelTypeNames3 );
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

	TEST( EmbeddedResourceTest, FormatDataTypes_EqualityOperators )
	{
		// Load same version twice
		auto formatDataTypes1 = EmbeddedResource::formatDataTypes( "v2024" );
		auto formatDataTypes2 = EmbeddedResource::formatDataTypes( "v2024" );

		ASSERT_TRUE( formatDataTypes1.has_value() );
		ASSERT_TRUE( formatDataTypes2.has_value() );

		// Test FormatDataTypesDto equality
		EXPECT_EQ( *formatDataTypes1, *formatDataTypes2 );
		EXPECT_FALSE( *formatDataTypes1 != *formatDataTypes2 );

		// Test individual FormatDataTypeDto equality
		if ( !formatDataTypes1->values.empty() && !formatDataTypes2->values.empty() )
		{
			EXPECT_EQ( formatDataTypes1->values[0], formatDataTypes2->values[0] );
			EXPECT_FALSE( formatDataTypes1->values[0] != formatDataTypes2->values[0] );
		}

		// Load different version
		auto formatDataTypes3 = EmbeddedResource::formatDataTypes( "v2018" );
		ASSERT_TRUE( formatDataTypes3.has_value() );

		// iso19848-v2018-format-data-types.json.gz and iso19848-v2024-format-data-types.json.gz are identical
		EXPECT_EQ( *formatDataTypes1, *formatDataTypes3 );
		EXPECT_TRUE( *formatDataTypes1 == *formatDataTypes3 );
	}
} // namespace dnv::vista::sdk::test
