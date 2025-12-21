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
 * @file GmodNode.h
 * @brief Gmod node and metadata representation for VIS Generic Product Model
 * @details Represents nodes in the Gmod (Generic Product Model) tree hierarchy.
 *            - GmodNodeMetadata: Descriptive information (category, type, names, definitions).
 *            - GmodNode: Tree node with code, location, metadata, and parent/child relationships.
 */

#pragma once

#include "Locations.h"
#include "StringBuilder.h"
#include "StringHash.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace dnv::vista::sdk
{
	enum class VisVersion : std::uint8_t;
	struct GmodNodeDto;

	/**
	 * @class GmodNodeMetadata
	 * @brief Metadata for a Gmod (Generic Product Model) node
	 * @details Contains descriptive information about a Gmod node including:
	 *          - Category: The node's category (e.g., "ASSET FUNCTION", "PRODUCT")
	 *          - Type: The node's type (e.g., "COMPOSITION", "TYPE", "SELECTION")
	 *          - Name: The node's name
	 *          - CommonName: Optional common/friendly name
	 *          - Definition: Optional technical definition
	 *          - CommonDefinition: Optional user-friendly definition
	 *          - InstallSubstructure: Optional flag for installation substructure
	 *          - NormalAssignmentNames: Dictionary of normal assignment names
	 *          - FullType: Computed property = "Category Type" (e.g., "ASSET FUNCTION COMPOSITION")
	 *
	 *          GmodNodeMetadata instances are created by GmodNode from GmodNodeDto data.
	 */
	class GmodNodeMetadata final
	{
		friend class GmodNode;

	private:
		/**
		 * @brief Construct GmodNodeMetadata with all properties
		 * @param category Node category (e.g., "ASSET FUNCTION", "PRODUCT")
		 * @param type Node type (e.g., "COMPOSITION", "TYPE", "SELECTION")
		 * @param name Node name
		 * @param commonName Optional common/friendly name
		 * @param definition Optional technical definition
		 * @param commonDefinition Optional user-friendly definition
		 * @param installSubstructure Optional installation substructure flag
		 * @param normalAssignmentNames Dictionary of normal assignment names (can be empty)
		 */
		explicit GmodNodeMetadata(
			std::string_view category,
			std::string_view type,
			std::string_view name,
			std::optional<std::string> commonName = std::nullopt,
			std::optional<std::string> definition = std::nullopt,
			std::optional<std::string> commonDefinition = std::nullopt,
			std::optional<bool> installSubstructure = std::nullopt,
			std::unordered_map<std::string, std::string, StringHash, std::equal_to<>> normalAssignmentNames = {} ) noexcept;

	public:
		/** @brief Default constructor */
		GmodNodeMetadata() = delete;

		/** @brief Copy constructor */
		GmodNodeMetadata( const GmodNodeMetadata& ) = default;

		/** @brief Move constructor */
		GmodNodeMetadata( GmodNodeMetadata&& ) noexcept = default;

		/** @brief Destructor */
		~GmodNodeMetadata() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		GmodNodeMetadata& operator=( const GmodNodeMetadata& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		GmodNodeMetadata& operator=( GmodNodeMetadata&& ) noexcept = default;

		/**
		 * @brief Equality comparison operator.
		 * @param other The metadata object to compare with.
		 * @return True if all properties of this metadata object are equal to the other, false otherwise.
		 */
		inline bool operator==( const GmodNodeMetadata& other ) const;

		/**
		 * @brief Inequality comparison operator.
		 * @param other The metadata object to compare with.
		 * @return True if any property of this metadata object differs from the other, false otherwise.
		 */
		inline bool operator!=( const GmodNodeMetadata& other ) const;

		/**
		 * @brief Get the node category
		 * @return Category string view (e.g., "ASSET FUNCTION", "PRODUCT")
		 */
		[[nodiscard]] inline std::string_view category() const noexcept;

		/**
		 * @brief Get the node type
		 * @return Type string view (e.g., "COMPOSITION", "TYPE", "SELECTION")
		 */
		[[nodiscard]] inline std::string_view type() const noexcept;

		/**
		 * @brief Get the full type (computed property)
		 * @return Full type as "Category Type" (e.g., "ASSET FUNCTION COMPOSITION")
		 */
		[[nodiscard]] inline std::string_view fullType() const noexcept;

		/**
		 * @brief Get the node name
		 * @return Name string view
		 */
		[[nodiscard]] inline std::string_view name() const noexcept;

		/**
		 * @brief Get the optional common name
		 * @return Optional common/friendly name
		 */
		[[nodiscard]] inline const std::optional<std::string>& commonName() const noexcept;

		/**
		 * @brief Get the optional technical definition
		 * @return Optional technical definition
		 */
		[[nodiscard]] inline const std::optional<std::string>& definition() const noexcept;

		/**
		 * @brief Get the optional common definition
		 * @return Optional user-friendly definition
		 */
		[[nodiscard]] inline const std::optional<std::string>& commonDefinition() const noexcept;

		/**
		 * @brief Get the optional installation substructure flag
		 * @return Optional bool indicating if node has installation substructure
		 */
		[[nodiscard]] inline const std::optional<bool>& installSubstructure() const noexcept;

		/**
		 * @brief Get the normal assignment names dictionary
		 * @return Read-only reference to normal assignment names map
		 */
		[[nodiscard]] inline const std::unordered_map<std::string, std::string, StringHash, std::equal_to<>>& normalAssignmentNames() const noexcept;

	private:
		std::string m_category;																			   ///< Node category (e.g., "ASSET FUNCTION")
		std::string m_type;																				   ///< Node type (e.g., "COMPOSITION")
		std::string m_name;																				   ///< Node name
		std::optional<std::string> m_commonName;														   ///< Optional common/friendly name
		std::optional<std::string> m_definition;														   ///< Optional technical definition
		std::optional<std::string> m_commonDefinition;													   ///< Optional user-friendly definition
		std::optional<bool> m_installSubstructure;														   ///< Optional installation substructure flag
		std::unordered_map<std::string, std::string, StringHash, std::equal_to<>> m_normalAssignmentNames; ///< Normal assignment names dictionary (transparent hash)
		std::string m_fullType;																			   ///< Computed: "Category Type"
	};

	/**
	 * @class GmodNode
	 * @brief Represents a node in the Gmod (Generic Product Model) tree hierarchy
	 * @details A GmodNode is a tree structure element containing:
	 *          - Code: Unique identifier (e.g., "VE" for root, "411.1" for specific nodes)
	 *          - Location: Optional VIS location
	 *          - Metadata: Descriptive information (category, type, names, definitions)
	 *          - Children/Parents: Navigation pointers (managed by Gmod class)
	 */
	class GmodNode final
	{
	private:
		/**
		 * @brief Internal constructor - creates GmodNode from DTO
		 * @param version The VIS version this node belongs to
		 * @param dto DTO containing node data
		 */
		explicit GmodNode( VisVersion version, const GmodNodeDto& dto ) noexcept;

	public:
		/** @brief Default constructor */
		GmodNode() = delete;

		/** @brief Copy constructor */
		GmodNode( const GmodNode& ) = default;

		/** @brief Move constructor */
		GmodNode( GmodNode&& ) noexcept = default;

		/** @brief Destructor */
		~GmodNode() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		GmodNode& operator=( const GmodNode& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		GmodNode& operator=( GmodNode&& ) noexcept = default;

		/**
		 * @brief Equality comparison operator
		 * @param other Path to compare with
		 * @return True if paths have identical parents and node
		 */
		inline bool operator==( const GmodNode& other ) const noexcept;

		/**
		 * @brief Inequality comparison operator
		 * @param other Path to compare with
		 * @return True if paths differ in parents or node
		 */
		inline bool operator!=( const GmodNode& other ) const noexcept;

		/**
		 * @brief Get the VIS version for this node
		 * @return The VisVersion enum value
		 */
		inline VisVersion version() const noexcept;

		/**
		 * @brief Get the node code
		 * @return Node code as string view (e.g., "VE", "411.1")
		 */
		inline std::string_view code() const noexcept;

		/**
		 * @brief Get the optional location
		 * @return Optional Location reference
		 */
		inline const std::optional<Location>& location() const noexcept;

		/**
		 * @brief Get the node metadata
		 * @return Reference to GmodNodeMetadata
		 */
		inline const GmodNodeMetadata& metadata() const noexcept;

		/**
		 * @brief Get child nodes
		 * @return Vector of child node pointers
		 */
		inline const std::vector<GmodNode*>& children() const noexcept;

		/**
		 * @brief Get parent nodes
		 * @return Vector of parent node pointers
		 */
		inline const std::vector<GmodNode*>& parents() const noexcept;

		/**
		 * @brief Get product type child if this is a function node
		 * @return Optional pointer to product type child node, or nullopt if not applicable
		 * @note Returns a non-owning pointer to the child node managed by Gmod
		 */
		std::optional<const GmodNode*> productType() const noexcept;

		/**
		 * @brief Get product selection child if this is a function node
		 * @return Optional pointer to product selection child node, or nullopt if not applicable
		 * @note Returns a non-owning pointer to the child node managed by Gmod
		 */
		std::optional<const GmodNode*> productSelection() const noexcept;

		/**
		 * @brief Check if this node can be individualized
		 * @param isTargetNode True if this is the target node in the path
		 * @param isInSet True if this node is in an individualizable set
		 * @return True if the node can be individualized
		 */
		bool isIndividualizable( bool isTargetNode = false, bool isInSet = false ) const noexcept;

		/**
		 * @brief Check if this is a function composition node
		 * @return True if category is ASSET/PRODUCT FUNCTION and type is COMPOSITION
		 */
		bool isFunctionComposition() const noexcept;

		/**
		 * @brief Check if this node is mappable
		 * @return True if the node can be used in mapping
		 */
		bool isMappable() const noexcept;

		/**
		 * @brief Check if this is a product selection node
		 * @return True if category is PRODUCT and type is SELECTION
		 */
		bool isProductSelection() const;

		/**
		 * @brief Check if this is a product type node
		 * @return True if category is PRODUCT and type is TYPE
		 */
		bool isProductType() const;

		/**
		 * @brief Check if this is an asset node
		 * @return True if category is ASSET
		 */
		bool isAsset() const;

		/**
		 * @brief Check if this is a leaf node
		 * @return True if this is an ASSET FUNCTION LEAF or PRODUCT FUNCTION LEAF
		 */
		bool isLeafNode() const;

		/**
		 * @brief Check if this is a function node
		 * @return True if category is not PRODUCT or ASSET
		 */
		bool isFunctionNode() const;

		/**
		 * @brief Check if this is an asset function node
		 * @return True if category is ASSET FUNCTION
		 */
		bool isAssetFunctionNode() const;

		/**
		 * @brief Check if this is the root node
		 * @return True if code is "VE"
		 */
		bool isRoot() const noexcept;

		/**
		 * @brief Check if a node is a child of this node
		 * @param node Node to check
		 * @return True if the node is a child
		 */
		bool isChild( const GmodNode& node ) const noexcept;

		/**
		 * @brief Check if a node code is a child of this node
		 * @param code Node code to check
		 * @return True if a child has this code
		 */
		bool isChild( std::string_view code ) const noexcept;

		/**
		 * @brief Get string representation of this node
		 * @return String in format "Code" or "Code-Location"
		 */
		std::string toString() const;

		/**
		 * @brief Append string representation to a StringBuilder
		 * @param builder StringBuilder to append to
		 */
		void toString( StringBuilder& builder ) const noexcept;

	private:
		/**
		 * @brief Create a copy of this node with a different location
		 * @param location The new location
		 * @return Copy of this node with the specified location
		 */
		GmodNode withLocation( const Location& location ) const;

		/**
		 * @brief Create a copy of this node with a location parsed from string
		 * @param locationStr Location string to parse
		 * @return Optional GmodNode with parsed location, nullopt if parsing fails
		 */
		std::optional<GmodNode> withLocation( std::string_view locationStr ) const;

		/**
		 * @brief Create a copy of this node with a location parsed from string with error details
		 * @param locationStr Location string to parse
		 * @param errors Output ParsingErrors containing detailed error information if parsing fails
		 * @return Optional GmodNode with parsed location, nullopt if parsing fails
		 */
		std::optional<GmodNode> withLocation( std::string_view locationStr, ParsingErrors& errors ) const;

		/**
		 * @brief Create a copy of this node without location
		 * @return Copy of this node with location removed
		 */
		GmodNode withoutLocation() const;

		/**
		 * @brief Add a child node to this node's children list
		 * @param child Pointer to the child node to add
		 * @note Only called by Gmod class during tree construction
		 */
		void addChild( GmodNode* child ) noexcept;

		/**
		 * @brief Add a parent node to this node's parents list
		 * @param parent Pointer to the parent node to add
		 * @note Only called by Gmod class during tree construction
		 */
		void addParent( GmodNode* parent ) noexcept;

		/**
		 * @brief Optimize memory usage and build child lookup set
		 * @details Shrinks vectors to fit and creates the m_childrenSet for fast lookups
		 * @note Only called by Gmod class after tree construction is complete
		 */
		void trim() noexcept;

	private:
		VisVersion m_visVersion;																	 ///< VIS version this node belongs to
		std::string m_code;																			 ///< Node code (e.g., "VE", "411.1")
		std::optional<Location> m_location;															 ///< Optional VIS location
		GmodNodeMetadata m_metadata;																 ///< Node metadata (category, type, names, etc.)
		std::vector<GmodNode*> m_children;															 ///< Child nodes (managed by Gmod)
		std::vector<GmodNode*> m_parents;															 ///< Parent nodes (managed by Gmod)
		std::shared_ptr<std::unordered_set<std::string, StringHash, std::equal_to<>>> m_childrenSet; ///< Fast child code lookup (transparent hash)
	};
} // namespace dnv::vista::sdk

#include "detail/GmodNode.inl"
