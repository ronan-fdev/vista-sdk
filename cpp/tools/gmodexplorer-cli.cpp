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
 * @file gmodexplorer-cli.cpp
 * @brief CLI tool for exploring VIS GMOD tree
 * @details Command-line interface to navigate and explore the Generic Product Model
 *
 * Usage:
 *   gmodexplorer-cli                    # Show tree overview for latest VIS version
 *   gmodexplorer-cli --version 3-4a     # Use specific VIS version
 *   gmodexplorer-cli --list-versions    # List available VIS versions
 *   gmodexplorer-cli --code 411.1       # Show node details
 *   gmodexplorer-cli --search "engine"  # Search nodes by name
 *   gmodexplorer-cli --tree 411         # Show subtree from node
 *   gmodexplorer-cli --path "411.1-1P"  # Parse and validate path
 *   gmodexplorer-cli --help             # Show help
 */

#include <dnv/vista/sdk/VIS.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <set>

void printHelp();
void printVersions( const dnv::vista::sdk::VIS& vis );
void printOverview( const dnv::vista::sdk::Gmod& gmod );
void printNodeDetails( const dnv::vista::sdk::Gmod& gmod, std::string_view code );
void searchNodes( const dnv::vista::sdk::Gmod& gmod, std::string_view searchTerm );
void printTree( const dnv::vista::sdk::Gmod& gmod, std::string_view rootCode, int maxDepth );
void validatePath( const dnv::vista::sdk::Gmod& gmod, const dnv::vista::sdk::Locations& locations, std::string_view path );

int main( int argc, char* argv[] )
{
	using namespace dnv::vista::sdk;

	const auto& vis = VIS::instance();

	// Parse command line arguments
	if ( argc == 1 )
	{
		// Default: print overview for latest version
		const auto& gmod = vis.gmod( vis.latest() );
		printOverview( gmod );
		return 0;
	}

	std::string_view arg1 = argv[1];

	if ( arg1 == "--help" || arg1 == "-h" )
	{
		printHelp();
		return 0;
	}

	if ( arg1 == "--list-versions" || arg1 == "-l" )
	{
		printVersions( vis );
		return 0;
	}

	// Version selection
	VisVersion version = vis.latest();
	int argOffset = 1;

	if ( arg1 == "--version" || arg1 == "-v" )
	{
		if ( argc < 3 )
		{
			std::cerr << "Error: --version requires a version argument\n";
			std::cerr << "Use --list-versions to see available versions\n";
			return 1;
		}

		std::string_view versionStr = argv[2];
		auto versionOpt = VisVersions::fromString( versionStr );
		if ( !versionOpt.has_value() )
		{
			std::cerr << "Error: Unknown VIS version '" << versionStr << "'\n";
			std::cerr << "Use --list-versions to see available versions\n";
			return 1;
		}

		version = versionOpt.value();
		argOffset = 3;

		// If only version was specified, show overview
		if ( argc == 3 )
		{
			const auto& gmod = vis.gmod( version );
			printOverview( gmod );
			return 0;
		}

		arg1 = argv[argOffset];
	}

	const auto& gmod = vis.gmod( version );
	const auto& locations = vis.locations( version );

	if ( arg1 == "--code" || arg1 == "-c" )
	{
		if ( argc < argOffset + 2 )
		{
			std::cerr << "Error: --code requires a node code argument\n";
			return 1;
		}

		std::string_view code = argv[argOffset + 1];
		printNodeDetails( gmod, code );
		return 0;
	}

	if ( arg1 == "--search" || arg1 == "-s" )
	{
		if ( argc < argOffset + 2 )
		{
			std::cerr << "Error: --search requires a search term\n";
			return 1;
		}

		std::string_view searchTerm = argv[argOffset + 1];
		searchNodes( gmod, searchTerm );
		return 0;
	}

	if ( arg1 == "--tree" || arg1 == "-t" )
	{
		if ( argc < argOffset + 2 )
		{
			std::cerr << "Error: --tree requires a root node code\n";
			return 1;
		}

		std::string_view rootCode = argv[argOffset + 1];
		int maxDepth = 3; // Default depth

		if ( argc > argOffset + 2 )
		{
			try
			{
				maxDepth = std::stoi( argv[argOffset + 2] );
			}
			catch ( ... )
			{
				std::cerr << "Warning: Invalid depth value, using default: 3\n";
			}
		}

		printTree( gmod, rootCode, maxDepth );
		return 0;
	}

	if ( arg1 == "--path" || arg1 == "-p" )
	{
		if ( argc < argOffset + 2 )
		{
			std::cerr << "Error: --path requires a path string\n";
			return 1;
		}

		std::string_view path = argv[argOffset + 1];
		validatePath( gmod, locations, path );
		return 0;
	}

	std::cerr << "Error: Unknown option '" << arg1 << "'\n";
	std::cerr << "Use --help to see available options\n";
	return 1;
}

