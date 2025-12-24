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
 * @file Sample_LocalId.cpp
 * @brief Demonstrates usage of vista-sdk-cpp LocalId and LocalIdBuilder APIs
 * @details This sample shows how to build LocalIds, parse LocalId strings,
 *          work with metadata tags, use verbose mode, and handle MQTT formatting
 */

#include <dnv/vista/sdk/VIS.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main()
{
	using namespace dnv::vista::sdk;

	std::cout << "=== vista-sdk-cpp LocalId Sample ===\n\n";

	//=====================================================================
	// 1. LocalIdBuilder: Building a simple LocalId
	//=====================================================================
	{
		std::cout << "1. LocalIdBuilder: Building a simple LocalId\n";
		std::cout << "-----------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );
		const auto& locations = vis.locations( vis.latest() );
		const auto& codebooks = vis.codebooks( vis.latest() );

		// Parse a GMOD path for the primary item
		auto primaryItem = GmodPath::fromString( "411.1/C101.31-2", gmod, locations );

		// Create a metadata tag
		auto quantityTag = codebooks[CodebookName::Quantity].createTag( "temperature" );

		// Build the LocalId
		auto localId = LocalIdBuilder::create( vis.latest() )
						   .withPrimaryItem( *primaryItem )
						   .withMetadataTag( *quantityTag )
						   .build();

		std::cout << "Built LocalId: " << localId.toString() << "\n";
		std::cout << "  VIS Version: " << VisVersions::toString( localId.version() ) << "\n";
		std::cout << "  Primary Item: " << localId.primaryItem().toString() << "\n";
		std::cout << "  Quantity: " << localId.quantity()->value() << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 2. LocalIdBuilder: Building with multiple metadata tags
	//=====================================================================
	{
		std::cout << "2. LocalIdBuilder: Building with multiple metadata tags\n";
		std::cout << "----------------------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );
		const auto& locations = vis.locations( vis.latest() );
		const auto& codebooks = vis.codebooks( vis.latest() );

		auto primaryItem = GmodPath::fromString( "621.21/S90", gmod, locations );
		auto qtyTag = codebooks[CodebookName::Quantity].createTag( "mass" );
		auto cntTag = codebooks[CodebookName::Content].createTag( "fuel.oil" );
		auto posTag = codebooks[CodebookName::Position].createTag( "inlet" );

		auto localId = LocalIdBuilder::create( vis.latest() )
						   .withPrimaryItem( *primaryItem )
						   .withMetadataTag( *qtyTag )
						   .withMetadataTag( *cntTag )
						   .withMetadataTag( *posTag )
						   .build();

		std::cout << "Built LocalId: " << localId.toString() << "\n";
		std::cout << "  Metadata tags:\n";
		std::cout << "    Quantity: " << localId.quantity()->value() << "\n";
		std::cout << "    Content: " << localId.content()->value() << "\n";
		std::cout << "    Position: " << localId.position()->value() << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 3. LocalIdBuilder: Building with secondary item
	//=====================================================================
	{
		std::cout << "3. LocalIdBuilder: Building with secondary item\n";
		std::cout << "--------------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );
		const auto& locations = vis.locations( vis.latest() );
		const auto& codebooks = vis.codebooks( vis.latest() );

		auto primaryItem = GmodPath::fromString( "621.21/S90", gmod, locations );
		auto secondaryItem = GmodPath::fromString( "411.1/C101", gmod, locations );
		auto qtyTag = codebooks[CodebookName::Quantity].createTag( "mass" );
		auto cntTag = codebooks[CodebookName::Content].createTag( "fuel.oil" );
		auto posTag = codebooks[CodebookName::Position].createTag( "inlet" );

		auto localId = LocalIdBuilder::create( vis.latest() )
						   .withPrimaryItem( *primaryItem )
						   .withSecondaryItem( *secondaryItem )
						   .withMetadataTag( *qtyTag )
						   .withMetadataTag( *cntTag )
						   .withMetadataTag( *posTag )
						   .build();

		std::cout << "Built LocalId: " << localId.toString() << "\n";
		std::cout << "  Primary item: " << localId.primaryItem().toString() << "\n";
		std::cout << "  Secondary item: " << localId.secondaryItem()->toString() << "\n";
		std::cout << "  Has secondary item: " << std::boolalpha << localId.secondaryItem().has_value() << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 4. LocalId: Parsing from string
	//=====================================================================
	{
		std::cout << "4. LocalId: Parsing from string\n";
		std::cout << "---------------------------------\n";

		std::string localIdStr = "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature";

		// Parse without error details
		auto localId = LocalId::fromString( localIdStr );

		if ( localId.has_value() )
		{
			std::cout << "Parsed successfully: " << localId->toString() << "\n";
			std::cout << "  VIS Version: " << VisVersions::toString( localId->version() ) << "\n";
			std::cout << "  Primary item: " << localId->primaryItem().toString() << "\n";
			std::cout << "  Quantity: " << localId->quantity()->value() << "\n";
		}
		else
		{
			std::cout << "Parse failed\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 5. LocalId: Parsing with error handling
	//=====================================================================
	{
		std::cout << "5. LocalId: Parsing with error handling\n";
		std::cout << "------------------------------------------\n";

		std::vector<std::string> testStrings = {
			"/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature",
			"/dnv-v2/vis-3-4a/621.21/S90/sec/411.1/C101/meta/qty-mass/cnt-fuel.oil/pos-inlet",
			"/dnv-v2/vis-3-7a/612.21/C701.23/C633/meta/calc~accumulate",
			"",
			"/dnv-v2/INVALID/411.1/meta/qty-temperature",
			"/dnv-v2/vis-3-4a/INVALID/meta/qty-temperature",
			"/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-invalid_quantity" };

		for ( const auto& str : testStrings )
		{
			ParsingErrors errors;
			auto localId = LocalId::fromString( str, errors );

			std::cout << "Parsing: \"" << ( str.empty() ? "(empty)" : str ) << "\"\n";

			if ( localId.has_value() )
			{
				std::cout << "  Success: " << localId->toString() << "\n";
			}
			else
			{
				std::cout << "  Failed:\n";
				if ( errors.hasErrors() )
				{
					for ( const auto& [type, message] : errors )
					{
						std::cout << "    - " << message << "\n";
					}
				}
			}
			std::cout << "\n";
		}
	}

	//=====================================================================
	// 6. LocalIdBuilder: Modifying existing LocalIds
	//=====================================================================
	{
		std::cout << "6. LocalIdBuilder: Modifying existing LocalIds\n";
		std::cout << "------------------------------------------------\n";

		auto original = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature" );

		if ( original.has_value() )
		{
			std::cout << "Original: " << original->toString() << "\n";

			const auto& vis = VIS::instance();
			const auto& codebooks = vis.codebooks( vis.latest() );

			// Add content tag
			auto cntTag = codebooks[CodebookName::Content].createTag( "water" );
			auto modified = original->builder()
								.withMetadataTag( *cntTag )
								.build();

			std::cout << "Modified (added content): " << modified.toString() << "\n";

			// Remove quantity, add position
			auto posTag = codebooks[CodebookName::Position].createTag( "outlet" );
			auto modified2 = original->builder()
								 .withoutMetadataTag( CodebookName::Quantity )
								 .withMetadataTag( *posTag )
								 .build();

			std::cout << "Modified (removed qty, added pos): " << modified2.toString() << "\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 7. LocalIdBuilder: Verbose mode
	//=====================================================================
	{
		std::cout << "7. LocalIdBuilder: Verbose mode\n";
		std::cout << "----------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );
		const auto& locations = vis.locations( vis.latest() );
		const auto& codebooks = vis.codebooks( vis.latest() );

		auto primaryItem = GmodPath::fromString( "411.1/C101.31-2", gmod, locations );
		auto qtyTag = codebooks[CodebookName::Quantity].createTag( "temperature" );

		// Build without verbose mode
		auto normalLocalId = LocalIdBuilder::create( vis.latest() )
								 .withPrimaryItem( *primaryItem )
								 .withMetadataTag( *qtyTag )
								 .build();

		std::cout << "Normal mode: " << normalLocalId.toString() << "\n";

		// Build with verbose mode
		auto verboseLocalId = LocalIdBuilder::create( vis.latest() )
								  .withPrimaryItem( *primaryItem )
								  .withMetadataTag( *qtyTag )
								  .withVerboseMode( true )
								  .build();

		std::cout << "Verbose mode: " << verboseLocalId.toString() << "\n";
		std::cout << "  Includes human-readable common names for better understanding\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 8. LocalId: Accessing components
	//=====================================================================
	{
		std::cout << "8. LocalId: Accessing components\n";
		std::cout << "----------------------------------\n";

		auto localId = LocalId::fromString( "/dnv-v2/vis-3-4a/621.21/S90/sec/411.1/C101/meta/qty-mass/cnt-fuel.oil/pos-inlet" );

		if ( localId.has_value() )
		{
			std::cout << "LocalId: " << localId->toString() << "\n\n";

			std::cout << "Components:\n";
			std::cout << "  VIS Version: " << VisVersions::toString( localId->version() ) << "\n";
			std::cout << "  Primary item: " << localId->primaryItem().toString() << "\n";

			if ( localId->secondaryItem().has_value() )
			{
				std::cout << "  Secondary item: " << localId->secondaryItem()->toString() << "\n";
			}

			std::cout << "\nMetadata tags:\n";
			if ( localId->quantity().has_value() )
				std::cout << "  Quantity: " << localId->quantity()->value() << "\n";
			if ( localId->content().has_value() )
				std::cout << "  Content: " << localId->content()->value() << "\n";
			if ( localId->calculation().has_value() )
				std::cout << "  Calculation: " << localId->calculation()->value() << "\n";
			if ( localId->state().has_value() )
				std::cout << "  State: " << localId->state()->value() << "\n";
			if ( localId->command().has_value() )
				std::cout << "  Command: " << localId->command()->value() << "\n";
			if ( localId->type().has_value() )
				std::cout << "  Type: " << localId->type()->value() << "\n";
			if ( localId->position().has_value() )
				std::cout << "  Position: " << localId->position()->value() << "\n";
			if ( localId->detail().has_value() )
				std::cout << "  Detail: " << localId->detail()->value() << "\n";

			std::cout << "\nAll metadata tags (" << localId->metadataTags().size() << "):\n";
			for ( const auto& tag : localId->metadataTags() )
			{
				std::cout << "  " << CodebookNames::toPrefix( tag.name() ) << "-" << tag.value()
						  << ( tag.isCustom() ? " (custom)" : "" ) << "\n";
			}
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 9. LocalId: Equality and comparison
	//=====================================================================
	{
		std::cout << "9. LocalId: Equality and comparison\n";
		std::cout << "-------------------------------------\n";

		auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature" );
		auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature" );
		auto localId3 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-pressure" );

		std::cout << std::boolalpha;
		if ( localId1.has_value() && localId2.has_value() && localId3.has_value() )
		{
			std::cout << "LocalId1: " << localId1->toString() << "\n";
			std::cout << "LocalId2: " << localId2->toString() << "\n";
			std::cout << "LocalId3: " << localId3->toString() << "\n\n";

			std::cout << "LocalId1 == LocalId2? " << ( *localId1 == *localId2 ) << "\n";
			std::cout << "LocalId1 == LocalId3? " << ( *localId1 == *localId3 ) << "\n";
			std::cout << "LocalId1 != LocalId3? " << ( *localId1 != *localId3 ) << "\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 10. mqtt::LocalId: MQTT-compatible formatting
	//=====================================================================
	{
		std::cout << "10. mqtt::LocalId: MQTT-compatible formatting\n";
		std::cout << "------------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );
		const auto& locations = vis.locations( vis.latest() );
		const auto& codebooks = vis.codebooks( vis.latest() );

		auto primaryItem = GmodPath::fromString( "411.1/C101.31-2", gmod, locations );
		auto qtyTag = codebooks[CodebookName::Quantity].createTag( "temperature" );

		auto builder = LocalIdBuilder::create( vis.latest() )
						   .withPrimaryItem( *primaryItem )
						   .withMetadataTag( *qtyTag );

		// Standard LocalId
		auto standardLocalId = builder.build();
		std::cout << "Standard LocalId: " << standardLocalId.toString() << "\n";

		// MQTT LocalId
		auto mqttLocalId = mqtt::LocalId{ builder };
		std::cout << "MQTT LocalId:     " << mqttLocalId.toString() << "\n";
		std::cout << "  Differences:\n";
		std::cout << "    - No leading '/'\n";
		std::cout << "    - Underscores instead of slashes in paths\n";
		std::cout << "    - No 'meta/' section\n";
		std::cout << "    - More compact for MQTT topics\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 11. LocalIdBuilder: Custom metadata tags
	//=====================================================================
	{
		std::cout << "11. LocalIdBuilder: Custom metadata tags\n";
		std::cout << "-------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );
		const auto& locations = vis.locations( vis.latest() );
		const auto& codebooks = vis.codebooks( vis.latest() );

		auto primaryItem = GmodPath::fromString( "411.1/C101.31-2", gmod, locations );

		// Create custom quantity tag
		auto customQtyTag = codebooks[CodebookName::Quantity].createTag( "my_custom_measurement" );

		auto localId = LocalIdBuilder::create( vis.latest() )
						   .withPrimaryItem( *primaryItem )
						   .withMetadataTag( *customQtyTag )
						   .build();

		std::cout << "LocalId with custom tag: " << localId.toString() << "\n";
		std::cout << "  Has custom tag: " << std::boolalpha << localId.hasCustomTag() << "\n";
		std::cout << "  Custom quantity: " << localId.quantity()->value() << "\n";
		std::cout << "  Is custom: " << localId.quantity()->isCustom() << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 12. LocalIdBuilder: Validation
	//=====================================================================
	{
		std::cout << "12. LocalIdBuilder: Validation\n";
		std::cout << "--------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );
		const auto& locations = vis.locations( vis.latest() );
		const auto& codebooks = vis.codebooks( vis.latest() );

		// Valid builder
		auto primaryItem = GmodPath::fromString( "411.1/C101.31-2", gmod, locations );
		auto qtyTag = codebooks[CodebookName::Quantity].createTag( "temperature" );

		auto validBuilder = LocalIdBuilder::create( vis.latest() )
								.withPrimaryItem( *primaryItem )
								.withMetadataTag( *qtyTag );

		std::cout << std::boolalpha;
		std::cout << "Valid builder:\n";
		std::cout << "  Is valid: " << validBuilder.isValid() << "\n";
		std::cout << "  Is empty: " << validBuilder.isEmpty() << "\n";

		// Empty builder
		auto emptyBuilder = LocalIdBuilder::create( vis.latest() );
		std::cout << "\nEmpty builder:\n";
		std::cout << "  Is valid: " << emptyBuilder.isValid() << "\n";
		std::cout << "  Is empty: " << emptyBuilder.isEmpty() << "\n";

		// Builder missing metadata
		auto incompleteBuilder = LocalIdBuilder::create( vis.latest() )
									 .withPrimaryItem( *primaryItem );
		std::cout << "\nBuilder with primary item but no metadata:\n";
		std::cout << "  Is valid: " << incompleteBuilder.isValid() << "\n";
		std::cout << "  Is empty: " << incompleteBuilder.isEmpty() << "\n";

		// Try to build invalid
		std::cout << "\nAttempting to build invalid LocalId:\n";
		try
		{
			auto invalid = incompleteBuilder.build();
			std::cout << "  ERROR: Invalid build succeeded (unexpected)\n";
		}
		catch ( const std::invalid_argument& e )
		{
			std::cout << "  Correctly rejected: " << e.what() << "\n";
		}

		std::cout << "\n";
	}

	return 0;
}
