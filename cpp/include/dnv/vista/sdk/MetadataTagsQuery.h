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
 * @file MetadataTagsQuery.h
 * @brief Query API for matching LocalId metadata tags
 * @details Provides fluent API for building queries to filter LocalIds based on their metadata tags.
 *          Supports exact matching (must have only these tags) or subset matching (must have at least these tags).
 */

#pragma once

#include "CodebookName.h"
#include "MetadataTag.h"

#include <unordered_map>

namespace dnv::vista::sdk
{
	class LocalId;
	class MetadataTagsQueryBuilder;

	/**
	 * @class MetadataTagsQuery
	 * @brief Immutable query for matching LocalId metadata tags
	 * @details Created via MetadataTagsQueryBuilder. Provides match() method to test LocalIds.
	 *
	 * @example
	 * @code
	 * auto query = MetadataTagsQueryBuilder::empty()
	 *     .withTag(CodebookName::Quantity, "temperature")
	 *     .withTag(CodebookName::Content, "water")
	 *     .withAllowOtherTags(false)  // Exact match
	 *     .build();
	 *
	 * bool matches = query.match(localId);
	 * @endcode
	 */
	class MetadataTagsQuery final
	{
		friend class MetadataTagsQueryBuilder;

	private:
		explicit MetadataTagsQuery( const MetadataTagsQueryBuilder* builder );

	public:
		/** @brief Copy constructor */
		MetadataTagsQuery( const MetadataTagsQuery& ) = default;

		/** @brief Move constructor */
		MetadataTagsQuery( MetadataTagsQuery&& ) noexcept = default;

		/** @brief Destructor */
		~MetadataTagsQuery() = default;

		/** @brief Copy assignment operator */
		MetadataTagsQuery& operator=( const MetadataTagsQuery& ) = default;

		/** @brief Move assignment operator */
		MetadataTagsQuery& operator=( MetadataTagsQuery&& ) noexcept = default;

		/**
		 * @brief Match against a LocalId
		 * @param localId LocalId to test (nullptr returns false)
		 * @return true if localId matches the query criteria
		 */
		[[nodiscard]] bool match( const LocalId* localId ) const;

		/**
		 * @brief Match against a LocalId
		 * @param localId LocalId to test
		 * @return true if localId matches the query criteria
		 */
		[[nodiscard]] bool match( const LocalId& localId ) const;

	private:
		std::unordered_map<CodebookName, MetadataTag> m_tags;
		bool m_matchExact = false;
	};

	/**
	 * @class MetadataTagsQueryBuilder
	 * @brief Fluent builder for MetadataTagsQuery
	 * @details Allows specifying metadata tags to match and whether to allow additional tags.
	 *          Two matching modes:
	 *          - Subset match (default): LocalId must have AT LEAST the specified tags (can have more)
	 *          - Exact match: LocalId must have EXACTLY the specified tags (no more, no less)
	 *
	 * @example
	 * @code
	 * // Subset matching (allows extra tags)
	 * auto query1 = MetadataTagsQueryBuilder::empty()
	 *     .withTag(CodebookName::Quantity, "temperature")
	 *     .build();
	 *
	 * // Exact matching (no extra tags allowed)
	 * auto query2 = MetadataTagsQueryBuilder::from(localId, false)
	 *     .withAllowOtherTags(false)
	 *     .build();
	 * @endcode
	 */
	class MetadataTagsQueryBuilder final
	{
		friend class MetadataTagsQuery;

	private:
		/** @brief Default constructor */
		MetadataTagsQueryBuilder() = default;

	public:
		/** @brief Copy constructor */
		MetadataTagsQueryBuilder( const MetadataTagsQueryBuilder& ) = default;

		/** @brief Move constructor */
		MetadataTagsQueryBuilder( MetadataTagsQueryBuilder&& ) noexcept = default;

		/** @brief Destructor */
		~MetadataTagsQueryBuilder() = default;

		/** @brief Copy assignment operator */
		MetadataTagsQueryBuilder& operator=( const MetadataTagsQueryBuilder& ) = default;

		/** @brief Move assignment operator */
		MetadataTagsQueryBuilder& operator=( MetadataTagsQueryBuilder&& ) noexcept = default;

		/**
		 * @brief Create empty builder (matches any LocalId by default)
		 * @return New empty builder
		 */
		[[nodiscard]] static MetadataTagsQueryBuilder empty();

		/**
		 * @brief Create builder from existing LocalId
		 * @param localId LocalId to copy tags from
		 * @param allowOtherTags If true, allow additional tags; if false, exact match only
		 * @return Builder initialized with localId's tags
		 */
		[[nodiscard]] static MetadataTagsQueryBuilder from( const LocalId& localId, bool allowOtherTags = true );

		/**
		 * @brief Add a metadata tag to match
		 * @param name Codebook name
		 * @param value Tag value
		 * @return Reference to this builder for chaining
		 * @note If a tag for this codebook already exists, it will be replaced
		 */
		MetadataTagsQueryBuilder& withTag( CodebookName name, const std::string& value );

		/**
		 * @brief Add a metadata tag to match
		 * @param tag MetadataTag to match
		 * @return Reference to this builder for chaining
		 * @note If a tag for this codebook already exists, it will be replaced
		 */
		MetadataTagsQueryBuilder& withTag( const MetadataTag& tag );

		/**
		 * @brief Set whether to allow tags not specified in the query
		 * @param allowOthers If true (default), LocalId can have extra tags (subset matching)
		 *                    If false, LocalId must have EXACTLY the specified tags (exact matching)
		 * @return Reference to this builder for chaining
		 */
		MetadataTagsQueryBuilder& withAllowOtherTags( bool allowOthers );

		/**
		 * @brief Build immutable query
		 * @return MetadataTagsQuery instance
		 */
		[[nodiscard]] MetadataTagsQuery build() const;

	private:
		std::unordered_map<CodebookName, MetadataTag> m_tags;
		bool m_matchExact = false;
	};
} // namespace dnv::vista::sdk