void printHelp()
{
	std::cout << "VIS GMOD Explorer CLI - Navigate the Generic Product Model\n\n";
	std::cout << "Usage:\n";
	std::cout << "  gmodexplorer-cli                    Show tree overview (latest version)\n";
	std::cout << "  gmodexplorer-cli --version <ver>    Use specific VIS version\n";
	std::cout << "  gmodexplorer-cli --list-versions    List available VIS versions\n";
	std::cout << "  gmodexplorer-cli --code <code>      Show node details\n";
	std::cout << "  gmodexplorer-cli --search <term>    Search nodes by name/definition\n";
	std::cout << "  gmodexplorer-cli --tree <code> [d]  Show subtree (depth d, default 3)\n";
	std::cout << "  gmodexplorer-cli --path <path>      Parse and validate GMOD path\n";
	std::cout << "  gmodexplorer-cli --help             Show this help\n\n";
	std::cout << "Options:\n";
	std::cout << "  -h, --help           Show this help message\n";
	std::cout << "  -l, --list-versions  List all available VIS versions\n";
	std::cout << "  -v, --version <ver>  Use specific VIS version (e.g., 3-4a)\n";
	std::cout << "  -c, --code <code>    Show detailed info for node code\n";
	std::cout << "  -s, --search <term>  Search for nodes matching term\n";
	std::cout << "  -t, --tree <code>    Display tree structure from node\n";
	std::cout << "  -p, --path <path>    Validate a GMOD path string\n\n";
	std::cout << "Examples:\n";
	std::cout << "  gmodexplorer-cli --code 411.1\n";
	std::cout << "  gmodexplorer-cli --search \"main engine\"\n";
	std::cout << "  gmodexplorer-cli --tree 400a 2\n";
	std::cout << "  gmodexplorer-cli --path \"411.1-1P\"\n";
	std::cout << "  gmodexplorer-cli --version 3-7a --code C101.31\n";
}

void printVersions( const dnv::vista::sdk::VIS& vis )
{
	using namespace dnv::vista::sdk;

	std::cout << "Available VIS versions:\n";
	for ( auto version : vis.versions() )
	{
		std::cout << "  " << VisVersions::toString( version );
		if ( version == vis.latest() )
		{
			std::cout << " (latest)";
		}
		std::cout << "\n";
	}
}

void printOverview( const dnv::vista::sdk::Gmod& gmod )
{
	using namespace dnv::vista::sdk;

	std::cout << "GMOD Overview - VIS version: " << VisVersions::toString( gmod.version() ) << "\n";
	std::cout << std::string( 60, '=' ) << "\n\n";

	// Count nodes
	size_t totalNodes = 0;
	for ( [[maybe_unused]] const auto& [code, node] : gmod )
	{
		totalNodes++;
	}

	std::cout << "Total nodes: " << totalNodes << "\n";
	std::cout << "Root node: " << gmod.rootNode().code() << " (" << gmod.rootNode().metadata().name() << ")\n\n";

	// Show top-level categories
	std::cout << "Top-level categories:\n";
	const auto& root = gmod.rootNode();
	for ( const auto* child : root.children() )
	{
		std::cout << "  " << std::setw( 10 ) << std::left << child->code()
				  << " - " << child->metadata().name() << "\n";
	}

	std::cout << "\nUse --help to see more options\n";
}

