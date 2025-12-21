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
 * @file GmodPathParsing.h
 * @brief Internal parsing functions for GMOD path string conversion
 * @details Free functions for parsing short and full GMOD path strings with detailed error reporting.
 *          Returns GmodParsePathResult for error propagation. Not exposed in public API.
 */

#pragma once

#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/Locations.h"
#include "dnv/vista/sdk/StringHash.h"
#include "GmodParsePathResult.h"
#include "LocationSetsVisitor.h"

#include <nfx/StringUtils.h>

#include <queue>
#include <unordered_map>

namespace dnv::vista::sdk::internal
{
	/**
	 * @brief Represents a single node in a path being parsed
	 * @details Holds the node code and optional location for short path parsing.
	 *          Used during tree traversal to match path segments.
	 */
	struct PathNode
	{
		std::string_view code;
		std::optional<Location> location;

		PathNode( std::string_view c )
			: code{ c },
			  location{ std::nullopt }
		{
		}

		PathNode( std::string_view c, const Location& loc )
			: code{ c },
			  location{ loc }
		{
		}
	};

	/**
	 * @brief Context state for short path parsing with tree traversal
	 * @details Maintains parsing state during gmod.traverse() calls, including
	 *          remaining path parts, current search target, accumulated locations,
	 *          and the final constructed path.
	 */
	struct ParseContext
	{
		std::queue<PathNode> parts;
		PathNode toFind;
		std::optional<std::unordered_map<std::string, Location, StringHash, std::equal_to<>>> locations;
		std::optional<GmodPath> path;
		const Gmod* gmod;

		ParseContext( std::queue<PathNode>&& pathNodeQueue, PathNode&& t, std::optional<std::unordered_map<std::string, Location, StringHash, std::equal_to<>>>&& l,
			std::optional<GmodPath>&& gmodPath, const Gmod& g )
			: parts{ std::move( pathNodeQueue ) },
			  toFind{ std::move( t ) },
			  locations{ std::move( l ) },
			  path{ std::move( gmodPath ) },
			  gmod{ &g }
		{
		}
	};

