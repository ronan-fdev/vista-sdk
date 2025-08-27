/**
 * @file GmodPathQuery.inl
 * @brief Inline implementations for performance-critical GmodPathQuery operations
 */

#pragma once

namespace dnv::vista::sdk
{
	//=====================================================================
	// NodeItem class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline NodeItem::NodeItem( std::string node, std::vector<Location> locations ) noexcept
		: m_node{ std::move( node ) },
		  m_locations{ std::move( locations ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::string& NodeItem::node() const noexcept
	{
		return m_node;
	}

	inline const std::vector<Location>& NodeItem::locations() const noexcept
	{
		return m_locations;
	}

	inline bool NodeItem::matchAllLocations() const noexcept
	{
		return m_matchAllLocations;
	}

	//----------------------------------------------
	// Mutators
	//----------------------------------------------

	inline void NodeItem::setLocations( std::vector<Location> locations ) noexcept
	{
		m_locations = std::move( locations );
	}

	inline void NodeItem::setMatchAllLocations( bool matchAll ) noexcept
	{
		m_matchAllLocations = matchAll;
	}

	//=====================================================================
	// GmodPathQuery class
	//=====================================================================

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::optional<GmodPath>& GmodPathQuery::sourcePath() const noexcept
	{
		return m_sourcePath;
	}

	//----------------------------------------------
	// Factory methods
	//----------------------------------------------

	inline GmodPathQuery GmodPathQuery::fromPath( const GmodPath& path )
	{
		return GmodPathQuery{ path };
	}

	inline GmodPathQuery GmodPathQuery::empty()
	{
		return GmodPathQuery{};
	}

	//----------------------------------------------
	// Location manipulation
	//----------------------------------------------

	inline GmodPathQuery GmodPathQuery::withoutLocations() const
	{
		GmodPathQuery result = *this;
		for ( auto& [code, item] : result.m_filter )
		{
			item.setLocations( {} );
			item.setMatchAllLocations( true );
		}
		return result;
	}
}
