/**
 * @file GmodPath.cpp
 * @brief Implementation of GmodPath and related classes for representing paths in the Generic Product Model (GMOD).
 */

#include <queue>

#include <nfx/string/StringBuilderPool.h>
#include <nfx/string/StringViewSplitter.h>
#include <nfx/string/Utils.h>

#include "internal/LocationSetsVisitor.h"
#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/Locations.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
	namespace
	{
		/** @brief Character set for null or whitespace detection in string parsing operations. */
		inline constexpr std::string_view NULL_OR_WHITESPACE = " \t\n\r\f\v";
	}
	namespace internal
	{
		struct PathNode
		{
			std::string_view code;
			std::optional<Location> location;

			PathNode( std::string_view c ) : code{ c }, location{ std::nullopt }
			{
			}

			PathNode( std::string_view c, const Location& loc ) : code{ c }, location{ loc }
			{
			}
		};

		struct ParseContext
		{
			std::queue<PathNode> parts;
			PathNode toFind;
			std::optional<nfx::containers::StringMap<Location>> locations;
			std::optional<GmodPath> path;
			const Gmod* gmod;

			ParseContext( std::queue<PathNode>&& p, PathNode&& t, std::optional<nfx::containers::StringMap<Location>>&& l,
				std::optional<GmodPath>&& path, const Gmod& g )
				: parts{ std::move( p ) },
				  toFind{ std::move( t ) },
				  locations{ std::move( l ) },
				  path{ std::move( path ) },
				  gmod{ &g }
			{
			}
		};

		inline static TraversalHandlerResult parseInternalHandler(
			ParseContext& context, const std::vector<const GmodNode*>& parents, const GmodNode& current )
		{
			PathNode& toFind = context.toFind;
			bool found = ( current.code() == toFind.code );

			if ( !found && Gmod::isLeafNode( current.metadata() ) )
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
					context.locations = nfx::containers::StringMap<Location>{};
				}
				context.locations->emplace( toFind.code, toFind.location.value() );
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
					auto locationIt = context.locations->find( parent->code() );
					if ( locationIt != context.locations->end() )
					{
						pathParents.emplace_back( parent->withLocation( locationIt->second ) );
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
				pathParents.insert( pathParents.begin(), *startNode );
				startNode = startNode->parents()[0];
				if ( startNode->parents().size() > 1 )
				{
					return TraversalHandlerResult::Stop;
				}
			}

			pathParents.insert( pathParents.begin(), context.gmod->rootNode() );

			internal::LocationSetsVisitor visitor;
			std::vector<GmodNode*> tempParentsView;
			tempParentsView.reserve( pathParents.size() + 1 );

			for ( size_t i = 0; i < pathParents.size() + 1; ++i )
			{
				GmodNode& n = ( i < pathParents.size() )
								  ? pathParents[i]
								  : endNode;

				tempParentsView.clear();
				for ( auto& p : pathParents )
				{
					tempParentsView.push_back( &p );
				}

				auto set = visitor.visit( n, i, tempParentsView, endNode );
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

				for ( size_t j = setStart; j <= setEnd; ++j )
				{
					if ( j < pathParents.size() )
					{
						pathParents[j] = pathParents[j].tryWithLocation( location );
					}
					else
					{
						endNode = endNode.tryWithLocation( location );
					}
				}
			}

			context.path = GmodPath( *context.gmod, std::move( endNode ), std::move( pathParents ), true );

			return TraversalHandlerResult::Stop;
		}
	}

	//=====================================================================
	// GmodPath class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	GmodPath::GmodPath( const Gmod& gmod, GmodNode node, std::vector<GmodNode> parents, bool skipVerify )
		: m_visVersion{ node.visVersion() },
		  m_gmod{ &gmod },
		  m_node{ std::move( node ) },
		  m_parents{ std::move( parents ) }
	{
		if ( !m_gmod )
		{
			throw std::invalid_argument{ "GmodPath constructor: gmod reference is null" };
		}

		if ( !m_node.has_value() )
		{
			throw std::invalid_argument{ "GmodPath constructor: node is not valid" };
		}

		if ( skipVerify || m_parents.empty() )
		{
			return;
		}
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	std::vector<GmodIndividualizableSet> GmodPath::individualizableSets() const
	{
		std::vector<GmodIndividualizableSet> result;
		result.reserve( 8 );

		internal::LocationSetsVisitor locationSetsVisitor;
		std::vector<GmodNode*> tempParents;
		tempParents.reserve( m_parents.size() );

		for ( const auto& parent : m_parents )
		{
			tempParents.push_back( const_cast<GmodNode*>( &parent ) );
		}

		for ( size_t i = 0; i < length(); ++i )
		{
			const GmodNode& node = ( i < m_parents.size() )
									   ? m_parents[i]
									   : *m_node;

			auto set = locationSetsVisitor.visit( node, i, tempParents, *m_node );
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

		auto enumerator = this->enumerator();
		while ( enumerator.next() )
		{
			const auto& [depth, nodePtr] = enumerator.current();

			if ( !nodePtr )
			{
				continue;
			}

			const GmodNode& node = *nodePtr;
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
				if ( m_node.has_value() )
				{
					auto nodeCodeIt = normalAssignmentNames.find( m_node->code() );
					if ( nodeCodeIt != normalAssignmentNames.end() )
					{
						name = nodeCodeIt->second;
					}
				}

				if ( !m_parents.empty() && depth < m_parents.size() )
				{
					for ( size_t i = m_parents.size() - 1; i >= depth; --i )
					{
						const GmodNode& parent = m_parents[i];
						auto parentCodeIt = normalAssignmentNames.find( parent.code() );
						if ( parentCodeIt != normalAssignmentNames.end() )
						{
							name = parentCodeIt->second;

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

	//----------------------------------------------
	// State inspection methods
	//----------------------------------------------

	bool GmodPath::isValid( const std::vector<GmodNode*>& parents, const GmodNode& node, int& missingLinkAt )
	{
		missingLinkAt = -1;

		if ( parents.empty() )
		{
			return false;
		}

		if ( !parents.empty() && !parents[0]->isRoot() )
		{
			return false;
		}

		for ( size_t i = 0; i < parents.size(); ++i )
		{
			const GmodNode* parent = parents[i];
			const GmodNode* child = ( i + 1 < parents.size() )
										? parents[i + 1]
										: &node;

			if ( !parent->isChild( *child ) )
			{
				missingLinkAt = static_cast<int>( i );

				return false;
			}
		}

		return true;
	}

	bool GmodPath::isIndividualizable() const
	{
		if ( !m_node.has_value() || length() == 0 )
		{
			return false;
		}

		internal::LocationSetsVisitor locationSetsVisitor;
		std::vector<GmodNode*> tempParents;
		tempParents.reserve( m_parents.size() );

		for ( const auto& parent : m_parents )
		{
			tempParents.push_back( const_cast<GmodNode*>( &parent ) );
		}

		for ( size_t i = 0; i < length(); ++i )
		{
			const GmodNode& node = ( i < m_parents.size() )
									   ? m_parents[i]
									   : *m_node;

			auto set = locationSetsVisitor.visit( node, i, tempParents, *m_node );
			if ( set.has_value() )
			{
				return true;
			}
		}

		return false;
	}

	//----------------------------------------------
	// Path manipulation methods
	//----------------------------------------------

	GmodPath GmodPath::withoutLocations() const
	{
		if ( !m_gmod || !m_node.has_value() )
		{
			return GmodPath{};
		}

		std::vector<GmodNode> newParents;
		newParents.reserve( m_parents.size() );

		for ( const GmodNode& parent : m_parents )
		{
			newParents.push_back( parent.withoutLocation() );
		}

		return GmodPath{ *m_gmod, m_node->withoutLocation(), std::move( newParents ) };
	}

	//----------------------------------------------
	// Private static parsing methods
	//----------------------------------------------

	GmodParsePathResult GmodPath::parseFullPathInternal( std::string_view item, const Gmod& gmod, const Locations& locations )
	{
		if ( item.empty() )
		{
			return GmodParsePathResult::Error{ "Item is empty" };
		}

		if ( !nfx::string::startsWith( item, gmod.rootNode().code() ) )
		{
			return GmodParsePathResult::Error{ "Path must start with {}" + gmod.rootNode().code() };
		}

		const size_t estimatedSegments = item.length() / 3;
		std::vector<GmodNode> nodes;
		nodes.reserve( estimatedSegments );

		for ( const auto& segment : nfx::string::splitView( item, '/' ) )
		{
			if ( segment.empty() )
			{
				continue;
			}

			const auto dashPos = segment.find( '-' );

			if ( dashPos != std::string_view::npos )
			{
				const std::string_view codePart = segment.substr( 0, dashPos );
				const std::string_view locationPart = segment.substr( dashPos + 1 );

				const GmodNode* nodePtr;
				if ( !gmod.tryGetNode( codePart, nodePtr ) )
				{
					return GmodParsePathResult::Error{ "Node lookup failed" };
				}

				Location parsedLocation;
				if ( !locations.tryParse( locationPart, parsedLocation ) )
				{
					return GmodParsePathResult::Error{ "Location parse failed" };
				}

				nodes.emplace_back( nodePtr->withLocation( std::move( parsedLocation ) ) );
			}
			else
			{
				const GmodNode* nodePtr;
				if ( !gmod.tryGetNode( segment, nodePtr ) )
				{
					return GmodParsePathResult::Error{ "Node lookup failed" };
				}

				nodes.emplace_back( std::move( *nodePtr ) );
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
			return GmodParsePathResult::Ok{ GmodPath( gmod, std::move( endNode ), std::move( nodes ), true ) };
		}

		if ( !nodes[0].isRoot() )
		{
			return GmodParsePathResult::Error{ "Sequence of nodes are invalid" };
		}

		int missingLinkAt = -1;
		std::vector<GmodNode*> nodePointers;
		nodePointers.reserve( nodes.size() );
		for ( auto& node : nodes )
		{
			nodePointers.push_back( &node );
		}

		if ( !isValid( nodePointers, endNode, missingLinkAt ) )
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
			return GmodParsePathResult::Ok{ GmodPath( gmod, std::move( endNode ), std::move( nodes ), true ) };
		}

		internal::LocationSetsVisitor locationSetsVisitor;
		std::optional<size_t> prevNonNullLocation;

		constexpr size_t MAX_SETS = 16;
		std::pair<size_t, size_t> sets[MAX_SETS];
		size_t setCounter = 0;

		constexpr size_t MAX_PARENTS = 16;
		GmodNode* tempParents[MAX_PARENTS];
		size_t tempParentsSize = 0;

		thread_local std::vector<GmodNode*> tempParentsView;

		for ( size_t i = 0; i < nodes.size() + 1; ++i )
		{
			const GmodNode& n = ( i < nodes.size() )
									? nodes[i]
									: endNode;

			tempParentsSize = 0;
			for ( auto& node : nodes )
			{
				if ( tempParentsSize < MAX_PARENTS )
				{
					tempParents[tempParentsSize++] = &node;
				}
			}

			tempParentsView.clear();
			tempParentsView.assign( tempParents, tempParents + tempParentsSize );
			auto set = locationSetsVisitor.visit( n, i, tempParentsView, endNode );
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

			for ( size_t j = setStart; j <= setEnd; ++j )
			{
				if ( j < nodes.size() )
				{
					nodes[j] = nodes[j].tryWithLocation( location );
				}
				else
				{
					endNode = endNode.tryWithLocation( location );
				}
			}
		}

		std::pair<size_t, size_t> currentSet = { SIZE_MAX, SIZE_MAX };
		size_t currentSetIndex = 0;

		for ( size_t i = 0; i < nodes.size() + 1; ++i )
		{
			while ( currentSetIndex < setCounter && ( currentSet.second == SIZE_MAX || currentSet.second < i ) )
				currentSet = sets[currentSetIndex++];

			bool insideSet = ( currentSet.first != SIZE_MAX && i >= currentSet.first && i <= currentSet.second );
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

		return GmodParsePathResult::Ok{ GmodPath( gmod, std::move( endNode ), std::move( nodes ), true ) };
	}

	GmodParsePathResult GmodPath::parseInternal( std::string_view item, const Gmod& gmod, const Locations& locations )
	{
		if ( gmod.visVersion() != locations.visVersion() )
		{
			return GmodParsePathResult::Error{ "Got different VIS versions for Gmod and Locations arguments" };
		}

		if ( item.empty() )
		{
			return GmodParsePathResult::Error{ "Item is empty" };
		}

		const size_t start = item.find_first_not_of( NULL_OR_WHITESPACE );
		if ( start == std::string_view::npos )
		{
			return GmodParsePathResult::Error{ "Item is empty" };
		}

		const size_t end = item.find_last_not_of( NULL_OR_WHITESPACE ) + 1;
		item = item.substr( start, end - start );

		if ( !item.empty() && item[0] == '/' )
		{
			item = item.substr( 1 );
		}

		std::queue<internal::PathNode> parts;

		for ( const auto& partStr : nfx::string::splitView( item, '/' ) )
		{
			if ( partStr.empty() )
				continue;

			if ( partStr.find( '-' ) != std::string_view::npos )
			{
				const size_t dashPos = partStr.find( '-' );
				const std::string_view codePart = partStr.substr( 0, dashPos );
				const std::string_view locationPart = partStr.substr( dashPos + 1 );

				const GmodNode* tempNode = nullptr;
				if ( !gmod.tryGetNode( codePart, tempNode ) || !tempNode )
				{
					return GmodParsePathResult::Error{ "Failed to get GmodNode for " + std::string{ partStr } };
				}

				Location parsedLocation;
				if ( !locations.tryParse( locationPart, parsedLocation ) )
				{
					return GmodParsePathResult::Error{ "Failed to parse location " + std::string{ locationPart } };
				}

				parts.emplace( codePart, parsedLocation );
			}
			else
			{
				const GmodNode* tempNode = nullptr;
				if ( !gmod.tryGetNode( partStr, tempNode ) || !tempNode )
				{
					return GmodParsePathResult::Error{ "Failed to get GmodNode for " + std::string{ partStr } };
				}

				parts.emplace( partStr );
			}
		}

		if ( parts.empty() )
		{
			return GmodParsePathResult::Error{ "Failed find any parts" };
		}

		internal::PathNode toFind = parts.front();
		parts.pop();

		const GmodNode* baseNode = nullptr;
		if ( !gmod.tryGetNode( toFind.code, baseNode ) || !baseNode )
		{
			return GmodParsePathResult::Error{ "Failed to find base node" };
		}

		internal::ParseContext context( std::move( parts ), std::move( toFind ), std::nullopt, std::nullopt, gmod );

		TraverseHandlerWithState<internal::ParseContext> handler = internal::parseInternalHandler;
		gmod.traverse( context, *baseNode, handler );

		if ( !context.path.has_value() )
		{
			return GmodParsePathResult::Error{ "Failed to find path after traversal" };
		}

		return GmodParsePathResult::Ok{ std::move( context.path.value() ) };
	}

	//----------------------------------------------
	// GmodPath::enumerator
	//----------------------------------------------

	//----------------------------
	// Construction
	//----------------------------

	GmodPath::Enumerator::Enumerator( const GmodPath* pathInst, size_t startIndex )
		: m_pathInstance{ pathInst }, m_currentIndex{ std::numeric_limits<size_t>::max() }, m_current{ std::numeric_limits<size_t>::max(), nullptr }
	{
		if ( startIndex != std::numeric_limits<size_t>::max() )
		{
			if ( startIndex > 0 )
			{
				m_currentIndex = startIndex - 1;
			}
		}
	}

	//=====================================================================
	// GmodIndividualizableSet class
	//=====================================================================

	GmodIndividualizableSet::GmodIndividualizableSet( const std::vector<int>& nodeIndices, const GmodPath& sourcePath )
		: m_nodeIndices{ nodeIndices }, m_path{ sourcePath }, m_isBuilt{ false }
	{
		if ( m_nodeIndices.empty() )
		{
			throw std::invalid_argument{ "GmodIndividualizableSet constructor: nodeIndices cannot be empty." };
		}

		for ( int nodeIdx : m_nodeIndices )
		{
			if ( static_cast<size_t>( nodeIdx ) >= m_path.length() || nodeIdx < 0 )
			{
				throw std::out_of_range{
					"GmodIndividualizableSet constructor: Node index " + std::to_string( nodeIdx ) +
					" is out of bounds for path length " + std::to_string( m_path.length() ) + "." };
			}

			const GmodNode& currentNode = m_path[static_cast<size_t>( nodeIdx )];

			bool isTargetNode = ( static_cast<size_t>( nodeIdx ) == m_path.length() - 1 );
			bool isInSet = ( m_nodeIndices.size() > 1 );
			if ( !currentNode.isIndividualizable( isTargetNode, isInSet ) )
			{
				throw std::invalid_argument{ "GmodIndividualizableSet constructor: Node '" + std::string{ currentNode.code().data() } + "' (at index " +
											 std::to_string( nodeIdx ) + ") is not individualizable in the given context." };
			}
		}

		if ( m_nodeIndices.size() > 1 )
		{
			const GmodNode& firstNode = m_path[static_cast<size_t>( m_nodeIndices[0] )];
			std::optional<Location> expectedLocation = firstNode.location();

			for ( size_t i = 1; i < m_nodeIndices.size(); ++i )
			{
				int currentIdx = m_nodeIndices[i];
				const GmodNode& currentNode = m_path[static_cast<size_t>( currentIdx )];
				if ( currentNode.location() != expectedLocation )
				{
					throw std::invalid_argument{
						"GmodIndividualizableSet constructor: Nodes have different locations. Node '" +
						std::string{ currentNode.code().data() } + "' (at index " + std::to_string( currentIdx ) +
						") has location while first node in set had different or no location." };
				}
			}
		}

		bool foundPartOfShortPath = false;
		for ( int nodeIdx : m_nodeIndices )
		{
			const GmodNode& currentNode = m_path[static_cast<size_t>( nodeIdx )];
			if ( currentNode == m_path.node() || currentNode.isLeafNode() )
			{
				foundPartOfShortPath = true;

				break;
			}
		}

		if ( !foundPartOfShortPath )
		{
			throw std::invalid_argument{
				"GmodIndividualizableSet constructor: No nodes in the set are part of the short path (final node or leaf node)." };
		}
	}

	//----------------------------------------------
	// Build
	//----------------------------------------------

	GmodPath GmodIndividualizableSet::build()
	{
		if ( m_isBuilt )
		{
			throw std::runtime_error{ "Tried to build individualizable set twice" };
		}

		m_isBuilt = true;

		return std::move( m_path );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	std::vector<GmodNode*> GmodIndividualizableSet::nodes() const
	{
		std::vector<GmodNode*> result;
		result.reserve( m_nodeIndices.size() );

		for ( int nodeIdx : m_nodeIndices )
		{
			if ( nodeIdx < 0 || static_cast<size_t>( nodeIdx ) >= m_path.length() )
			{
				throw std::out_of_range{ "Node index out of bounds" };
			}

			result.push_back( const_cast<GmodNode*>( &m_path[static_cast<size_t>( nodeIdx )] ) );
		}

		return result;
	}

	void GmodIndividualizableSet::setLocation( const std::optional<Location>& location )
	{
		for ( size_t i = 0; i < m_nodeIndices.size(); ++i )
		{
			int nodeIdx = m_nodeIndices[i];

			if ( nodeIdx < 0 || static_cast<size_t>( nodeIdx ) >= m_path.length() )
			{
				continue;
			}

			GmodNode& nodeInPath = const_cast<GmodPath&>( m_path )[static_cast<size_t>( nodeIdx )];

			if ( !location.has_value() )
			{
				nodeInPath = nodeInPath.withoutLocation();
			}
			else
			{
				nodeInPath = nodeInPath.withLocation( location.value() );
			}
		}
	}

	std::string GmodIndividualizableSet::toString() const
	{
		auto lease = nfx::string::StringBuilderPool::lease();
		bool firstNodeAppended = false;

		for ( size_t j = 0; j < m_nodeIndices.size(); ++j )
		{
			int nodeIdx = m_nodeIndices[j];
			if ( nodeIdx < 0 || static_cast<size_t>( nodeIdx ) >= m_path.length() )
			{
				continue;
			}

			const GmodNode& currentNode = m_path[static_cast<size_t>( nodeIdx )];

			if ( currentNode.isLeafNode() || j == m_nodeIndices.size() - 1 )
			{
				if ( firstNodeAppended )
				{
					lease.builder().push_back( '/' );
				}

				lease.builder().append( currentNode.toString() );
				firstNodeAppended = true;
			}
		}

		return lease.toString();
	}
}