	/**
	 * @brief Traversal handler for short path parsing
	 * @param context Parsing context with state
	 * @param parents Parent nodes in current traversal path
	 * @param current Current node being visited
	 * @return TraversalHandlerResult indicating whether to continue, skip, or stop
	 * @details Searches for matching nodes, builds parent chain, applies locations,
	 *          and constructs the final GmodPath when all parts are found.
	 */
	inline TraversalHandlerResult parseHandler(
		ParseContext& context, const std::vector<const GmodNode*>& parents, const GmodNode& current )
	{
		PathNode& toFind = context.toFind;
		bool found = ( current.code() == toFind.code );

		if ( !found && current.isLeafNode() )
		{
			return TraversalHandlerResult::SkipSubtree;
		}

		if ( !found )
		{
			return TraversalHandlerResult::Continue;
		}

		if ( toFind.location.has_value() )
		{
			if ( !context.locations.has_value() )
			{
				context.locations = std::unordered_map<std::string, Location, StringHash, std::equal_to<>>{};
			}
			std::string codeStr{ toFind.code };
			Location loc = toFind.location.value();
			context.locations->insert_or_assign( std::move( codeStr ), std::move( loc ) );
		}
		if ( !context.parts.empty() )
		{
			toFind = context.parts.front();
			context.parts.pop();
			return TraversalHandlerResult::Continue;
		}

		std::vector<GmodNode> pathParents;
		pathParents.reserve( parents.size() + 1 );

		for ( const GmodNode* parent : parents )
		{
			if ( context.locations.has_value() )
			{
				auto it = context.locations->find( parent->code() );
				if ( it != context.locations->end() )
				{
					pathParents.emplace_back( parent->withLocation( it->second ) );
					continue;
				}
			}
			pathParents.emplace_back( *parent );
		}
		GmodNode endNode = toFind.location.has_value()
							   ? current.withLocation( toFind.location.value() )
							   : current;

		const GmodNode* startNode = nullptr;
		if ( !pathParents.empty() && pathParents[0].parents().size() == 1 )
		{
			startNode = pathParents[0].parents()[0];
		}
		else if ( endNode.parents().size() == 1 )
		{
			startNode = endNode.parents()[0];
		}

		if ( startNode == nullptr || startNode->parents().size() > 1 )
		{
			return TraversalHandlerResult::Stop;
		}

		while ( startNode->parents().size() == 1 )
		{
			// Check if this node is already in pathParents to avoid duplicates
			bool alreadyExists = false;
			for ( const auto& existing : pathParents )
			{
				if ( existing.code() == startNode->code() )
				{
					alreadyExists = true;
					break;
				}
			}

			if ( !alreadyExists )
			{
				pathParents.insert( pathParents.begin(), *startNode );
			}

			startNode = startNode->parents()[0];
			if ( startNode->parents().size() > 1 )
			{
				return TraversalHandlerResult::Stop;
			}
		}

		// Check if root node already exists before inserting
		bool rootExists = false;
		for ( const auto& existing : pathParents )
		{
			if ( existing.code() == context.gmod->rootNode().code() )
			{
				rootExists = true;
				break;
			}
		}

		if ( !rootExists )
		{
			pathParents.insert( pathParents.begin(), context.gmod->rootNode() );
		}

		internal::LocationSetsVisitor visitor;

		for ( size_t i = 0; i < pathParents.size() + 1; ++i )
		{
			GmodNode& n = ( i < pathParents.size() )
							  ? pathParents[i]
							  : endNode;

			auto set = visitor.visit( n, i, pathParents, endNode );
			if ( !set.has_value() )
			{
				if ( n.location().has_value() )
				{
					return TraversalHandlerResult::Stop;
				}
				continue;
			}

			const auto& [setStart, setEnd, location] = set.value();
			if ( setStart == setEnd )
			{
				continue;
			}

			if ( !location.has_value() )
			{
				continue;
			}

			for ( size_t j = setStart; j <= setEnd; ++j )
			{
				if ( j < pathParents.size() )
				{
					pathParents[j] = pathParents[j].withLocation( *location );
				}
				else
				{
					endNode = endNode.withLocation( *location );
				}
			}
		}
		context.path = GmodPath{ std::move( pathParents ), std::move( endNode ) };

		return TraversalHandlerResult::Stop;
	}

	/**
	 * @brief Parse a short GMOD path string with detailed error reporting
	 * @param item Short path string (e.g., "411.1/513-14C")
	 * @param gmod GMOD instance to resolve nodes from
	 * @param locations Locations instance to parse location suffixes
	 * @return GmodParsePathResult containing either the parsed GmodPath or detailed error message
	 * @details Uses tree traversal with gmod.traverse() to find complete paths from root.
	 *          More complex than parseFullGmodPath as it must search the tree.
	 */
	inline GmodParsePathResult parseGmodPath(
		std::string_view item,
		const Gmod& gmod,
		const Locations& locations ) noexcept
	{
		if ( gmod.version() != locations.version() )
		{
			return GmodParsePathResult::Error{ "Got different VIS versions for Gmod and Locations arguments" };
		}

		if ( nfx::string::isEmpty( item ) )
		{
			return GmodParsePathResult::Error{ "Item is empty" };
		}

		item = nfx::string::trim( item );
		if ( nfx::string::isEmpty( item ) )
		{
			return GmodParsePathResult::Error{ "Item is empty" };
		}

		if ( nfx::string::startsWith( item, "/" ) )
		{
			item = item.substr( 1 );
		}

		std::queue<PathNode> parts;

		for ( const auto partStr : nfx::string::splitView( item, '/' ) )
		{
			if ( nfx::string::isEmpty( partStr ) )
			{
				continue;
			}

			if ( nfx::string::contains( partStr, "-" ) )
			{
				const size_t dashPos = partStr.find( '-' );
				const std::string_view codePart = partStr.substr( 0, dashPos );
				const std::string_view locationPart = partStr.substr( dashPos + 1 );

				auto tempNode = gmod.node( codePart );
				if ( !tempNode.has_value() )
				{
					return GmodParsePathResult::Error{
						"Failed to get GmodNode for " + std::string{ partStr } };
				}

				auto parsedLocationOpt = locations.fromString( locationPart );
				if ( !parsedLocationOpt.has_value() )
				{
					return GmodParsePathResult::Error{
						"Failed to parse location " + std::string{ locationPart } };
				}
				parts.emplace( codePart, parsedLocationOpt.value() );
			}
			else
			{
				auto tempNode = gmod.node( partStr );
				if ( !tempNode.has_value() )
				{
					return GmodParsePathResult::Error{
						"Failed to get GmodNode for " + std::string{ partStr } };
				}

				parts.emplace( partStr );
			}
		}

		if ( parts.empty() )
		{
			return GmodParsePathResult::Error{ "Failed find any parts" };
		}

		PathNode toFind = parts.front();
		parts.pop();

		auto baseNode = gmod.node( toFind.code );
		if ( !baseNode.has_value() )
		{
			return GmodParsePathResult::Error{ "Failed to find base node" };
		}

		ParseContext context( std::move( parts ), std::move( toFind ), std::nullopt, std::nullopt, gmod );

		Gmod::TraverseHandlerWithState<ParseContext> handler = parseHandler;
		gmod.traverse( context, *baseNode.value(), handler, TraversalOptions{} );
		if ( !context.path.has_value() )
		{
			return GmodParsePathResult::Error{ "Failed to find path after traversal" };
		}

		return GmodParsePathResult::Ok{ std::move( context.path.value() ) };
	}

