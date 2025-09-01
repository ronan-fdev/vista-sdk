/**
 * @file Sample_GMOD.cpp
 * @brief GMOD Operations - Vista SDK C++
 *
This example demonstrates working with GMOD (Generic Product Model):
* - Parsing and traversing GMOD paths
 * - Exploring node hierarchies
 * - Working with node metadata
 * - Version conversion operations
 *
 * For more examples, see the other files in this directory.
 */

#include <iostream>

#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/GmodNode.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISVersion.h"

using namespace dnv::vista::sdk;

/**
 * @brief Demonstrate GMOD operations.
 */
int main()
{
	try
	{
		std::cout << "=== GMOD Operations Example ===\n\n";

		auto& vis = VIS::instance();
		auto version = VisVersion::v3_4a;
		const auto& gmod = vis.gmod( version );

		std::cout << "1. Basic GMOD Path Operations...\n";

		std::vector<std::string> samplePaths{
			"411.1",
			"411.1/C101.31-2",
			"612.21-1/C701.13/S93",
			"1021.1i-6P/H123" };

		for ( const auto& pathStr : samplePaths )
		{
			try
			{
				auto path = GmodPath::parse( pathStr, version );
				const auto& node = path.node();
				std::cout << "\tPath: " << pathStr << "\n";
				std::cout << "\t\t→ Code: " << node.code() << "\n";
				std::cout << "\t\t→ Name: " << node.metadata().name() << "\n";
				std::cout << "\t\t→ Common Name: " << ( node.metadata().commonName().has_value() ? node.metadata().commonName().value() : "N/A" ) << "\n";
				std::cout << "\t\t→ Depth: " << path.length() << "\n";
				std::cout << "\n";
			}
			catch ( const std::exception& e )
			{
				std::cout << "\t✗ Failed to parse " << pathStr << ": " << e.what() << "\n";
			}
		}

		std::cout << "2. Exploring Node Hierarchies...\n";

		auto rootPath = GmodPath::parse( "411.1", version );
		const auto& rootNode = rootPath.node();

		std::cout << "\tRoot node: " << rootNode.code() << " - " << ( rootNode.metadata().commonName().has_value() ? rootNode.metadata().commonName().value() : "N/A" ) << "\n";
		std::cout << "\tChild nodes:\n";

		auto children = rootNode.children();
		int childCount = 0;
		for ( auto it = children.begin(); it != children.end() && childCount < 5; ++it, ++childCount )
		{
			const auto& child = **it;
			std::cout << "\t\t- " << child.code() << ": " << ( child.metadata().commonName().has_value() ? child.metadata().commonName().value() : "N/A" ) << "\n";
		}

		std::cout << "\n3. Path Traversal and Analysis...\n";

		auto deepPath = GmodPath::parse( "411.1/C101.31-2", version );

		std::cout << "\tAnalyzing path: " << deepPath.toString() << "\n";
		std::cout << "\tFull path traversal:\n";

		std::cout << "\tDepth 1: " << deepPath.node().code()
				  << " - " << ( deepPath.node().metadata().commonName().has_value() ? deepPath.node().metadata().commonName().value() : "N/A" ) << "\n";

		std::cout << "\t(Path traversal details limited in C++ API)\n";
		std::cout << "\n4. Node Properties and Metadata...\n";

		std::vector<std::string> sampleNodes{ "411.1", "C101.31", "S206" };

		for ( const auto& nodeCode : sampleNodes )
		{
			try
			{
				const auto& node = gmod[nodeCode];
				std::cout << "\tNode: " << nodeCode << "\n";
				std::cout << "\t\t→ Name: " << node.metadata().name() << "\n";
				std::cout << "\t\t→ Common Name: " << ( node.metadata().commonName().has_value() ? node.metadata().commonName().value() : "N/A" ) << "\n";
				std::cout << "\t\t→ Is Mappable: " << ( node.isMappable() ? "true" : "false" ) << "\n";

				auto nodeChildren = node.children();
				size_t childrenCount = 0;
				for ( auto it = nodeChildren.begin(); it != nodeChildren.end(); ++it )
				{
					childrenCount++;
				}
				std::cout << "\t\t→ Has Children: " << ( childrenCount > 0 ? "true" : "false" ) << "\n";
				std::cout << "\n";
			}
			catch ( const std::exception& e )
			{
				std::cout << "\t✗ Node " << nodeCode << " not found: " << e.what() << "\n";
			}
		}

		std::cout << "5. Working with Different VIS Versions...\n";

		std::vector<VisVersion> versionsToTest{ VisVersion::v3_4a, VisVersion::v3_5a, VisVersion::v3_6a };
		std::string testPath = "411.1/C101.31";

		for ( const auto& testVersion : versionsToTest )
		{
			try
			{
				const auto& versionGmod = vis.gmod( testVersion );
				auto parsedPath = GmodPath::parse( testPath, testVersion );
				const auto& node = parsedPath.node();

				std::cout << "\tVIS " << VisVersionExtensions::toVersionString( testVersion ) << ":\n";
				std::cout << "\t\t→ Path exists: " << ( !parsedPath.toString().empty() ? "true" : "false" ) << "\n";
				std::cout << "\t\t→ Node name: " << ( node.metadata().commonName().has_value() ? node.metadata().commonName().value() : "N/A" ) << "\n";

				size_t totalNodes = 0;
				auto enumerator = versionGmod.enumerator();
				while ( enumerator.next() )
				{
					totalNodes++;
				}
				std::cout << "\t\t→ Total nodes in GMOD: " << totalNodes << "\n";
			}
			catch ( const std::exception& e )
			{
				std::cout << "\t✗ Error with version " << VisVersionExtensions::toVersionString( testVersion ) << ": " << e.what() << "\n";
			}
		}

		std::cout << "\n6. Path Validation and Error Handling...\n";

		std::vector<std::string> invalidPaths{ "invalid.node", "411.1/invalid-child", "999.999/does-not-exist" };

		for ( const auto& invalidPath : invalidPaths )
		{
			try
			{
				std::optional<GmodPath> result;
				bool success = GmodPath::tryParse( invalidPath, version, result );
				if ( !success || !result.has_value() )
				{
					std::cout << "\t✗ Path '" << invalidPath << "' is invalid (as expected)\n";
				}
				else
				{
					std::cout << "\t⚠ Path '" << invalidPath << "' unexpectedly parsed as: " << result.value().toString() << "\n";
				}
			}
			catch ( const std::exception& e )
			{
				std::cout << "\t✗ Exception parsing '" << invalidPath << "': " << e.what() << "\n";
			}
		}

		std::cout << "\n7. GMOD Version Conversion...\n";

		try
		{
			std::vector<std::tuple<std::string, VisVersion, VisVersion>> conversionTests{
				{ "411.1/C101.72/I101", VisVersion::v3_4a, VisVersion::v3_5a },
				{ "612.21/C701.13/S93", VisVersion::v3_5a, VisVersion::v3_6a } };

			for ( const auto& [oldPathStr, sourceVersion, targetVersion] : conversionTests )
			{
				try
				{
					auto oldPath = GmodPath::parse( oldPathStr, sourceVersion );

					auto newPath = vis.convertPath( sourceVersion, oldPath, targetVersion );
					if ( newPath.has_value() )
					{
						std::cout << "\t✓ Converted: " << oldPathStr << " → " << newPath.value().toString() << "\n";
					}
					else
					{
						std::cout << "\t✗ Conversion returned empty for " << oldPathStr << "\n";
					}
				}
				catch ( const std::exception& e )
				{
					std::cout << "\t✗ Conversion failed for " << oldPathStr << ": " << e.what() << "\n";
				}
			}
		}
		catch ( const std::exception& e )
		{
			std::cout << "\t⚠ Version conversion not available: " << e.what() << "\n";
		}

		std::cout << "\n=== GMOD operations completed! ===\n";
		return 0;
	}
	catch ( const std::exception& ex )
	{
		std::cerr << "Error: " << ex.what() << "\n";
		return 1;
	}
}
