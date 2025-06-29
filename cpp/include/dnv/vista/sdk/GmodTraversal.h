/**
 * @file GmodTraversal.h
 * @brief High-performance GMOD tree traversal algorithms
 * @details Optimized traversal functions with cycle detection and branch prediction.
 *          Supports both stateless and stateful handlers with zero-overhead templates.
 */

#pragma once

#include "Gmod.h"
#include "GmodNode.h"
#include "utils/StringUtils.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Enumerations
	//=====================================================================

	/**
	 * @enum TraversalHandlerResult
	 * @brief Controls traversal flow
	 */
	enum class TraversalHandlerResult
	{
		Stop = 0,
		SkipSubtree,
		Continue,
	};

	//=====================================================================
	// Configuration
	//=====================================================================

	/**
	 * @struct TraversalOptions
	 * @brief Traversal configuration with cycle detection
	 */
	struct TraversalOptions
	{
		static constexpr int DEFAULT_MAX_TRAVERSAL_OCCURRENCE = 1;
		int maxTraversalOccurrence = DEFAULT_MAX_TRAVERSAL_OCCURRENCE;
	};

	//=====================================================================
	// Handler types
	//=====================================================================

	/**
	 * @typedef TraverseHandler
	 * @brief Function pointer for stateless traversal handlers
	 */
	using TraverseHandler = TraversalHandlerResult ( * )( const std::vector<GmodNode>& parents, const GmodNode& node );

	/**
	 * @typedef TraverseHandlerWithState
	 * @brief Function pointer for stateful traversal handlers
	 */
	template <typename TState>
	using TraverseHandlerWithState = TraversalHandlerResult ( * )( TState& state, const std::vector<GmodNode>& parents, const GmodNode& node );

	//=====================================================================
	// Traversal algorithms
	//=====================================================================

	/**
	 * @namespace GmodTraversal
	 * @brief High-performance GMOD tree traversal algorithms
	 */
	namespace GmodTraversal
	{
		/**
		 * @namespace detail
		 * @brief Internal implementation details
		 */
		namespace detail
		{
			//----------------------------------------------
			// Parents class
			//----------------------------------------------

			/**
			 * @class Parents
			 * @brief Parent stack with occurrence tracking
			 */
			class Parents
			{
			public:
				//----------------------------
				// Construction
				//----------------------------

				/** @brief Constructor */
				inline Parents();

				//----------------------------
				// Stack operations
				//----------------------------

				/** @brief Push parent */
				inline void push( const GmodNode& parent );

				/** @brief Pop parent */
				inline void pop();

				/** @brief Get occurrence count */
				[[nodiscard]] inline int occurrences( const GmodNode& node ) const noexcept;

				/** @brief Get last parent */
				[[nodiscard]] inline const GmodNode* lastOrDefault() const noexcept;

				/** @brief Get complete parent chain */
				[[nodiscard]] inline const std::vector<GmodNode>& asList() const noexcept;

			private:
				//----------------------------
				// Private member variables
				//----------------------------

				/** @brief Parent chain from root to current */
				std::vector<GmodNode> m_parents;

				/** @brief Occurrence count per node code */
				StringMap<int> m_occurrences;
			};

			//----------------------------------------------
			// TraversalContext struct
			//----------------------------------------------

			/**
			 * @struct TraversalContext
			 * @brief Bundles traversal state
			 */
			template <typename TState>
			struct TraversalContext
			{
				Parents& parents;
				TraverseHandlerWithState<TState> handler;
				TState& state;
				int maxTraversalOccurrence;

				explicit TraversalContext( Parents& p, TraverseHandlerWithState<TState> h, TState& s, int maxOcc )
					: parents{ p },
					  handler{ h },
					  state{ s },
					  maxTraversalOccurrence{ maxOcc }
				{
				}

				TraversalContext( const TraversalContext& ) = delete;
				TraversalContext& operator=( const TraversalContext& ) = delete;
				TraversalContext( TraversalContext&& ) = delete;
				TraversalContext& operator=( TraversalContext&& ) = delete;
			};

			//----------------------------------------------
			// PathExistsContext struct
			//----------------------------------------------

			/**
			 * @struct PathExistsContext
			 * @brief Context for path existence checking
			 */
			struct PathExistsContext
			{
				const GmodNode& to;
				std::vector<GmodNode> remainingParents;
				std::vector<GmodNode> fromPath;

				PathExistsContext( const GmodNode& toNode, const std::vector<GmodNode>& fromPathList )
					: to{ toNode },
					  fromPath{ fromPathList }
				{
					remainingParents.reserve( 32 );
				}

				PathExistsContext( const PathExistsContext& ) = delete;
				PathExistsContext& operator=( const PathExistsContext& ) = delete;
				PathExistsContext( PathExistsContext&& ) = delete;
				PathExistsContext& operator=( PathExistsContext&& ) = delete;
			};

			//----------------------------------------------
			// Core algorithm
			//----------------------------------------------

			/**
			 * @brief Core recursive traversal
			 * @tparam TState User-defined state type
			 * @param context Traversal context
			 * @param node Current node to traverse
			 * @return Traversal result
			 */
			template <typename TState>
			[[nodiscard]] TraversalHandlerResult traverseNode( TraversalContext<TState>& context, const GmodNode& node )
			{
				if ( node.metadata().installSubstructure().has_value() && !node.metadata().installSubstructure().value() )
					return TraversalHandlerResult::Continue;

				TraversalHandlerResult result = context.handler( context.state, context.parents.asList(), node );
				if ( result == TraversalHandlerResult::Stop || result == TraversalHandlerResult::SkipSubtree )
				{
					return result;
				}

				bool skipOccurrenceCheck = Gmod::isProductSelectionAssignment( context.parents.lastOrDefault(), &node );
				if ( !skipOccurrenceCheck )
				{
					int occ = context.parents.occurrences( node );
					if ( occ == context.maxTraversalOccurrence )
					{
						return TraversalHandlerResult::SkipSubtree;
					}
					if ( occ > context.maxTraversalOccurrence )
					{
						throw std::runtime_error( "Invalid state - node occurred more than expected" );
					}
				}

				context.parents.push( node );

				const auto& children = node.children();
				for ( const auto& child : children )
				{
					result = traverseNode( context, child );
					if ( result == TraversalHandlerResult::Stop )
					{
						context.parents.pop();
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
		}

		//=====================================================================
		// Public API
		//=====================================================================

		//----------------------------------------------
		// Non-template functions
		//----------------------------------------------

		/**
		 * @brief Traverse GMOD tree from root with stateless handler
		 * @param gmodInstance GMOD instance to traverse
		 * @param handler Function pointer to handler
		 * @param options Traversal configuration
		 * @return true if completed, false if stopped early
		 */
		[[nodiscard]] bool traverse( const Gmod& gmodInstance, TraverseHandler handler, const TraversalOptions& options = {} );

		/**
		 * @brief Traverse GMOD tree from specific node with stateless handler
		 * @param rootNode Starting node
		 * @param handler Function pointer to handler
		 * @param options Traversal configuration
		 * @return true if completed, false if stopped early
		 */
		[[nodiscard]] bool traverse( const GmodNode& rootNode, TraverseHandler handler, const TraversalOptions& options = {} );

		/**
		 * @brief Check if path exists between nodes
		 * @param gmodInstance GMOD instance
		 * @param fromPath Source path
		 * @param to Target node
		 * @param remainingParents Output: parents not in fromPath
		 * @return true if path exists
		 */
		[[nodiscard]] bool pathExistsBetween(
			const Gmod& gmodInstance,
			const std::vector<GmodNode>& fromPath,
			const GmodNode& to,
			std::vector<GmodNode>& remainingParents );

		//----------------------------------------------
		// Template functions
		//----------------------------------------------

		/**
		 * @brief Traverse GMOD tree from root with stateful handler
		 * @tparam TState User-defined state type
		 * @param state User state reference
		 * @param gmodInstance GMOD instance to traverse
		 * @param handler Function pointer to stateful handler
		 * @param options Traversal configuration
		 * @return true if completed, false if stopped early
		 */
		template <typename TState>
		bool traverse( TState& state, const Gmod& gmodInstance, TraverseHandlerWithState<TState> handler, const TraversalOptions& options = {} )
		{
			detail::Parents parentsStack;
			detail::TraversalContext<TState> context( parentsStack, handler, state, options.maxTraversalOccurrence );

			return detail::traverseNode( context, gmodInstance.rootNode() ) == TraversalHandlerResult::Continue;
		}

		/**
		 * @brief Traverse GMOD tree from specific node with stateful handler
		 * @tparam TState User-defined state type
		 * @param state User state reference
		 * @param rootNode Starting node
		 * @param handler Function pointer to stateful handler
		 * @param options Traversal configuration
		 * @return true if completed, false if stopped early
		 */
		template <typename TState>
		bool traverse( TState& state, const GmodNode& rootNode, TraverseHandlerWithState<TState> handler, const TraversalOptions& options = {} )
		{
			detail::Parents parentsStack;
			detail::TraversalContext<TState> context( parentsStack, handler, state, options.maxTraversalOccurrence );

			return detail::traverseNode( context, rootNode ) == TraversalHandlerResult::Continue;
		}
	}
}

#include "GmodTraversal.inl"
