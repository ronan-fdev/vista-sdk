/**
 * @file Gmod.inl
 * @brief Inline implementations for performance-critical Gmod operations
 */

#pragma once

#include "constants/CodebookConstants.h"
#include "utils/StringUtils.h"

#include "GmodNode.h"

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

	VISTA_SDK_CPP_FORCE_INLINE bool Gmod::tryGetNode( std::string_view code, const GmodNode*& node ) const noexcept
	{
		return m_nodeMap.tryGetValue( code, node );
	}

	//----------------------------------------------
	// Static state inspection methods
	//----------------------------------------------

	inline constexpr bool Gmod::isPotentialParent( std::string_view type ) noexcept
	{
		/**
		 * Length 4: "Leaf" - constants::gmod::GMODNODE_TYPE_LEAF
		 *   - Represents terminal nodes in the GMOD hierarchy
		 *   - Cannot have child nodes
		 *
		 * Length 5: "Group" - constants::gmod::GMODNODE_TYPE_GROUP
		 *   - Represents container nodes that can hold other nodes
		 *   - Used for organizational grouping in the hierarchy
		 *
		 * Length 9: "Selection" - constants::gmod::GMODNODE_TYPE_SELECTION
		 *   - Represents selection nodes for product configurations
		 *   - Can contain multiple selectable options
		 */

		if ( utils::hasExactLength( type, 4 ) )
		{
			return utils::equals( type, constants::gmod::GMODNODE_TYPE_LEAF );
		}
		else if ( utils::hasExactLength( type, 5 ) )
		{
			return utils::equals( type, constants::gmod::GMODNODE_TYPE_GROUP );
		}
		else if ( utils::hasExactLength( type, 9 ) )
		{
			return utils::equals( type, constants::gmod::GMODNODE_TYPE_SELECTION );
		}

		return false;
	}

	inline bool Gmod::isLeafNode( const GmodNodeMetadata& metadata ) noexcept
	{
		const auto& fullType = metadata.fullType();
		return utils::equals( fullType, constants::gmod::GMODNODE_FULLTYPE_ASSET_FUNCTION_LEAF ) ||
			   utils::equals( fullType, constants::gmod::GMODNODE_FULLTYPE_PRODUCT_FUNCTION_LEAF );
	}

	inline bool Gmod::isFunctionNode( const GmodNodeMetadata& metadata ) noexcept
	{
		const auto& category = metadata.category();

		return !utils::equals( category, constants::gmod::GMODNODE_CATEGORY_PRODUCT ) &&
			   !utils::equals( category, constants::gmod::GMODNODE_CATEGORY_ASSET );
	}

	inline bool Gmod::isProductSelection( const GmodNodeMetadata& metadata ) noexcept
	{
		return utils::equals( metadata.category(), constants::gmod::GMODNODE_CATEGORY_PRODUCT ) &&
			   utils::equals( metadata.type(), constants::gmod::GMODNODE_TYPE_SELECTION );
	}

	inline bool Gmod::isProductType( const GmodNodeMetadata& metadata ) noexcept
	{
		return utils::equals( metadata.category(), constants::gmod::GMODNODE_CATEGORY_PRODUCT ) &&
			   utils::equals( metadata.type(), constants::gmod::GMODNODE_TYPE_TYPE );
	}

	inline bool Gmod::isAsset( const GmodNodeMetadata& metadata ) noexcept
	{
		return utils::equals( metadata.category(), constants::gmod::GMODNODE_CATEGORY_ASSET );
	}

	inline bool Gmod::isAssetFunctionNode( const GmodNodeMetadata& metadata ) noexcept
	{
		return utils::equals( metadata.category(), constants::gmod::GMODNODE_CATEGORY_ASSET_FUNCTION );
	}

	inline bool Gmod::isProductTypeAssignment( const GmodNode* parent, const GmodNode* child ) noexcept
	{
		if ( !parent || !child )
		{
			return false;
		}

		const auto& parentCategory = parent->metadata().category();
		const auto& childCategory = child->metadata().category();
		const auto& childType = child->metadata().type();

		if ( !utils::contains( parentCategory, constants::gmod::GMODNODE_CATEGORY_FUNCTION ) )
		{
			return false;
		}
		if ( !utils::equals( childCategory, constants::gmod::GMODNODE_CATEGORY_PRODUCT ) ||
			 !utils::equals( childType, constants::gmod::GMODNODE_TYPE_TYPE ) )
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

		if ( !utils::contains( parentCategory, constants::gmod::GMODNODE_CATEGORY_FUNCTION ) )
		{
			return false;
		}
		if ( !utils::equals( childCategory, constants::gmod::GMODNODE_CATEGORY_PRODUCT ) ||
			 !utils::equals( childType, constants::gmod::GMODNODE_TYPE_SELECTION ) )
		{
			return false;
		}

		return true;
	}

	//----------------------------------------------
	// Enumeration
	//----------------------------------------------

	inline Gmod::Enumerator Gmod::enumerator() const { return Enumerator( &m_nodeMap ); }

	//----------------------------------------------
	// Gmod::Enumerator class
	//----------------------------------------------

	//-----------------------------
	// Construction
	//-----------------------------

	VISTA_SDK_CPP_FORCE_INLINE Gmod::Enumerator::Enumerator( const internal::ChdDictionary<GmodNode>* map ) noexcept
		: m_sourceMapPtr{ map }, m_isInitialState{ true }
	{
		if ( m_sourceMapPtr )
		{
			m_currentMapIterator = m_sourceMapPtr->begin();
		}
	}

	//-----------------------------
	// Iteration interface
	//-----------------------------

	VISTA_SDK_CPP_FORCE_INLINE const GmodNode& Gmod::Enumerator::current() const
	{
		if ( !m_sourceMapPtr || m_isInitialState || m_currentMapIterator == m_sourceMapPtr->end() )
		{
			throw std::out_of_range( "Gmod::Enumerator::getCurrent() called in an invalid state or past the end." );
		}

		return m_currentMapIterator->second;
	}

	VISTA_SDK_CPP_FORCE_INLINE bool Gmod::Enumerator::next() noexcept
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

	VISTA_SDK_CPP_FORCE_INLINE void Gmod::Enumerator::reset() noexcept
	{
		m_isInitialState = true;

		if ( m_sourceMapPtr )
		{
			m_currentMapIterator = m_sourceMapPtr->begin();
		}
	}

	//=====================================================================
	// Traversal implementation
	//=====================================================================

	//----------------------------------------------
	// Parents class implementation
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE Gmod::Parents::Parents()
	{
		m_parents.reserve( 64 );
		m_occurrences.reserve( 4 );
	}

	VISTA_SDK_CPP_FORCE_INLINE void Gmod::Parents::push( const GmodNode* parent )
	{
		m_parents.push_back( parent );
		const auto* countPtr = m_occurrences.tryGetValue( parent->code() );
		if ( countPtr )
		{
			m_occurrences.insertOrAssign( std::string{ parent->code() }, *countPtr + 1 );
		}
		else
		{
			m_occurrences.insertOrAssign( std::string{ parent->code() }, 1 );
		}
	}

	VISTA_SDK_CPP_FORCE_INLINE void Gmod::Parents::pop()
	{
		if ( m_parents.empty() )
		{
			return;
		}

		const GmodNode* parent = m_parents.back();
		m_parents.pop_back();

		const auto* countPtr = m_occurrences.tryGetValue( parent->code() );
		if ( countPtr )
		{
			if ( *countPtr == 1 )
			{
				m_occurrences.erase( parent->code() );
			}
			else
			{
				m_occurrences.insertOrAssign( std::string{ parent->code() }, *countPtr - 1 );
			}
		}
	}

	inline size_t Gmod::Parents::occurrences( const GmodNode& node ) const noexcept
	{
		const auto* countPtr = m_occurrences.tryGetValue( node.code() );
		if ( countPtr )
		{
			return *countPtr;
		}
		else
		{
			return 0;
		}
	}

	inline const GmodNode* Gmod::Parents::lastOrDefault() const noexcept
	{
		return m_parents.empty() ? nullptr : m_parents.back();
	}

	inline const std::vector<const GmodNode*>& Gmod::Parents::asList() const noexcept
	{
		return m_parents;
	}

	//----------------------------------------------
	// Core traversal implementation
	//----------------------------------------------

	template <typename TState>
	inline TraversalHandlerResult Gmod::traverseNode( TraversalContext<TState>& context, const GmodNode& node ) const
	{
		if ( node.metadata().installSubstructure().has_value() && !node.metadata().installSubstructure().value() )
		{
			return TraversalHandlerResult::Continue;
		}

		TraversalHandlerResult result = context.handler( context.state, context.parents.asList(), node );
		if ( result == TraversalHandlerResult::Stop || result == TraversalHandlerResult::SkipSubtree )
		{
			return result;
		}

		bool skipOccurrenceCheck = isProductSelectionAssignment( context.parents.lastOrDefault(), &node );
		if ( !skipOccurrenceCheck )
		{
			size_t occ = context.parents.occurrences( node );
			if ( occ == context.maxTraversalOccurrence )
			{
				return TraversalHandlerResult::SkipSubtree;
			}
			if ( occ > context.maxTraversalOccurrence )
			{
				throw std::runtime_error( "Invalid state - node occurred more than expected" );
			}
		}

		context.parents.push( &node );

		const auto& children = node.children();
		for ( size_t i = 0; i < children.size(); ++i )
		{
			const GmodNode& child = *children[i];
			result = traverseNode( context, child );
			if ( result == TraversalHandlerResult::Stop )
			{
				return TraversalHandlerResult::Stop;
			}
			else if ( result == TraversalHandlerResult::SkipSubtree )
			{
				continue;
			}
		}

		context.parents.pop();

		return TraversalHandlerResult::Continue;
	}

	//----------------------------------------------
	// Public traverse method implementations
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE bool Gmod::traverse( TraverseHandler handler, const TraversalOptions& options ) const
	{
		TraverseHandler capturedHandler = handler;
		TraverseHandlerWithState<TraverseHandler> wrapperHandler =
			[]( TraverseHandler& h, const std::vector<const GmodNode*>& parents,
				const GmodNode& node ) -> TraversalHandlerResult { return h( parents, node ); };

		return traverse( capturedHandler, wrapperHandler, options );
	}

	template <typename TState>
	VISTA_SDK_CPP_FORCE_INLINE bool Gmod::traverse(
		TState& state,
		TraverseHandlerWithState<TState> handler,
		const TraversalOptions& options ) const
	{
		Parents parentsStack;
		TraversalContext<TState> context( parentsStack, handler, state, options.maxTraversalOccurrence );

		return traverseNode( context, *m_rootNode ) == TraversalHandlerResult::Continue;
	}

	VISTA_SDK_CPP_FORCE_INLINE bool Gmod::traverse( const GmodNode& rootNode, TraverseHandler handler, const TraversalOptions& options ) const
	{
		TraverseHandler capturedHandler = handler;
		TraverseHandlerWithState<TraverseHandler> wrapperHandler =
			[]( TraverseHandler& h, const std::vector<const GmodNode*>& parents,
				const GmodNode& node ) -> TraversalHandlerResult { return h( parents, node ); };

		Parents parentsStack;
		TraversalContext<TraverseHandler> context( parentsStack, wrapperHandler, capturedHandler, options.maxTraversalOccurrence );

		return traverseNode( context, rootNode ) == TraversalHandlerResult::Continue;
	}

	template <typename TState>
	VISTA_SDK_CPP_FORCE_INLINE bool Gmod::traverse(
		TState& state, const GmodNode& rootNode, TraverseHandlerWithState<TState> handler, const TraversalOptions& options ) const
	{
		Parents parentsStack;
		TraversalContext<TState> context( parentsStack, handler, state, options.maxTraversalOccurrence );

		return traverseNode( context, rootNode ) == TraversalHandlerResult::Continue;
	}
}
