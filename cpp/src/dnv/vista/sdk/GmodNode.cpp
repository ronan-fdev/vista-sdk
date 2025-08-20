/**
 * @file GmodNode.cpp
 * @brief Implementation of the GmodNode and GmodNodeMetadata classes
 */

#include "pch.h"

#include "dnv/vista/sdk/GmodNode.h"

#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// GmodNode class
	//=====================================================================

	//----------------------------------------------
	// Node location methods
	//----------------------------------------------

	GmodNode GmodNode::withoutLocation() const
	{
		if ( !m_location.has_value() )
		{
			return *this;
		}

		GmodNode result = *this;
		result.m_location = std::nullopt;

		return result;
	}

	GmodNode GmodNode::withLocation( std::string_view locationStr ) const
	{
		Locations locations = VIS::instance().locations( m_visVersion );
		Location location = locations.parse( locationStr );

		return withLocation( location );
	}

	GmodNode GmodNode::tryWithLocation( std::string_view locationStr ) const
	{
		Locations locations = VIS::instance().locations( m_visVersion );
		Location parsedLocation;

		if ( !locations.tryParse( locationStr, parsedLocation ) )
		{
			return *this;
		}

		return withLocation( parsedLocation );
	}

	GmodNode GmodNode::tryWithLocation( std::string_view locationStr, ParsingErrors& errors ) const
	{
		auto locations = VIS::instance().locations( m_visVersion );

		Location location;
		if ( !locations.tryParse( locationStr, location, errors ) )
		{
			return *this;
		}

		return withLocation( location );
	}

	GmodNode GmodNode::tryWithLocation( const std::optional<Location>& location ) const
	{
		if ( !location.has_value() )
		{
			return *this;
		}

		if ( m_location.has_value() && m_location.value() == location.value() )
		{
			return *this;
		}

		return withLocation( location.value() );
	}

	//----------------------------------------------
	// Node type checking methods
	//----------------------------------------------

	bool GmodNode::isProductSelection() const
	{
		return Gmod::isProductSelection( m_metadata );
	}

	bool GmodNode::isProductType() const
	{
		return Gmod::isProductType( m_metadata );
	}

	bool GmodNode::isAsset() const
	{
		return Gmod::isAsset( m_metadata );
	}

	bool GmodNode::isLeafNode() const
	{
		return Gmod::isLeafNode( m_metadata );
	}

	bool GmodNode::isFunctionNode() const
	{
		return Gmod::isFunctionNode( m_metadata );
	}

	bool GmodNode::isAssetFunctionNode() const
	{
		return Gmod::isAssetFunctionNode( m_metadata );
	}
}
