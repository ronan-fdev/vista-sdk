/**
 * @file GmodPath.cpp
 * @brief Implementation of GmodPath and related classes for representing paths in the Generic Product Model (GMOD).
 */

#include "pch.h"

#include "dnv/vista/sdk/GmodPath.h"

#include "dnv/vista/sdk/GmodTraversal.h"
#include "dnv/vista/sdk/Locations.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
	namespace internal
	{
		struct LocationSetsVisitor
		{
			size_t currentParentStart;

			LocationSetsVisitor()
				: currentParentStart{ std::numeric_limits<size_t>().max() } {}

			std::optional<std::tuple<size_t, size_t, std::optional<Location>>> visit(
				const GmodNode& node,
				size_t i,
				const std::vector<GmodNode*>& pathParents,
				const GmodNode& pathTargetNode )
			{
				bool isParent = Gmod::isPotentialParent( node.metadata().type() );
				bool isTargetNode = ( static_cast<size_t>( i ) == pathParents.size() );

				if ( currentParentStart == std::numeric_limits<size_t>().max() )
				{
					if ( isParent )
					{
						currentParentStart = i;
					}
					if ( node.isIndividualizable( isTargetNode ) )
					{
						return std::make_tuple( i, i, node.location() );
					}
				}
				else
				{
					if ( isParent || isTargetNode )
					{
						std::optional<std::tuple<size_t, size_t, std::optional<Location>>> nodes = std::nullopt;

						if ( currentParentStart + 1 == i )
						{
							if ( node.isIndividualizable( isTargetNode ) )
								nodes = std::make_tuple( i, i, node.location() );
						}
						else
						{
							size_t skippedOne = std::numeric_limits<size_t>().max();
							bool hasComposition = false;

							for ( size_t j = currentParentStart + 1; j <= i; ++j )
							{
								const GmodNode* setNode = ( j < pathParents.size() ) ? pathParents[j] : &pathTargetNode;

								if ( !setNode->isIndividualizable( j == pathParents.size(), true ) )
								{
									if ( nodes.has_value() )
									{
										skippedOne = j;
									}

									continue;
								}

								if ( nodes.has_value() &&
									 std::get<2>( nodes.value() ).has_value() &&
									 setNode->location().has_value() &&
									 std::get<2>( nodes.value() ) != setNode->location() )
								{
									throw std::runtime_error( "Mapping error: different locations in the same nodeset" );
								}

								if ( skippedOne != std::numeric_limits<size_t>().max() )
								{
									throw std::runtime_error( "Can't skip in the middle of individualizable set" );
								}

								if ( setNode->isFunctionComposition() )
								{
									hasComposition = true;
								}

								auto location = nodes.has_value() && std::get<2>( nodes.value() ).has_value() ? std::get<2>( nodes.value() ) : setNode->location();
								size_t start = nodes.has_value() ? std::get<0>( nodes.value() ) : j;
								size_t end = j;
								nodes = std::make_tuple( start, end, location );
							}

							if ( nodes.has_value() && std::get<0>( nodes.value() ) == std::get<1>( nodes.value() ) && hasComposition )
							{
								nodes = std::nullopt;
							}
						}

						currentParentStart = i;
						if ( nodes.has_value() )
						{
							bool hasLeafNode = false;
							size_t startIdx = std::get<0>( nodes.value() );
							size_t endIdx = std::get<1>( nodes.value() );

							for ( size_t j = startIdx; j <= endIdx; ++j )
							{
								const GmodNode* setNode = ( j < pathParents.size() ) ? pathParents[j] : &pathTargetNode;
								if ( setNode->isLeafNode() || j == pathParents.size() )
								{
									hasLeafNode = true;
									break;
								}
							}
							if ( hasLeafNode )
							{
								return nodes;
							}
						}
					}

					if ( isTargetNode && node.isIndividualizable( isTargetNode ) )
					{
						return std::make_tuple( i, i, node.location() );
					}
				}

				return std::nullopt;
			}
		};
	}

	//=====================================================================
	// GmodPath class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	GmodPath::GmodPath( const Gmod& gmod, GmodNode node, std::vector<GmodNode> parents )
		: m_visVersion{ node.visVersion() },
		  m_gmod{ &gmod },
		  m_node{ std::move( node ) },
		  m_parents{ std::move( parents ) }
	{
		if ( !m_gmod )
		{
			throw std::invalid_argument( "GmodPath constructor: gmod reference is null" );
		}

		if ( !m_node.has_value() )
		{
			throw std::invalid_argument( "GmodPath constructor: node is not valid" );
		}

		if ( m_parents.empty() )
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

		internal::LocationSetsVisitor visitor;
		std::vector<GmodNode*> tempParents;
		tempParents.reserve( m_parents.size() );

		for ( const auto& parent : m_parents )
		{
			tempParents.push_back( const_cast<GmodNode*>( &parent ) );
		}

		for ( size_t i = 0; i < length(); ++i )
		{
			const GmodNode& node = ( i < m_parents.size() ) ? m_parents[i] : *m_node;

			auto set = visitor.visit( node, i, tempParents, *m_node );
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
			const GmodNode* child = ( i + 1 < parents.size() ) ? parents[i + 1] : &node;

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

		internal::LocationSetsVisitor visitor;
		std::vector<GmodNode*> tempParents;
		tempParents.reserve( m_parents.size() );

		for ( const auto& parent : m_parents )
		{
			tempParents.push_back( const_cast<GmodNode*>( &parent ) );
		}

		for ( size_t i = 0; i < length(); ++i )
		{
			const GmodNode& node = ( i < m_parents.size() ) ? m_parents[i] : *m_node;

			auto set = visitor.visit( node, i, tempParents, *m_node );
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

		return GmodPath( *m_gmod, m_node->withoutLocation(), std::move( newParents ) );
	}

	//----------------------------------------------
	// Private static parsing methods
	//----------------------------------------------

	GmodParsePathResult GmodPath::parseFullPathInternal(
		std::string_view item, const Gmod& gmod, const Locations& locations )
	{
		constexpr size_t MAX_NODES = 32;

		const size_t estimatedSegments = [item]() -> size_t {
			size_t count = 1;
			for ( char c : item )
			{
				if ( c == '/' )
					++count;
			}
			return count;
		}();

		std::vector<GmodNode> nodes;
		nodes.reserve( std::min( estimatedSegments, MAX_NODES ) );

		std::string_view remaining = item;

		while ( !remaining.empty() && nodes.size() < MAX_NODES )
		{
			const auto slashPos = remaining.find( '/' );
			const std::string_view segment = remaining.substr( 0, slashPos );

			const auto dashPos = segment.find( '-' );

			if ( dashPos != std::string_view::npos )
			{
				const std::string_view codePart = segment.substr( 0, dashPos );
				const std::string_view locationPart = segment.substr( dashPos + 1 );

				const GmodNode* nodePtr;
				if ( !gmod.tryGetNode( codePart, nodePtr ) )
				{
					return GmodParsePathResult::Error( "Node lookup failed" );
				}

				Location parsedLocation;
				if ( !locations.tryParse( locationPart, parsedLocation ) )
				{
					return GmodParsePathResult::Error( "Location parse failed" );
				}

				nodes.emplace_back( nodePtr->withLocation( parsedLocation ) );
			}
			else
			{
				const GmodNode* nodePtr;
				if ( !gmod.tryGetNode( segment, nodePtr ) )
				{
					return GmodParsePathResult::Error( "Node lookup failed" );
				}

				nodes.emplace_back( *nodePtr );
			}

			if ( slashPos == std::string_view::npos )
			{
				break;
			}
			remaining = remaining.substr( slashPos + 1 );
		}

		if ( nodes.empty() )
		{
			return GmodParsePathResult::Error( "No nodes found" );
		}

		GmodNode endNode = std::move( nodes.back() );
		nodes.pop_back();

		return GmodParsePathResult::Ok(
			GmodPath( gmod, std::move( endNode ), std::move( nodes ) ) );
	}

	GmodParsePathResult GmodPath::parseInternal( std::string_view item, const Gmod& gmod, const Locations& locations )
	{
		if ( gmod.visVersion() != locations.visVersion() )
		{
			return GmodParsePathResult::Error( "Got different VIS versions for Gmod and Locations arguments" );
		}

		if ( item.empty() )
		{
			return GmodParsePathResult::Error( "Item is empty" );
		}

		size_t start = item.find_first_not_of( " \t\n\r\f\v" );
		if ( start == std::string_view::npos )
		{
			return GmodParsePathResult::Error( "Item is empty" );
		}

		item = item.substr( start );
		size_t end = item.find_last_not_of( " \t\n\r\f\v" );
		item = item.substr( 0, end + 1 );

		if ( !item.empty() && item.front() == '/' )
		{
			item.remove_prefix( 1 );
		}

		if ( item.empty() )
		{
			return GmodParsePathResult::Error( "Item is empty" );
		}

		struct PathNode
		{
			std::string_view code;
			std::optional<Location> location;
		};

		std::vector<PathNode> parts;

		std::string_view remaining = item;
		while ( !remaining.empty() )
		{
			size_t slashPos = remaining.find( '/' );
			std::string_view part = remaining.substr( 0, slashPos );

			if ( part.empty() )
			{
				return GmodParsePathResult::Error( "Failed find any parts" );
			}

			PathNode pathNode;
			size_t dashPos = part.find( '-' );

			if ( dashPos != std::string_view::npos )
			{
				std::string_view codePart = part.substr( 0, dashPos );
				std::string_view locationPart = part.substr( dashPos + 1 );

				pathNode.code = codePart;

				const GmodNode* tempNode = nullptr;
				if ( !gmod.tryGetNode( codePart, tempNode ) || !tempNode )
				{
					return GmodParsePathResult::Error(
						fmt::format( "Failed to get GmodNode for {}", std::string{ codePart } ) );
				}

				Location parsedLocation;
				if ( !locations.tryParse( locationPart, parsedLocation ) )
				{
					return GmodParsePathResult::Error(
						fmt::format( "Failed to parse location {}", std::string{ locationPart } ) );
				}

				pathNode.location = parsedLocation;
			}
			else
			{
				pathNode.code = part;

				const GmodNode* tempNode = nullptr;
				if ( !gmod.tryGetNode( part, tempNode ) || !tempNode )
				{
					return GmodParsePathResult::Error(
						fmt::format( "Failed to get GmodNode for {}", std::string{ part } ) );
				}
			}

			parts.push_back( std::move( pathNode ) );

			if ( slashPos == std::string_view::npos )
			{
				break;
			}
			remaining = remaining.substr( slashPos + 1 );
		}

		if ( parts.empty() )
		{
			return GmodParsePathResult::Error( "Failed find any parts" );
		}

		const GmodNode* baseNode = nullptr;
		if ( !gmod.tryGetNode( parts[0].code, baseNode ) || !baseNode )
		{
			return GmodParsePathResult::Error( "Failed to find base node" );
		}

		struct ParseContext
		{
			std::vector<PathNode> parts;
			size_t currentPartIndex = 0;
			StringMap<Location> locationMap;
			std::vector<GmodNode> foundPathParents;
			std::optional<GmodNode> foundEndNode;
			bool pathFound = false;
			const Gmod* gmod = nullptr;
		};

		ParseContext context;
		context.parts = std::move( parts );
		context.gmod = &gmod;

		for ( const auto& part : context.parts )
		{
			if ( part.location.has_value() )
			{
				context.locationMap[part.code] = part.location.value();
			}
		}

		auto handler = []( ParseContext& ctx, const std::vector<const GmodNode*>& parents, const GmodNode& current ) -> TraversalHandlerResult {
			const auto& targetCode = ctx.parts[ctx.currentPartIndex].code;
			bool found = ( current.code() == targetCode );

			if ( !found && Gmod::isLeafNode( current.metadata() ) )
			{
				return TraversalHandlerResult::SkipSubtree;
			}

			if ( !found )
			{
				return TraversalHandlerResult::Continue;
			}

			GmodNode nodeWithLocation = current;
			if ( ctx.locationMap.find( current.code() ) != ctx.locationMap.end() )
			{
				nodeWithLocation = current.withLocation( ctx.locationMap[current.code()] );
			}

			ctx.currentPartIndex++;

			if ( ctx.currentPartIndex >= ctx.parts.size() )
			{
				std::vector<GmodNode> pathParents;
				pathParents.reserve( parents.size() + 1 );

				for ( const GmodNode* parent : parents )
				{
					GmodNode parentWithLocation = *parent;
					if ( ctx.locationMap.find( parent->code() ) != ctx.locationMap.end() )
					{
						parentWithLocation = parent->withLocation( ctx.locationMap[parent->code()] );
					}
					pathParents.push_back( parentWithLocation );
				}

				GmodNode endNode = nodeWithLocation;

				std::optional<GmodNode> startNodeOpt;
				if ( pathParents.size() > 0 )
				{
					const auto& parentsOfFirst = pathParents[0].parents();
					if ( parentsOfFirst.size() == 1 )
					{
						startNodeOpt = *parentsOfFirst[0];
					}
				}
				else
				{
					const auto& endParents = endNode.parents();
					if ( endParents.size() == 1 )
					{
						startNodeOpt = *endParents[0];
					}
				}

				if ( !startNodeOpt.has_value() )
				{
					return TraversalHandlerResult::Stop;
				}

				const auto& startNodeParents = startNodeOpt->parents();
				if ( startNodeParents.size() > 1 )
				{
					return TraversalHandlerResult::Stop;
				}

				std::vector<GmodNode> reverseHierarchy;

				GmodNode currentNode = *startNodeOpt;
				auto currentParents = currentNode.parents();
				while ( currentParents.size() == 1 )
				{
					GmodNode nodeToAdd = currentNode;
					if ( ctx.locationMap.find( currentNode.code() ) != ctx.locationMap.end() )
					{
						nodeToAdd = currentNode.withLocation( ctx.locationMap[currentNode.code()] );
					}
					reverseHierarchy.push_back( nodeToAdd );

					currentNode = *currentParents[0];
					currentParents = currentNode.parents();

					if ( currentParents.size() > 1 )
					{
						return TraversalHandlerResult::Stop;
					}
				}

				std::reverse( reverseHierarchy.begin(), reverseHierarchy.end() );
				reverseHierarchy.insert( reverseHierarchy.begin(), ctx.gmod->rootNode() );
				reverseHierarchy.insert( reverseHierarchy.end(), pathParents.begin(), pathParents.end() );

				ctx.foundPathParents = std::move( reverseHierarchy );
				ctx.foundEndNode = std::move( endNode );
				ctx.pathFound = true;
				return TraversalHandlerResult::Stop;
			}

			return TraversalHandlerResult::Continue;
		};

		TraverseHandlerWithState<ParseContext> handlerPtr = handler;
		TraversalOptions options;
		GmodTraversal::traverse( context, *baseNode, handlerPtr, options );

		if ( !context.pathFound )
		{
			return GmodParsePathResult::Error( "Failed to find path after traversal" );
		}

		internal::LocationSetsVisitor locationVisitor;

		std::vector<GmodNode*> tempParents;
		tempParents.reserve( context.foundPathParents.size() );
		for ( GmodNode& parent : context.foundPathParents )
		{
			tempParents.push_back( &parent );
		}

		for ( size_t i = 0; i < context.foundPathParents.size() + 1; ++i )
		{
			GmodNode& node = ( i < context.foundPathParents.size() ) ? context.foundPathParents[i] : context.foundEndNode.value();

			auto set = locationVisitor.visit( node, i, tempParents, context.foundEndNode.value() );
			if ( !set.has_value() )
			{
				if ( node.location().has_value() )
				{
					return GmodParsePathResult::Error( "Invalid location set configuration" );
				}

				continue;
			}

			const auto& [setStart, setEnd, setLocation] = set.value();

			if ( setStart == setEnd )
			{
				continue;
			}

			for ( size_t j = setStart; j <= setEnd; ++j )
			{
				if ( j < context.foundPathParents.size() )
				{
					auto oldNode = context.foundPathParents[j];
					context.foundPathParents[j] = context.foundPathParents[j].tryWithLocation( setLocation );
				}
				else
				{
					auto oldNode = context.foundEndNode.value();
					context.foundEndNode = context.foundEndNode.value().tryWithLocation( setLocation );
				}
			}
		}

		return GmodParsePathResult::Ok( GmodPath( gmod, std::move( context.foundEndNode.value() ), std::move( context.foundPathParents ) ) );
	}

	//----------------------------------------------
	// GmodPath::enumerator
	//----------------------------------------------

	//----------------------------
	// Construction
	//----------------------------

	GmodPath::Enumerator::Enumerator( const GmodPath* pathInst, size_t startIndex )
		: m_pathInstance{ pathInst },
		  m_currentIndex{ std::numeric_limits<size_t>::max() },
		  m_current{ std::numeric_limits<size_t>::max(), nullptr }
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
		: m_nodeIndices{ nodeIndices },
		  m_path{ sourcePath },
		  m_isBuilt{ false }
	{
		if ( m_nodeIndices.empty() )
		{
			throw std::invalid_argument( "GmodIndividualizableSet constructor: nodeIndices cannot be empty." );
		}

		for ( int nodeIdx : m_nodeIndices )
		{
			if ( static_cast<size_t>( nodeIdx ) >= m_path.length() || nodeIdx < 0 )
			{
				throw std::out_of_range( "GmodIndividualizableSet constructor: Node index " +
										 std::to_string( nodeIdx ) + " is out of bounds for path length " +
										 std::to_string( m_path.length() ) + "." );
			}

			const GmodNode& currentNode = m_path[static_cast<size_t>( nodeIdx )];

			bool isTargetNode = ( static_cast<size_t>( nodeIdx ) == m_path.length() - 1 );
			bool isInSet = ( m_nodeIndices.size() > 1 );
			if ( !currentNode.isIndividualizable( isTargetNode, isInSet ) )
			{
				throw std::invalid_argument( "GmodIndividualizableSet constructor: Node '" +
											 std::string{ currentNode.code().data() } + "' (at index " +
											 std::to_string( nodeIdx ) + ") is not individualizable in the given context." );
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
					throw std::invalid_argument( "GmodIndividualizableSet constructor: Nodes have different locations. Node '" +
												 std::string{ currentNode.code().data() } + "' (at index " +
												 std::to_string( currentIdx ) + ") has location while first node in set had different or no location." );
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
			throw std::invalid_argument( "GmodIndividualizableSet constructor: No nodes in the set are part of the short path (final node or leaf node)." );
		}
	}

	//----------------------------------------------
	// Build
	//----------------------------------------------

	GmodPath GmodIndividualizableSet::build()
	{
		if ( m_isBuilt )
		{
			throw std::runtime_error( "Tried to build individualizable set twice" );
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
				throw std::out_of_range( "Node index out of bounds" );
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
		fmt::memory_buffer builder;
		bool firstNodeAppended = false;

		for ( size_t j = 0; j < m_nodeIndices.size(); ++j )
		{
			int nodeIdx = m_nodeIndices[j];
			if ( nodeIdx < 0 || static_cast<size_t>( nodeIdx ) >= m_path.length() )
			{
				SPDLOG_ERROR( "GmodIndividualizableSet::toString(): Node index {} is out of bounds for path length {}. Skipping.",
					nodeIdx,
					m_path.length() );

				continue;
			}

			const GmodNode& currentNode = m_path[static_cast<size_t>( nodeIdx )];

			if ( currentNode.isLeafNode() || j == m_nodeIndices.size() - 1 )
			{
				if ( firstNodeAppended )
				{
					builder.push_back( '/' );
				}

				currentNode.toString( std::back_inserter( builder ) );
				firstNodeAppended = true;
			}
		}

		return fmt::to_string( builder );
	}
}
