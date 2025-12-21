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
 * @file GmodPath.cpp
 * @brief Implementation of GmodPath class methods
 * @details Provides constructor, validation logic, and path manipulation methods
 */

#include "dnv/vista/sdk/GmodPath.h"

#include "dnv/vista/sdk/VIS.h"
#include "parsing/GmodPathParsing.h"

#include <stdexcept>
#include <unordered_set>

namespace dnv::vista::sdk
{
	GmodPath::GmodPath( std::vector<GmodNode>&& parents, GmodNode node, bool skipVerify )
		: m_visVersion{ node.version() },
		  m_parents{ std::move( parents ) },
		  m_node{ std::move( node ) }
	{
		if ( !skipVerify )
		{
			// Validate that we have at least one parent
			if ( m_parents.empty() )
			{
				throw std::invalid_argument( "Invalid gmod path - no parents, and " + std::string( m_node.code() ) +
											 " is not the root of gmod" );
			}

			// Validate that the first parent is the root node (VE)
			if ( !m_parents.empty() && !m_parents[0].isRoot() )
			{
				throw std::invalid_argument( "Invalid gmod path - first parent should be root of gmod (VE), but was " +
											 std::string( m_parents[0].code() ) );
			}

			// Validate parent-child relationships
			std::unordered_set<std::string> codeSet;
			codeSet.insert( "VE" );

			for ( size_t i = 0; i < m_parents.size(); ++i )
			{
				const auto& parent = m_parents[i];
				const auto nextIndex = i + 1;
				const auto& child = nextIndex < m_parents.size() ? m_parents[nextIndex] : m_node;

				if ( !parent.isChild( child ) )
				{
					throw std::invalid_argument( "Invalid gmod path - " + std::string( child.code() ) +
												 " not child of " + std::string( parent.code() ) );
				}

				// if (!codeSet.insert(std::string(child.code())).second)
				// {
				//     throw std::invalid_argument("Recursion in gmod path argument for code: " +
				//     std::string(child.code()));
				// }
			}

			// Visit all nodes with LocationSetsVisitor to validate individualizable sets
			internal::LocationSetsVisitor visitor;
			for ( size_t i = 0; i < m_parents.size() + 1; ++i )
			{
				const auto& n = i < m_parents.size() ? m_parents[i] : m_node;
				auto _ = visitor.visit( n, i, m_parents, m_node );
			}
		}
	}

	GmodPath::GmodPath( std::vector<GmodNode>&& parents, GmodNode node )
		: GmodPath{ std::move( parents ), std::move( node ), false }
	{
	}

	bool GmodPath::isValid( const std::vector<GmodNode>& parents, const GmodNode& node )
	{
		// Validate that we have at least one parent
		if ( parents.empty() )
		{
			return false;
		}

		// Validate that the first parent is the root node (VE)
		if ( !parents[0].isRoot() )
		{
			return false;
		}

		// Validate parent-child relationships
		for ( size_t i = 0; i < parents.size(); ++i )
		{
			const auto& parent = parents[i];
			const auto nextIndex = i + 1;
			const auto& child = nextIndex < parents.size() ? parents[nextIndex] : node;

			if ( !parent.isChild( child ) )
			{
				return false;
			}
		}

		return true;
	}

	bool GmodPath::isIndividualizable() const
	{
		if ( length() == 0 )
		{
			return false;
		}

		internal::LocationSetsVisitor locationSetsVisitor;

		for ( size_t i = 0; i < length(); ++i )
		{
			const GmodNode& node = ( i < m_parents.size() ) ? m_parents[i] : m_node;

			auto set = locationSetsVisitor.visit( node, i, m_parents, m_node );
			if ( set.has_value() )
			{
				return true;
			}
		}

		return false;
	}

	GmodPath GmodPath::withoutLocations() const
	{
		std::vector<GmodNode> newParents;
		newParents.reserve( m_parents.size() );

		for ( const auto& parent : m_parents )
		{
			newParents.push_back( parent.withoutLocation() );
		}

		GmodNode newNode = m_node.withoutLocation();

		return GmodPath{ std::move( newParents ), std::move( newNode ), true };
	}

	std::string GmodPath::toFullPathString() const
	{
		StringBuilder builder;
		toFullPathString( builder );
		return builder.toString();
	}

