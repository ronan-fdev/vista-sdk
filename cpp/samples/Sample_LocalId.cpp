/**
 * @file Sample_LocalId.cpp
 * @brief Advanced Local ID Operations - Vista SDK C++
 *
 * This example demonstrates advanced Local ID operations:
 * - Building complex Local IDs with multiple components
 * - Error handling during parsing
 * - Working with custom tags
 * - Local ID validation and inspection
 *
 * For more examples, see the other files in this directory.
 */

#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "dnv/vista/sdk/Codebooks.h"
#include "dnv/vista/sdk/Codebook.h"
#include "dnv/vista/sdk/CodebookName.h"
#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/GmodNode.h"
#include "dnv/vista/sdk/LocalId.h"
#include "dnv/vista/sdk/LocalIdBuilder.h"
#include "dnv/vista/sdk/MetadataTag.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISVersion.h"

using namespace dnv::vista::sdk;

/**
 * @brief Demonstrate advanced Local ID operations.
 */
int main()
{
	try
	{
		std::cout << "=== Advanced Local ID Operations ===\n\n";

		auto& vis = VIS::instance();
		auto version = VisVersion::v3_4a;
		const auto& codebooks = vis.codebooks( version );

		std::cout << "1. Building Complex Local IDs...\n";

		auto primaryPath = GmodPath::parse( "411.1/C101.31-2", version );
		auto secondaryPath = GmodPath::parse( "411.1/C101.63/S206", version );

		auto complexLocalId = LocalIdBuilder::create( version )
								  .withPrimaryItem( std::move( primaryPath ) )
								  .withSecondaryItem( std::move( secondaryPath ) )
								  .withMetadataTag( codebooks.createTag( CodebookName::Quantity, "temperature" ) )
								  .withMetadataTag( codebooks.createTag( CodebookName::Content, "exhaust.gas" ) )
								  .withMetadataTag( codebooks.createTag( CodebookName::State, "high" ) )
								  .withMetadataTag( codebooks.createTag( CodebookName::Position, "inlet" ) )
								  .build();

		std::cout << "\tComplex Local ID: " << complexLocalId.toString() << "\n";
		std::cout << "\tHas secondary item: " << ( complexLocalId.secondaryItem().has_value() ? "true" : "false" ) << "\n";
		std::cout << "\tNumber of metadata tags: " << complexLocalId.metadataTags().size() << "\n";

		std::cout << "\n2. Working with Custom Tags...\n";

		auto customQuantity = codebooks.tryCreateTag( CodebookName::Quantity, "custom_temperature" );
		auto customPosition = codebooks.tryCreateTag( CodebookName::Position, "custom_location" );

		if ( customQuantity.has_value() && customPosition.has_value() )
		{
			auto primaryPathCopy = GmodPath::parse( "411.1/C101.31-2", version );
			auto customLocalId = LocalIdBuilder::create( version )
									 .withPrimaryItem( std::move( primaryPathCopy ) )
									 .withMetadataTag( customQuantity.value() )
									 .withMetadataTag( customPosition.value() )
									 .build();

			std::cout << "\tCustom Local ID: " << customLocalId.toString() << "\n";
			std::cout << "\tQuantity tag is custom: " << ( customQuantity.value().isCustom() ? "true" : "false" ) << "\n";
			std::cout << "\tPosition tag is custom: " << ( customPosition.value().isCustom() ? "true" : "false" ) << "\n";
		}

		std::cout << "\n3. Error Handling and Validation...\n";

		std::vector<std::string> invalidLocalIds{
			"/dnv-v2/vis-3-4a/invalid-path/meta/qty-temperature",
			"/invalid-naming-rule/vis-3-4a/411.1/meta/qty-temperature",
			"/dnv-v2/vis-3-4a/411.1/meta/qty-invalid_quantity",
			"/dnv-v2/vis-3-4a/411.1/meta/qty-temperature/pos-1-centre" };

		for ( const auto& invalidId : invalidLocalIds )
		{
			std::cout << "\n\tTesting invalid Local ID: " << invalidId << "\n";

			try
			{
				auto localId = LocalId::parse( invalidId );
				std::cout << "\t\t⚠ Unexpectedly parsed successfully: " << localId.toString() << "\n";
			}
			catch ( const std::exception& e )
			{
				std::cout << "\t\t✗ Parsing failed as expected:\n"
						  << e.what() << "\n";
			}
		}

		std::cout << "\n4. Local ID Inspection and Analysis...\n";

		std::vector<std::string> validLocalIds{
			"/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
			"/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened",
			"/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage" };

		for ( const auto& localIdStr : validLocalIds )
		{
			std::cout << "\n\tAnalyzing: " << localIdStr << "\n";

			try
			{
				auto localId = LocalId::parse( localIdStr );

				std::cout << "\t\t✓ Successfully parsed and built\n";
				std::cout << "\t\t✓ VIS Version: " << VisVersionExtensions::toVersionString( localId.visVersion() ) << "\n";
				std::cout << "\t\t✓ Has custom tags: " << ( localId.hasCustomTag() ? "true" : "false" ) << "\n";

				if ( localId.primaryItem().has_value() )
				{
					std::cout << "\t\t✓ Primary item: " << localId.primaryItem().value().toString() << "\n";
				}

				if ( localId.secondaryItem().has_value() )
				{
					std::cout << "\t\t✓ Secondary item: " << localId.secondaryItem().value().toString() << "\n";
				}

				std::cout << "\t\t✓ Metadata tags:\n";
				for ( const auto& tag : localId.metadataTags() )
				{
					std::cout << "\t\t\t- " << CodebookNames::toPrefix( tag.name() ) << ": " << tag.value()
							  << " (custom: " << ( tag.isCustom() ? "true" : "false" ) << ")\n";
				}
			}
			catch ( const std::exception& e )
			{
				std::cout << "\t\t✗ Failed to parse: " << e.what() << "\n";
			}
		}

		std::cout << "\n5. Builder Pattern Variations...\n";

		auto builder = LocalIdBuilder::create( version );

		std::optional<GmodPath> pathForBuilder;
		bool success = GmodPath::tryParse( "411.1/C101.31", version, pathForBuilder );
		if ( success && pathForBuilder.has_value() )
		{
			builder = builder.withPrimaryItem( std::move( pathForBuilder.value() ) );
		}

		auto tempTag = codebooks.tryCreateTag( CodebookName::Quantity, "temperature" );
		if ( tempTag.has_value() )
		{
			builder = builder.withMetadataTag( tempTag.value() );
		}

		auto exhaustTag = codebooks.tryCreateTag( CodebookName::Content, "exhaust.gas" );
		if ( exhaustTag.has_value() )
		{
			builder = builder.withMetadataTag( exhaustTag.value() );
		}

		auto invalidTag = codebooks.tryCreateTag( CodebookName::Position, "invalid-position-format" );
		if ( invalidTag.has_value() )
		{
			builder = builder.withMetadataTag( invalidTag.value() );
		}

		try
		{
			auto safeLocalId = builder.build();
			std::cout << "\tSafe building result: " << safeLocalId.toString() << "\n";
		}
		catch ( const std::exception& e )
		{
			std::cout << "\tSafe building failed: " << e.what() << "\n";
		}

		std::cout << "\n6. Verbose Mode...\n";

		auto primaryPath2 = GmodPath::parse( "411.1/C101.31-2", version );
		auto verboseLocalId = LocalIdBuilder::create( version )
								  .withVerboseMode( true )
								  .withPrimaryItem( std::move( primaryPath2 ) )
								  .withMetadataTag( codebooks.createTag( CodebookName::Quantity, "temperature" ) )
								  .build();

		auto primaryPath3 = GmodPath::parse( "411.1/C101.31-2", version );
		auto regularLocalId = LocalIdBuilder::create( version )
								  .withVerboseMode( false )
								  .withPrimaryItem( std::move( primaryPath3 ) )
								  .withMetadataTag( codebooks.createTag( CodebookName::Quantity, "temperature" ) )
								  .build();

		std::cout << "\tVerbose mode: " << verboseLocalId.toString() << "\n";
		std::cout << "\tRegular mode: " << regularLocalId.toString() << "\n";

		std::cout << "\n7. Tag Manipulation...\n";

		auto primaryPath4 = GmodPath::parse( "411.1/C101.31-2", version );
		auto manipulationBuilder = LocalIdBuilder::create( version )
									   .withPrimaryItem( std::move( primaryPath4 ) )
									   .withMetadataTag( codebooks.createTag( CodebookName::Quantity, "temperature" ) )
									   .withMetadataTag( codebooks.createTag( CodebookName::Content, "cooling.water" ) )
									   .withMetadataTag( codebooks.createTag( CodebookName::Position, "centre" ) );

		std::cout << "\tOriginal with 3 tags: " << manipulationBuilder.build().toString() << "\n";

		auto modifiedBuilder = manipulationBuilder.withoutMetadataTag( CodebookName::Position );
		std::cout << "\tAfter removing position tag: " << modifiedBuilder.build().toString() << "\n";

		std::cout << "\n8. Different Naming Rules and Versions...\n";

		std::vector<VisVersion> versionsToTest{ VisVersion::v3_4a, VisVersion::v3_5a };

		for ( const auto& testVersion : versionsToTest )
		{
			try
			{
				auto versionLocalId = LocalIdBuilder::create( testVersion )
										  .withPrimaryItem( GmodPath::parse( "411.1", testVersion ) )
										  .withMetadataTag( vis.codebooks( testVersion ).createTag( CodebookName::Quantity, "temperature" ) )
										  .build();

				std::cout << "\t" << VisVersionExtensions::toVersionString( testVersion )
						  << ": " << versionLocalId.toString() << "\n";
			}
			catch ( const std::exception& e )
			{
				std::cout << "\t" << VisVersionExtensions::toVersionString( testVersion )
						  << ": Failed - " << e.what() << "\n";
			}
		}

		std::cout << "\n=== Advanced operations completed! ===\n";
		return 0;
	}
	catch ( const std::exception& ex )
	{
		std::cerr << "Error: " << ex.what() << "\n";
		return 1;
	}
}