void printNodeDetails( const dnv::vista::sdk::Gmod& gmod, std::string_view code )
{
	using namespace dnv::vista::sdk;

	auto nodeOpt = gmod.node( code );
	if ( !nodeOpt.has_value() )
	{
		std::cerr << "Error: Node '" << code << "' not found in GMOD\n";
		return;
	}

	const auto* node = *nodeOpt;
	const auto& meta = node->metadata();

	std::cout << "Node Details: " << node->code() << "\n";
	std::cout << std::string( 60, '=' ) << "\n\n";

	std::cout << "Code:       " << node->code() << "\n";
	std::cout << "Category:   " << meta.category() << "\n";
	std::cout << "Type:       " << meta.type() << "\n";
	std::cout << "Name:       " << meta.name() << "\n";

	if ( meta.commonName().has_value() )
	{
		const auto& commonName = meta.commonName().value();
		if ( !commonName.empty() )
		{
			std::cout << "Common:     " << commonName << "\n";
		}
	}

	if ( meta.definition().has_value() )
	{
		std::cout << "Definition: " << meta.definition().value() << "\n";
	}

	if ( meta.installSubstructure().has_value() )
	{
		std::cout << "Install:    " << meta.installSubstructure().value() << "\n";
	}

	std::cout << "\nHierarchy:\n";
	std::cout << "Is root:    " << std::boolalpha << node->isRoot() << "\n";

	if ( !node->parents().empty() )
	{
		std::cout << "Parents (" << node->parents().size() << "):\n";
		for ( const auto* parent : node->parents() )
		{
			std::cout << "  - " << parent->code() << " (" << parent->metadata().name() << ")\n";
		}
	}

	if ( !node->children().empty() )
	{
		std::cout << "Children (" << node->children().size() << "):\n";
		size_t maxShow = 10;
		size_t count = 0;
		for ( const auto* child : node->children() )
		{
			if ( count++ >= maxShow )
			{
				std::cout << "  ... and " << ( node->children().size() - maxShow ) << " more\n";
				break;
			}
			std::cout << "  - " << child->code() << " (" << child->metadata().name() << ")\n";
		}
	}

	// Product type/selection info
	if ( auto prodTypeOpt = node->productType() )
	{
		const auto* prodType = *prodTypeOpt;
		std::cout << "\nProduct Type: " << prodType->code() << " (" << prodType->metadata().name() << ")\n";
	}

	if ( auto prodSelOpt = node->productSelection() )
	{
		const auto* prodSel = *prodSelOpt;
		std::cout << "Product Selection: " << prodSel->code() << " (" << prodSel->metadata().name() << ")\n";
	}
}

void searchNodes( const dnv::vista::sdk::Gmod& gmod, std::string_view searchTerm )
{
	using namespace dnv::vista::sdk;

	// Case-insensitive search
	std::string searchLower( searchTerm );
	std::transform( searchLower.begin(), searchLower.end(), searchLower.begin(),
		[]( unsigned char c ) { return std::tolower( c ); } );

	std::cout << "Searching for: \"" << searchTerm << "\"\n";
	std::cout << std::string( 60, '=' ) << "\n\n";

	std::vector<const GmodNode*> matches;

	for ( const auto& [code, node] : gmod )
	{
		std::string_view name = node.metadata().name();
		std::string nameLower( name );
		std::transform( nameLower.begin(), nameLower.end(), nameLower.begin(),
			[]( unsigned char c ) { return std::tolower( c ); } );

		std::string commonLower;
		if ( node.metadata().commonName().has_value() )
		{
			const auto& commonName = node.metadata().commonName().value();
			commonLower = commonName;
			std::transform( commonLower.begin(), commonLower.end(), commonLower.begin(),
				[]( unsigned char c ) { return std::tolower( c ); } );
		}

		std::string defLower;
		if ( node.metadata().definition().has_value() )
		{
			const auto& definition = node.metadata().definition().value();
			defLower = definition;
			std::transform( defLower.begin(), defLower.end(), defLower.begin(),
				[]( unsigned char c ) { return std::tolower( c ); } );
		}

		if ( nameLower.find( searchLower ) != std::string::npos ||
			 commonLower.find( searchLower ) != std::string::npos ||
			 defLower.find( searchLower ) != std::string::npos ||
			 code.find( searchTerm ) != std::string::npos )
		{
			matches.push_back( &node );
		}
	}

	if ( matches.empty() )
	{
		std::cout << "No matches found.\n";
		return;
	}

	std::cout << "Found " << matches.size() << " match(es):\n\n";

	for ( const auto* node : matches )
	{
		std::cout << std::setw( 12 ) << std::left << node->code()
				  << " - " << node->metadata().name();

		if ( node->metadata().commonName().has_value() )
		{
			const auto& commonName = node->metadata().commonName().value();
			if ( !commonName.empty() )
			{
				std::cout << " (" << commonName << ")";
			}
		}

		std::cout << "\n";

		if ( matches.size() <= 20 && node->metadata().definition().has_value() )
		{
			std::cout << std::string( 15, ' ' ) << "\"" << node->metadata().definition().value() << "\"\n";
		}
	}

	if ( matches.size() > 20 )
	{
		std::cout << "\nTip: Use --code <code> to see details for a specific node\n";
	}
}

