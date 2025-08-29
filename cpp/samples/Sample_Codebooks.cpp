/**
 * @file Sample_Codebooks.cpp
 * @brief Codebooks and Metadata Tags - Vista SDK C++
 *
 * This example demonstrates working with codebooks and metadata tags:
 * - Loading and exploring codebooks
 * - Creating standard and custom metadata tags
 * - Validating tag values
 * - Working with different tag types
 *
 * For more examples, see the other files in this directory.
 */

#include "dnv/vista/sdk/Codebook.h"
#include "dnv/vista/sdk/Codebooks.h"
#include "dnv/vista/sdk/CodebookName.h"
#include "dnv/vista/sdk/MetadataTag.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISVersion.h"

using namespace dnv::vista::sdk;

//=====================================================================
// Application entry point
//=====================================================================

/**
 * @brief Demonstrate codebook operations
 */
int main()
{
	try
	{
		std::cout << "=== Codebooks and Metadata Tags Example ===\n\n";

		auto& vis = VIS::instance();
		auto version = VisVersion::v3_4a;
		const auto& codebooks = vis.codebooks( version );

		std::cout << "1. Available Codebooks...\n";

		std::vector<CodebookName> availableCodebooks = {
			CodebookName::Quantity,
			CodebookName::Content,
			CodebookName::Position,
			CodebookName::State,
			CodebookName::Command,
			CodebookName::Type,
			CodebookName::Detail };

		for ( const auto& codebookName : availableCodebooks )
		{
			const auto& codebook = codebooks[codebookName];

			size_t standardValueCount = 0;
			auto standardValues = codebook.standardValues();
			for ( auto it = standardValues.begin(); it != standardValues.end(); ++it )
			{
				++standardValueCount;
			}

			size_t groupCount = 0;
			auto groups = codebook.groups();
			for ( auto it = groups.begin(); it != groups.end(); ++it )
			{
				++groupCount;
			}

			std::cout << "\t" << CodebookNames::toPrefix( codebookName ) << ":\n";
			std::cout << "\t\t→ Standard values: " << standardValueCount << "\n";
			std::cout << "\t\t→ Groups: " << groupCount << "\n";
			std::cout << "\n";
		}

		std::cout << "2. Creating Standard Metadata Tags...\n";

		std::map<std::string, std::vector<std::string>> standardTags{
			{ "quantity", { "temperature", "pressure", "flow" } },
			{ "position", { "centre", "inlet", "outlet" } },
			{ "state", { "opened", "closed", "high", "low" } },
			{ "content", { "cooling.water", "exhaust.gas", "fuel.oil" } } };

		for ( const auto& [tagType, values] : standardTags )
		{
			std::cout << "\t" << tagType << " tags:\n";

			CodebookName codebookName = CodebookName::Quantity; // Default initialization
			if ( tagType == "quantity" )
			{
				codebookName = CodebookName::Quantity;
			}
			else if ( tagType == "position" )
			{
				codebookName = CodebookName::Position;
			}
			else if ( tagType == "state" )
			{
				codebookName = CodebookName::State;
			}
			else if ( tagType == "content" )
			{
				codebookName = CodebookName::Content;
			}

			const auto& codebook = codebooks[codebookName];

			for ( const auto& value : values )
			{
				try
				{
					auto tag = codebook.createTag( value );
					std::cout << "\t\t✓ " << value << ": " << tag.value()
							  << " (custom: " << ( tag.isCustom() ? "true" : "false" ) << ")\n";
				}
				catch ( const std::exception& e )
				{
					std::cout << "\t\t✗ " << value << ": Failed - " << e.what() << "\n";
				}
			}
			std::cout << "\n";
		}

		std::cout << "3. Working with Custom Tags...\n";

		std::vector<std::pair<CodebookName, std::string>> customExamples = {
			{ CodebookName::Quantity, "custom_temperature" },
			{ CodebookName::Position, "custom_location" },
			{ CodebookName::Content, "special.fluid" },
			{ CodebookName::State, "partially_open" } };

		for ( const auto& [codebookName, customValue] : customExamples )
		{
			const auto& codebook = codebooks[codebookName];
			try
			{
				auto customTag = codebook.tryCreateTag( customValue );
				if ( customTag.has_value() )
				{
					std::cout << "\t✓ " << CodebookNames::toPrefix( codebookName )
							  << ": " << customTag->value()
							  << " (custom: " << ( customTag->isCustom() ? "true" : "false" ) << ")\n";
				}
				else
				{
					std::cout << "\t✗ " << CodebookNames::toPrefix( codebookName )
							  << ": Failed to create tag for '" << customValue << "'\n";
				}
			}
			catch ( const std::exception& e )
			{
				std::cout << "\t✗ " << CodebookNames::toPrefix( codebookName )
						  << ": Exception - " << e.what() << "\n";
			}
		}

		std::cout << "\n4. Tag Validation...\n";

		std::vector<std::tuple<CodebookName, std::string, bool>> validationTests = {
			{ CodebookName::Position, "centre", true },
			{ CodebookName::Position, "invalid_position", false },
			{ CodebookName::Quantity, "temperature", true },
			{ CodebookName::Quantity, "nonexistent_quantity", false },
			{ CodebookName::State, "opened", true },
			{ CodebookName::State, "maybe_opened", false } };

		for ( const auto& [codebookName, testValue, expectedValid] : validationTests )
		{
			const auto& codebook = codebooks[codebookName];
			bool isValid = codebook.hasStandardValue( testValue );
			std::string status = ( isValid == expectedValid ) ? "✓" : "✗";

			std::cout << "\t" << status << " " << CodebookNames::toPrefix( codebookName )
					  << "." << testValue << ": valid=" << ( isValid ? "true" : "false" )
					  << " (expected: " << ( expectedValid ? "true" : "false" ) << ")\n";
		}

		std::cout << "\n5. Position Validation (Special Rules)...\n";

		const auto& positionCodebook = codebooks[CodebookName::Position];
		std::vector<std::string> positionTests = {
			"centre",
			"centre-1",
			"1-centre",		  // Invalid order
			"port-starboard", // Invalid grouping
			"phase.w.u",
			"outside-phase.w.u-10",
			"custom_position" };

		for ( const auto& position : positionTests )
		{
			try
			{
				auto validationResult = positionCodebook.validatePosition( position );
				std::string resultString;
				switch ( validationResult )
				{
					case PositionValidationResult::Invalid:
					{
						resultString = "Invalid";
						break;
					}
					case PositionValidationResult::InvalidOrder:
					{
						resultString = "InvalidOrder";
						break;
					}
					case PositionValidationResult::InvalidGrouping:
					{
						resultString = "InvalidGrouping";
						break;
					}
					case PositionValidationResult::Valid:
					{
						resultString = "Valid";
						break;
					}
					case PositionValidationResult::Custom:
					{
						resultString = "Custom";
						break;
					}
				}
				std::cout << "\tPosition '" << position << "': " << resultString << "\n";
			}
			catch ( const std::exception& e )
			{
				std::cout << "\tPosition '" << position << "': Error - " << e.what() << "\n";
			}
		}

		std::cout << "\n6. Exploring Codebook Content...\n";

		const auto& quantityCodebook = codebooks[CodebookName::Quantity];
		std::cout << "\tQuantity codebook sample values:\n";

		auto standardValues = quantityCodebook.standardValues();
		size_t count = 0;
		for ( auto it = standardValues.begin(); it != standardValues.end() && count < 10; ++it, ++count )
		{
			std::cout << "\t\t- " << *it << "\n";
		}

		std::cout << "\tQuantity codebook sample groups:\n";
		auto groups = quantityCodebook.groups();
		count = 0;
		for ( auto it = groups.begin(); it != groups.end() && count < 5; ++it, ++count )
		{
			std::cout << "\t\t- " << *it << "\n";
		}

		std::cout << "\n=== Codebook operations completed! ===\n";
		return 0;
	}
	catch ( const std::exception& ex )
	{
		std::cerr << "Error: " << ex.what() << "\n";
		return 1;
	}
}