	std::vector<GmodIndividualizableSet> GmodPath::individualizableSets() const
	{
		std::vector<GmodIndividualizableSet> result;
		result.reserve( 8 );

		internal::LocationSetsVisitor locationSetsVisitor;

		for ( size_t i = 0; i < length(); ++i )
		{
			const GmodNode& node = ( i < m_parents.size() ) ? m_parents[i] : m_node;

			auto set = locationSetsVisitor.visit( node, i, m_parents, m_node );
			if ( !set.has_value() )
			{
				continue;
			}

			const auto& [startIdx, endIdx, location] = set.value();

			if ( startIdx == endIdx )
			{
				result.emplace_back( std::vector<int>{ static_cast<int>( startIdx ) }, *this );

				continue;
			}

			std::vector<int> nodes;
			nodes.reserve( endIdx - startIdx + 1 );
			for ( size_t j = startIdx; j <= endIdx; ++j )
			{
				nodes.push_back( static_cast<int>( j ) );
			}

			result.emplace_back( std::move( nodes ), *this );
		}

		return result;
	}

	std::optional<std::string> GmodPath::normalAssignmentName( size_t nodeDepth ) const
	{
		if ( nodeDepth >= length() )
		{
			return std::nullopt;
		}

		const GmodNode& nodeInPath = ( *this )[nodeDepth];

		const auto& normalAssignmentNames = nodeInPath.metadata().normalAssignmentNames();
		if ( normalAssignmentNames.empty() )
		{
			return std::nullopt;
		}

		if ( length() == 0 )
		{
			return std::nullopt;
		}

		for ( size_t i = length() - 1;; --i )
		{
			const GmodNode& child = ( *this )[i];
			auto it = normalAssignmentNames.find( child.code() );
			if ( it != normalAssignmentNames.end() )
			{
				return it->second;
			}

			if ( i == 0 )
			{
				break;
			}
		}

		return std::nullopt;
	}

	std::vector<std::pair<size_t, std::string>> GmodPath::commonNames() const
	{
		std::vector<std::pair<size_t, std::string>> result;

		for ( const auto& [depth, node] : fullPath() )
		{
			bool isTarget = ( depth == m_parents.size() );

			if ( !( node.isLeafNode() || isTarget ) || !node.isFunctionNode() )
			{
				continue;
			}

			std::string name;
			const auto& commonName = node.metadata().commonName();
			if ( commonName.has_value() && !commonName->empty() )
			{
				name = *commonName;
			}
			else
			{
				name = node.metadata().name();
			}

			const auto& normalAssignmentNames = node.metadata().normalAssignmentNames();

			if ( !normalAssignmentNames.empty() )
			{
				// Check against the target node code
				auto it = normalAssignmentNames.find( m_node.code() );
				if ( it != normalAssignmentNames.end() )
				{
					name = it->second;
				}

				// Check against parent codes from bottom to top
				if ( !m_parents.empty() && depth < m_parents.size() )
				{
					for ( size_t i = m_parents.size() - 1; i >= depth; --i )
					{
						const GmodNode& parent = m_parents[i];
						auto parentIt = normalAssignmentNames.find( parent.code() );
						if ( parentIt != normalAssignmentNames.end() )
						{
							name = parentIt->second;
							break;
						}

						if ( i == depth )
						{
							break;
						}
					}
				}
			}

			result.emplace_back( depth, name );
		}

		return result;
	}

	std::optional<GmodPath> GmodPath::fromString( std::string_view item, VisVersion visVersion ) noexcept
	{
		try
		{
			const auto& vis = VIS::instance();
			const auto& gmod = vis.gmod( visVersion );
			const auto& locations = vis.locations( visVersion );
			return fromString( item, gmod, locations );
		}
		catch ( ... )
		{
			return std::nullopt;
		}
	}

	std::optional<GmodPath> GmodPath::fromString( std::string_view item, const Gmod& gmod, const Locations& locations ) noexcept
	{
		auto result = internal::parseGmodPath( item, gmod, locations );
		if ( result.isOk() )
		{
			return std::move( result.ok().path );
		}
		return std::nullopt;
	}

	std::optional<GmodPath> GmodPath::fromString( std::string_view item, const Gmod& gmod, const Locations& locations, ParsingErrors& errors ) noexcept
	{
		auto result = internal::parseGmodPath( item, gmod, locations );

		if ( result.isOk() )
		{
			return std::move( result.ok().path );
		}

		std::vector<ParsingErrors::ErrorEntry> errorEntries;
		errorEntries.push_back( { "GmodPath.Parse", result.error().error } );
		errors = ParsingErrors( std::move( errorEntries ) );

		return std::nullopt;
	}

	std::optional<GmodPath> GmodPath::fromFullPathString( std::string_view fullPathStr, const Gmod& gmod, const Locations& locations ) noexcept
	{
		auto result = internal::parseFullGmodPath( fullPathStr, gmod, locations );
		if ( result.isOk() )
		{
			return std::move( result.ok().path );
		}
		return std::nullopt;
	}

