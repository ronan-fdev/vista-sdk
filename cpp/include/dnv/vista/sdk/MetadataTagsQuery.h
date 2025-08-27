/**
 * @file MetadataTagsQuery.h
 * @brief Metadata tags query builder and matcher for filtering LocalId based on metadata tags
 * @details Provides fluent builder pattern for constructing queries that match LocalId objects
 *          based on their metadata tags. Supports exact matching and partial matching modes.
 */

#pragma once

#include "internal/StringMap.h"
#include "LocalId.h"
#include "MetadataTag.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	enum class CodebookName : std::uint8_t;

	//=====================================================================
	// MetadataTagsQuery class
	//=====================================================================

	/**
	 * @brief Query for matching LocalId objects based on metadata tags
	 * @details Immutable query object that can test if LocalId objects match
	 *          specific metadata tag criteria with configurable matching behavior
	 */
	class MetadataTagsQuery final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor deleted */
		MetadataTagsQuery() = delete;

		/** @brief Internal constructor from builder state */
		inline explicit MetadataTagsQuery(
			std::unordered_map<CodebookName, MetadataTag> tags,
			bool matchExact ) noexcept;

		/** @brief Copy constructor */
		MetadataTagsQuery( const MetadataTagsQuery& ) = default;

		/** @brief Move constructor */
		MetadataTagsQuery( MetadataTagsQuery&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~MetadataTagsQuery() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		MetadataTagsQuery& operator=( const MetadataTagsQuery& ) = default;

		/** @brief Move assignment operator */
		MetadataTagsQuery& operator=( MetadataTagsQuery&& ) noexcept = default;

		//----------------------------------------------
		// Factory methods
		//----------------------------------------------

		/**
		 * @brief Create an empty query that matches all LocalIds (when allowOtherTags=true)
		 * @return New empty query instance
		 */
		[[nodiscard]] static MetadataTagsQuery empty();

		/**
		 * @brief Create a query from an existing LocalId's metadata tags
		 * @param localId The LocalId to extract metadata tags from
		 * @param allowOtherTags If true, allows additional tags; if false, requires exact match
		 * @return New query instance with the LocalId's metadata tags
		 */
		[[nodiscard]] static MetadataTagsQuery from( const LocalId& localId, bool allowOtherTags = true );

		//----------------------------------------------
		// Fluent builder methods (Immutable)
		//----------------------------------------------

		/**
		 * @brief Add a metadata tag constraint by name and value
		 * @param name The codebook name for the tag
		 * @param value The required tag value
		 * @return New query instance with the tag constraint added
		 */
		[[nodiscard]] MetadataTagsQuery withTag( CodebookName name, const std::string& value ) const;

		/**
		 * @brief Add a metadata tag constraint
		 * @param tag The metadata tag to add to the query
		 * @return New query instance with the tag constraint added
		 */
		[[nodiscard]] MetadataTagsQuery withTag( const MetadataTag& tag ) const;

		/**
		 * @brief Add a metadata tag constraint (move version for performance)
		 * @param tag The metadata tag to add to the query (will be moved)
		 * @return New query instance with the tag constraint added
		 */
		[[nodiscard]] MetadataTagsQuery withTag( MetadataTag&& tag ) const;

		/**
		 * @brief Configure whether to allow additional tags beyond those specified
		 * @param allowOthers If true, allows additional tags; if false, requires exact match
		 * @return New query instance with the matching mode set
		 */
		[[nodiscard]] MetadataTagsQuery withAllowOtherTags( bool allowOthers ) const;

		//----------------------------------------------
		// Query execution
		//----------------------------------------------

		/**
		 * @brief Test if a LocalId matches this query's metadata tag criteria
		 * @param localId The LocalId to test against this query (nullable)
		 * @return True if the LocalId matches all query criteria
		 */
		[[nodiscard]] bool match( const LocalId* localId ) const;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the metadata tags in this query
		 * @return Const reference to the tags map
		 */
		[[nodiscard]] inline const std::unordered_map<CodebookName, MetadataTag>& tags() const noexcept;

		/**
		 * @brief Check if this query requires exact matching
		 * @return True if exact matching is required, false if additional tags are allowed
		 */
		[[nodiscard]] inline bool matchExact() const noexcept;

		/**
		 * @brief Check if this query is empty (no tag constraints)
		 * @return True if no tag constraints are specified
		 */
		[[nodiscard]] inline bool isEmpty() const noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		std::unordered_map<CodebookName, MetadataTag> m_tags;
		bool m_matchExact;
	};
}

#include "MetadataTagsQuery.inl"
