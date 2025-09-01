/**
 * @file LocalIdQuery.h
 * @brief LocalId query builder and matcher for filtering LocalId objects
 * @details Provides fluent builder pattern for constructing queries that match LocalId objects
 *          based on primary item, secondary item, and metadata tag constraints.
 */

#pragma once

#include <string>

#include "GmodPathQuery.h"
#include "LocalId.h"
#include "MetadataTagsQuery.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	class LocalIdQueryBuilder;

	//=====================================================================
	// Configuration tag types for overload resolution
	//=====================================================================

	/**
	 * @brief Tag enum for distinguishing query configuration approaches
	 * @details Used to distinguish overloads when configuring queries
	 */
	enum class ConfigureFrom : bool
	{
		Path = 0,
		Nodes
	};

	//=====================================================================
	// Type aliases for configuration delegates
	//=====================================================================

	/**
	 * @brief Function type for configuring queries
	 * @details Takes a GmodPathQuery and returns a modified version
	 */
	using QueryConfiguration = std::function<GmodPathQuery( const GmodPathQuery& )>;

	/**
	 * @brief Function type for configuring metadata tags
	 * @details Takes a MetadataTagsQuery and returns a modified version
	 */
	using TagsConfiguration = std::function<MetadataTagsQuery( const MetadataTagsQuery& )>;

	//=====================================================================
	// LocalIdQuery class
	//=====================================================================

	/**
	 * @brief Query for matching LocalId objects based on various criteria
	 * @details Immutable query object that can test if LocalId objects match
	 *          specific primary item, secondary item, and metadata tag criteria
	 */
	class LocalIdQuery final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor deleted */
		LocalIdQuery() = delete;

		/** @brief Internal constructor from builder */
		explicit LocalIdQuery( const LocalIdQueryBuilder& builder );

		/** @brief Copy constructor */
		LocalIdQuery( const LocalIdQuery& ) = default;

		/** @brief Move constructor */
		LocalIdQuery( LocalIdQuery&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~LocalIdQuery() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		LocalIdQuery& operator=( const LocalIdQuery& ) = default;

		/** @brief Move assignment operator */
		LocalIdQuery& operator=( LocalIdQuery&& ) noexcept = default;

		//----------------------------------------------
		// Query execution
		//----------------------------------------------

		/**
		 * @brief Test if a LocalId matches this query's criteria
		 * @param localId The LocalId to test against this query
		 * @return True if the LocalId matches all query criteria
		 */
		[[nodiscard]] bool match( const LocalId& localId ) const;

		/**
		 * @brief Test if a LocalId string matches this query's criteria
		 * @param localIdStr The LocalId string to parse and test against this query
		 * @return True if the parsed LocalId matches all query criteria
		 */
		[[nodiscard]] bool match( const std::string& localIdStr ) const;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		std::optional<GmodPathQuery> m_primaryItem;
		std::optional<GmodPathQuery> m_secondaryItem;
		std::optional<MetadataTagsQuery> m_tags;
	};

	//=====================================================================
	// LocalIdQueryBuilder class
	//=====================================================================

	/**
	 * @brief Builder for constructing LocalIdQuery objects with fluent interface
	 * @details Provides methods to configure primary item, secondary item, and metadata
	 *          tag constraints before building the final immutable query object
	 */
	class LocalIdQueryBuilder final
	{
	private:
		friend class LocalIdQuery;

	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		LocalIdQueryBuilder() = default;

		/** @brief Copy constructor */
		LocalIdQueryBuilder( const LocalIdQueryBuilder& ) = default;

		/** @brief Move constructor */
		LocalIdQueryBuilder( LocalIdQueryBuilder&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~LocalIdQueryBuilder() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		LocalIdQueryBuilder& operator=( const LocalIdQueryBuilder& ) = default;

		/** @brief Move assignment operator */
		LocalIdQueryBuilder& operator=( LocalIdQueryBuilder&& ) noexcept = default;

		//----------------------------------------------
		// Factory methods
		//----------------------------------------------

		/**
		 * @brief Create an empty query builder
		 * @return New empty query builder instance
		 */
		[[nodiscard]] static LocalIdQueryBuilder empty();

		/**
		 * @brief Create a query builder from an existing LocalId string
		 * @param localIdStr The LocalId string to extract configuration from
		 * @return New query builder instance configured from the LocalId
		 */
		[[nodiscard]] static LocalIdQueryBuilder from( const std::string& localIdStr );

		/**
		 * @brief Create a query builder from an existing LocalId
		 * @param localId The LocalId to extract configuration from
		 * @return New query builder instance configured from the LocalId
		 */
		[[nodiscard]] static LocalIdQueryBuilder from( const LocalId& localId );

		//----------------------------------------------
		// Build method
		//----------------------------------------------

		/**
		 * @brief Build the final immutable LocalIdQuery
		 * @return New LocalIdQuery instance with current configuration
		 */
		[[nodiscard]] LocalIdQuery build() const;

		//----------------------------------------------
		// Primary item configuration
		//----------------------------------------------

		/**
		 * @brief Configure primary item using node-based query from scratch
		 * @param from From::Nodes to indicate node-based configuration
		 * @param configure Function to configure the node-based query
		 * @return New builder instance with primary item configured
		 */
		[[nodiscard]] LocalIdQueryBuilder withPrimaryItem( ConfigureFrom from, QueryConfiguration configure ) const;

		/**
		 * @brief Configure primary item from existing path with additional configuration
		 * @param from From::Path to indicate path-based configuration
		 * @param primaryItem The base GmodPath for the primary item
		 * @param configure Function to further configure the path query
		 * @return New builder instance with primary item configured
		 */
		[[nodiscard]] LocalIdQueryBuilder withPrimaryItem( ConfigureFrom from, const GmodPath& primaryItem, QueryConfiguration configure ) const;

		/**
		 * @brief Set primary item from GmodPath directly
		 * @param primaryItem The GmodPath for the primary item
		 * @return New builder instance with primary item set
		 */
		[[nodiscard]] LocalIdQueryBuilder withPrimaryItem( const GmodPath& primaryItem ) const;

		/**
		 * @brief Set primary item from GmodPathQuery directly
		 * @param primaryItem The GmodPathQuery for the primary item
		 * @return New builder instance with primary item set
		 */
		[[nodiscard]] LocalIdQueryBuilder withPrimaryItem( const GmodPathQuery& primaryItem ) const;

		//----------------------------------------------
		// Secondary item configuration
		//----------------------------------------------

		/**
		 * @brief Configure secondary item using node-based query from scratch
		 * @param from From::Nodes to indicate node-based configuration
		 * @param configure Function to configure the node-based query
		 * @return New builder instance with secondary item configured
		 */
		[[nodiscard]] LocalIdQueryBuilder withSecondaryItem( ConfigureFrom from, QueryConfiguration configure ) const;

		/**
		 * @brief Configure secondary item from existing path with additional configuration
		 * @param from From::Path to indicate path-based configuration
		 * @param secondaryItem The base GmodPath for the secondary item
		 * @param configure Function to further configure the path query
		 * @return New builder instance with secondary item configured
		 */
		[[nodiscard]] LocalIdQueryBuilder withSecondaryItem( ConfigureFrom from, const GmodPath& secondaryItem, QueryConfiguration configure ) const;

		/**
		 * @brief Set secondary item from GmodPath directly
		 * @param secondaryItem The GmodPath for the secondary item
		 * @return New builder instance with secondary item set
		 */
		[[nodiscard]] LocalIdQueryBuilder withSecondaryItem( const GmodPath& secondaryItem ) const;

		/**
		 * @brief Set secondary item from GmodPathQuery directly
		 * @param secondaryItem The GmodPathQuery for the secondary item
		 * @return New builder instance with secondary item set
		 */
		[[nodiscard]] LocalIdQueryBuilder withSecondaryItem( const GmodPathQuery& secondaryItem ) const;

		//----------------------------------------------
		// Metadata tags configuration
		//----------------------------------------------

		/**
		 * @brief Configure metadata tags using existing tags with additional configuration
		 * @param configure Function to configure the metadata tags
		 * @return New builder instance with tags configured
		 */
		[[nodiscard]] LocalIdQueryBuilder withTags( TagsConfiguration configure ) const;

		/**
		 * @brief Set metadata tags query directly
		 * @param tags The MetadataTagsQuery for tag filtering
		 * @return New builder instance with tags set
		 */
		[[nodiscard]] LocalIdQueryBuilder withTags( const MetadataTagsQuery& tags ) const;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the current primary item path (if configured from path)
		 * @return Optional GmodPath of the primary item, or nullopt if not set or configured differently
		 */
		[[nodiscard]] std::optional<GmodPath> primaryItem() const;

		/**
		 * @brief Get the current secondary item path (if configured from path)
		 * @return Optional GmodPath of the secondary item, or nullopt if not set or configured differently
		 */
		[[nodiscard]] std::optional<GmodPath> secondaryItem() const;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		std::optional<GmodPathQuery> m_primaryItem;
		std::optional<GmodPathQuery> m_secondaryItem;
		std::optional<MetadataTagsQuery> m_tags;

		//----------------------------------------------
		// Internal matching methods
		//----------------------------------------------

		[[nodiscard]] bool match( const std::string& localIdStr ) const;
		[[nodiscard]] bool match( const LocalId& localId ) const;
	};
}

#include "LocalIdQuery.inl"
