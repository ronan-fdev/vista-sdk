/**
 * @file GmodPath.h
 * @brief Declarations for GmodPath and related classes for representing paths in the Generic Product Model (GMOD).
 * @details Defines the `GmodPath` class for representing hierarchical paths according to ISO 19848,
 *          along with helper classes for parsing, validation, iteration, and modification of path segments.
 */

#pragma once

#include <limits>
#include <variant>
#include <vector>

#include <nfx/string/StringBuilderPool.h>

#include "config/config.h"
#include "GmodNode.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	class Gmod;
	class GmodIndividualizableSet;
	class Location;
	class Locations;

	struct GmodParsePathResult;

	enum class TraversalHandlerResult;
	enum class VisVersion;

	//=====================================================================
	// GmodPath class
	//=====================================================================

	class GmodPath final
	{
	public:
		class Enumerator;

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		GmodPath( const Gmod& gmod, GmodNode node, std::vector<GmodNode> parents = {}, bool skipVerify = false );

		/** @brief Default constructor. */
		inline GmodPath();

		/** @brief Copy constructor */
		inline GmodPath( const GmodPath& other );

		/** @brief Move constructor */
		inline GmodPath( GmodPath&& other ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~GmodPath() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		inline GmodPath& operator=( const GmodPath& other );

		/** @brief Move assignment operator */
		inline GmodPath& operator=( GmodPath&& other ) noexcept;

		//----------------------------------------------
		// Equality operators
		//----------------------------------------------

		/**
		 * @brief Compares two GmodPath instances for equality
		 * @param other The other GmodPath to compare with
		 * @return True if paths are equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator==( const GmodPath& other ) const noexcept;

		/**
		 * @brief Compares two GmodPath instances for inequality
		 * @param other The other GmodPath to compare with
		 * @return True if paths are not equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator!=( const GmodPath& other ) const noexcept;

		//----------------------------------------------
		// Lookup operators
		//----------------------------------------------

		/**
		 * @brief Accesses a node in the path by index (const version)
		 * @param index Zero-based index into the path (0 = first parent, length-1 = target node)
		 * @return Const reference to the GmodNode at the specified index
		 * @throws std::out_of_range If index is out of bounds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const GmodNode& operator[]( size_t index ) const;

		/**
		 * @brief Accesses a node in the path by index (non-const version)
		 * @param index Zero-based index into the path (0 = first parent, length-1 = target node)
		 * @return Reference to the GmodNode at the specified index
		 * @throws std::out_of_range If index is out of bounds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline GmodNode& operator[]( size_t index );

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the VIS version associated with these codebooks.
		 * @return The `VisVersion` enum value.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline VisVersion visVersion() const noexcept;

		/**
		 * @brief Gets the GMOD instance associated with this path
		 * @return Pointer to the GMOD instance, or nullptr if not set
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Gmod* gmod() const noexcept;

		/**
		 * @brief Gets the target node of this path
		 * @return Reference to the target GmodNode (the last node in the path)
		 * @throws std::runtime_error If the path has no target node
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const GmodNode& node() const;

		/**
		 * @brief Gets the parent nodes in this path
		 * @return Vector of parent GmodNodes leading to the target node
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::vector<GmodNode>& parents() const noexcept;

		/**
		 * @brief Gets the total length of the path (parents + target node)
		 * @return Number of nodes in the complete path
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline size_t length() const noexcept;

		/**
		 * @brief Gets all individualizable sets in this path
		 * @return Vector of individualizable sets that can be modified independently
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::vector<GmodIndividualizableSet> individualizableSets() const;

		/**
		 * @brief Gets the normal assignment name for a node at specified depth
		 * @param nodeDepth The depth of the node to get assignment name for (0-based index)
		 * @return Optional assignment name, nullopt if not found
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::optional<std::string> normalAssignmentName( size_t nodeDepth ) const;

		/**
		 * @brief Gets common names for all function nodes in the path
		 * @return Vector of pairs containing depth and common name
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::vector<std::pair<size_t, std::string>> commonNames() const;

		//----------------------------------------------
		// State inspection methods
		//----------------------------------------------

		/**
		 * @brief Validates the hierarchical relationship between parents and target node
		 * @param parents Vector of parent nodes in hierarchical order
		 * @param node The target node to validate
		 * @return True if the path is valid, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static bool isValid( const std::vector<GmodNode*>& parents, const GmodNode& node );

		/**
		 * @brief Validates path and identifies where the hierarchy breaks
		 * @param parents Vector of parent nodes in hierarchical order
		 * @param node The target node to validate
		 * @param missingLinkAt [out] Index where validation failed (-1 if valid)
		 * @return True if the path is valid, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool isValid( const std::vector<GmodNode*>& parents, const GmodNode& node, int& missingLinkAt );

		/**
		 * @brief Checks if this path can be mapped to physical locations
		 * @return True if the target node is mappable, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isMappable() const;

		/**
		 * @brief Checks if this path contains individualizable nodes
		 * @return True if any nodes can be individualized, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isIndividualizable() const;

		//----------------------------------------------
		// String conversion methods
		//----------------------------------------------

		/**
		 * @brief Converts the path to its short string representation
		 * @return Short path string showing only leaf nodes and target
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::string toString() const;

		/**
		 * @brief Writes the path's string representation to a StringBuilder.
		 * @param builder The StringBuilder to write to.
		 * @param separator Character to use between path segments (default: '/').
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		inline void toString( nfx::string::StringBuilder& builder, char separator = '/' ) const;

		/**
		 * @brief Converts the path to full hierarchical string representation
		 * @return Complete path string from root to target
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toFullPathString() const;

		/**
		 * @brief Writes the path's full hierarchical string representation to a StringBuilder.
		 * @param builder The StringBuilder to write to.
		 */
		inline void toFullPathString( nfx::string::StringBuilder& builder ) const;

		/**
		 * @brief Creates detailed debug representation of the path
		 * @return Detailed string with codes, names, and assignment names
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::string toStringDump() const;

		/**
		 * @brief Writes the path's detailed debug representation to a StringBuilder.
		 * @param builder The StringBuilder to write to.
		 */
		inline void toStringDump( nfx::string::StringBuilder& builder ) const;

		//----------------------------------------------
		// Path manipulation methods
		//----------------------------------------------

		/**
		 * @brief Creates a copy of this path with all location information removed
		 * @return New GmodPath instance without any location data
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] GmodPath withoutLocations() const;

		//----------------------------------------------
		// Path enumeration methods
		//----------------------------------------------

		/**
		 * @brief Gets the full hierarchical path with all intermediate nodes
		 * @return Enumerator for the complete path from root to target
		 */
		Enumerator inline fullPath() const;

		/**
		 * @brief Gets the full hierarchical path starting from a specific depth
		 * @param fromDepth The depth to start enumeration from (0 = root, -1 = from beginning)
		 * @return Enumerator starting from the specified depth
		 */
		Enumerator inline fullPathFrom( size_t fromDepth ) const;

		//----------------------------------------------
		// Parsing methods
		//----------------------------------------------

		/**
		 * @brief Parses a path string using specified VIS version
		 * @param item The path string to parse
		 * @param visVersion The VIS version to use for parsing
		 * @return Parsed GmodPath instance
		 * @throws std::invalid_argument if parsing fails
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE static GmodPath parse( std::string_view item, VisVersion visVersion );

		/**
		 * @brief Parses a path string using specified GMOD and locations
		 * @param item The path string to parse
		 * @param gmod The GMOD instance to use for node lookup
		 * @param locations The locations instance for location parsing
		 * @return Parsed GmodPath instance
		 * @throws std::invalid_argument if parsing fails
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE static GmodPath parse( std::string_view item, const Gmod& gmod, const Locations& locations );

		/**
		 * @brief Parses a full path string using specified VIS version
		 * @param item The full path string to parse (must start from root)
		 * @param visVersion The VIS version to use for parsing
		 * @return Parsed GmodPath instance
		 * @throws std::invalid_argument if parsing fails
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE static GmodPath parseFullPath( std::string_view item, VisVersion visVersion );

		/**
		 * @brief Attempts to parse a path string, returning success/failure
		 * @param item The path string to parse
		 * @param visVersion The VIS version to use for parsing
		 * @param outPath [out] The parsed path if successful
		 * @return True if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE static bool tryParse(
			std::string_view item, VisVersion visVersion,
			std::optional<GmodPath>& outPath );

		/**
		 * @brief Attempts to parse a path string using GMOD and locations
		 * @param item The path string to parse
		 * @param gmod The GMOD instance to use for node lookup
		 * @param locations The locations instance for location parsing
		 * @param outPath [out] The parsed path if successful
		 * @return True if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE static bool tryParse(
			std::string_view item, const Gmod& gmod, const Locations& locations, std::optional<GmodPath>& outPath );

		/**
		 * @brief Attempts to parse a full path string using VIS version
		 * @param item The full path string to parse (must start from root)
		 * @param visVersion The VIS version to use for parsing
		 * @param outPath [out] The parsed path if successful
		 * @return True if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE static bool tryParseFullPath(
			std::string_view item,
			VisVersion visVersion, std::optional<GmodPath>& outPath );

		/**
		 * @brief Attempts to parse a full path string using GMOD and locations
		 * @param item The full path string to parse (must start from root)
		 * @param gmod The GMOD instance to use for node lookup
		 * @param locations The locations instance for location parsing
		 * @param outPath [out] The parsed path if successful
		 * @return True if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE static bool tryParseFullPath(
			std::string_view item, const Gmod& gmod, const Locations& locations, std::optional<GmodPath>& outPath );

		//----------------------------------------------
		// Enumeration
		//----------------------------------------------

		/**
		 * @brief Creates an enumerator for traversing nodes in this path
		 * @param fromDepth The depth to start enumeration from (default: from target node backwards)
		 * @return Enumerator instance for iterating through path nodes
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Enumerator enumerator( size_t fromDepth = std::numeric_limits<size_t>::max() ) const;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		VisVersion m_visVersion;
		const Gmod* m_gmod;
		std::optional<GmodNode> m_node;
		std::vector<GmodNode> m_parents;

	private:
		//----------------------------------------------
		// Private static parsing methods
		//----------------------------------------------

		/**
		 * @brief Internal helper for parsing partial path strings with traversal
		 * @param item The path string to parse (allows partial paths)
		 * @param gmod The GMOD instance for node lookup and traversal
		 * @param locations The locations instance for location parsing
		 * @return Parse result with either success path or error message
		 * @details Uses GMOD traversal to find complete hierarchical path from partial input
		 */
		static GmodParsePathResult parseInternal( std::string_view item, const Gmod& gmod, const Locations& locations );

		/**
		 * @brief Internal helper for parsing full path strings
		 * @param item The path string to parse
		 * @param gmod The GMOD instance for node lookup
		 * @param locations The locations instance for location parsing
		 * @return Parse result with either success path or error message
		 */
		static GmodParsePathResult parseFullPathInternal( std::string_view item, const Gmod& gmod, const Locations& locations );

	public:
		//----------------------------------------------
		// GmodPath::enumerator
		//----------------------------------------------

		class Enumerator final
		{
			friend class GmodPath;

		public:
			//----------------------------
			// Types definitions
			//----------------------------

			/** @brief Represents a single path element entry. */
			using PathElement = std::pair<size_t, const GmodNode*>;

		private:
			//----------------------------
			// Construction
			//----------------------------

			/**
			 * @brief Internal constructor for creating Enumerator.
			 * @param pathInst Pointer to the GmodPath to enumerate.
			 * @param startIndex Starting index for enumeration.
			 */
			Enumerator( const GmodPath* pathInst, size_t startIndex = std::numeric_limits<size_t>::max() );

		public:
			/** @brief Default constructor */
			Enumerator() = delete;

			/** @brief Copy constructor */
			Enumerator( const Enumerator& ) = default;

			/** @brief Move constructor */
			Enumerator( Enumerator&& ) noexcept = default;

			//----------------------------
			// Destruction
			//----------------------------

			/** @brief Destructor */
			~Enumerator() = default;

			//----------------------------
			// Assignment operators
			//----------------------------

			/** @brief Copy assignment operator */
			Enumerator& operator=( const Enumerator& ) = default;

			/** @brief Move assignment operator */
			Enumerator& operator=( Enumerator&& ) noexcept = default;

			//----------------------------
			// Enumeration methods
			//----------------------------

			/**
			 * @brief Advances the enumerator to the next element.
			 * @return True if the enumerator successfully moved to the next element;
			 *         false if the enumerator has passed the end of the collection.
			 */
			inline bool next();

			/**
			 * @brief Gets the current element.
			 * @return The current path element (depth, node).
			 */
			inline const PathElement& current() const;

			/**
			 * @brief Resets the enumerator to its initial position before first element
			 */
			inline void reset();

		private:
			//----------------------------
			// Private member variables
			//----------------------------

			const GmodPath* m_pathInstance;
			size_t m_currentIndex;
			mutable PathElement m_current;
		};
	};

	//=====================================================================
	// GmodIndividualizableSet class
	//=====================================================================

	class GmodIndividualizableSet final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		GmodIndividualizableSet( const std::vector<int>& nodeIndices, const GmodPath& sourcePath );

		GmodIndividualizableSet() = delete;
		GmodIndividualizableSet( const GmodIndividualizableSet& ) = delete;
		GmodIndividualizableSet( GmodIndividualizableSet&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		~GmodIndividualizableSet() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		GmodIndividualizableSet& operator=( const GmodIndividualizableSet& ) = delete;
		GmodIndividualizableSet& operator=( GmodIndividualizableSet&& ) noexcept = default;

		//----------------------------------------------
		// Build
		//----------------------------------------------

		GmodPath build();

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Gets the nodes that can be individualized in this set
		 * @return Vector of pointers to GmodNodes that can be modified independently
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::vector<GmodNode*> nodes() const;

		/**
		 * @brief Gets the indices of nodes in this individualizable set
		 * @return Vector of indices identifying which nodes can be individualized
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::vector<int>& nodeIndices() const noexcept;

		/**
		 * @brief Gets the location associated with this individualizable set
		 * @return Optional location, or nullopt if no location is set
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::optional<Location> location() const;

		//----------------------------------------------
		// Utility methods
		//----------------------------------------------

		/**
		 * @brief Sets the location for this individualizable set
		 * @param location The location to associate with this set, or nullopt to clear
		 */
		void setLocation( const std::optional<Location>& location );

		/**
		 * @brief Converts this individualizable set to its string representation
		 * @return String representation showing the set's nodes and location
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		std::vector<int> m_nodeIndices;
		GmodPath m_path;
		bool m_isBuilt;
	};

	//=====================================================================
	// GmodParsePathResult
	//=====================================================================

	struct GmodParsePathResult
	{
		struct Ok
		{
			GmodPath path;
			inline explicit Ok( GmodPath p );
		};

		struct Error
		{
			std::string error;
			inline explicit Error( std::string e );
		};

		std::variant<Ok, Error> result;

		inline GmodParsePathResult( Ok ok );
		inline GmodParsePathResult( Error err );

		inline bool isOk() const noexcept;
		inline bool isError() const noexcept;

		inline Ok& ok();
		inline const Ok& ok() const;

		inline Error& error();
		inline const Error& error() const;
	};
}

#include "GmodPath.inl"
