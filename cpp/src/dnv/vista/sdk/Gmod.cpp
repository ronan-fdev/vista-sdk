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
	// Construction / destruction
	//----------------------------------------------

	Gmod::Gmod( VisVersion version, const GmodDto& dto )
		: m_visVersion{ version },
		  m_rootNode{ nullptr },
		  m_nodeMap{ [&dto, version]() {
			  std::vector<std::pair<std::string, GmodNode>> nodePairs;
			  nodePairs.reserve( dto.items().size() );
			  for ( const auto& nodeDto : dto.items() )
			  {
				  nodePairs.emplace_back( nodeDto.code(), GmodNode( version, nodeDto ) );
			  }

			  return ChdDictionary<GmodNode>( std::move( nodePairs ) );
		  }() }
	{
		if ( ( m_nodeMap.begin() == m_nodeMap.end() ) && !dto.items().empty() )
		{
			return;
		}

		for ( const auto& relation : dto.relations() )
		{
			if ( relation.size() >= 2 )
			{
				const std::string& parentCode = relation[0];
				const std::string& childCode = relation[1];

				const GmodNode* parentNodePtr = nullptr;
				bool parentFound = m_nodeMap.tryGetValue( parentCode, parentNodePtr );
				if ( !parentFound || !parentNodePtr )
				{
					SPDLOG_WARN( "Gmod constructor (linking): Parent node '{}' not found in m_nodeMap. Relation skipped.", parentCode );
					continue;
				}

				const GmodNode* childNodePtr = nullptr;
				bool childFound = m_nodeMap.tryGetValue( childCode, childNodePtr );
				if ( !childFound || !childNodePtr )
				{
					SPDLOG_WARN( "Gmod constructor (linking): Child node '{}' not found in m_nodeMap. Relation skipped.", childCode );
					continue;
				}

				GmodNode& parentNode = const_cast<GmodNode&>( *parentNodePtr );
				GmodNode& childNode = const_cast<GmodNode&>( *childNodePtr );

				try
				{
					parentNode.addChild( &childNode );
					childNode.addParent( &parentNode );
				}
				catch ( [[maybe_unused]] const std::exception& ex )
				{
					SPDLOG_ERROR( "Gmod constructor (linking): Exception while linking '{}' -> '{}'. Error: {}", parentCode, childCode, ex.what() );
				}
			}
			else
			{
				SPDLOG_WARN( "Gmod constructor (linking): Relation with insufficient size ({}) encountered. Skipping.", relation.size() );
			}
		}

		if ( !m_nodeMap.isEmpty() )
		{
			for ( auto& [key, node] : m_nodeMap )
			{
				try
				{
					const_cast<GmodNode&>( node ).trim();
				}
				catch ( [[maybe_unused]] const std::exception& ex )
				{
					SPDLOG_ERROR( "Gmod constructor (trimming): Exception while trimming node '{}'. Error: {}", key, ex.what() );
				}
			}
		}

		const GmodNode* rootNodePtr = nullptr;
		bool rootFound = m_nodeMap.tryGetValue( "VE", rootNodePtr );
		if ( rootFound && rootNodePtr )
		{
			m_rootNode = const_cast<GmodNode*>( rootNodePtr );
		}
		else
		{
			m_rootNode = nullptr;
		}
	}

	Gmod::Gmod( VisVersion version, const std::unordered_map<std::string, GmodNode>& nodeMap )
		: m_visVersion{ version },
		  m_rootNode{ nullptr },
		  m_nodeMap{ [&nodeMap]() {
			  std::vector<std::pair<std::string, GmodNode>> pairs;
			  pairs.reserve( nodeMap.size() );
			  for ( const auto& [code, node] : nodeMap )
			  {
				  pairs.emplace_back( code, node );
			  }
			  return ChdDictionary<GmodNode>( std::move( pairs ) );
		  }() }
	{
		const GmodNode* rootNodePtr = nullptr;
		bool rootFound = m_nodeMap.tryGetValue( "VE", rootNodePtr );
		if ( rootFound && rootNodePtr )
		{
			m_rootNode = const_cast<GmodNode*>( rootNodePtr );
		}
		else
		{
			m_rootNode = nullptr;
		}

		if ( m_rootNode == nullptr && nodeMap.count( "VE" ) )
		{
		}
		else if ( m_rootNode != nullptr && m_rootNode->code() != "VE" )
		{
			SPDLOG_ERROR( "m_rootNode code is '{}', expected 'VE'.", m_rootNode->code() );
		}

		if ( m_nodeMap.isEmpty() && !nodeMap.empty() )
		{
		}
		if ( !m_nodeMap.isEmpty() && m_nodeMap.size() != nodeMap.size() )
		{
			SPDLOG_WARN( "Gmod constructor from map: m_nodeMap size ({}) does not match input nodeMap size ({}).", m_nodeMap.size(), nodeMap.size() );
		}
	}

	//----------------------------------------------
	// Path parsing & navigation
	//----------------------------------------------

	GmodPath Gmod::parsePath( std::string_view item ) const
	{
		return GmodPath::parse( item, m_visVersion );
	}

	bool Gmod::tryParsePath( std::string_view item, std::optional<GmodPath>& path ) const
	{
		return GmodPath::tryParse( item, m_visVersion, path );
	}

	GmodPath Gmod::parseFromFullPath( const std::string& item ) const
	{
		return GmodPath::parseFullPath( item, m_visVersion );
	}

	bool Gmod::tryParseFromFullPath( const std::string& item, std::optional<GmodPath>& path ) const
	{
		return GmodPath::tryParseFullPath( item, m_visVersion, path );
	}

	//----------------------------------------------
	// Gmod::Enumerator class
	//----------------------------------------------

	//-----------------------------
	// Construction / destruction
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
