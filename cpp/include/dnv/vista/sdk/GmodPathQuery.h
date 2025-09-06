/**
 * @file GmodPathQuery.h
 * @brief GMOD path query builder and matcher for filtering and searching GMOD hierarchies
 * @details Provides fluent builder pattern for constructing queries that match GMOD paths
 *          based on node codes and location constraints. Supports building queries from
 *          existing paths or from individual nodes with location filtering.
 */

#pragma once

#include <nfx/containers/StringMap.h>

#include "GmodPath.h"
#include "Locations.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// NodeItem class
	//=====================================================================

	/**
	 * @brief Internal node item for query filtering
	 * @details Stores GmodNode, associated locations, and matching behavior
	 */
	class NodeItem final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		NodeItem() = delete;

		/** @brief Construct NodeItem with specified node and locations */
		NodeItem( GmodNode node, std::vector<Location> locations ) noexcept;

		/** @brief Copy constructor */
		NodeItem( const NodeItem& ) = default;

		/** @brief Move constructor */
		NodeItem( NodeItem&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~NodeItem() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		NodeItem& operator=( const NodeItem& ) = default;

		/** @brief Move assignment operator */
		NodeItem& operator=( NodeItem&& ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the GmodNode associated with this filter item
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const GmodNode& node() const noexcept;

		/**
		 * @brief Get the vector of locations to match for this node
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::vector<Location>& locations() const noexcept;

		/**
		 * @brief Check if this item matches all locations (no location filtering)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool matchAllLocations() const noexcept;

		//----------------------------------------------
		// Mutators
		//----------------------------------------------

		/** @brief Set the locations vector for this node filter */
		inline void setLocations( std::vector<Location> locations ) noexcept;

		/** @brief Set whether this node should match all locations */
		inline void setMatchAllLocations( bool matchAll ) noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		GmodNode m_node;
		std::vector<Location> m_locations;
		bool m_matchAllLocations{ false };
	};

	//=====================================================================
	// GmodPathQuery class
	//=====================================================================

	/**
	 * @brief Immutable query object for matching GMOD paths
	 * @details Contains compiled query criteria for efficient path matching.
	 *          Provides factory methods for creating queries from paths or nodes.
	 */
	class GmodPathQuery final
	{
	private:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		GmodPathQuery() = default;

	public:
		/** @brief Construct query from existing GMOD path with automatic version conversion */
		explicit GmodPathQuery( const GmodPath& path );

		/** @brief Copy constructor */
		GmodPathQuery( const GmodPathQuery& ) = default;

		/** @brief Move constructor */
		GmodPathQuery( GmodPathQuery&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~GmodPathQuery() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		GmodPathQuery& operator=( const GmodPathQuery& ) = default;

		/** @brief Move assignment operator */
		GmodPathQuery& operator=( GmodPathQuery&& ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the source GMOD path (if created from a path)
		 * @return Optional reference to the source path
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<GmodPath>& sourcePath() const noexcept;

		//----------------------------------------------
		// Factory methods
		//----------------------------------------------

		/**
		 * @brief Create a query from an existing GMOD path
		 * @param path The GMOD path to base the query on
		 * @return New query that can be further configured
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static GmodPathQuery fromPath( const GmodPath& path );

		/**
		 * @brief Create an empty query for node-based filtering
		 * @return New empty query that can be configured with individual nodes
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static GmodPathQuery empty();

		//----------------------------------------------
		// Build methods (Immutable fluent interface)
		//----------------------------------------------

		//----------------------------
		// Paths
		//----------------------------

		/**
		 * @brief Add node constraint with selector function and location matching behavior
		 * @param selector Function to select a node from available set nodes
		 * @param matchAllLocations Whether to match all locations for this node
		 * @return New query instance with the constraint added
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] GmodPathQuery withNode(
			std::function<const GmodNode&( const nfx::containers::StringMap<GmodNode>& )> selector,
			bool matchAllLocations = false ) const;

		/**
		 * @brief Add node constraint with selector function and specific locations
		 * @param selector Function to select a node from available set nodes
		 * @param locations Vector of specific locations to match
		 * @return New query instance with the constraint added
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] GmodPathQuery withNode(
			std::function<const GmodNode&( const nfx::containers::StringMap<GmodNode>& )> selector,
			const std::vector<Location>& locations ) const;

		//----------------------------
		// Nodes
		//----------------------------

		/**
		 * @brief Add node constraint with location matching behavior
		 * @param node The GMOD node to add to the query
		 * @param matchAllLocations Whether to match all locations for this node
		 * @return New query instance with the constraint added
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] GmodPathQuery withNode( const GmodNode& node, bool matchAllLocations = false ) const;

		/**
		 * @brief Add node constraint with specific locations
		 * @param node The GMOD node to add to the query
		 * @param locations Vector of specific locations to match
		 * @return New query instance with the constraint added
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] GmodPathQuery withNode( const GmodNode& node, const std::vector<Location>& locations ) const;

		//----------------------------------------------
		// Location manipulation
		//----------------------------------------------

		/**
		 * @brief Remove location constraints for all nodes in the query
		 * @return New query instance without location constraints
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline GmodPathQuery withoutLocations() const;

		//----------------------------------------------
		// Query execution
		//----------------------------------------------

		/**
		 * @brief Test if a GMOD path matches this query
		 * @param other The path to test against this query
		 * @return True if the path matches all query criteria
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool match( const GmodPath* other ) const;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		nfx::containers::StringMap<NodeItem> m_filter;
		std::optional<GmodPath> m_sourcePath;
		nfx::containers::StringMap<GmodNode> m_setNodes;

		//----------------------------------------------
		// Helper methods
		//----------------------------------------------

		/**
		 * @brief Ensures the path uses the correct VIS version for query operations
		 * @param path The path to validate and potentially convert
		 * @return Path with proper VIS version for querying
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static GmodPath ensurePathVersion( const GmodPath& path );

		/**
		 * @brief Ensures the node uses the correct VIS version for query operations
		 * @param node The node to validate and potentially convert
		 * @return Node with proper VIS version for querying
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static GmodNode ensureNodeVersion( const GmodNode& node );
	};
}

#include "GmodPathQuery.inl"
