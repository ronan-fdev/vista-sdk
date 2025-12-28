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
 * @file LocalIdQuery.h
 * @brief Immutable query pattern matching for LocalId instances
 * @details Provides LocalIdQuery and LocalIdQueryBuilder for declarative matching
 *          of LocalId instances. Supports matching on primary/secondary items via
 *          GmodPathQuery and metadata tags via MetadataTagsQuery.
 */

#pragma once

#include "GmodPathQuery.h"
#include "LocalId.h"
#include "MetadataTagsQuery.h"

#include <functional>
#include <optional>

namespace dnv::vista::sdk
{
	class LocalIdQueryBuilder;

	/**
	 * @class LocalIdQuery
	 * @brief Immutable query for matching LocalId instances
	 * @details Orchestrates GmodPathQuery and MetadataTagsQuery to match LocalId instances.
	 *          Built using LocalIdQueryBuilder fluent API. Once constructed, LocalIdQuery
	 *          is immutable and thread-safe.
	 *
	 * Query matching rules:
	 * - Primary item: Must match if specified
	 * - Secondary item: Must match if specified, or checked for presence/absence if required
	 * - Metadata tags: Must match if specified
	 * - All specified criteria must match (AND semantics)
	 */
	class LocalIdQuery
	{
		friend class LocalIdQueryBuilder;

	protected:
		/**
		 * @brief Construct from builder
		 * @param builder Builder containing query configuration
		 */
		explicit LocalIdQuery( const LocalIdQueryBuilder& builder );

	public:
		/** @brief Default constructor */
		LocalIdQuery() = delete;

		/** @brief Copy constructor */
		LocalIdQuery( const LocalIdQuery& ) = default;

		/** @brief Move constructor */
		LocalIdQuery( LocalIdQuery&& ) noexcept = default;

		/** @brief Destructor */
		~LocalIdQuery() = default;

		/** @brief Copy assignment operator */
		LocalIdQuery& operator=( const LocalIdQuery& ) = default;

		/** @brief Move assignment operator */
		LocalIdQuery& operator=( LocalIdQuery&& ) noexcept = default;

		/**
		 * @brief Match a LocalId instance against this query
		 * @param localId LocalId to match
		 * @return True if localId matches all query criteria
		 * @note If localId version < latest VIS version, automatic version conversion is attempted
		 */
		[[nodiscard]] bool match( const LocalId& localId ) const;

		/**
		 * @brief Match a LocalId string against this query
		 * @param localIdStr LocalId string in dnv-v2 format
		 * @return True if parsed LocalId matches all query criteria, false if parsing fails or no match
		 */
		[[nodiscard]] bool match( std::string_view localIdStr ) const;

	private:
		std::optional<GmodPathQuery> m_primaryItem;
		std::optional<GmodPathQuery> m_secondaryItem;
		std::optional<MetadataTagsQuery> m_tags;
		std::optional<bool> m_requireSecondaryItem; ///< true = must have, false = must not have, nullopt = don't care
	};

	/**
	 * @class LocalIdQueryBuilder
	 * @brief Fluent builder for constructing LocalIdQuery instances
	 * @details Provides declarative API for building LocalId queries with support for:
	 *          - Primary/secondary item matching via GmodPathQuery
	 *          - Metadata tags matching via MetadataTagsQuery
	 *          - Secondary item presence/absence requirements
	 *
	 * @example
	 * @code
	 * // Match LocalIds with specific primary item node
	 * auto query = LocalIdQueryBuilder::create()
	 *     .withPrimaryItem([](GmodPathQueryBuilder::Nodes& nodes) {
	 *         return nodes.withNode(someNode, true).build();
	 *     })
	 *     .build();
	 *
	 * // Match LocalIds from existing LocalId
	 * auto query2 = LocalIdQueryBuilder::from(existingLocalId).build();
	 *
	 * // Match LocalIds with specific tags
	 * auto query3 = LocalIdQueryBuilder::create()
	 *     .withTags([](MetadataTagsQueryBuilder& tags) {
	 *         return tags.withTag(CodebookName::quantity, "temperature").build();
	 *     })
	 *     .build();
	 * @endcode
	 */
	class LocalIdQueryBuilder
	{
		friend class LocalIdQuery;

	public:
		/** @brief Default constructor */
		LocalIdQueryBuilder() = default;

		/** @brief Copy constructor */
		LocalIdQueryBuilder( const LocalIdQueryBuilder& ) = default;

		/** @brief Move constructor */
		LocalIdQueryBuilder( LocalIdQueryBuilder&& ) noexcept = default;

		/** @brief Destructor */
		~LocalIdQueryBuilder() = default;

		/** @brief Copy assignment operator */
		LocalIdQueryBuilder& operator=( const LocalIdQueryBuilder& ) = default;

		/** @brief Move assignment operator */
		LocalIdQueryBuilder& operator=( LocalIdQueryBuilder&& ) noexcept = default;

		/**
		 * @brief Create empty builder (matches all LocalIds)
		 * @return New LocalIdQueryBuilder instance
		 */
		[[nodiscard]] static LocalIdQueryBuilder create() noexcept;

		/**
		 * @brief Create builder from existing LocalId
		 * @param localId LocalId to build query from
		 * @return LocalIdQueryBuilder configured to match the given LocalId exactly
		 */
		[[nodiscard]] static LocalIdQueryBuilder from( const LocalId& localId ) noexcept;

