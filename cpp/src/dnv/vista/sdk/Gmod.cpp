/**
 * @file Gmod.cpp
 * @brief Implementation of the Generic Product Model (GMOD) class.
 */

#include "pch.h"

#include "dnv/vista/sdk/Gmod.h"

#include "dnv/vista/sdk/GmodPath.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Gmod class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Gmod::Gmod( VisVersion version, const GmodDto& dto )
		: m_visVersion{ version },
		  m_rootNode{ nullptr }
	{
		const auto& items = dto.items();
		std::vector<std::pair<std::string, GmodNode>> nodePairs;
		nodePairs.reserve( items.size() );

		for ( const auto& nodeDto : items )
		{
			nodePairs.emplace_back(
				std::string{ nodeDto.code() },
				GmodNode{ version, nodeDto } );
		}

		m_nodeMap = ChdDictionary<GmodNode>( std::move( nodePairs ) );

		for ( const auto& relation : dto.relations() )
		{
			if ( relation.size() >= 2 )
			{
				auto& parentNode = const_cast<GmodNode&>( m_nodeMap[relation[0]] );
				auto& childNode = const_cast<GmodNode&>( m_nodeMap[relation[1]] );

				parentNode.addChild( &childNode );
				childNode.addParent( &parentNode );
			}
		}

		for ( auto& [key, node] : m_nodeMap )
		{
			const_cast<GmodNode&>( node ).trim();
		}

		m_rootNode = const_cast<GmodNode*>( &m_nodeMap["VE"] );
	}

	Gmod::Gmod( VisVersion version, const std::unordered_map<std::string, GmodNode>& nodeMap )
		: m_visVersion{ version }
	{
		std::vector<std::pair<std::string, GmodNode>> pairs;
		pairs.reserve( nodeMap.size() );
		for ( const auto& [code, node] : nodeMap )
		{
			pairs.emplace_back( code, node );
		}
		m_nodeMap = ChdDictionary<GmodNode>( std::move( pairs ) );

		for ( auto& [key, node] : m_nodeMap )
		{
			const_cast<GmodNode&>( node ).trim();
		}

		m_rootNode = const_cast<GmodNode*>( &m_nodeMap["VE"] );
	}

	//----------------------------------------------
	// Path parsing & navigation
	//----------------------------------------------

	GmodPath Gmod::parsePath( std::string_view item ) const
	{
		return GmodPath::parse( item, m_visVersion );
	}

	bool Gmod::tryParsePath( std::string_view item, std::optional<GmodPath>& path ) const noexcept
	{
		return GmodPath::tryParse( item, m_visVersion, path );
	}

	GmodPath Gmod::parseFromFullPath( std::string_view item ) const
	{
		return GmodPath::parseFullPath( item, m_visVersion );
	}

	bool Gmod::tryParseFromFullPath( std::string_view item, std::optional<GmodPath>& path ) const noexcept
	{
		return GmodPath::tryParseFullPath( item, m_visVersion, path );
	}

	//----------------------------------------------
	// Gmod::Enumerator class
	//----------------------------------------------

	//-----------------------------
	// Construction
	//-----------------------------

	Gmod::Enumerator::Enumerator( const ChdDictionary<GmodNode>* map ) noexcept
		: m_sourceMapPtr{ map },
		  m_isInitialState{ true }
	{
		if ( m_sourceMapPtr )
		{
			m_currentMapIterator = m_sourceMapPtr->begin();
		}
	}
}
