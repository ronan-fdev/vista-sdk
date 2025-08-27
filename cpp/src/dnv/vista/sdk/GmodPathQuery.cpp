/**
 * @file GmodPathQuery.cpp
 * @brief Implementation of GmodPathQuery class
 */

#include "dnv/vista/sdk/GmodPathQuery.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// GmodPathQuery class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	GmodPathQuery::GmodPathQuery( const GmodPath& path )
		: m_sourcePath{ ensurePathVersion( path ) }
	{
		for ( const auto& set : m_sourcePath->individualizableSets() )
		{
			const auto nodes = set.nodes();
			if ( !nodes.empty() )
			{
				const auto& setNode = *nodes.back();
				const auto nodeCode = setNode.code();

				m_setNodes.emplace( nodeCode, setNode );

				std::vector<Location> locations;
				if ( set.location().has_value() )
				{
					locations.push_back( set.location().value() );
				}

				NodeItem item( nodeCode, std::move( locations ) );
				m_filter.emplace( nodeCode, std::move( item ) );
			}
		}
	}

	//----------------------------------------------
	// Build methods (Immutable fluent interface)
	//----------------------------------------------

	//----------------------------
	// Paths
	//----------------------------

	GmodPathQuery GmodPathQuery::withNode(
		std::function<const GmodNode&( const internal::StringMap<GmodNode>& )> selector,
		bool matchAllLocations ) const
	{
		if ( !m_sourcePath.has_value() )
		{
			throw std::runtime_error( "withNode with selector can only be used on path-based queries" );
		}

		GmodPathQuery result = *this;
		const auto& node = selector( m_setNodes );
		auto it = result.m_filter.find( node.code() );
		if ( it == result.m_filter.end() )
		{
			throw std::runtime_error( "Expected to find a filter on the node in the path" );
		}

		it->second.setLocations( {} );
		it->second.setMatchAllLocations( matchAllLocations );

		return result;
	}

	GmodPathQuery GmodPathQuery::withNode(
		std::function<const GmodNode&( const internal::StringMap<GmodNode>& )> selector,
		const std::vector<Location>& locations ) const
	{
		if ( !m_sourcePath.has_value() )
		{
			throw std::runtime_error( "withNode with selector can only be used on path-based queries" );
		}

		GmodPathQuery result = *this;
		const auto& node = selector( m_setNodes );
		auto it = result.m_filter.find( node.code() );
		if ( it == result.m_filter.end() )
		{
			throw std::runtime_error( "Expected to find a filter on the node in the path" );
		}

		std::vector<Location> locationSet{ locations.begin(), locations.end() };
		it->second.setLocations( std::move( locationSet ) );

		return result;
	}

	//----------------------------
	// Nodes
	//----------------------------

	GmodPathQuery GmodPathQuery::withNode( const GmodNode& node, bool matchAllLocations ) const
	{
		GmodPathQuery result = *this;
		const auto& n = result.ensureNodeVersion( node );
		auto it = result.m_filter.find( n.code() );
		if ( it != result.m_filter.end() )
		{
			it->second.setLocations( {} );
			it->second.setMatchAllLocations( matchAllLocations );
		}
		else
		{
			NodeItem item( n.code(), {} );
			item.setMatchAllLocations( matchAllLocations );
			result.m_filter.emplace( n.code(), std::move( item ) );
		}

		return result;
	}

	GmodPathQuery GmodPathQuery::withNode( const GmodNode& node, const std::vector<Location>& locations ) const
	{
		GmodPathQuery result = *this;
		const auto& n = result.ensureNodeVersion( node );
		std::vector<Location> locationSet{ locations.begin(), locations.end() };

		auto it = result.m_filter.find( n.code() );
		if ( it != result.m_filter.end() )
		{
			it->second.setLocations( std::move( locationSet ) );
		}
		else
		{
			NodeItem item( n.code(), std::move( locationSet ) );
			result.m_filter.emplace( n.code(), std::move( item ) );
		}

		return result;
	}

	//----------------------------------------------
	// Query execution
	//----------------------------------------------

	bool GmodPathQuery::match( const GmodPath* other ) const
	{
		if ( other == nullptr )
		{
			return false;
		}

		const auto target = ensurePathVersion( *other );

		// Build map of target nodes and their locations
		internal::StringMap<std::vector<Location>> targetNodes;

		// Include all parents and the final node
		for ( const auto& parent : target.parents() )
		{
			auto& locations = targetNodes[parent.code()];
			if ( parent.location().has_value() )
			{
				locations.push_back( parent.location().value() );
			}
		}

		auto& nodeLocations = targetNodes[target.node().code()];
		if ( target.node().location().has_value() )
			nodeLocations.push_back( target.node().location().value() );

		// Check each filter criterion
		for ( const auto& [code, item] : m_filter )
		{
			auto targetIt = targetNodes.find( code );
			if ( targetIt == targetNodes.end() )
			{
				return false;
			}

			const auto& potentialLocations = targetIt->second;

			if ( item.matchAllLocations() )
			{
				continue;
			}

			if ( !item.locations().empty() )
			{
				if ( potentialLocations.empty() )
				{
					return false;
				}

				bool hasMatch = false;
				for ( const auto& location : potentialLocations )
				{
					if ( std::find( item.locations().begin(), item.locations().end(), location ) != item.locations().end() )
					{
						hasMatch = true;
						break;
					}
				}
				if ( !hasMatch )
				{
					return false;
				}
			}
			else
			{
				if ( !potentialLocations.empty() )
				{
					return false;
				}
			}
		}

		return true;
	}

	//----------------------------------------------
	// Helper methods
	//----------------------------------------------

	GmodPath GmodPathQuery::ensurePathVersion( const GmodPath& path ) const
	{
		const auto targetVersion = VIS::instance().latestVisVersion();

		if ( path.visVersion() == targetVersion )
		{
			return path;
		}

		auto convertedPath = VIS::instance().convertPath( path, targetVersion );
		if ( convertedPath.has_value() )
		{
			return std::move( convertedPath.value() );
		}

		return path;
	}

	GmodNode GmodPathQuery::ensureNodeVersion( const GmodNode& node ) const
	{
		const auto targetVersion = VIS::instance().latestVisVersion();

		if ( node.visVersion() == targetVersion )
		{
			return node;
		}

		auto convertedNode = VIS::instance().convertNode( node, targetVersion );
		if ( convertedNode.has_value() )
		{
			return std::move( convertedNode.value() );
		}

		return node;
	}
}
