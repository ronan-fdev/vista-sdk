/**
 * @file Gmod.cpp
 * @brief Implementation of the Generic Product Model (GMOD) class.
 */

#include "pch.h"

#include "dnv/vista/sdk/Gmod.h"

#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// Constants
		//=====================================================================

		static constexpr const char* NODE_CATEGORY_PRODUCT = "PRODUCT";
		static constexpr const char* NODE_CATEGORY_VALUE_FUNCTION = "FUNCTION";
		static constexpr const char* NODE_CATEGORY_ASSET = "ASSET";
		static constexpr const char* NODE_CATEGORY_ASSET_FUNCTION = "ASSET FUNCTION";

		static constexpr const char* NODE_TYPE_VALUE_SELECTION = "SELECTION";
		static constexpr const char* NODE_TYPE_VALUE_TYPE = "TYPE";

		/** List of node types classified as leaf nodes */
		static const std::unordered_set<std::string> s_leafTypesSet = { "ASSET FUNCTION LEAF", "PRODUCT FUNCTION LEAF" };

		/** List of node types classified as function nodes */
		static const std::unordered_set<std::string> s_potentialParentScopeTypes = { "SELECTION", "GROUP", "LEAF" };
	}

	//=====================================================================
	// Gmod Class
	//=====================================================================

	//----------------------------------------------
	// Construction / destruction
	//----------------------------------------------

	Gmod::Gmod( VisVersion version, const GmodDto& dto )
		: m_visVersion{ version },
		  m_rootNode{ nullptr },
		  m_nodeMap{}
	{
		const size_t nodeCount = dto.items().size();

		std::vector<std::pair<std::string, GmodNode>> nodePairs;
		nodePairs.reserve( nodeCount );

		for ( const auto& nodeDto : dto.items() )
		{
			nodePairs.emplace_back( nodeDto.code(), GmodNode( version, nodeDto ) );
		}

		m_nodeMap = ChdDictionary<GmodNode>( std::move( nodePairs ) );

		if ( m_nodeMap.isEmpty() && !dto.items().empty() )
		{
			return;
		}

		const auto& relations = dto.relations();
		for ( const auto& relation : relations )
		{
			if ( relation.size() < 2 )
				continue;

			const std::string& parentCode = relation[0];
			const std::string& childCode = relation[1];

			const GmodNode* parentPtr = nullptr;
			const GmodNode* childPtr = nullptr;

			if ( m_nodeMap.tryGetValue( parentCode, parentPtr ) &&
				 m_nodeMap.tryGetValue( childCode, childPtr ) &&
				 parentPtr && childPtr )
			{
				GmodNode* parentNode = const_cast<GmodNode*>( parentPtr );
				GmodNode* childNode = const_cast<GmodNode*>( childPtr );

				parentNode->addChild( childNode );
				childNode->addParent( parentNode );
			}
		}

		if ( !m_nodeMap.isEmpty() )
		{
			for ( auto& [key, node] : m_nodeMap )
			{
				const_cast<GmodNode&>( node ).trim();
			}
		}

		const GmodNode* rootPtr = nullptr;
		if ( m_nodeMap.tryGetValue( "VE", rootPtr ) && rootPtr )
		{
			m_rootNode = const_cast<GmodNode*>( rootPtr );
		}
	}

	Gmod::Gmod( VisVersion version, const std::unordered_map<std::string, GmodNode>& nodeMap )
		: m_visVersion{ version },
		  m_rootNode{ nullptr },
		  m_nodeMap{}
	{
		std::vector<std::pair<std::string, GmodNode>> pairs;
		pairs.reserve( nodeMap.size() );

		for ( const auto& [code, node] : nodeMap )
		{
			pairs.emplace_back( code, node );
		}

		m_nodeMap = ChdDictionary<GmodNode>( std::move( pairs ) );

		const GmodNode* rootPtr = nullptr;
		if ( m_nodeMap.tryGetValue( "VE", rootPtr ) && rootPtr )
		{
			m_rootNode = const_cast<GmodNode*>( rootPtr );
		}
	}

	//----------------------------------------------
	// Lookup operators
	//----------------------------------------------

	const GmodNode& Gmod::operator[]( std::string_view key ) const
	{
		const GmodNode* nodePtr = nullptr;
		if ( m_nodeMap.tryGetValue( key, nodePtr ) && nodePtr != nullptr )
		{
			return *nodePtr;
		}
		throw std::out_of_range( "Key not found in Gmod node map: " + std::string( key ) );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	VisVersion Gmod::visVersion() const
	{
		return m_visVersion;
	}

	const GmodNode& Gmod::rootNode() const
	{
		if ( !m_rootNode )
		{
			SPDLOG_ERROR( "Gmod::rootNode() called when m_rootNode is null!" );
			throw std::runtime_error( "Root node is not initialized or 'VE' was not found." );
		}
		return *m_rootNode;
	}

	//----------------------------------------------
	// Node Query Methods
	//----------------------------------------------

	bool Gmod::tryGetNode( std::string_view code, const GmodNode*& node ) const
	{
		return m_nodeMap.tryGetValue( code, node );
	}

	//----------------------------------------------
	// Path Parsing & Navigation
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
	// Static Utility Methods
	//----------------------------------------------

	bool Gmod::isPotentialParent( const std::string& type )
	{
		return s_potentialParentScopeTypes.find( type ) != s_potentialParentScopeTypes.end();
	}

	bool Gmod::isLeafNode( const GmodNodeMetadata& metadata )
	{
		return s_leafTypesSet.find( metadata.fullType() ) != s_leafTypesSet.end();
	}

	bool Gmod::isFunctionNode( const GmodNodeMetadata& metadata )
	{
		const auto& category = metadata.category();
		return category != NODE_CATEGORY_PRODUCT && category != NODE_CATEGORY_ASSET;
	}

	bool Gmod::isProductSelection( const GmodNodeMetadata& metadata )
	{
		return metadata.category() == NODE_CATEGORY_PRODUCT && metadata.type() == NODE_TYPE_VALUE_SELECTION;
	}

	bool Gmod::isProductType( const GmodNodeMetadata& metadata )
	{
		return metadata.category() == NODE_CATEGORY_PRODUCT && metadata.type() == NODE_TYPE_VALUE_TYPE;
	}

	bool Gmod::isAsset( const GmodNodeMetadata& metadata )
	{
		return metadata.category() == NODE_CATEGORY_ASSET;
	}

	bool Gmod::isAssetFunctionNode( const GmodNodeMetadata& metadata )
	{
		return metadata.category() == NODE_CATEGORY_ASSET_FUNCTION;
	}

	bool Gmod::isProductTypeAssignment( const GmodNode* parent, const GmodNode* child )
	{
		if ( parent == nullptr || child == nullptr )
			return false;
		if ( parent->metadata().category().find( NODE_CATEGORY_VALUE_FUNCTION ) == std::string::npos )
			return false;
		if ( child->metadata().category() != NODE_CATEGORY_PRODUCT || child->metadata().type() != NODE_TYPE_VALUE_TYPE )
			return false;
		return true;
	}

	bool Gmod::isProductSelectionAssignment( const GmodNode* parent, const GmodNode* child )
	{
		if ( parent == nullptr || child == nullptr )
			return false;
		if ( parent->metadata().category().find( NODE_CATEGORY_VALUE_FUNCTION ) == std::string::npos )
			return false;
		if ( child->metadata().category().find( NODE_CATEGORY_PRODUCT ) == std::string::npos || child->metadata().type() != NODE_TYPE_VALUE_SELECTION )
			return false;
		return true;
	}

	//----------------------------------------------
	// Iteration
	//----------------------------------------------

	Gmod::Enumerator Gmod::enumerator() const
	{
		return Enumerator( &m_nodeMap );
	}

	//----------------------------------------------
	// Gmod::Enumerator Class
	//----------------------------------------------

	//-----------------------------
	// Construction / destruction
	//-----------------------------

	Gmod::Enumerator::Enumerator( const ChdDictionary<GmodNode>* map )
		: m_sourceMapPtr( map ), m_isInitialState( true )
	{
		if ( m_sourceMapPtr )
		{
			m_currentMapIterator = m_sourceMapPtr->begin();
		}
	}

	//-----------------------------
	// Iteration Interface
	//-----------------------------

	const GmodNode& Gmod::Enumerator::current() const
	{
		if ( !m_sourceMapPtr || m_isInitialState || m_currentMapIterator == m_sourceMapPtr->end() )
		{
			throw std::out_of_range( "Gmod::Enumerator::getCurrent() called in an invalid state or past the end." );
		}
		return m_currentMapIterator->second;
	}

	bool Gmod::Enumerator::next()
	{
		if ( !m_sourceMapPtr || m_sourceMapPtr->isEmpty() )
		{
			m_isInitialState = false;
			return false;
		}

		if ( m_isInitialState )
		{
			m_isInitialState = false;
			return m_currentMapIterator != m_sourceMapPtr->end();
		}

		if ( m_currentMapIterator != m_sourceMapPtr->end() )
		{
			++m_currentMapIterator;
			return m_currentMapIterator != m_sourceMapPtr->end();
		}
		return false;
	}

	void Gmod::Enumerator::reset()
	{
		m_isInitialState = true;
		if ( m_sourceMapPtr )
		{
			m_currentMapIterator = m_sourceMapPtr->begin();
		}
	}
}