	/**
	 * @brief Parse a full GMOD path string with detailed error reporting
	 * @param fullPathStr Full path string starting with root (e.g., "VE/400a/410/411/411i/411.1")
	 * @param gmod GMOD instance to resolve nodes from
	 * @param locations Locations instance to parse location suffixes
	 * @return GmodParsePathResult containing either the parsed GmodPath or detailed error message
	 * @details Direct parsing without tree traversal. Validates parent-child relationships
	 *          and location individualization rules. Provides specific error messages for
	 *          each failure point (empty string, missing root, invalid nodes, bad locations, etc.).
	 */
	inline GmodParsePathResult parseFullGmodPath(
		std::string_view fullPathStr,
		const Gmod& gmod,
		const Locations& locations ) noexcept
	{
		if ( nfx::string::isEmpty( fullPathStr ) )
		{
			return GmodParsePathResult::Error{ "Item is empty" };
		}

		if ( !nfx::string::startsWith( fullPathStr, gmod.rootNode().code() ) )
		{
			return GmodParsePathResult::Error{
				"Path must start with '" + std::string{ gmod.rootNode().code() } + "'" };
		}

		const size_t estimatedSegments = fullPathStr.length() / 3;
		std::vector<GmodNode> nodes;
		nodes.reserve( estimatedSegments > 4
						   ? estimatedSegments
						   : 4 );

		for ( const auto segment : nfx::string::splitView( fullPathStr, '/' ) )
		{
			if ( nfx::string::isEmpty( segment ) )
			{
				continue;
			}

			const auto dashPos = segment.find( '-' );

			if ( dashPos != std::string_view::npos )
			{
				const std::string_view codePart = segment.substr( 0, dashPos );
				const std::string_view locationPart = segment.substr( dashPos + 1 );

				auto nodePtr = gmod.node( codePart );
				if ( !nodePtr.has_value() )
				{
					return GmodParsePathResult::Error{ "Node lookup failed" };
				}

				auto parsedLocationOpt = locations.fromString( locationPart );
				if ( !parsedLocationOpt.has_value() )
				{
					return GmodParsePathResult::Error{
						"Failed to parse location " + std::string{ locationPart } };
				}

				nodes.emplace_back( ( *nodePtr )->withLocation( std::move( parsedLocationOpt.value() ) ) );
			}
			else
			{
				auto nodePtr = gmod.node( segment );
				if ( !nodePtr.has_value() )
				{
					return GmodParsePathResult::Error{ "Node lookup failed" };
				}

				nodes.emplace_back( **nodePtr );
			}
		}
		if ( nodes.empty() )
		{
			return GmodParsePathResult::Error{ "No nodes found" };
		}

		GmodNode endNode = std::move( nodes.back() );
		nodes.pop_back();

		if ( nodes.empty() )
		{
			return GmodParsePathResult::Ok{ GmodPath{ std::vector<GmodNode>{}, std::move( endNode ) } };
		}

		if ( !nodes[0].isRoot() )
		{
			return GmodParsePathResult::Error{ "Sequence of nodes are invalid" };
		}

		if ( !GmodPath::isValid( nodes, endNode ) )
		{
			return GmodParsePathResult::Error{ "Sequence of nodes are invalid" };
		}

		bool hasLocations = endNode.location().has_value();
		for ( const auto& node : nodes )
		{
			if ( node.location().has_value() )
			{
				hasLocations = true;

				break;
			}
		}

		if ( !hasLocations )
		{
			return GmodParsePathResult::Ok{ GmodPath{ std::move( nodes ), std::move( endNode ) } };
		}

		LocationSetsVisitor locationSetsVisitor;
		std::optional<size_t> prevNonNullLocation;

		constexpr size_t MAX_SETS = 16;
		std::pair<size_t, size_t> sets[MAX_SETS];
		size_t setCounter = 0;

		for ( size_t i = 0; i < nodes.size() + 1; ++i )
		{
			const GmodNode& n = ( i < nodes.size() )
									? nodes[i]
									: endNode;

			auto set = locationSetsVisitor.visit( n, i, nodes, endNode );
			if ( !set.has_value() )
			{
				if ( !prevNonNullLocation.has_value() && n.location().has_value() )
				{
					prevNonNullLocation = i;
				}

				continue;
			}

			const auto& [setStart, setEnd, location] = set.value();

			if ( prevNonNullLocation.has_value() )
			{
				for ( size_t j = prevNonNullLocation.value(); j < setStart; ++j )
				{
					const GmodNode& pn = ( j < nodes.size() )
											 ? nodes[j]
											 : endNode;

					if ( pn.location().has_value() )
					{
						return GmodParsePathResult::Error{ "Expected all nodes in the set to be without individualization" };
					}
				}
			}
			prevNonNullLocation = std::nullopt;

			if ( setCounter < MAX_SETS )
			{
				sets[setCounter++] = { setStart, setEnd };
			}

			if ( setStart == setEnd )
			{
				continue;
			}

			if ( !location.has_value() )
			{
				continue;
			}

			for ( size_t j = setStart; j <= setEnd; ++j )
			{
				if ( j < nodes.size() )
				{
					nodes[j] = nodes[j].withLocation( *location );
				}
				else
				{
					endNode = endNode.withLocation( *location );
				}
			}
		}

		std::pair<size_t, size_t> currentSet = { std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::max() };
		size_t currentSetIndex = 0;

		for ( size_t i = 0; i < nodes.size() + 1; ++i )
		{
			while ( currentSetIndex < setCounter && ( currentSet.second == std::numeric_limits<size_t>::max() || currentSet.second < i ) )
			{
				currentSet = sets[currentSetIndex++];
			}

			bool insideSet = ( currentSet.first != std::numeric_limits<size_t>::max() && i >= currentSet.first && i <= currentSet.second );
			const GmodNode& n = ( i < nodes.size() )
									? nodes[i]
									: endNode;

			if ( insideSet )
			{
				const GmodNode& expectedLocationNode = ( currentSet.second < nodes.size() )
														   ? nodes[currentSet.second]
														   : endNode;

				if ( n.location() != expectedLocationNode.location() )
				{
					return GmodParsePathResult::Error{ "Expected all nodes in the set to be individualized the same" };
				}
			}
			else
			{
				if ( n.location().has_value() )
				{
					return GmodParsePathResult::Error{ "Expected all nodes in the set to be without individualization" };
				}
			}
		}

		return GmodParsePathResult::Ok{ GmodPath{ std::move( nodes ), std::move( endNode ) } };
	}
} // namespace dnv::vista::sdk::internal
