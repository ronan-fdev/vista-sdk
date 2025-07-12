/**
 * @file GmodTraversal.h
 * @brief High-performance GMOD tree traversal algorithms
 * @details Optimized traversal functions with cycle detection and branch prediction.
 *          Supports both stateless and stateful handlers with zero-overhead templates.
 */

#pragma once

#include "Gmod.h"
#include "GmodNode.h"

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
		static constexpr size_t DEFAULT_MAX_TRAVERSAL_OCCURRENCE = 1;
		size_t maxTraversalOccurrence = DEFAULT_MAX_TRAVERSAL_OCCURRENCE;
	};

	/**
	 * @typedef TraverseHandler
	 * @brief Function pointer for stateless traversal handlers
	 */
	using TraverseHandler = TraversalHandlerResult ( * )( const std::vector<const GmodNode*>& parents, const GmodNode& node );

	/**
	 * @typedef TraverseHandlerWithState
	 * @brief Function pointer for stateful traversal handlers
	 * @tparam TState User-defined state type
	 */
	template <typename TState>
	using TraverseHandlerWithState = TraversalHandlerResult ( * )( TState& state, const std::vector<const GmodNode*>& parents, const GmodNode& node );

	//=====================================================================
	// Traversal algorithms
	//=====================================================================

	/**
	 * @namespace GmodTraversal
	 * @brief High-performance GMOD tree traversal algorithms
	 * @details Optimized implementations with cycle detection and branch prediction.
	 */
	namespace GmodTraversal
	{
		/**
		 * @namespace detail
		 * @brief Internal implementation details
		 * @warning Internal use only - API may change without notice
		 */
		namespace detail
		{
			//----------------------------------------------
			// GmodTraversal::Parents class
			//----------------------------------------------

			/**
			 * @class Parents
			 * @brief Optimized parent stack with occurrence tracking
			 * @details Pre-allocated for 64 parents and 4 occurrence entries.
			 */
			class Parents
			{
			public:
				//----------------------------
				// Construction
				//----------------------------

				/** @brief Default constructor. */
				inline Parents( size_t maxOccurrence = 1 );

				//----------------------------
				// Stack operations
				//----------------------------

				/** @brief Push parent and update occurrence count */
				inline void push( const GmodNode* parent );

				/** @brief Pop parent and update occurrence count */
				inline void pop();

				/** @brief Get occurrence count for node */
				[[nodiscard]] inline size_t occurrences( const GmodNode& node ) const noexcept;

				/** @brief Get last parent or nullptr */
				[[nodiscard]] inline const GmodNode* lastOrDefault() const noexcept;

				/** @brief Get complete parent chain */
				[[nodiscard]] inline const std::vector<const GmodNode*>& asList() const noexcept;

			private:
				//----------------------------
				// Private member variables
				//----------------------------

				/** @brief Parent chain from root to current */
				std::vector<const GmodNode*> m_parents;

				/** @brief Occurrence count per node code */
				StringMap<size_t> m_occurrences;

				size_t m_maxTraversalOccurrence;
			};

			//----------------------------------------------
			// TraversalContext struct
			//----------------------------------------------

			/**
			 * @struct TraversalContext
			 * @brief Bundles traversal state for optimal performance
			 * @tparam TState User-defined state type
			 */
			template <typename TState>
			struct TraversalContext
			{
				Parents& parents;
				TraverseHandlerWithState<TState> handler;
				TState& state;
				size_t maxTraversalOccurrence;

				explicit TraversalContext( Parents& p, TraverseHandlerWithState<TState> h, TState& s, size_t maxOcc )
					: parents( p ), handler( h ), state( s ), maxTraversalOccurrence( maxOcc ) {}

				TraversalContext() = delete;
				TraversalContext( const TraversalContext& ) = delete;
				TraversalContext( TraversalContext&& ) noexcept = delete;
				~TraversalContext() = default;
				TraversalContext& operator=( const TraversalContext& ) = delete;
				TraversalContext& operator=( TraversalContext&& ) noexcept = delete;
			};

			//----------------------------------------------
			// Core algorithm
			//----------------------------------------------

			/**
			 * @brief Core recursive traversal with optimizations
			 * @details Performance-critical hot path with branch prediction hints
			 * @tparam TState User-defined state type
			 * @param context Traversal context
			 * @param node Current node to traverse
			 * @return Traversal result
			 */
			template <typename TState>
			[[nodiscard]] TraversalHandlerResult traverseNode( TraversalContext<TState>& context, const GmodNode& node );
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
			const std::vector<const GmodNode*>& fromPath,
			const GmodNode& to,
			std::vector<const GmodNode*>& remainingParents );

		//----------------------------------------------
		// Zero-overhead template functions
		//----------------------------------------------

		/**
		 * @brief Traverse GMOD tree from root with stateful handler
		 * @details Zero-overhead template for maximum performance
		 * @tparam TState User-defined state type
		 * @param gmodInstance GMOD instance to traverse
		 * @param state User state reference
		 * @param handler Function pointer to stateful handler
		 * @param options Traversal configuration
		 * @return true if completed, false if stopped early
		 */
		template <typename TState>
		bool traverse( TState& state, const Gmod& gmodInstance, TraverseHandlerWithState<TState> handler, const TraversalOptions& options = {} );

		/**
		 * @brief Traverse GMOD tree from specific node with stateful handler
		 * @details Zero-overhead template for maximum performance
		 * @tparam TState User-defined state type
		 * @param state User state reference
		 * @param rootNode Starting node
		 * @param handler Function pointer to stateful handler
		 * @param options Traversal configuration
		 * @return true if completed, false if stopped early
		 */
		template <typename TState>
		bool traverse( TState& state, const GmodNode& rootNode, TraverseHandlerWithState<TState> handler, const TraversalOptions& options = {} );
	}
}

#include "GmodTraversal.inl"
