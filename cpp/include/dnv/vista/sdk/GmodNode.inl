/**
 * @file GmodNode.inl
 * @brief Inline implementations for performance-critical GmodNode operations
 */

#include "GmodConstants.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// GmodNodeMetadata class
	//=====================================================================

	//----------------------------------------------
	// Operators
	//----------------------------------------------

	inline bool GmodNodeMetadata::operator==( const GmodNodeMetadata& other ) const
	{
		return m_category == other.m_category &&
			   m_type == other.m_type &&
			   m_name == other.m_name &&
			   m_commonName == other.m_commonName &&
			   m_definition == other.m_definition &&
			   m_commonDefinition == other.m_commonDefinition &&
			   m_installSubstructure == other.m_installSubstructure &&
			   m_normalAssignmentNames == other.m_normalAssignmentNames;
	}

	inline bool GmodNodeMetadata::operator!=( const GmodNodeMetadata& other ) const
	{
		return !( *this == other );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::string& GmodNodeMetadata::category() const
	{
		return m_category;
	}

	inline const std::string& GmodNodeMetadata::type() const
	{
		return m_type;
	}

	inline const std::string& GmodNodeMetadata::fullType() const
	{
		return m_fullType;
	}

	inline const std::string& GmodNodeMetadata::name() const
	{
		return m_name;
	}

	inline const std::optional<std::string>& GmodNodeMetadata::commonName() const
	{
		return m_commonName;
	}

	inline const std::optional<std::string>& GmodNodeMetadata::definition() const
	{
		return m_definition;
	}

	inline const std::optional<std::string>& GmodNodeMetadata::commonDefinition() const
	{
		return m_commonDefinition;
	}

	inline const std::optional<bool>& GmodNodeMetadata::installSubstructure() const
	{
		return m_installSubstructure;
	}

	inline const std::unordered_map<std::string, std::string>& GmodNodeMetadata::normalAssignmentNames() const
	{
		return m_normalAssignmentNames;
	}

	//=====================================================================
	// GmodNode class
	//=====================================================================

	//----------------------------------------------
	// Operators
	//----------------------------------------------

	inline bool GmodNode::operator==( const GmodNode& other ) const
	{
		if ( m_code != other.m_code )
		{
			return false;
		}

		if ( !m_location.has_value() && !other.m_location.has_value() )
		{
			return true;
		}

		return m_location == other.m_location;
	}

	inline bool GmodNode::operator!=( const GmodNode& other ) const
	{
		return !( *this == other );
	}

	inline bool GmodNode::equals( const GmodNode& other ) const
	{
		return ( *this == other );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::string& GmodNode::code() const
	{
		return m_code;
	}

	inline const std::optional<Location>& GmodNode::location() const
	{
		return m_location;
	}

	inline VisVersion GmodNode::visVersion() const
	{
		return m_visVersion;
	}

	inline const GmodNodeMetadata& GmodNode::metadata() const
	{
		return m_metadata;
	}

	inline size_t GmodNode::hashCode() const noexcept
	{
		size_t hash = std::hash<std::string>{}( m_code );

		if ( m_location.has_value() )
		{
			hash ^= ( std::hash<std::string>{}( m_location->toString() ) ) + 0x9e3779b9 + ( hash << 6 ) + ( hash >> 2 );
		}

		return hash;
	}

	//----------------------------------------------
	// Relationship accessors
	//----------------------------------------------

	inline const std::vector<GmodNode*>& GmodNode::children() const
	{
		return m_children;
	}

	inline const std::vector<GmodNode*>& GmodNode::parents() const
	{
		return m_parents;
	}

	inline const GmodNode* GmodNode::productType() const
	{
		if ( m_children.size() != 1 )
		{
			return nullptr;
		}

		if ( m_metadata.category().find( NODE_CATEGORY_FUNCTION ) == std::string::npos )
		{
			return nullptr;
		}

		const GmodNode* child = m_children[0];
		if ( !child )
		{
			return nullptr;
		}

		if ( child->m_metadata.category() != NODE_CATEGORY_PRODUCT )
		{
			return nullptr;
		}

		if ( child->m_metadata.type() != NODE_TYPE_TYPE )
		{
			return nullptr;
		}

		return child;
	}

	inline const GmodNode* GmodNode::productSelection() const
	{
		if ( m_children.size() != 1 )
		{
			return nullptr;
		}

		if ( m_metadata.category().find( NODE_CATEGORY_FUNCTION ) == std::string::npos )
		{
			return nullptr;
		}

		const GmodNode* child = m_children[0];
		if ( !child )
		{
			return nullptr;
		}

		if ( child->m_metadata.category().find( NODE_CATEGORY_PRODUCT ) == std::string::npos )
		{
			return nullptr;
		}

		if ( child->m_metadata.type() != NODE_TYPE_SELECTION )
		{
			return nullptr;
		}

		return child;
	}

	//----------------------------------------------
	// Node type checking methods
	//----------------------------------------------

	inline bool GmodNode::isIndividualizable( bool isTargetNode, bool isInSet ) const
	{
		if ( m_metadata.type() == NODE_TYPE_GROUP )
		{
			return false;
		}
		if ( m_metadata.type() == NODE_TYPE_SELECTION )
		{
			return false;
		}
		if ( isProductType() )
		{
			return false;
		}
		if ( m_metadata.category() == NODE_CATEGORY_ASSET && m_metadata.type() == NODE_TYPE_TYPE )
		{
			return false;
		}
		if ( isFunctionComposition() )
		{
			if ( m_code.empty() )
			{
				return false;
			}

			return m_code.back() == 'i' || isInSet || isTargetNode;
		}

		return true;
	}

	inline bool GmodNode::isFunctionComposition() const
	{
		return ( m_metadata.category() == NODE_CATEGORY_ASSET_FUNCTION ||
				   m_metadata.category() == NODE_CATEGORY_PRODUCT_FUNCTION ) &&
			   m_metadata.type() == NODE_TYPE_COMPOSITION;
	}

	inline bool GmodNode::isMappable() const noexcept
	{
		if ( !m_cachedProductType.has_value() )
		{
			if ( m_children.size() == 1 &&
				 m_metadata.category().find( NODE_CATEGORY_FUNCTION ) != std::string::npos )
			{
				const GmodNode* child = m_children[0];
				if ( child &&
					 child->m_metadata.category() == NODE_CATEGORY_PRODUCT &&
					 child->m_metadata.type() == NODE_TYPE_TYPE )
				{
					m_cachedProductType = child;
				}
				else
				{
					m_cachedProductType = nullptr;
				}
			}
			else
			{
				m_cachedProductType = nullptr;
			}
		}

		if ( m_cachedProductType.value() != nullptr )
		{
			return false;
		}

		if ( !m_cachedProductSelection.has_value() )
		{
			if ( m_children.size() == 1 &&
				 m_metadata.category().find( NODE_CATEGORY_FUNCTION ) != std::string::npos )
			{
				const GmodNode* child = m_children[0];
				if ( child &&
					 child->m_metadata.category().find( NODE_CATEGORY_PRODUCT ) != std::string::npos &&
					 child->m_metadata.type() == NODE_TYPE_SELECTION )
				{
					m_cachedProductSelection = child;
				}
				else
				{
					m_cachedProductSelection = nullptr;
				}
			}
			else
			{
				m_cachedProductSelection = nullptr;
			}
		}

		if ( m_cachedProductSelection.value() != nullptr )
		{
			return false;
		}

		if ( m_metadata.category().find( NODE_CATEGORY_PRODUCT ) != std::string::npos && m_metadata.type() == NODE_TYPE_SELECTION )
		{
			return false;
		}

		if ( m_metadata.category() == NODE_CATEGORY_ASSET )
		{
			return false;
		}

		if ( m_code.empty() )
		{
			return false;
		}

		char lastChar = m_code.back();
		return lastChar != 'a' && lastChar != 's';
	}

	inline bool GmodNode::isRoot() const noexcept
	{
		return m_code == "VE";
	}

	//----------------------------------------------
	// Node relationship query methods
	//----------------------------------------------

	inline bool GmodNode::isChild( const GmodNode& node ) const
	{
		return isChild( node.m_code );
	}

	inline bool GmodNode::isChild( const std::string& code ) const
	{
		if ( m_children.size() <= 8 )
		{
			for ( const auto* child : m_children )
			{
				if ( child->code() == code )
				{
					return true;
				}
			}

			return false;
		}

		return m_childrenSet.contains( code );
	}

	//----------------------------------------------
	// String conversion methods
	//----------------------------------------------

	inline std::string GmodNode::toString() const
	{
		fmt::memory_buffer builder;
		toString( std::back_inserter( builder ) );

		return fmt::to_string( builder );
	}

	template <typename OutputIt>
	inline OutputIt GmodNode::toString( OutputIt out ) const
	{
		out = fmt::format_to( out, "{}", m_code );
		if ( m_location.has_value() )
		{
			out = fmt::format_to( out, "-{}", m_location->toString() );
		}

		return out;
	}
}
