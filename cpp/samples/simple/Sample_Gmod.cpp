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
 * @file Sample_Gmod.cpp
 * @brief Demonstrates usage of vista-sdk-cpp Gmod API
 * @details This sample shows how to access the Generic Product Model (Gmod),
 *          navigate nodes, query metadata, and work with product types/selections
 */

#include <dnv/vista/sdk/VIS.h>

#include <iomanip>
#include <iostream>
#include <string>

int main()
{
	using namespace dnv::vista::sdk;

	std::cout << "=== vista-sdk-cpp Gmod Sample ===\n\n";

	//=====================================================================
	// 1. Gmod: Accessing Gmod for a VIS version
	//=====================================================================
	{
		std::cout << "1. Gmod: Accessing Gmod for a VIS version\n";
		std::cout << "--------------------------------------------\n";

		const auto& vis = VIS::instance();
		auto version = vis.latest();
		const auto& gmod = vis.gmod( version );

		std::cout << "Gmod for version: " << VisVersions::toString( gmod.version() ) << "\n";

		size_t nodeCount = 0;
		for ( [[maybe_unused]] const auto& [code, node] : gmod )
		{
			nodeCount++;
		}

		std::cout << "Total nodes in Gmod: " << nodeCount << "\n";
		std::cout << "Root node code: " << gmod.rootNode().code() << "\n";
		std::cout << "\n";
	}

	//=====================================================================
	// 2. GmodNode: Accessing nodes by code
	//=====================================================================
	{
		std::cout << "2. GmodNode: Accessing nodes by code\n";
		std::cout << "---------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );

		// Access node using operator[]
		const auto& node1 = gmod["411.1"];
		std::cout << "Node code: " << node1.code() << "\n";
		std::cout << "Node name: " << node1.metadata().name() << "\n";
		std::cout << "Node category: " << node1.metadata().category() << "\n";
		std::cout << "Node type: " << node1.metadata().type() << "\n";

		// Safe access using node
		auto node2Opt = gmod.node( "C101.31" );
		if ( node2Opt.has_value() )
		{
			std::cout << "\nFound node: " << ( *node2Opt )->code() << "\n";
			std::cout << "Name: " << ( *node2Opt )->metadata().name() << "\n";
		}

		// Try invalid code
		auto invalidOpt = gmod.node( "INVALID" );
		std::cout << "\nInvalid code lookup: " << std::boolalpha << invalidOpt.has_value() << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 3. GmodNode: Navigating the tree hierarchy
	//=====================================================================
	{
		std::cout << "3. GmodNode: Navigating the tree hierarchy\n";
		std::cout << "---------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );

		const auto& node = gmod["411.1"];

		// Parents (can have multiple in Gmod graph)
		if ( !node.parents().empty() )
		{
			std::cout << "Parents of " << node.code() << " (" << node.parents().size() << "):\n";
			for ( const auto* parent : node.parents() )
			{
				std::cout << "  - " << parent->code() << "\n";
			}
		}

		// Children
		std::cout << "\nChildren of " << node.code() << " (" << node.children().size() << "):\n";
		size_t maxDisplay = 5;
		size_t count = 0;
		for ( const auto* child : node.children() )
		{
			if ( count++ >= maxDisplay )
			{
				std::cout << "  ... and " << ( node.children().size() - maxDisplay ) << " more\n";
				break;
			}
			std::cout << "  - " << child->code() << " (" << child->metadata().name() << ")\n";
		}

		// Root check
		std::cout << "\nIs root? " << std::boolalpha << node.isRoot() << "\n";
		std::cout << "Root node: " << gmod.rootNode().code() << "\n";
		std::cout << "Is root? " << std::boolalpha << gmod.rootNode().isRoot() << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 4. GmodNode: Node metadata
	//=====================================================================
	{
		std::cout << "4. GmodNode: Node metadata\n";
		std::cout << "----------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );

		const auto& node = gmod["C101.31"];
		const auto& metadata = node.metadata();

		std::cout << "Node: " << node.code() << "\n";
		std::cout << "Category: " << metadata.category() << "\n";
		std::cout << "Type: " << metadata.type() << "\n";
		std::cout << "Name: " << metadata.name() << "\n";

		if ( metadata.definition().has_value() )
		{
			std::cout << "Definition: " << metadata.definition().value() << "\n";
		}

		if ( metadata.installSubstructure().has_value() )
		{
			std::cout << "Install substructure: " << metadata.installSubstructure().value() << "\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 5. GmodNode: Product types and selections
	//=====================================================================
	{
		std::cout << "5. GmodNode: Product types and selections\n";
		std::cout << "--------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );

		// Product type example
		const auto& node1 = gmod["411.3"];
		std::cout << "Node: " << node1.code() << "\n";
		std::cout << "Has product type: " << std::boolalpha << node1.productType().has_value() << "\n";
		if ( auto prodTypeOpt = node1.productType() )
		{
			const GmodNode* prodType = prodTypeOpt.value();
			std::cout << "Product type code: " << prodType->code() << "\n";
			std::cout << "Product type name: " << prodType->metadata().name() << "\n";
		}

		// Product selection example
		const auto& node2 = gmod["411.2"];
		std::cout << "\nNode: " << node2.code() << "\n";
		std::cout << "Has product selection: " << std::boolalpha << node2.productSelection().has_value() << "\n";
		if ( auto prodSelOpt = node2.productSelection() )
		{
			const GmodNode* prodSel = prodSelOpt.value();
			std::cout << "Product selection code: " << prodSel->code() << "\n";
			std::cout << "Product selection name: " << prodSel->metadata().name() << "\n";
		}

		// Check if node is product selection
		auto csNode = gmod.node( "CS1" );
		if ( csNode.has_value() )
		{
			std::cout << "\nNode: " << ( *csNode )->code() << "\n";
			std::cout << "Is product selection: " << std::boolalpha << ( *csNode )->isProductSelection() << "\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 6. GmodNode: Mappability
	//=====================================================================
	{
		std::cout << "6. GmodNode: Mappability\n";
		std::cout << "--------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );

		std::vector<std::string> testCodes = {
			"VE",	   // Root - not mappable
			"411",	   // Mappable
			"411.1",   // Not mappable
			"C101",	   // Mappable
			"C101.31", // Mappable
			"CS1"	   // Product selection - not mappable
		};

		std::cout << std::boolalpha;
		for ( const auto& code : testCodes )
		{
			auto nodeOpt = gmod.node( code );
			if ( nodeOpt.has_value() )
			{
				std::cout << "  " << std::setw( 10 ) << std::left << code;
				std::cout << ": " << ( *nodeOpt )->isMappable() << "\n";
			}
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 7. Gmod: Iterating all nodes
	//=====================================================================
	{
		std::cout << "7. Gmod: Iterating all nodes\n";
		std::cout << "------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );

		std::cout << "First 10 nodes:\n";
		size_t count = 0;
		for ( const auto& [code, node] : gmod )
		{
			if ( count++ >= 10 )
				break;

			std::cout << "  " << std::setw( 15 ) << std::left << code;
			std::cout << " - " << node.metadata().name() << "\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 8. GmodNode: Finding specific node types
	//=====================================================================
	{
		std::cout << "8. GmodNode: Finding specific node types\n";
		std::cout << "-------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );

		// Find PRODUCT TYPE nodes
		std::cout << "Sample PRODUCT TYPE nodes:\n";
		size_t count = 0;
		for ( const auto& [code, node] : gmod )
		{
			if ( node.metadata().category() == "PRODUCT" && node.metadata().type() == "TYPE" )
			{
				if ( count++ >= 5 )
					break;
				std::cout << "  " << std::setw( 10 ) << std::left << code;
				std::cout << " - " << node.metadata().name() << "\n";
			}
		}

		// Find ASSET FUNCTION LEAF nodes
		std::cout << "\nSample ASSET FUNCTION LEAF nodes:\n";
		count = 0;
		for ( const auto& [code, node] : gmod )
		{
			if ( node.isLeafNode() && node.metadata().category() == "ASSET FUNCTION" )
			{
				if ( count++ >= 5 )
					break;
				std::cout << "  " << std::setw( 10 ) << std::left << code;
				std::cout << " - " << node.metadata().name() << "\n";
			}
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 9. GmodNode: Tree navigation example
	//=====================================================================
	{
		std::cout << "9. GmodNode: Tree navigation example\n";
		std::cout << "----------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );

		// Navigate from a node to root
		const auto& node = gmod["C101.31"];
		std::cout << "Path from " << node.code() << " to root:\n";

		const GmodNode* current = &node;
		int depth = 0;
		while ( current != nullptr )
		{
			std::cout << std::string( depth * 2, ' ' ) << "- " << current->code();
			std::cout << " (" << current->metadata().name() << ")\n";

			// Navigate to first parent (if any)
			if ( !current->parents().empty() )
			{
				current = current->parents()[0];
			}
			else
			{
				current = nullptr;
			}
			depth++;
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 10. GmodNode: Node properties comparison
	//=====================================================================
	{
		std::cout << "10. GmodNode: Node properties comparison\n";
		std::cout << "-------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( vis.latest() );

		std::vector<std::string> codes = { "411.1", "C101", "CS1", "F201" };

		std::cout << std::left;
		std::cout << std::setw( 12 ) << "Code";
		std::cout << std::setw( 20 ) << "Category";
		std::cout << std::setw( 15 ) << "Type";
		std::cout << std::setw( 12 ) << "Mappable";
		std::cout << std::setw( 12 ) << "IsLeaf";
		std::cout << "\n";
		std::cout << std::string( 71, '-' ) << "\n";

		std::cout << std::boolalpha;
		for ( const auto& code : codes )
		{
			auto nodeOpt = gmod.node( code );
			if ( nodeOpt.has_value() )
			{
				const auto& node = **nodeOpt;
				std::cout << std::setw( 12 ) << code;
				std::cout << std::setw( 20 ) << node.metadata().category();
				std::cout << std::setw( 15 ) << node.metadata().type();
				std::cout << std::setw( 12 ) << node.isMappable();
				std::cout << std::setw( 12 ) << node.isLeafNode();
				std::cout << "\n";
			}
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 11. Gmod: Working with different versions
	//=====================================================================
	{
		std::cout << "12. Gmod: Working with different versions\n";
		std::cout << "--------------------------------------------\n";

		const auto& vis = VIS::instance();

		// Compare node counts across versions
		std::cout << "Node counts across VIS versions:\n";
		for ( const auto& version : vis.versions() )
		{
			const auto& gmod = vis.gmod( version );
			size_t count = 0;
			for ( [[maybe_unused]] const auto& [code, node] : gmod )
			{
				count++;
			}

			std::cout << "  " << VisVersions::toString( version ) << ": " << count << " nodes\n";
		}

		std::cout << "\n";
	}

	return 0;
}