void printTreeRecursive( const dnv::vista::sdk::GmodNode& node, int currentDepth, int maxDepth, std::set<const dnv::vista::sdk::GmodNode*>& visited )
{
	if ( currentDepth > maxDepth )
		return;

	// Prevent infinite loops in graph
	if ( visited.count( &node ) > 0 )
	{
		std::cout << std::string( currentDepth * 2, ' ' ) << "- " << node.code() << " (already shown)\n";
		return;
	}

	visited.insert( &node );

	std::cout << std::string( currentDepth * 2, ' ' ) << "- " << node.code()
			  << " (" << node.metadata().name() << ")\n";

	for ( const auto* child : node.children() )
	{
		printTreeRecursive( *child, currentDepth + 1, maxDepth, visited );
	}
}

void printTree( const dnv::vista::sdk::Gmod& gmod, std::string_view rootCode, int maxDepth )
{
	using namespace dnv::vista::sdk;

	auto nodeOpt = gmod.node( rootCode );
	if ( !nodeOpt.has_value() )
	{
		std::cerr << "Error: Node '" << rootCode << "' not found in GMOD\n";
		return;
	}

	const auto* node = *nodeOpt;

	std::cout << "Tree from node: " << node->code() << " (max depth: " << maxDepth << ")\n";
	std::cout << std::string( 60, '=' ) << "\n\n";

	std::set<const GmodNode*> visited;
	printTreeRecursive( *node, 0, maxDepth, visited );
}

void validatePath( const dnv::vista::sdk::Gmod& gmod, const dnv::vista::sdk::Locations& locations, std::string_view pathStr )
{
	using namespace dnv::vista::sdk;

	std::cout << "Validating path: \"" << pathStr << "\"\n";
	std::cout << std::string( 60, '=' ) << "\n\n";

	// Try parsing as short path first
	auto pathOpt = GmodPath::fromString( pathStr, gmod, locations );
	if ( pathOpt.has_value() )
	{
		const auto& path = *pathOpt;

		std::cout << "Valid short path\n\n";
		std::cout << "    Short path:     " << path.toString() << "\n";
		std::cout << "    Full path:      " << path.toFullPathString() << "\n";
		std::cout << "    Length:         " << path.length() << "\n";

		if ( path.node().location().has_value() )
		{
			std::cout << "Location:       " << path.node().location()->value() << "\n";
		}

		std::cout << "\nPath nodes:\n";
		for ( size_t i = 0; i < path.length(); ++i )
		{
			const auto& parents = path.parents();
			const GmodNode* node = ( i == path.length() - 1 ) ? &path.node() : &parents[i];
			std::cout << "  " << ( i + 1 ) << ". " << node->code() << " - " << node->metadata().name() << "\n";
		}

		return;
	}

	// Try parsing as full path
	pathOpt = GmodPath::fromFullPathString( pathStr, gmod, locations );
	if ( pathOpt.has_value() )
	{
		const auto& path = *pathOpt;

		std::cout << "Valid full path\n\n";
		std::cout << "    Short path:     " << path.toString() << "\n";
		std::cout << "    Full path:      " << path.toFullPathString() << "\n";
		std::cout << "    Length:         " << path.length() << "\n";

		if ( path.node().location().has_value() )
		{
			std::cout << "Location:       " << path.node().location()->value() << "\n";
		}

		return;
	}

	std::cout << "Invalid path\n";
	std::cout << "    The path could not be parsed as either:\n";
	std::cout << "      - Short path (e.g., \"411.1-1P\")\n";
	std::cout << "      - Full path (e.g., \"VE/400a/410/411/411.1-1P\")\n";
}
