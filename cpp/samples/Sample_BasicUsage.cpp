/**
 * @file Sample_BasicUsage.cpp
 * @brief Basic Usage Example - Vista SDK C++
 *
 * This example demonstrates the fundamental operations of the Vista SDK:
 * - Initializing VIS
 * - Working with GMOD paths
 * - Creating Local IDs
 * - Using codebooks
 *
 * For more examples, see the other files in this directory.
 */

#include <iostream>

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
 * @brief Demonstrate basic Vista SDK usage.
 */
int main()
{
	try
	{
		std::cout << "=== Vista SDK Basic Usage Example ===\n\n";

		std::cout << "1. Initializing VIS...\n";
		auto& vis = VIS::instance();

		auto version = VisVersion::v3_4a;
		std::cout << "\tLoading data for VIS version: " << VisVersionExtensions::toVersionString( version ) << "\n";

		const auto& gmod = vis.gmod( version );
		const auto& codebooks = vis.codebooks( version );

		size_t nodeCount = 0;
		auto enumerator = gmod.enumerator();
		while ( enumerator.next() )
		{
			nodeCount++;
		}
		std::cout << "\t✓ GMOD loaded with " << nodeCount << " nodes\n";
		std::cout << "\t✓ Codebooks loaded\n";
		std::cout << "\t✓ Locations loaded\n\n";

		std::cout << "2. Working with GMOD paths...\n";
		std::vector<std::string> pathStrings{ "411.1/C101.31-2", "612.21-1/C701.13/S93", "1021.1i-6P/H123" };

		std::vector<GmodPath> paths;
		for ( const auto& pathStr : pathStrings )
		{
			try
			{
				auto path = GmodPath::parse( pathStr, version );
				paths.push_back( path );
				const auto& node = path.node();
				std::cout << "\t✓ Parsed: " << pathStr << "\n";
				std::cout << "\t\t→ Node: " << node.code() << " (" << node.metadata().commonName().value_or( "N/A" ) << ")\n";
				std::cout << "\t\t→ Depth: " << path.length() << "\n";
			}
			catch ( const std::exception& e )
			{
				std::cout << "\t✗ Failed to parse " << pathStr << ": " << e.what() << "\n";
			}
		}

		std::cout << "\n";

		std::cout << "3. Using codebooks...\n";

		const auto& quantityBook = codebooks[CodebookName::Quantity];
		const auto& positionBook = codebooks[CodebookName::Position];
		const auto& contentBook = codebooks[CodebookName::Content];

		std::vector<MetadataTag> tags;
		try
		{
			auto quantityTag = quantityBook.createTag( "temperature" );
			auto positionTag = positionBook.createTag( "centre" );
			auto contentTag = contentBook.createTag( "cooling.water" );

			tags.push_back( quantityTag );
			tags.push_back( positionTag );
			tags.push_back( contentTag );

			std::cout << "\t✓ Created quantity tag: " << quantityTag.value() << " (custom: " << ( quantityTag.isCustom() ? "true" : "false" ) << ")\n";
			std::cout << "\t✓ Created position tag: " << positionTag.value() << " (custom: " << ( positionTag.isCustom() ? "true" : "false" ) << ")\n";
			std::cout << "\t✓ Created content tag: " << contentTag.value() << " (custom: " << ( contentTag.isCustom() ? "true" : "false" ) << ")\n";
		}
		catch ( const std::exception& e )
		{
			std::cout << "\tFailed to create metadata tags: " << e.what() << "\n";
		}

		std::cout << "\n";

		std::cout << "4. Creating Local IDs...\n";

		for ( size_t i = 0; i < std::min( paths.size(), size_t( 2 ) ); ++i )
		{
			try
			{
				auto localId = LocalIdBuilder::create( version )
								   .withPrimaryItem( GmodPath( paths[i] ) )
								   .withMetadataTag( tags[0] )
								   .withMetadataTag( tags[2] )
								   .withMetadataTag( tags[1] )
								   .build();

				std::cout << "\t✓ Local ID " << ( i + 1 ) << ": " << localId.toString() << "\n";
				if ( localId.primaryItem().has_value() )
				{
					std::cout << "\t\t→ Primary item: " << localId.primaryItem().value().toString() << "\n";
				}
			}
			catch ( const std::exception& e )
			{
				std::cout << "\t✗ Failed to create Local ID for path " << ( i + 1 ) << ": " << e.what() << "\n";
			}
		}

		std::cout << "\n";

		std::cout << "5. Parsing existing Local IDs...\n";

		std::vector<std::string> sampleLocalIds{
			"/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
			"/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage" };

		for ( const auto& localIdStr : sampleLocalIds )
		{
			try
			{
				auto localId = LocalId::parse( localIdStr );
				std::cout << "\t✓ Parsed: " << localIdStr << "\n";
				if ( localId.primaryItem().has_value() )
				{
					std::cout << "\t\t→ Primary: " << localId.primaryItem().value().toString() << "\n";
				}
				std::cout << "\t\t→ Metadata tags: " << localId.metadataTags().size() << "\n";

				for ( const auto& tag : localId.metadataTags() )
				{
					std::string tagName = LocalIdBuilder::codebookNametoString( tag.name() );

					std::cout << "\t\t\t- " << tagName << ": " << tag.value() << "\n";
				}
			}
			catch ( const std::exception& e )
			{
				std::cout << "\t✗ Failed to parse " << localIdStr << ": " << e.what() << "\n";
			}
		}

		std::cout << "\n=== Example completed successfully! ===\n";
		return 0;
	}
	catch ( const std::exception& ex )
	{
		std::cerr << "Error: " << ex.what() << "\n";
		return 1;
	}
}
