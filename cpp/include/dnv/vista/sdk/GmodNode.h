/**
 * @file GmodNode.h
 * @brief Generic Model (GMOD) node and metadata classes.
 * @details This file defines the GmodNode and GmodNodeMetadata classes which represent
 *          the fundamental building blocks of the Generic Product Model as defined in
 *          ISO 19848. These classes provide the node structure, relationships, and
 *          type classification used throughout the VISTA SDK.
 * @see ISO 19848 - Ships and marine technology - Standard data for shipboard machinery and equipment
 */

#pragma once

#include "GmodDto.h"
#include "Locations.h"

#include "utils/StringUtils.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	enum class VisVersion;
	class ParsingErrors;
	class Gmod;

	//=====================================================================
	// GmodNodeMetadata class
	//=====================================================================

	/**
	 * @brief Metadata for a GMOD (Generic Product Model) node.
	 * @details Contains descriptive information about a node including its category, type,
	 *          name, and other optional properties as defined in ISO 19848.
	 *          This class is designed to be immutable after construction.
	 */
	class GmodNodeMetadata final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs a new GmodNodeMetadata object.
		 * @param category The category classification of the node (e.g., "PRODUCT", "ASSET").
		 * @param type The type classification within the category (e.g., "SELECTION", "TYPE").
		 * @param name The human-readable name of the node.
		 * @param commonName Optional common name or alias for the node. Defaults to std::nullopt.
		 * @param definition Optional detailed definition of the node. Defaults to std::nullopt.
		 * @param commonDefinition Optional common definition for the node. Defaults to std::nullopt.
		 * @param installSubstructure Optional flag indicating if substructure should be installed. Defaults to std::nullopt.
		 * @param normalAssignmentNames Optional mapping of normal assignment names. Defaults to an empty map.
		 */
		inline GmodNodeMetadata(
			std::string_view category,
			std::string_view type,
			std::string_view name,
			const std::optional<std::string>& commonName = std::nullopt,
			const std::optional<std::string>& definition = std::nullopt,
			const std::optional<std::string>& commonDefinition = std::nullopt,
			const std::optional<bool>& installSubstructure = std::nullopt,
			const StringMap<std::string>& normalAssignmentNames = {} ) noexcept;

		/** @brief Default constructor. */
		GmodNodeMetadata() = default;

		/** @brief Copy constructor */
		inline GmodNodeMetadata( const GmodNodeMetadata& ) noexcept;

		/** @brief Move constructor */
		GmodNodeMetadata( GmodNodeMetadata&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~GmodNodeMetadata() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		inline GmodNodeMetadata& operator=( const GmodNodeMetadata& ) noexcept;

		/** @brief Move assignment operator */
		GmodNodeMetadata& operator=( GmodNodeMetadata&& ) noexcept = default;

		//----------------------------------------------
		// Operators
		//----------------------------------------------

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

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the category of the node
		 * @return Reference to the category string
		 */
		[[nodiscard]] inline std::string_view category() const noexcept;

		/**
		 * @brief Get the type of the node
		 * @return Reference to the type string
		 */
		[[nodiscard]] inline std::string_view type() const noexcept;

		/**
		 * @brief Get the full type string (category + type)
		 * @return Reference to the full type string
		 */
		[[nodiscard]] inline std::string_view fullType() const noexcept;

		/**
		 * @brief Get the name of the node
		 * @return Reference to the name string
		 */
		[[nodiscard]] inline std::string_view name() const noexcept;

		/**
		 * @brief Get the common name of the node
		 * @return Optional reference to the common name
		 */
		[[nodiscard]] inline const std::optional<std::string>& commonName() const noexcept;

		/**
		 * @brief Get the definition of the node
		 * @return Optional reference to the definition
		 */
		[[nodiscard]] inline const std::optional<std::string>& definition() const noexcept;

		/**
		 * @brief Get the common definition of the node
		 * @return Optional reference to the common definition
		 */
		[[nodiscard]] inline const std::optional<std::string>& commonDefinition() const noexcept;

		/**
		 * @brief Get the install substructure flag
		 * @return Optional reference to the install substructure flag
		 */
		[[nodiscard]] inline const std::optional<bool>& installSubstructure() const noexcept;

		/**
		 * @brief Get the normal assignment names mapping
		 * @return Reference to the map of normal assignment names
		 */
		[[nodiscard]] inline const StringMap<std::string>& normalAssignmentNames() const noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Category classification of the node. */
		std::string m_category;

		/** @brief Type classification within the category. */
		std::string m_type;

		/** @brief Human-readable name of the node. */
		std::string m_name;

		/** @brief Optional common name or alias for the node. */
		std::optional<std::string> m_commonName;

		/** @brief Optional detailed definition of the node. */
		std::optional<std::string> m_definition;

		/** @brief Optional common definition for the node. */
		std::optional<std::string> m_commonDefinition;

		/** @brief Optional flag indicating if substructure should be installed. */
		std::optional<bool> m_installSubstructure;

		/** @brief Optional mapping of normal assignment names. */
		StringMap<std::string> m_normalAssignmentNames;

		/** @brief Combined category and type string, e.g., "PRODUCT TYPE", generated at construction. */
		std::string m_fullType;
	};

	//=====================================================================
	// GmodNode class
	//=====================================================================

	/**
	 * @brief A node in the Generic Product Model (GMOD).
	 * @details Represents a single node in the hierarchical structure of the Generic Product Model
	 *          as defined by ISO 19848. Contains metadata, relationships to parent/child
	 *          nodes, and optional location information.
	 *          This class is designed to be non-copyable and movable. Its relationships
	 *          (children, parents) are managed by the `Gmod` class.
	 */
	class GmodNode final
	{
		friend class Gmod;

	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs a GmodNode from a Data Transfer Object (DTO).
		 * @param version The VIS version associated with this node.
		 * @param dto The DTO containing the node's data.
		 */
		inline GmodNode( VisVersion version, const GmodNodeDto& dto ) noexcept;

		/** @brief Default constructor. */
		GmodNode() = default;

		/** @brief Copy constructor */
		inline GmodNode( const GmodNode& ) noexcept;

		/** @brief Move constructor */
		GmodNode( GmodNode&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~GmodNode() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		inline GmodNode& operator=( const GmodNode& ) noexcept;

		/** @brief Move assignment operator */
		GmodNode& operator=( GmodNode&& ) noexcept = default;

		//----------------------------------------------
		// Operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison operator.
		 * @details Nodes are considered equal if they have the same code and location.
		 * @param other The node to compare with.
		 * @return True if this node is equal to the other node, false otherwise.
		 */
		inline bool operator==( const GmodNode& other ) const;

		/**
		 * @brief Inequality comparison operator.
		 * @param other The node to compare with.
		 * @return True if this node is not equal to the other node, false otherwise.
		 */
		inline bool operator!=( const GmodNode& other ) const;

		/**
		 * @brief Explicit equality comparison method.
		 * @details Nodes are considered equal if they have the same code and location.
		 *          This method provides an explicit way to check equality, complementing `operator==`.
		 * @param other The node to compare with.
		 * @return True if this node is equal to the other node, false otherwise.
		 */
		[[nodiscard]] inline bool equals( const GmodNode& other ) const;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the unique code identifier
		 * @return Reference to the code string
		 */
		[[nodiscard]] inline const std::string& code() const noexcept;

		/**
		 * @brief Get the location information
		 * @return Optional reference to the location
		 */
		[[nodiscard]] inline const std::optional<Location>& location() const noexcept;

		/**
		 * @brief Get the VIS version
		 * @return The VIS version enum value
		 */
		[[nodiscard]] inline VisVersion visVersion() const noexcept;

		/**
		 * @brief Get the node metadata
		 * @return Reference to the metadata
		 */
		[[nodiscard]] inline const GmodNodeMetadata& metadata() const noexcept;

		/**
		 * @brief Calculates a hash code for this GmodNode.
		 * @details The hash code is typically based on the node's code and location.
		 *          Nodes that are equal according to `operator==` must produce the same hash code.
		 * @return A `size_t` hash code value.
		 */
		[[nodiscard]] inline size_t hashCode() const noexcept;

		//----------------------------------------------
		// Relationship accessors
		//----------------------------------------------

		/**
		 * @brief Get the child nodes
		 * @return Reference to the vector of child node pointers
		 */
		[[nodiscard]] inline const std::vector<GmodNode*>& children() const noexcept;

		/**
		 * @brief Get the parent nodes
		 * @return Reference to the vector of parent node pointers
		 */
		[[nodiscard]] inline const std::vector<GmodNode*>& parents() const noexcept;

		/**
		 * @brief Gets the product type node associated with this function node, if applicable.
		 * @details For a function node, this typically refers to its single child node if that child is a product type.
		 * @return A const non-owning pointer to the product type node, or `nullptr` if not applicable or not found.
		 */
		[[nodiscard]] inline const GmodNode* productType() const noexcept;

		/**
		 * @brief Gets the product selection node associated with this function node, if applicable.
		 * @details For a function node, this typically refers to its single child node if that child is a product selection.
		 * @return A const non-owning pointer to the product selection node, or `nullptr` if not applicable or not found.
		 */
		[[nodiscard]] inline const GmodNode* productSelection() const noexcept;

		//----------------------------------------------
		// Node location methods
		//----------------------------------------------

		/**
		 * @brief Creates a new GmodNode instance identical to this one but without location information.
		 * @return A new `GmodNode` instance with `location` set to `std::nullopt`.
		 */
		[[nodiscard]] GmodNode withoutLocation() const;

		/**
		 * @brief Creates a new GmodNode instance identical to this one but with the specified Location object.
		 * @param location The Location object to set for the new node.
		 * @return A new `GmodNode` instance with the given location.
		 */
		[[nodiscard]] GmodNode withLocation( const Location& location ) const;

		/**
		 * @brief Creates a new GmodNode instance identical to this one but with the specified location string.
		 * @param locationStr The location string to parse and set.
		 * @return A new `GmodNode` instance with the parsed location.
		 * @throws std::invalid_argument If the `locationStr` is invalid or cannot be parsed.
		 */
		[[nodiscard]] GmodNode withLocation( std::string_view locationStr ) const;

		/**
		 * @brief Tries to create a new GmodNode instance with the specified location string, returning a copy on failure.
		 * @details This method attempts to parse the `locationStr`. If successful, a new node with the
		 *          parsed location is returned. If parsing fails, a new node identical to the original
		 *          (without the new location) is returned. No exceptions are thrown for parsing errors.
		 * @param locationStr The location string to parse and set.
		 * @return A new `GmodNode` instance, with the location set if parsing was successful.
		 */
		[[nodiscard]] GmodNode tryWithLocation( std::string_view locationStr ) const;

		/**
		 * @brief Tries to create a new GmodNode instance with the specified location string, capturing parsing errors.
		 * @details Attempts to parse `locationStr`. If successful, a new node with the parsed location is returned.
		 *          If parsing fails, errors are added to the `errors` object, and a new node identical
		 *          to the original is returned.
		 * @param locationStr The location string to parse and set.
		 * @param errors A `ParsingErrors` object to collect detailed error information if parsing fails.
		 * @return A new `GmodNode` instance, with the location set if parsing was successful.
		 */
		[[nodiscard]] GmodNode tryWithLocation( std::string_view locationStr, ParsingErrors& errors ) const;

		/**
		 * @brief Tries to create a new GmodNode instance with the specified optional Location object.
		 * @details If `location` has a value, a new node with that location is returned.
		 *          Otherwise, a new node identical to the original is returned.
		 * @param location An `std::optional<Location>` to set.
		 * @return A new `GmodNode` instance, with the location set if provided.
		 */
		[[nodiscard]] GmodNode tryWithLocation( const std::optional<Location>& location ) const;

		//----------------------------------------------
		// Node type checking methods
		//----------------------------------------------

		/**
		 * @brief Checks if the node is individualizable.
		 * @details A node is individualizable if it can be assigned a unique instance identifier
		 *          based on its GMOD classification and context.
		 * @param isTargetNode Flag indicating if this node is the target of an operation (e.g., in mapping). Defaults to false.
		 * @param isInSet Flag indicating if this node is part of a set. Defaults to false.
		 * @return True if the node is individualizable under the given conditions, false otherwise.
		 */
		[[nodiscard]] inline bool isIndividualizable( bool isTargetNode = false, bool isInSet = false ) const noexcept;

		/**
		 * @brief Checks if the node represents a function composition.
		 * @details A function composition node typically groups other function or product nodes.
		 * @return True if the node's metadata indicates it's an asset or product function composition, false otherwise.
		 */
		[[nodiscard]] inline bool isFunctionComposition() const noexcept;

		/**
		 * @brief Checks if the node is mappable.
		 * @details A mappable node is one that can be related to other nodes in a specific context,
		 *          often used in creating Local IDs or other semantic links.
		 * @return True if the node is considered mappable based on its type and properties, false otherwise.
		 */
		[[nodiscard]] inline bool isMappable() const noexcept;

		/**
		 * @brief Checks if the node is a product selection.
		 * @return True if the node's metadata identifies it as a product selection, false otherwise.
		 */
		[[nodiscard]] bool isProductSelection() const;

		/**
		 * @brief Checks if the node is a product type.
		 * @return True if the node's metadata identifies it as a product type, false otherwise.
		 */
		[[nodiscard]] bool isProductType() const;

		/**
		 * @brief Checks if the node is an asset.
		 * @return True if the node's metadata identifies it as an asset, false otherwise.
		 */
		[[nodiscard]] bool isAsset() const;

		/**
		 * @brief Checks if the node is a leaf node in the GMOD hierarchy (has no children).
		 * @return True if the node has no children, false otherwise.
		 */
		[[nodiscard]] bool isLeafNode() const;

		/**
		 * @brief Checks if the node is a function node (asset function or product function).
		 * @return True if the node's metadata identifies it as any kind of function node, false otherwise.
		 */
		[[nodiscard]] bool isFunctionNode() const;

		/**
		 * @brief Checks if the node is an asset function node.
		 * @return True if the node's metadata identifies it as an asset function, false otherwise.
		 */
		[[nodiscard]] bool isAssetFunctionNode() const;

		/**
		 * @brief Checks if the node is the root node of the GMOD hierarchy.
		 * @details The root node is conventionally identified by the code "VE".
		 * @return True if the node's code is "VE", false otherwise.
		 */
		[[nodiscard]] inline bool isRoot() const noexcept;

		//----------------------------------------------
		// Node relationship query methods
		//----------------------------------------------

		/**
		 * @brief Checks if this node has a specific GmodNode instance as a direct child.
		 * @param node The potential child node to check for.
		 * @return True if the provided `node` is a direct child of this node (checked by code), false otherwise.
		 */
		[[nodiscard]] inline bool isChild( const GmodNode& node ) const noexcept;

		/**
		 * @brief Checks if this node has a direct child with a specific code.
		 * @details This check is efficient due to an internal set of child codes (`m_childrenSet`).
		 * @param code The code of the potential child node.
		 * @return True if a direct child with the specified `code` exists, false otherwise.
		 */
		[[nodiscard]] inline bool isChild( std::string_view code ) const noexcept;

		//----------------------------------------------
		// String conversion methods
		//----------------------------------------------

		/**
		 * @brief Converts the node to its string representation.
		 * @details The format is typically "<code>" or "<code>-<location>" if location is present.
		 * @return A `std::string` representing the node.
		 */
		[[nodiscard]] inline std::string toString() const noexcept;

		/**
		 * @brief Appends the node's string representation to a generic output iterator.
		 * @tparam OutputIt The type of the output iterator.
		 * @param out The output iterator to write to.
		 * @return The iterator pointing to the end of the written output.
		 */
		template <typename OutputIt>
		inline OutputIt toString( OutputIt out ) const noexcept;

	private:
		//----------------------------------------------
		// Relationship management methods
		//----------------------------------------------

		/**
		 * @brief Adds a GmodNode instance as a child to this node.
		 * @details This method is intended for use by the `Gmod` class during graph construction.
		 *          It updates both the children list and the children set for quick lookups.
		 * @param child A non-owning pointer to the GmodNode to add as a child. Must not be null.
		 */
		inline void addChild( GmodNode* child ) noexcept;

		/**
		 * @brief Adds a GmodNode instance as a parent to this node.
		 * @details This method is intended for use by the `Gmod` class during graph construction.
		 * @param parent A non-owning pointer to the GmodNode to add as a parent. Must not be null.
		 */
		inline void addParent( GmodNode* parent ) noexcept;

		/**
		 * @brief Optimizes memory usage for child/parent vectors and rebuilds the child code set.
		 * @details This method is intended for use by the `Gmod` class after the graph structure is finalized.
		 *          It calls `shrink_to_fit()` on internal vectors and ensures `m_childrenSet` is consistent.
		 */
		inline void trim() noexcept;

		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Unique identifier code of the node. */
		std::string m_code;

		/** @brief Optional location information for this specific node instance. */
		std::optional<Location> m_location;

		/** @brief VIS (Vessel Information Structure) version associated with this node. */
		VisVersion m_visVersion;

		/** @brief Metadata object containing descriptive information about this node. */
		GmodNodeMetadata m_metadata;

		/** @brief Vector of non-owning pointers to direct child nodes. Managed by Gmod. */
		std::vector<GmodNode*> m_children;

		/** @brief Vector of non-owning pointers to direct parent nodes. Managed by Gmod. */
		std::vector<GmodNode*> m_parents;

		/** @brief Set of child codes for efficient `isChild(std::string_view)` lookups. Maintained by addChild/trim. */
		StringSet m_childrenSet;

		mutable std::optional<const GmodNode*> m_cachedProductType;
		mutable std::optional<const GmodNode*> m_cachedProductSelection;
	};
}

#include "GmodNode.inl"