		/**
		 * @brief Create builder from LocalId string
		 * @param localIdStr LocalId string in dnv-v2 format
		 * @return Optional containing builder if parsing succeeded, nullopt otherwise
		 */
		[[nodiscard]] static std::optional<LocalIdQueryBuilder> from( std::string_view localIdStr ) noexcept;

		/**
		 * @brief Build immutable LocalIdQuery
		 * @return Constructed LocalIdQuery instance
		 */
		[[nodiscard]] LocalIdQuery build() const;

		/**
		 * @brief Set primary item query using GmodPath
		 * @param primaryItem GmodPath to match
		 * @return Reference to this builder for chaining
		 */
		LocalIdQueryBuilder& withPrimaryItem( const GmodPath& primaryItem );

		/**
		 * @brief Set primary item query
		 * @param primaryItem GmodPathQuery to use
		 * @return Reference to this builder for chaining
		 */
		LocalIdQueryBuilder& withPrimaryItem( const GmodPathQuery& primaryItem );

		/**
		 * @brief Set primary item query using Nodes builder configuration
		 * @param configure Function receiving GmodPathQueryBuilder::Nodes reference
		 * @return Reference to this builder for chaining
		 */
		LocalIdQueryBuilder& withPrimaryItem(
			std::function<GmodPathQuery( GmodPathQueryBuilder::Nodes& )> configure );

		/**
		 * @brief Set primary item query using Path builder configuration
		 * @param primaryItem Base GmodPath to configure
		 * @param configure Function receiving GmodPathQueryBuilder::Path reference
		 * @return Reference to this builder for chaining
		 */
		LocalIdQueryBuilder& withPrimaryItem( const GmodPath& primaryItem,
			std::function<GmodPathQuery( GmodPathQueryBuilder::Path& )> configure );

		/**
		 * @brief Set secondary item query using GmodPath
		 * @param secondaryItem GmodPath to match
		 * @return Reference to this builder for chaining
		 */
		LocalIdQueryBuilder& withSecondaryItem( const GmodPath& secondaryItem );

		/**
		 * @brief Set secondary item query
		 * @param secondaryItem GmodPathQuery to use
		 * @return Reference to this builder for chaining
		 */
		LocalIdQueryBuilder& withSecondaryItem( const GmodPathQuery& secondaryItem );

		/**
		 * @brief Set secondary item query using Nodes builder configuration
		 * @param configure Function receiving GmodPathQueryBuilder::Nodes reference
		 * @return Reference to this builder for chaining
		 */
		LocalIdQueryBuilder& withSecondaryItem(
			std::function<GmodPathQuery( GmodPathQueryBuilder::Nodes& )> configure );

		/**
		 * @brief Set secondary item query using Path builder configuration
		 * @param secondaryItem Base GmodPath to configure
		 * @param configure Function receiving GmodPathQueryBuilder::Path reference
		 * @return Reference to this builder for chaining
		 */
		LocalIdQueryBuilder& withSecondaryItem( const GmodPath& secondaryItem,
			std::function<GmodPathQuery( GmodPathQueryBuilder::Path& )> configure );

		/**
		 * @brief Match any LocalId regardless of secondary item presence
		 * @return Reference to this builder for chaining
		 */
		LocalIdQueryBuilder& withAnySecondaryItem();

		/**
		 * @brief Match only LocalIds without secondary item
		 * @return Reference to this builder for chaining
		 */
		LocalIdQueryBuilder& withoutSecondaryItem();

		/**
		 * @brief Set metadata tags query
		 * @param tags MetadataTagsQuery to use
		 * @return Reference to this builder for chaining
		 */
		LocalIdQueryBuilder& withTags( const MetadataTagsQuery& tags );

		/**
		 * @brief Set metadata tags query using builder configuration
		 * @param configure Function receiving MetadataTagsQueryBuilder reference
		 * @return Reference to this builder for chaining
		 */
		LocalIdQueryBuilder& withTags( std::function<MetadataTagsQuery( MetadataTagsQueryBuilder& )> configure );

		/**
		 * @brief Remove all location requirements from primary and secondary items
		 * @return Reference to this builder for chaining
		 * @note Only affects items configured as GmodPathQueryBuilder::Path (not Nodes)
		 */
		LocalIdQueryBuilder& withoutLocations();

		/**
		 * @brief Get the primary item path (if configured as Path query)
		 * @return Pointer to GmodPath if primary item is a Path query, nullptr otherwise
		 */
		[[nodiscard]] const GmodPath* primaryItem() const noexcept;

		/**
		 * @brief Get the secondary item path (if configured as Path query)
		 * @return Pointer to GmodPath if secondary item is a Path query, nullptr otherwise
		 */
		[[nodiscard]] const GmodPath* secondaryItem() const noexcept;

		/**
		 * @brief Get the metadata tags query
		 * @return Optional containing tags query if set
		 */
		[[nodiscard]] inline const std::optional<MetadataTagsQuery>& tags() const noexcept;

		/**
		 * @brief Get the secondary item requirement flag
		 * @return Optional: true = must have, false = must not have, nullopt = don't care
		 */
		[[nodiscard]] inline const std::optional<bool>& requireSecondaryItem() const noexcept;

	private:
		std::optional<GmodPathQuery> m_primaryItem;
		std::optional<GmodPathQuery> m_secondaryItem;
		std::optional<MetadataTagsQuery> m_tags;
		std::optional<bool> m_requireSecondaryItem;
	};
} // namespace dnv::vista::sdk

#include "detail/LocalIdQuery.inl"
