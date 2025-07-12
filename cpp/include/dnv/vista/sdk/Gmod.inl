/**
 * @file Gmod.inl
 * @brief Inline implementations for performance-critical Gmod operations
 */

#pragma once

#include "Config.h"
#include "utils/StringUtils.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Gmod class
	//=====================================================================

	//----------------------------------------------
	// Lookup operators
	//----------------------------------------------

	inline const GmodNode& Gmod::operator[]( std::string_view key ) const
	{
		const GmodNode* nodePtr = nullptr;
		bool found = m_nodeMap.tryGetValue( key, nodePtr );
		if ( found && nodePtr != nullptr )
		{
			return *nodePtr;
		}

		throw std::out_of_range( fmt::format( "Key not found in Gmod node map: {}", key ) );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline VisVersion Gmod::visVersion() const
	{
		return m_visVersion;
	}

	inline const GmodNode& Gmod::rootNode() const
	{
		if ( !m_rootNode )
		{
			throw std::runtime_error( "Root node is not initialized or 'VE' was not found." );
		}

		return *m_rootNode;
	}

	//----------------------------------------------
	// Node query methods
	//----------------------------------------------

	inline bool Gmod::tryGetNode( std::string_view code, const GmodNode*& node ) const noexcept
	{
		return m_nodeMap.tryGetValue( code, node );
	}

	//----------------------------------------------
	// Static state inspection methods
	//----------------------------------------------

	inline bool Gmod::isPotentialParent( std::string_view type ) noexcept
	{
		switch ( type.length() )
		{
			case 4:
				return equals( type, GMODNODE_TYPE_LEAF );
			case 5:
				return equals( type, GMODNODE_TYPE_GROUP );
			case 9:
				return equals( type, GMODNODE_TYPE_SELECTION );
			default:
				return false;
		}
	}

	inline bool Gmod::isLeafNode( const GmodNodeMetadata& metadata ) noexcept
	{
		const auto& fullType = metadata.fullType();
		return equals( fullType, GMODNODE_FULLTYPE_ASSET_FUNCTION_LEAF ) ||
			   equals( fullType, GMODNODE_FULLTYPE_PRODUCT_FUNCTION_LEAF );
	}

	inline bool Gmod::isFunctionNode( const GmodNodeMetadata& metadata ) noexcept
	{
		const auto& category = metadata.category();

		return !equals( category, GMODNODE_CATEGORY_PRODUCT ) &&
			   !equals( category, GMODNODE_CATEGORY_ASSET );
	}

	inline bool Gmod::isProductSelection( const GmodNodeMetadata& metadata ) noexcept
	{
		return equals( metadata.category(), GMODNODE_CATEGORY_PRODUCT ) &&
			   equals( metadata.type(), GMODNODE_TYPE_SELECTION );
	}

	inline bool Gmod::isProductType( const GmodNodeMetadata& metadata ) noexcept
	{
		return equals( metadata.category(), GMODNODE_CATEGORY_PRODUCT ) &&
			   equals( metadata.type(), GMODNODE_TYPE_TYPE );
	}

	inline bool Gmod::isAsset( const GmodNodeMetadata& metadata ) noexcept
	{
		return equals( metadata.category(), GMODNODE_CATEGORY_ASSET );
	}

	inline bool Gmod::isAssetFunctionNode( const GmodNodeMetadata& metadata ) noexcept
	{
		return equals( metadata.category(), GMODNODE_CATEGORY_ASSET_FUNCTION );
	}

	inline bool Gmod::isProductTypeAssignment( const GmodNode* parent, const GmodNode* child ) noexcept
	{
		if ( !parent || !child )
			return false;

		const auto& parentCategory = parent->metadata().category();
		const auto& childCategory = child->metadata().category();
		const auto& childType = child->metadata().type();

		if ( !contains( parentCategory, GMODNODE_CATEGORY_FUNCTION ) )
		{
			return false;
		}
		if ( !equals( childCategory, GMODNODE_CATEGORY_PRODUCT ) || !equals( childType, GMODNODE_TYPE_TYPE ) )
		{
			return false;
		}

		return true;
	}

	inline bool Gmod::isProductSelectionAssignment( const GmodNode* parent, const GmodNode* child ) noexcept
	{
		if ( !parent || !child )
		{
			return false;
		}

		const auto& parentCategory = parent->metadata().category();
		const auto& childCategory = child->metadata().category();
		const auto& childType = child->metadata().type();

		if ( !contains( parentCategory, GMODNODE_CATEGORY_FUNCTION ) )
		{
			return false;
		}
		if ( !equals( childCategory, GMODNODE_CATEGORY_PRODUCT ) || !equals( childType, GMODNODE_TYPE_SELECTION ) )
		{
			return false;
		}

		return true;
	}

	//----------------------------------------------
	// Enumeration
	//----------------------------------------------

	inline Gmod::Enumerator Gmod::enumerator() const
	{
		return Enumerator( &m_nodeMap );
	}

	//----------------------------------------------
	// Gmod::Enumerator class
	//----------------------------------------------

	//-----------------------------
	// Iteration interface
	//-----------------------------

	inline const GmodNode& Gmod::Enumerator::current() const
	{
		if ( !m_sourceMapPtr || m_isInitialState || m_currentMapIterator == m_sourceMapPtr->end() )
		{
			throw std::out_of_range( "Gmod::Enumerator::getCurrent() called in an invalid state or past the end." );
		}

		return m_currentMapIterator->second;
	}

	inline bool Gmod::Enumerator::next() noexcept
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

	inline void Gmod::Enumerator::reset() noexcept
	{
		m_isInitialState = true;

		if ( m_sourceMapPtr )
		{
			m_currentMapIterator = m_sourceMapPtr->begin();
		}
	}
}
