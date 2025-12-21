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
 * @file GmodPath.h
 * @brief GMOD path representation with traversal and individualization support
 * @details Represents a validated path through a GMOD tree, consisting of a sequence of parent nodes
 *          and a target node. Provides path validation, location-based individualization, and iteration
 *          over the full path hierarchy. Used for VIS Local ID construction and equipment mapping.
 */

#pragma once

#include "GmodNode.h"
#include "StringBuilder.h"

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace dnv::vista::sdk
{
	class Gmod;
	class GmodIndividualizableSet;
	class Location;
	class Locations;

	enum class VisVersion : std::uint8_t;

	/**
	 * @class GmodPath
	 * @brief Represents a validated path through a GMOD tree hierarchy
	 * @details A GmodPath consists of a sequence of parent nodes leading to a target node,
	 *          forming a validated path through the Generic Product Model. The path provides
	 *          functionality for location-based individualization, traversal, validation, and
	 *          string representation. Used extensively in VIS Local ID construction and equipment
	 *          mapping workflows.
	 */
	class GmodPath final
	{
		friend class GmodIndividualizableSet;

	private:
		/**
		 * @brief Internal constructor with optional validation skip
		 * @param parents Parent nodes in the path hierarchy
		 * @param node Target node at the end of the path
		 * @param skipVerify If true, skips validation checks (used internally for performance)
		 */
		GmodPath( std::vector<GmodNode>&& parents, GmodNode node, bool skipVerify );

		/**
		 * @brief Internal non-const indexer for modification by GmodIndividualizableSet
		 * @param index Depth in the path (0 = root, length-1 = node)
		 * @return Mutable reference to the node at the specified depth
		 */
		inline GmodNode& operator[]( size_t index );

	public:
		/**
		 * @brief Constructs a validated GMOD path
		 * @param parents Parent nodes in the path hierarchy (moved)
		 * @param node Target node at the end of the path
		 * @throws std::invalid_argument If path validation fails
		 */
		explicit GmodPath( std::vector<GmodNode>&& parents, GmodNode node );

		/** @brief Default constructor */
		GmodPath() = delete;

		/** @brief Copy constructor */
		GmodPath( const GmodPath& ) = default;

		/** @brief Move constructor */
		GmodPath( GmodPath&& ) noexcept = default;

		/** @brief Destructor */
		~GmodPath() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		GmodPath& operator=( const GmodPath& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		GmodPath& operator=( GmodPath&& ) noexcept = default;

		/**
		 * @brief Equality comparison operator
		 * @param other Path to compare with
		 * @return True if paths have identical parents and node
		 */
		inline bool operator==( const GmodPath& other ) const noexcept;

		/**
		 * @brief Inequality comparison operator
		 * @param other Path to compare with
		 * @return True if paths differ in parents or node
		 */
		inline bool operator!=( const GmodPath& other ) const noexcept;

		/**
		 * @brief Access node at specified depth (const version)
		 * @param index Depth in the path (0 = root, length-1 = node)
		 * @return Const reference to the node at the specified depth
		 */
		inline const GmodNode& operator[]( size_t index ) const;

		/**
		 * @brief Get the VIS version for this path
		 * @return The VisVersion enum value
		 */
		inline VisVersion version() const noexcept;

		/**
		 * @brief Get the target node at the end of the path
		 * @return Const reference to the target node
		 */
		inline const GmodNode& node() const;

		/**
		 * @brief Get the parent nodes in the path hierarchy
		 * @return Const reference to vector of parent nodes
		 */
		inline const std::vector<GmodNode>& parents() const noexcept;

		/**
		 * @brief Get the total length of the path (parents + node)
		 * @return Number of nodes in the path (parents.size() + 1)
		 */
		inline size_t length() const noexcept;

		/**
		 * @brief Get all individualizable sets in this path
		 * @return Vector of sets that can have locations assigned
		 * @details Identifies contiguous sequences of nodes that can be individualized
		 *          with location information for equipment mapping
		 */
		std::vector<GmodIndividualizableSet> individualizableSets() const;

		/**
		 * @brief Get the normal assignment name for a node at specified depth
		 * @param nodeDepth Depth of the node to query (0-based)
		 * @return Normal assignment name if found, nullopt otherwise
		 * @details Searches the node's normal assignment names map using child codes
		 */
		std::optional<std::string> normalAssignmentName( size_t nodeDepth ) const;

		/**
		 * @brief Get common names for all function nodes in the path
		 * @return Vector of (depth, name) pairs for each function node
		 * @details Returns common names or normal assignment names for leaf/target function nodes
		 */
		std::vector<std::pair<size_t, std::string>> commonNames() const;

		/**
		 * @brief Check if a sequence of parents and node form a valid path
		 * @param parents Parent nodes to validate
		 * @param node Target node to validate
		 * @return True if the sequence forms a valid GMOD path
		 * @details Validates parent-child relationships and root node presence
		 */
		static bool isValid( const std::vector<GmodNode>& parents, const GmodNode& node );

		/**
		 * @brief Check if the target node is mappable
		 * @return True if the target node can be mapped to equipment
		 */
		inline bool isMappable() const;

		/**
		 * @brief Check if the path contains any individualizable nodes
		 * @return True if at least one node can have a location assigned
		 */
		bool isIndividualizable() const;

		/**
		 * @brief Create a copy of this path with all locations removed
		 * @return New GmodPath with all nodes having nullopt locations
		 */
		GmodPath withoutLocations() const;

		/**
		 * @brief Convert path to string representation (short form)
		 * @return String with only leaf nodes separated by '/'
		 * @details Only includes nodes where isLeafNode is true
		 */
		inline std::string toString() const;

		/**
		 * @brief Write path to string builder (short form)
		 * @param builder StringBuilder to append to
		 * @param separator Character to use between nodes (default '/')
		 */
		inline void toString( StringBuilder& builder, char separator = '/' ) const;

		/**
		 * @brief Convert path to full string representation
		 * @return String with all nodes separated by '/'
		 * @details Includes all nodes in the path hierarchy
		 */
		std::string toFullPathString() const;

		/**
		 * @brief Write full path to string builder
		 * @param builder StringBuilder to append to
		 */
		inline void toFullPathString( StringBuilder& builder ) const;

		/**
		 * @brief Convert path to diagnostic dump string
		 * @return String with node codes, names, common names, and normal assignments
		 * @details Used for debugging and diagnostics
		 */
		inline std::string toStringDump() const;

		/**
		 * @brief Write diagnostic dump to string builder
		 * @param builder StringBuilder to append to
		 */
		inline void toStringDump( StringBuilder& builder ) const;

		/**
		 * @brief Iterator range for full path traversal
		 * @details Allows range-based for loops: for (auto [depth, node] : path.fullPath())
		 */
		class FullPathRange final
		{
		public:
			/**
			 * @struct Iterator
			 * @brief Forward iterator for traversing GMOD path nodes
			 * @details Standard forward iterator that yields (depth, node) pairs during path traversal.
			 *          Satisfies ForwardIterator concept with zero-overhead abstraction.
			 */
			struct Iterator
			{
				using iterator_category = std::forward_iterator_tag;   ///< Iterator category tag
				using value_type = std::pair<size_t, const GmodNode&>; ///< Type of value returned by dereference
				using difference_type = std::ptrdiff_t;				   ///< Type for iterator differences
				using pointer = const value_type*;					   ///< Pointer to value type
				using reference = value_type;						   ///< Reference to value type

				/**
				 * @brief Constructs an iterator at specified depth
				 * @param path Pointer to the GmodPath to iterate over
				 * @param depth Starting depth for this iterator
				 */
				explicit Iterator( const GmodPath* path, size_t depth )
					: m_path{ path },
					  m_depth{ depth }
				{
				}

				/**
				 * @brief Dereference operator
				 * @return Pair of (depth, node reference) for current position
				 */
				reference operator*() const
				{
					return { m_depth, ( *m_path )[m_depth] };
				}

				/**
				 * @brief Pre-increment operator
				 * @return Reference to this iterator after incrementing depth
				 */
				Iterator& operator++()
				{
					++m_depth;
					return *this;
				}

				/**
				 * @brief Post-increment operator
				 * @return Copy of iterator before incrementing
				 */
				Iterator operator++( int )
				{
					Iterator tmp = *this;
					++m_depth;
					return tmp;
				}

				/**
				 * @brief Equality comparison operator
				 * @param other Iterator to compare with
				 * @return True if both iterators point to the same depth
				 */
				bool operator==( const Iterator& other ) const { return m_depth == other.m_depth; }

				/**
				 * @brief Inequality comparison operator
				 * @param other Iterator to compare with
				 * @return True if iterators point to different depths
				 */
				bool operator!=( const Iterator& other ) const
				{
					return m_depth != other.m_depth;
				}

			private:
				const GmodPath* m_path; ///< Pointer to the GmodPath being iterated
				size_t m_depth;			///< Current depth/index in the iteration
			};

			/**
			 * @brief Constructs a path range for iteration
			 * @param path Pointer to the GmodPath to iterate over
			 * @param start Starting depth for iteration (default 0)
			 */
			explicit FullPathRange( const GmodPath* path, size_t start = 0 )
				: m_path{ path },
				  m_start{ start }
			{
			}

			/**
			 * @brief Get iterator to the beginning of the range
			 * @return Iterator pointing to the start depth
			 */
			Iterator begin() const
			{
				return Iterator( m_path, m_start );
			}

			/**
			 * @brief Get iterator to the end of the range
			 * @return Iterator pointing past the last node
			 */
			Iterator end() const
			{
				return Iterator( m_path, m_path->length() );
			}

		private:
			const GmodPath* m_path; ///< Pointer to the path being iterated
			size_t m_start;			///< Starting depth for iteration
		};

		/**
		 * @brief Get iterator range for full path traversal
		 * @return Range object supporting range-based for loops
		 * @code
		 * for (auto [depth, node] : path.fullPath()) {
		 *     // Process each node with its depth
		 * }
		 * @endcode
		 */
		inline FullPathRange fullPath() const;

		/**
		 * @brief Get iterator range for full path traversal starting from specific depth
		 * @param fromDepth Starting depth (0-based)
		 * @return Range object supporting range-based for loops
		 */
		inline FullPathRange fullPathFrom( size_t fromDepth ) const;

		/**
		 * @brief Parse a short GMOD path string into a GmodPath object
		 * @param item Short path string (e.g., "411.1/513-14C")
		 * @param visVersion VIS version to use for GMOD and Locations lookup
		 * @return Optional containing GmodPath if parsing succeeded, nullopt otherwise
		 * @details Uses VIS instance to get GMOD and Locations, then delegates to the main fromString method.
		 *          Short path parsing uses tree traversal to find complete paths from root.
		 */
		[[nodiscard]] static std::optional<GmodPath> fromString(
			std::string_view item,
			VisVersion visVersion ) noexcept;

		/**
		 * @brief Parse a short GMOD path string into a GmodPath object
		 * @param item Short path string (e.g., "411.1/513-14C")
		 * @param gmod GMOD instance to resolve nodes from
		 * @param locations Locations instance to parse location suffixes
		 * @return Optional containing GmodPath if parsing succeeded, nullopt otherwise
		 * @details Uses tree traversal with gmod.traverse() to find complete paths.
		 *          More complex than fromFullPathString as it must search the tree.
		 *          This method is not yet implemented - returns nullopt.
		 */
		[[nodiscard]] static std::optional<GmodPath> fromString(
			std::string_view item,
			const Gmod& gmod,
			const Locations& locations ) noexcept;

		/**
		 * @brief Parse a short GMOD path string with error reporting
		 * @param item Short path string (e.g., "411.1/513-14C")
		 * @param gmod GMOD instance to resolve nodes from
		 * @param locations Locations instance to parse location suffixes
		 * @param errors Output parameter for detailed parsing errors
		 * @return Optional containing GmodPath if parsing succeeded, nullopt otherwise
		 * @details Uses tree traversal to find complete paths from root.
		 *          Provides detailed error messages when parsing fails.
		 */
		[[nodiscard]] static std::optional<GmodPath> fromString(
			std::string_view item,
			const Gmod& gmod,
			const Locations& locations,
			ParsingErrors& errors ) noexcept;

		/**
		 * @brief Parse a full GMOD path string (starting with root) into a GmodPath object
		 * @param fullPathStr Full path string starting with root (e.g., "VE/400a/410/411/411i/411.1")
		 * @param gmod GMOD instance to resolve nodes from
		 * @param locations Locations instance to parse location suffixes
		 * @return Optional containing GmodPath if parsing succeeded, nullopt otherwise
		 */
		[[nodiscard]] static std::optional<GmodPath> fromFullPathString(
			std::string_view fullPathStr,
			const Gmod& gmod,
			const Locations& locations ) noexcept;

		/**
		 * @brief Parse a full GMOD path string with error reporting
		 * @param fullPathStr Full path string starting with root
		 * @param gmod GMOD instance to resolve nodes from
		 * @param locations Locations instance to parse location suffixes
		 * @param errors Output parameter for detailed parsing errors
		 * @return Optional containing GmodPath if parsing succeeded, nullopt otherwise
		 */
		[[nodiscard]] static std::optional<GmodPath> fromFullPathString(
			std::string_view fullPathStr,
			const Gmod& gmod,
			const Locations& locations,
			ParsingErrors& errors ) noexcept;

	private:
		VisVersion m_visVersion;		 ///< VIS version for this path
		std::vector<GmodNode> m_parents; ///< Parent nodes in path hierarchy (root to target-1)
		GmodNode m_node;				 ///< Target node at end of path
	};

	/**
	 * @brief Represents a set of nodes in a path that can be individualized with location information
	 * @details An individualizable set is a contiguous sequence of nodes in a GMOD path that can
	 *          have locations assigned for equipment mapping. Used to modify locations on path nodes
	 *          and then build the modified path.
	 */
	class GmodIndividualizableSet final
	{
	public:
		/**
		 * @brief Constructs an individualizable set from node indices
		 * @param nodeIndices Indices of nodes in the path that form this set
		 * @param sourcePath Source path containing the nodes (will be copied)
		 * @throws std::runtime_error If set validation fails
		 */
		explicit GmodIndividualizableSet( const std::vector<int>& nodeIndices, const GmodPath& sourcePath );

		/** @brief Default constructor */
		GmodIndividualizableSet() = delete;

		/** @brief Copy constructor */
		GmodIndividualizableSet( const GmodIndividualizableSet& ) = default;

		/** @brief Move constructor */
		GmodIndividualizableSet( GmodIndividualizableSet&& ) noexcept = default;

		/** @brief Destructor */
		~GmodIndividualizableSet() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		GmodIndividualizableSet& operator=( const GmodIndividualizableSet& ) = delete;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		GmodIndividualizableSet& operator=( GmodIndividualizableSet&& ) noexcept = delete;

		/**
		 * @brief Build and return the modified path
		 * @return GmodPath with modified locations
		 * @throws std::runtime_error If already built (can only build once)
		 * @details After building, the set becomes invalid and cannot be used again
		 */
		GmodPath build();

		/**
		 * @brief Get the nodes in this individualizable set
		 * @return Vector of nodes (copies)
		 * @throws std::runtime_error If already built
		 */
		std::vector<GmodNode> nodes() const;

		/**
		 * @brief Get the indices of nodes in this set
		 * @return Const reference to vector of node indices
		 */
		inline const std::vector<int>& nodeIndices() const noexcept;

		/**
		 * @brief Get the current location assigned to this set
		 * @return Location if set, nullopt otherwise
		 * @throws std::runtime_error If already built
		 */
		inline std::optional<Location> location() const;

		/**
		 * @brief Convert set to string representation
		 * @return String with nodes separated by '/'
		 * @throws std::runtime_error If already built
		 */
		[[nodiscard]] std::string toString() const;

	private:
		std::vector<int> m_nodeIndices; ///< Indices of individualizable nodes in the path
		std::optional<GmodPath> m_path; ///< Copy of path with modifiable nodes (nullopt after build())
	};
} // namespace dnv::vista::sdk

#include "detail/GmodPath.inl"
