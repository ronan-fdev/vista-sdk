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
 * @file Sample_Codebooks.cpp
 * @brief Demonstrates usage of vista-sdk-cpp Codebooks API
 * @details This sample shows how to use VIS singleton, access codebooks,
 *          validate values, create metadata tags, and work with VIS versions
 */

#include <dnv/vista/sdk/VIS.h>

#include <iomanip>
#include <iostream>
#include <string>

int main()
{
	using namespace dnv::vista::sdk;

	std::cout << "=== vista-sdk-cpp Codebooks Sample ===\n\n";

	//=====================================================================
	// 1. VIS Singleton: Version management
	//=====================================================================
	{
		std::cout << "1. VIS Singleton: Version management\n";
		std::cout << "---------------------------------------\n";

		const auto& vis = VIS::instance();
		auto latest = vis.latest();
		const auto& allVersions = vis.versions();

		std::cout << "Latest VIS version: " << VisVersions::toString( latest ) << "\n";
		std::cout << "Available versions (" << allVersions.size() << "):\n";
		for ( const auto& version : allVersions )
		{
			std::cout << "  - " << VisVersions::toString( version ) << "\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 2. Codebooks: Accessing codebooks for a version
	//=====================================================================
	{
		std::cout << "2. Codebooks: Accessing codebooks for a version\n";
		std::cout << "--------------------------------------------------\n";

		const auto& vis = VIS::instance();
		auto version = vis.latest();
		const auto& codebooks = vis.codebooks( version );

		std::cout << "Codebooks for version: " << VisVersions::toString( codebooks.version() ) << "\n";

		// Access individual codebooks
		const auto& quantity = codebooks[CodebookName::Quantity];
		const auto& position = codebooks[CodebookName::Position];
		const auto& state = codebooks[CodebookName::State];

		std::cout << "Quantity codebook has " << quantity.standardValues().size() << " standard values\n";
		std::cout << "Position codebook has " << position.standardValues().size() << " standard values\n";
		std::cout << "State codebook has " << state.standardValues().size() << " standard values\n";
		std::cout << "\n";
	}

	//=====================================================================
	// 3. Codebook: Checking standard values and groups
	//=====================================================================
	{
		std::cout << "3. Codebook: Checking standard values and groups\n";
		std::cout << "---------------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& codebooks = vis.codebooks( vis.latest() );
		const auto& quantity = codebooks[CodebookName::Quantity];

		// Check standard values
		std::cout << std::boolalpha;
		std::cout << "Has 'temperature'? " << quantity.hasStandardValue( "temperature" ) << "\n";
		std::cout << "Has 'pressure'? " << quantity.hasStandardValue( "pressure" ) << "\n";
		std::cout << "Has 'invalid_qty'? " << quantity.hasStandardValue( "invalid_qty" ) << "\n";

		// Check groups
		std::cout << "\nGroups in Quantity codebook: " << quantity.groups().size() << "\n";
		for ( const auto& group : quantity.groups() )
		{
			std::cout << "  - " << group << "\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 4. Position Codebook: Special numeric value handling
	//=====================================================================
	{
		std::cout << "4. Position Codebook: Special numeric value handling\n";
		std::cout << "-------------------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& codebooks = vis.codebooks( vis.latest() );
		const auto& position = codebooks[CodebookName::Position];

		// Numbers are always valid for Position
		std::cout << std::boolalpha;
		std::cout << "Has '1'? " << position.hasStandardValue( "1" ) << "\n";
		std::cout << "Has '42'? " << position.hasStandardValue( "42" ) << "\n";
		std::cout << "Has '1234'? " << position.hasStandardValue( "1234" ) << "\n";

		// Standard position values
		std::cout << "Has 'centre'? " << position.hasStandardValue( "centre" ) << "\n";
		std::cout << "Has 'port'? " << position.hasStandardValue( "port" ) << "\n";
		std::cout << "Has 'starboard'? " << position.hasStandardValue( "starboard" ) << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 5. MetadataTag: Creating tags from standard values
	//=====================================================================
	{
		std::cout << "5. MetadataTag: Creating tags from standard values\n";
		std::cout << "-----------------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& codebooks = vis.codebooks( vis.latest() );
		const auto& quantity = codebooks[CodebookName::Quantity];

		// Create standard tags
		auto tempTag = quantity.createTag( "temperature" );
		auto pressTag = quantity.createTag( "pressure" );

		if ( tempTag.has_value() )
		{
			std::cout << "Temperature tag created:\n";
			std::cout << "  Name: " << CodebookNames::toPrefix( tempTag->name() ) << "\n";
			std::cout << "  Value: " << tempTag->value() << "\n";
			std::cout << "  Prefix: " << tempTag->prefix() << "\n";
			std::cout << "  Is Custom: " << std::boolalpha << tempTag->isCustom() << "\n";

			auto sb = StringBuilder( 128 );
			tempTag->toString( sb );
			std::cout << "  String repr: " << sb.toString() << "\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 6. MetadataTag: Creating custom tags
	//=====================================================================
	{
		std::cout << "6. MetadataTag: Creating custom tags\n";
		std::cout << "---------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& codebooks = vis.codebooks( vis.latest() );
		const auto& quantity = codebooks[CodebookName::Quantity];

		// Try to create tag with non-standard value
		auto customTag = quantity.createTag( "custom_measurement" );

		if ( customTag.has_value() )
		{
			std::cout << "Custom tag created:\n";
			std::cout << "  Name: " << CodebookNames::toPrefix( customTag->name() ) << "\n";
			std::cout << "  Value: " << customTag->value() << "\n";
			std::cout << "  Prefix: " << customTag->prefix() << " (note: '~' for custom)\n";
			std::cout << "  Is Custom: " << std::boolalpha << customTag->isCustom() << "\n";

			auto sb = StringBuilder( 128 );
			customTag->toString( sb );
			std::cout << "  String repr: " << sb.toString() << "\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 7. Position Validation
	//=====================================================================
	{
		std::cout << "7. Position Validation\n";
		std::cout << "---------------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& codebooks = vis.codebooks( vis.latest() );
		const auto& position = codebooks[CodebookName::Position];

		// Test various position strings
		std::vector<std::string> testPositions = {
			"centre",
			"port",
			"1",
			"42",
			"centre-starboard",
			"port-1",
			"centre-starboard-2",
			"invalid position!",
			"starboard-centre", // Wrong order
		};

		std::cout << "Position validation results:\n";
		for ( const auto& pos : testPositions )
		{
			auto result = position.validatePosition( pos );
			std::cout << "  \"" << pos << "\": ";

			switch ( result )
			{
				case PositionValidationResult::Valid:
					std::cout << "Valid";
					break;
				case PositionValidationResult::Custom:
					std::cout << "Custom";
					break;
				case PositionValidationResult::Invalid:
					std::cout << "Invalid";
					break;
				case PositionValidationResult::InvalidOrder:
					std::cout << "Invalid (wrong order)";
					break;
				case PositionValidationResult::InvalidGrouping:
					std::cout << "Invalid (duplicate groups)";
					break;
			}
			std::cout << "\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 8. CodebookName: String conversions
	//=====================================================================
	{
		std::cout << "8. CodebookName: String conversions\n";
		std::cout << "--------------------------------------\n";

		// Convert enum to prefix
		std::cout << "Enum to prefix conversions:\n";
		std::cout << "  Quantity -> \"" << CodebookNames::toPrefix( CodebookName::Quantity ) << "\"\n";
		std::cout << "  Position -> \"" << CodebookNames::toPrefix( CodebookName::Position ) << "\"\n";
		std::cout << "  State -> \"" << CodebookNames::toPrefix( CodebookName::State ) << "\"\n";
		std::cout << "  Command -> \"" << CodebookNames::toPrefix( CodebookName::Command ) << "\"\n";
		std::cout << "  Detail -> \"" << CodebookNames::toPrefix( CodebookName::Detail ) << "\"\n";

		// Convert prefix to enum
		std::cout << "\nPrefix to enum conversions:\n";
		auto qtyEnum = CodebookNames::fromPrefix( "qty" );
		auto posEnum = CodebookNames::fromPrefix( "pos" );
		std::cout << "  \"qty\" -> CodebookName::" << ( qtyEnum == CodebookName::Quantity ? "Quantity" : "?" ) << "\n";
		std::cout << "  \"pos\" -> CodebookName::" << ( posEnum == CodebookName::Position ? "Position" : "?" ) << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 9. Invalid operations
	//=====================================================================
	{
		std::cout << "9. Invalid operations\n";
		std::cout << "------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& codebooks = vis.codebooks( vis.latest() );
		const auto& quantity = codebooks[CodebookName::Quantity];

		// Try to create tag with empty/whitespace values
		auto emptyTag = quantity.createTag( "" );
		auto whitespaceTag = quantity.createTag( "   " );

		std::cout << std::boolalpha;
		std::cout << "Empty string tag created? " << emptyTag.has_value() << "\n";
		std::cout << "Whitespace tag created? " << whitespaceTag.has_value() << "\n";

		// Try invalid prefix conversion
		try
		{
			[[maybe_unused]] auto invalid = CodebookNames::fromPrefix( "invalid_prefix" );
			std::cout << "Invalid prefix accepted (unexpected)\n";
		}
		catch ( const std::invalid_argument& e )
		{
			std::cout << "Invalid prefix rejected: " << e.what() << "\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 10. Building a complete metadata path
	//=====================================================================
	{
		std::cout << "10. Building a complete metadata path\n";
		std::cout << "-----------------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& codebooks = vis.codebooks( vis.latest() );

		// Create multiple tags
		auto qtyTag = codebooks[CodebookName::Quantity].createTag( "temperature" );
		auto cntTag = codebooks[CodebookName::Content].createTag( "oil" );
		auto posTag = codebooks[CodebookName::Position].createTag( "1" );
		auto stateTag = codebooks[CodebookName::State].createTag( "running" );

		if ( qtyTag && cntTag && posTag && stateTag )
		{
			auto sb = StringBuilder( 128 );

			std::cout << "Building metadata path:\n";

			qtyTag->toString( sb );
			std::cout << "  After Quantity: " << sb.toString() << "\n";

			cntTag->toString( sb );
			std::cout << "  After Content: " << sb.toString() << "\n";

			posTag->toString( sb );
			std::cout << "  After Position: " << sb.toString() << "\n";

			stateTag->toString( sb );
			std::cout << "  After State: " << sb.toString() << "\n";

			std::cout << "\nComplete path: " << sb.toString() << "\n";
		}

		std::cout << "\n";
	}

	return 0;
}
