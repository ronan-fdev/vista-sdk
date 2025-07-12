/**
 * @file GmodNode.inl
 * @brief Inline implementations for performance-critical GmodNode operations
 */

#include "Config.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Relationship utility
	//=====================================================================

	namespace
	{
		using dnv::vista::sdk::contains;

		static constexpr size_t estimateChildrenCount( std::string_view category, std::string_view type ) noexcept
		{
			if ( category == GMODNODE_CATEGORY_PRODUCT && type == GMODNODE_TYPE_TYPE )
			{
				return 0;
			}
			if ( contains( category, GMODNODE_CATEGORY_FUNCTION ) )
			{
				return 16;
			}
			if ( category == GMODNODE_CATEGORY_ASSET )
			{
				return 4;
			}
			return 8;
		}

		static constexpr size_t estimateParentsCount( std::string_view category, std::string_view type ) noexcept
		{
			if ( category == GMODNODE_CATEGORY_PRODUCT && type == GMODNODE_TYPE_TYPE )
			{
				return 1;
			}
			if ( contains( category, GMODNODE_CATEGORY_FUNCTION ) )
			{
				return 2;
			}
			if ( category == GMODNODE_CATEGORY_ASSET )
			{
				return 1;
			}

			return 1;
		}
	}

	//=====================================================================
	// GmodNodeMetadata class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline GmodNodeMetadata::GmodNodeMetadata(
		std::string_view category,
		std::string_view type,
		std::string_view name,
		const std::optional<std::string>& commonName,
		const std::optional<std::string>& definition,
		const std::optional<std::string>& commonDefinition,
		const std::optional<bool>& installSubstructure,
		const StringMap<std::string>& normalAssignmentNames ) noexcept
		: m_category{ category },
		  m_type{ type },
		  m_name{ name },
		  m_commonName{ commonName },
		  m_definition{ definition },
		  m_commonDefinition{ commonDefinition },
		  m_installSubstructure{ installSubstructure },
		  m_normalAssignmentNames{ normalAssignmentNames },
		  m_fullType{ std::string{ category } + " " + std::string{ type } }
	{
	}

	inline GmodNodeMetadata::GmodNodeMetadata( const GmodNodeMetadata& other ) noexcept
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

	//----------------------------------------------
	// Assignment operators
	//----------------------------------------------

	inline GmodNodeMetadata& GmodNodeMetadata::operator=( const GmodNodeMetadata& other ) noexcept
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

	inline std::string_view GmodNodeMetadata::category() const noexcept
	{
		return m_category;
	}

	inline std::string_view GmodNodeMetadata::type() const noexcept
	{
		return m_type;
	}

	inline std::string_view GmodNodeMetadata::fullType() const noexcept
	{
		return m_fullType;
	}

	inline std::string_view GmodNodeMetadata::name() const noexcept
	{
		return m_name;
	}

	inline const std::optional<std::string>& GmodNodeMetadata::commonName() const noexcept
	{
		return m_commonName;
	}

	inline const std::optional<std::string>& GmodNodeMetadata::definition() const noexcept
	{
		return m_definition;
	}

	inline const std::optional<std::string>& GmodNodeMetadata::commonDefinition() const noexcept
	{
		return m_commonDefinition;
	}

	inline const std::optional<bool>& GmodNodeMetadata::installSubstructure() const noexcept
	{
		return m_installSubstructure;
	}

	inline const StringMap<std::string>& GmodNodeMetadata::normalAssignmentNames() const noexcept
	{
		return m_normalAssignmentNames;
	}

	//=====================================================================
	// GmodNode class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline GmodNode::GmodNode( VisVersion version, const GmodNodeDto& dto ) noexcept
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
			  dto.normalAssignmentNames().has_value() ? *dto.normalAssignmentNames() : StringMap<std::string>() },
		  m_children{},
		  m_parents{},
		  m_childrenSet{}
	{
		size_t expectedChildren = estimateChildrenCount( dto.category(), dto.type() );
		size_t expectedParents = estimateParentsCount( dto.category(), dto.type() );

		m_children.reserve( expectedChildren );
		m_parents.reserve( expectedParents );
		m_childrenSet.reserve( expectedChildren );
	}

	inline GmodNode::GmodNode( const GmodNode& other ) noexcept
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

	inline GmodNode& GmodNode::operator=( const GmodNode& other ) noexcept
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

	inline const std::string& GmodNode::code() const noexcept
	{
		return m_code;
	}

	inline const std::optional<Location>& GmodNode::location() const noexcept
	{
		return m_location;
	}

	inline VisVersion GmodNode::visVersion() const noexcept
	{
		return m_visVersion;
	}

	inline const GmodNodeMetadata& GmodNode::metadata() const noexcept
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

	inline const std::vector<GmodNode*>& GmodNode::children() const noexcept
	{
		return m_children;
	}

	inline const std::vector<GmodNode*>& GmodNode::parents() const noexcept
	{
		return m_parents;
	}

	inline const GmodNode* GmodNode::productType() const noexcept
	{
		if ( m_children.size() != 1 )
		{
			return nullptr;
		}

		if ( !contains( m_metadata.category(), GMODNODE_CATEGORY_FUNCTION ) )
		{
			return nullptr;
		}

		const GmodNode* child = m_children[0];
		if ( !child )
		{
			return nullptr;
		}

		if ( child->m_metadata.category() != GMODNODE_CATEGORY_PRODUCT )
		{
			return nullptr;
		}

		if ( child->m_metadata.type() != GMODNODE_TYPE_TYPE )
		{
			return nullptr;
		}

		return child;
	}

	inline const GmodNode* GmodNode::productSelection() const noexcept
	{
		if ( m_children.size() != 1 )
		{
			return nullptr;
		}

		if ( !contains( m_metadata.category(), GMODNODE_CATEGORY_FUNCTION ) )
		{
			return nullptr;
		}

		const GmodNode* child = m_children[0];
		if ( !child )
		{
			return nullptr;
		}

		if ( !contains( child->m_metadata.category(), GMODNODE_CATEGORY_PRODUCT ) )
		{
			return nullptr;
		}

		if ( child->m_metadata.type() != GMODNODE_TYPE_SELECTION )
		{
			return nullptr;
		}

		return child;
	}

	//----------------------------------------------
	// Node type checking methods
	//----------------------------------------------

	inline bool GmodNode::isIndividualizable( bool isTargetNode, bool isInSet ) const noexcept
	{
		if ( m_metadata.type() == GMODNODE_TYPE_GROUP )
		{
			return false;
		}
		if ( m_metadata.type() == GMODNODE_TYPE_SELECTION )
		{
			return false;
		}
		if ( isProductType() )
		{
			return false;
		}
		if ( m_metadata.category() == GMODNODE_CATEGORY_ASSET && m_metadata.type() == GMODNODE_TYPE_TYPE )
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

	inline bool GmodNode::isFunctionComposition() const noexcept
	{
		return ( m_metadata.category() == GMODNODE_CATEGORY_ASSET_FUNCTION ||
				   m_metadata.category() == GMODNODE_CATEGORY_PRODUCT_FUNCTION ) &&
			   m_metadata.type() == GMODNODE_TYPE_COMPOSITION;
	}

	inline bool GmodNode::isMappable() const noexcept
	{
		if ( !m_cachedProductType.has_value() )
		{
			if ( m_children.size() == 1 &&
				 m_metadata.category().find( GMODNODE_CATEGORY_FUNCTION ) != std::string::npos )
			{
				const GmodNode* child = m_children[0];
				if ( child &&
					 child->m_metadata.category() == GMODNODE_CATEGORY_PRODUCT &&
					 child->m_metadata.type() == GMODNODE_TYPE_TYPE )
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
				 m_metadata.category().find( GMODNODE_CATEGORY_FUNCTION ) != std::string::npos )
			{
				const GmodNode* child = m_children[0];
				if ( child &&
					 child->m_metadata.category().find( GMODNODE_CATEGORY_PRODUCT ) != std::string::npos &&
					 child->m_metadata.type() == GMODNODE_TYPE_SELECTION )
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

		if ( m_metadata.category().find( GMODNODE_CATEGORY_PRODUCT ) != std::string::npos && m_metadata.type() == GMODNODE_TYPE_SELECTION )
		{
			return false;
		}

		if ( m_metadata.category() == GMODNODE_CATEGORY_ASSET )
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

	inline bool GmodNode::isChild( const GmodNode& node ) const noexcept
	{
		return isChild( node.m_code );
	}

	inline bool GmodNode::isChild( std::string_view code ) const noexcept
	{
		return m_childrenSet.contains( code );
	}

	//----------------------------------------------
	// String conversion methods
	//----------------------------------------------

	inline std::string GmodNode::toString() const noexcept
	{
		fmt::memory_buffer builder;
		toString( std::back_inserter( builder ) );

		return fmt::to_string( builder );
	}

	template <typename OutputIt>
	inline OutputIt GmodNode::toString( OutputIt out ) const noexcept
	{
		out = fmt::format_to( out, "{}", m_code );
		if ( m_location.has_value() )
		{
			out = fmt::format_to( out, "-{}", m_location->toString() );
		}

		return out;
	}

	//----------------------------------------------
	// Relationship management methods
	//----------------------------------------------

	inline void GmodNode::addChild( GmodNode* child ) noexcept
	{
		if ( !child )
		{
			return;
		}

		auto childCode = child->code();
		if ( m_childrenSet.contains( childCode ) )
		{
			return;
		}

		m_children.push_back( child );
		m_childrenSet.emplace( childCode );

		m_cachedProductType.reset();
		m_cachedProductSelection.reset();
	}

	inline void GmodNode::addParent( GmodNode* parent ) noexcept
	{
		if ( !parent )
		{
			return;
		}

		m_parents.push_back( parent );
	}

	inline void GmodNode::trim() noexcept
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
