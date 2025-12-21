/*
 * MIT License
 *
 * Copyright (c) 2025 DNV
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file Gmod.h
 * @brief Generic Product Model (Gmod) container and tree structure
 * @details Manages the complete Gmod hierarchy for a specific VIS version.
 *          Provides node lookup, tree navigation, and iteration capabilities.
 *          All GmodNode instances are owned and managed by this class.
 */

#pragma once

#include "GmodNode.h"
#include "VisVersions.h"

#include <nfx/Containers.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace dnv::vista::sdk
{
	struct GmodDto;

	/**
	 * @class Gmod
	 * @brief Generic Product Model container for a specific VIS version
	 * @details The Gmod class manages the complete tree hierarchy of GmodNode instances
	 *          for a specific VIS version. It provides:
	 *          - Node storage and ownership
	 *          - Fast node lookup by code with operator[] and node()
	 *          - Root node access ("VE")
	 *          - Static helper methods for node classification
	 */
	class Gmod final
	{
		friend class VIS;

	private:
		/**
		 * @brief Construct Gmod from DTO data
		 * @param version The VIS version for this Gmod
		 * @param dto DTO containing nodes and relations
		 */
		explicit Gmod( VisVersion version, const GmodDto& dto );

		/**
		 * @brief Construct Gmod from existing node map
		 * @param version The VIS version for this Gmod
		 * @param nodeMap Pre-built map of nodes (used for versioning conversions)
		 */
		explicit Gmod( VisVersion version, const std::unordered_map<std::string, GmodNode, StringHash, std::equal_to<>>& nodeMap );

	public:
		/** @brief Default constructor */
		Gmod() = delete;

		/** @brief Copy constructor */
		Gmod( const Gmod& ) = delete;

		/** @brief Move constructor */
		Gmod( Gmod&& ) noexcept = default;

		/** @brief Destructor */
		~Gmod() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		Gmod& operator=( const Gmod& ) = delete;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		Gmod& operator=( Gmod&& ) noexcept = default;

		/**
		 * @brief Access node by code with bounds checking
		 * @param code Node code (e.g., "411.1", "VE", "C101")
		 * @return Const reference to the GmodNode
		 * @throws std::out_of_range if code not found
		 */
		[[nodiscard]] const GmodNode& operator[]( std::string_view code ) const;

		/**
		 * @brief Get the VIS version for this Gmod
		 * @return The VisVersion enum value
		 */
		[[nodiscard]] inline VisVersion version() const noexcept;

		/**
		 * @brief Get the root node of the tree
		 * @return Const reference to the root GmodNode (code "VE" - Vessel Equipment)
		 */
		[[nodiscard]] inline const GmodNode& rootNode() const noexcept;

		/**
		 * @brief Check if a node type can have child scopes
		 * @param type Node type string (e.g., "SELECTION", "GROUP", "LEAF", "COMPOSITION")
		 * @return True if type is "SELECTION", "GROUP", or "LEAF"
		 * @details These types can have location-based child scopes in the tree hierarchy.
		 */
		[[nodiscard]] static bool isPotentialParent( std::string_view type ) noexcept;

		/**
		 * @brief Check if parent-child relationship is a product type assignment
		 * @param parent Parent node (can be nullptr)
		 * @param child Child node (can be nullptr)
		 * @return True if parent is a FUNCTION node and child is a PRODUCT TYPE
		 * @details Product type assignments link functions to their product implementations.
		 */
		[[nodiscard]] static bool isProductTypeAssignment( const GmodNode* parent, const GmodNode* child ) noexcept;

		/**
		 * @brief Check if parent-child relationship is a product selection assignment
		 * @param parent Parent node (can be nullptr)
		 * @param child Child node (can be nullptr)
		 * @return True if parent is a FUNCTION node and child is a PRODUCT SELECTION
		 * @details Product selection assignments link functions to product choice points.
		 */
		[[nodiscard]] static bool isProductSelectionAssignment( const GmodNode* parent, const GmodNode* child ) noexcept;

		/**
		 * @brief Try to get a node by code
		 * @param code Node code to search for (e.g., "411.1", "VE")
		 * @return Optional pointer to the node if found, std::nullopt otherwise
		 */
		[[nodiscard]] std::optional<const GmodNode*> node( std::string_view code ) const noexcept;

		/**
		 * @brief Get iterator to first node
		 * @return Iterator to the first element in the node map
		 */
		[[nodiscard]] auto begin() const noexcept;
		/**
		 * @brief Get iterator past last node
		 * @return Iterator to the element following the last element
		 */
		[[nodiscard]] auto end() const noexcept;
		/**
		 * @brief Get const iterator to first node
		 * @return Const iterator to the first element in the node map
		 */
		[[nodiscard]] auto cbegin() const noexcept;
		/**
		 * @brief Get const iterator past last node
		 * @return Const iterator to the element following the last element
		 */
		[[nodiscard]] auto cend() const noexcept;

		VisVersion m_visVersion;													///< VIS version for this Gmod
		GmodNode* m_rootNode;														///< Pointer to root node ("VE") in m_nodeMap
		nfx::containers::PerfectHashMap<std::string, GmodNode, uint32_t> m_nodeMap; ///< All nodes indexed by code (owns all nodes, transparent hash)
	};
} // namespace dnv::vista::sdk

#include "detail/Gmod.inl"
