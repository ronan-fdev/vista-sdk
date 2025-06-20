/**
 * @file GmodNode.cpp
 * @brief Implementation of the GmodNode and GmodNodeMetadata classes
 */

#include "pch.h"

#include "dnv/vista/sdk/GmodNode.h"

#include "dnv/vista/sdk/GmodConstants.h"
#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// GmodNodeMetadata class
	//=====================================================================

	//----------------------------------------------
	// Construction / destruction
	//----------------------------------------------

	GmodNodeMetadata::GmodNodeMetadata(
		const std::string& category,
		const std::string& type,
		const std::string& name,
		const std::optional<std::string>& commonName,
		const std::optional<std::string>& definition,
		const std::optional<std::string>& commonDefinition,
		const std::optional<bool>& installSubstructure,
		const std::unordered_map<std::string, std::string>& normalAssignmentNames )
		: m_category{ category },
		  m_type{ type },
		  m_name{ name },
		  m_commonName{ commonName },
		  m_definition{ definition },
		  m_commonDefinition{ commonDefinition },
		  m_installSubstructure{ installSubstructure },
		  m_normalAssignmentNames{ normalAssignmentNames },
		  m_fullType{ category + " " + type }
	{
	}

	GmodNodeMetadata::GmodNodeMetadata( const GmodNodeMetadata& other )
		: m_category{ other.m_category },
		  m_type{ other.m_type },
		  m_name{ other.m_name },
		  m_commonName{ other.m_commonName },
		  m_definition{ other.m_definition },
		  m_commonDefinition{ other.m_commonDefinition },
		  m_installSubstructure{ other.m_installSubstructure },
		  m_normalAssignmentNames{ other.m_normalAssignmentNames },
		  m_fullType{ other.m_fullType }
	{
	}

	GmodNodeMetadata& GmodNodeMetadata::operator=( const GmodNodeMetadata& other )
	{
		if ( this == &other )
		{
			return *this;
		}

		m_category = other.m_category;
		m_type = other.m_type;
		m_name = other.m_name;
		m_commonName = other.m_commonName;
		m_definition = other.m_definition;
		m_commonDefinition = other.m_commonDefinition;
		m_installSubstructure = other.m_installSubstructure;
		m_normalAssignmentNames = other.m_normalAssignmentNames;
		m_fullType = other.m_fullType;

		return *this;
	}

	//=====================================================================
	// GmodNode class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	GmodNode::GmodNode( VisVersion version, const GmodNodeDto& dto )
		: m_code{ dto.code() },
		  m_location{ std::nullopt },
		  m_visVersion{ version },
		  m_metadata{
			  dto.category(),
			  dto.type(),
			  dto.name(),
			  dto.commonName(),
			  dto.definition(),
			  dto.commonDefinition(),
			  dto.installSubstructure(),
			  dto.normalAssignmentNames().has_value() ? *dto.normalAssignmentNames() : std::unordered_map<std::string, std::string>() },
		  m_children{},
		  m_parents{},
		  m_childrenSet{}
	{
		m_children.reserve( 8 );
		m_parents.reserve( 4 );
		m_childrenSet.reserve( 8 );
	}

	GmodNode::GmodNode( const GmodNode& other )
		: m_code{ other.m_code },
		  m_location{ other.m_location },
		  m_visVersion{ other.m_visVersion },
		  m_metadata{ other.m_metadata },
		  m_children{ other.m_children },
		  m_parents{ other.m_parents },
		  m_childrenSet{ other.m_childrenSet },
		  m_cachedProductType{ other.m_cachedProductType },
		  m_cachedProductSelection{ other.m_cachedProductSelection }
	{
	}

	//----------------------------------------------
	// Assignment operators
	//----------------------------------------------

	GmodNode& GmodNode::operator=( const GmodNode& other )
	{
		if ( this == &other )
		{
			return *this;
		}

		m_code = other.m_code;
		m_location = other.m_location;
		m_visVersion = other.m_visVersion;
		m_metadata = other.m_metadata;
		m_children = other.m_children;
		m_parents = other.m_parents;
		m_childrenSet = other.m_childrenSet;
		m_cachedProductType = other.m_cachedProductType;
		m_cachedProductSelection = other.m_cachedProductSelection;

		return *this;
	}

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

	GmodNode GmodNode::withLocation( const Location& location ) const
	{
		GmodNode result = *this;
		result.m_location = location;

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

	//----------------------------------------------
	// Relationship management methods
	//----------------------------------------------

	void GmodNode::addChild( GmodNode* child )
	{
		if ( !child )
		{
			return;
		}

		const std::string& childCode = child->code();
		if ( m_childrenSet.contains( childCode ) )
		{
			return;
		}

		m_children.push_back( child );
		m_childrenSet.emplace( childCode );

		m_cachedProductType.reset();
		m_cachedProductSelection.reset();
	}

	void GmodNode::addParent( GmodNode* parent )
	{
		if ( !parent )
		{
			return;
		}

		m_parents.push_back( parent );
	}

	void GmodNode::trim()
	{
		m_children.shrink_to_fit();
		m_parents.shrink_to_fit();

		if ( m_childrenSet.size() != m_children.size() )
		{
			m_childrenSet.clear();
			m_childrenSet.reserve( m_children.size() );

			for ( const auto* child : m_children )
			{
				m_childrenSet.emplace( child->code() );
			}
		}
	}
}
