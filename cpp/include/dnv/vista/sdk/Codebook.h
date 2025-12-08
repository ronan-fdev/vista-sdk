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
 * @file Codebook.h
 * @brief VIS codebook representation with validation and metadata tag creation
 * @details Represents a VIS codebook containing standard values, groups, and validation logic.
 *          Codebooks are used to validate metadata tag values and create MetadataTag instances
 *          for use in VIS Local IDs. Special validation logic is provided for Position codebook.
 */

#pragma once

#include "CodebookName.h"
#include "MetadataTag.h"
#include "StringHash.h"

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace dnv::vista::sdk
{
	struct CodebookDto;

	/**
	 * @enum PositionValidationResult
	 * @brief Result codes for position string validation
	 * @details Values < 100 indicate errors (invalid), values >= 100 indicate acceptable (valid or custom).
	 *          Used by Codebook::validatePosition() to determine if a position string is valid.
	 */
	enum class PositionValidationResult : std::uint8_t
	{
		Invalid = 0,		 ///< Invalid format, whitespace, or non-ISO characters
		InvalidOrder = 1,	 ///< Numbers not at end or not alphabetically sorted
		InvalidGrouping = 2, ///< Duplicate groups (except DEFAULT_GROUP)
		Valid = 100,		 ///< Standard value, number, or valid composite
		Custom = 101		 ///< Custom value not in standard codebook
	};

	/**
	 * @class Codebook
	 * @brief Represents a VIS codebook with standard values, groups, and validation logic
	 * @details A codebook contains validated standard values organized into groups.
	 *          It provides factory methods to create MetadataTag instances with automatic
	 *          validation and custom tag detection. Special validation logic is provided
	 *          for the Position codebook (composite positions, ordering, grouping rules).
	 *
	 *          Codebook instances are created by the Codebooks class from CodebookDto data.
	 */
	class Codebook final
	{
		friend class Codebooks;

	private:
		/**
		 * @brief Internal constructor
		 * @param dto CodebookDto containing raw codebook data
		 * @details Constructs codebook by parsing DTO values, building group mappings,
		 *          and extracting standard values and group sets.
		 */
		explicit Codebook( const CodebookDto& dto );

	public:
		/** @brief Default constructor */
		Codebook() = delete;

		/** @brief Copy constructor */
		Codebook( const Codebook& ) = default;

		/** @brief Move constructor */
		Codebook( Codebook&& ) noexcept = default;

		/** @brief Destructor */
		~Codebook() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		Codebook& operator=( const Codebook& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		Codebook& operator=( Codebook&& ) noexcept = default;

		/**
		 * @brief Get the codebook name
		 * @return The CodebookName enum value
		 */
		inline CodebookName name() const noexcept;

		/**
		 * @brief Get the set of standard values
		 * @return Const reference to unordered_set of standard value strings
		 */
		inline const std::unordered_set<std::string, StringHash, std::equal_to<>>& standardValues() const noexcept;

		/**
		 * @brief Get the set of group names
		 * @return Const reference to unordered_set of group name strings
		 */
		inline const std::unordered_set<std::string, StringHash, std::equal_to<>>& groups() const noexcept;

		/**
		 * @brief Check if a group exists in this codebook
		 * @param group Group name to check
		 * @return True if group exists
		 */
		inline bool hasGroup( std::string_view group ) const noexcept;

		/**
		 * @brief Check if a value is a standard value in this codebook
		 * @param value Value to check
		 * @return True if value is a standard value (for Position codebook, numbers always return true)
		 */
		inline bool hasStandardValue( std::string_view value ) const noexcept;

		/**
		 * @brief Create a metadata tag with validation
		 * @param value The tag value to validate
		 * @return Optional MetadataTag if value is valid, std::nullopt otherwise
		 */
		[[nodiscard]] std::optional<MetadataTag> createTag( std::string_view value ) const noexcept;

		/**
		 * @brief Validate a position string with comprehensive rules
		 * @param position The position string to validate
		 * @return PositionValidationResult indicating validation outcome
		 * @details Validates position strings according to VIS rules:
		 *          - ISO string characters only (RFC 3986 unreserved)
		 *          - No leading/trailing whitespace
		 *          - Standard values and numbers are Valid
		 *          - Single non-standard values are Custom
		 *          - Composite positions (with '-') must have:
		 *            * All parts individually valid
		 *            * Numbers only at the end
		 *            * Non-numbers alphabetically sorted
		 *            * No duplicate groups (except DEFAULT_GROUP)
		 * @warning Only valid for Position codebook (undefined behavior for others)
		 */
		PositionValidationResult validatePosition( std::string_view position ) const noexcept;

	private:
		CodebookName m_name;																  ///< Codebook type identifier
		std::unordered_map<std::string, std::string, StringHash, std::equal_to<>> m_groupMap; ///< Maps value -> group name (transparent hash)
		std::unordered_set<std::string, StringHash, std::equal_to<>> m_standardValues;		  ///< Set of standard values (transparent hash)
		std::unordered_set<std::string, StringHash, std::equal_to<>> m_groups;				  ///< Set of group names (transparent hash)
	};
} // namespace dnv::vista::sdk

#include "detail/Codebook.inl"