	std::optional<GmodPath> GmodPath::fromFullPathString( std::string_view fullPathStr, const Gmod& gmod, const Locations& locations, ParsingErrors& errors ) noexcept
	{
		auto result = internal::parseFullGmodPath( fullPathStr, gmod, locations );

		if ( result.isOk() )
		{
			return std::move( result.ok().path );
		}

		std::vector<ParsingErrors::ErrorEntry> errorEntries;
		errorEntries.push_back( { "GmodPath.Parse", result.error().error } );
		errors = ParsingErrors( std::move( errorEntries ) );

		return std::nullopt;
	}

	GmodIndividualizableSet::GmodIndividualizableSet( const std::vector<int>& nodeIndices, const GmodPath& sourcePath )
		: m_nodeIndices{ nodeIndices },
		  m_path{ sourcePath }
	{
		if ( m_nodeIndices.empty() )
		{
			throw std::runtime_error( "GmodIndividualizableSet cant be empty" );
		}

		// Validate that all nodes are individualizable
		for ( int i : m_nodeIndices )
		{
			const auto& node = sourcePath[static_cast<size_t>( i )];
			bool isTargetNode = ( static_cast<size_t>( i ) == sourcePath.length() - 1 );
			bool isInSet = ( m_nodeIndices.size() > 1 );

			if ( !node.isIndividualizable( isTargetNode, isInSet ) )
			{
				throw std::runtime_error( "GmodIndividualizableSet nodes must be individualizable" );
			}
		}

		// Validate that all nodes have the same location
		if ( m_nodeIndices.size() > 1 )
		{
			const auto& firstLocation = sourcePath[static_cast<size_t>( m_nodeIndices[0] )].location();

			for ( size_t i = 1; i < m_nodeIndices.size(); ++i )
			{
				const auto& nodeLocation = sourcePath[static_cast<size_t>( m_nodeIndices[i] )].location();
				if ( firstLocation != nodeLocation )
				{
					throw std::runtime_error( "GmodIndividualizableSet nodes have different locations" );
				}
			}
		}

		// Validate that at least one node is part of the short path (is leaf or target)
		bool hasShortPathNode = false;
		for ( int i : m_nodeIndices )
		{
			const auto& node = sourcePath[static_cast<size_t>( i )];
			bool isTarget = ( static_cast<size_t>( i ) == sourcePath.length() - 1 );

			if ( node.isLeafNode() || isTarget )
			{
				hasShortPathNode = true;
				break;
			}
		}

		if ( !hasShortPathNode )
		{
			throw std::runtime_error( "GmodIndividualizableSet has no nodes that are part of short path" );
		}
	}

	GmodPath GmodIndividualizableSet::build()
	{
		if ( !m_path.has_value() )
		{
			throw std::runtime_error{ "Tried to build individualizable set twice" };
		}

		GmodPath result = std::move( *m_path );
		m_path.reset();

		return result;
	}

	std::vector<GmodNode> GmodIndividualizableSet::nodes() const
	{
		if ( !m_path.has_value() )
		{
			throw std::runtime_error( "GmodIndividualizableSet has already been built" );
		}

		std::vector<GmodNode> result;
		result.reserve( m_nodeIndices.size() );

		for ( int nodeIdx : m_nodeIndices )
		{
			if ( nodeIdx < 0 || static_cast<size_t>( nodeIdx ) >= m_path->length() )
			{
				throw std::out_of_range{ "Node index out of bounds" };
			}

			result.push_back( ( *m_path )[static_cast<size_t>( nodeIdx )] );
		}

		return result;
	}

	std::string GmodIndividualizableSet::toString() const
	{
		if ( !m_path.has_value() )
		{
			throw std::runtime_error( "GmodIndividualizableSet has already been built" );
		}

		auto sb = StringBuilder{};
		bool firstNodeAppended = false;

		for ( size_t j = 0; j < m_nodeIndices.size(); ++j )
		{
			int nodeIdx = m_nodeIndices[j];
			if ( nodeIdx < 0 || static_cast<size_t>( nodeIdx ) >= m_path->length() )
			{
				continue;
			}

			const GmodNode& currentNode = ( *m_path )[static_cast<size_t>( nodeIdx )];

			if ( currentNode.isLeafNode() || j == m_nodeIndices.size() - 1 )
			{
				if ( firstNodeAppended )
				{
					sb.append( '/' );
				}

				sb.append( currentNode.toString() );
				firstNodeAppended = true;
			}
		}

		return sb.toString();
	}
} // namespace dnv::vista::sdk
