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
 * @file LocalId.h
 * @brief Immutable VIS Local ID representation
 * @details Provides an immutable LocalId class representing a unique sensor identifier
 *          according to the dnv-v2 naming rule. LocalId combines GmodPath(s) with
 *          metadata tags to create globally unique sensor identifiers within a vessel.
 *          Format: /dnv-v2/vis-3-4a/411.1/H101/meta/qty-power/cnt-fuel.oil
 */

#pragma once

#include "CodebookName.h"
#include "GmodPath.h"
#include "LocalIdBuilder.h"
#include "MetadataTag.h"
#include "VisVersions.h"

#include <optional>
#include <string>
#include <vector>

namespace dnv::vista::sdk
{
	class ParsingErrors;

	/**
	 * @class LocalId
	 * @brief Immutable representation of a VIS Local ID
	 * @details Represents a unique sensor identifier within a vessel according to the dnv-v2 naming rule.
	 *          LocalId is constructed from a LocalIdBuilder and provides read-only access to all components.
	 *          Once constructed, a LocalId cannot be modified. To create variations, access the builder
	 *          via builder() and create a new LocalId instance.
	 *
	 * @note LocalId follows the dnv-v2 naming rule format:
	 *       /dnv-v2/vis-{version}/{primary-item}[/sec/{secondary-item}]/meta[/qty-{value}][/cnt-{value}]...
	 */
	class LocalId
	{
		friend class LocalIdBuilder;

	public:
		/** @brief Naming rule identifier for LocalId */
		static constexpr const char* NamingRule = "dnv-v2";

	protected:
		/**
		 * @brief Construct LocalId from builder
		 * @param builder LocalIdBuilder with validated state
		 * @throws std::invalid_argument if builder is invalid (missing primary item)
		 */
		explicit LocalId( const LocalIdBuilder& builder );

	public:
		/** @brief Default constructor */
		LocalId() = delete;

		/** @brief Copy constructor */
		LocalId( const LocalId& ) = default;

		/** @brief Move constructor */
		LocalId( LocalId&& ) noexcept = default;

		/** @brief Virtual destructor for inheritance */
		virtual ~LocalId() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		LocalId& operator=( const LocalId& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		LocalId& operator=( LocalId&& ) noexcept = default;

		/**
		 * @brief Check equality with another LocalId
		 * @param other LocalId to compare with
		 * @return True if all components are equal
		 */
		[[nodiscard]] inline bool operator==( const LocalId& other ) const noexcept;

		/**
		 * @brief Check inequality with another LocalId
		 * @param other LocalId to compare with
		 * @return True if any component differs
		 */
		[[nodiscard]] inline bool operator!=( const LocalId& other ) const noexcept;

		/**
		 * @brief Get the VIS version for this LocalId
		 * @return The VisVersion enum value
		 */
		[[nodiscard]] inline VisVersion version() const noexcept;

		/**
		 * @brief Get the primary item (GMOD path)
		 * @return Reference to the primary GmodPath
		 */
		[[nodiscard]] inline const GmodPath& primaryItem() const noexcept;

		/**
		 * @brief Get the secondary item (GMOD path)
		 * @return Optional containing the secondary GmodPath if set, nullopt otherwise
		 */
		[[nodiscard]] inline const std::optional<GmodPath>& secondaryItem() const noexcept;

		/**
		 * @brief Get the verbose mode flag
		 * @return True if verbose mode is enabled
		 */
		[[nodiscard]] inline bool isVerboseMode() const noexcept;

		/**
		 * @brief Get all metadata tags that are set
		 * @return Vector of all non-empty metadata tags
		 */
		[[nodiscard]] inline std::vector<MetadataTag> metadataTags() const;

		/**
		 * @brief Get the Quantity metadata tag
		 * @return Optional containing the Quantity tag if set, nullopt otherwise
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& quantity() const noexcept;

		/**
		 * @brief Get the Content metadata tag
		 * @return Optional containing the Content tag if set, nullopt otherwise
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& content() const noexcept;

		/**
		 * @brief Get the Calculation metadata tag
		 * @return Optional containing the Calculation tag if set, nullopt otherwise
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& calculation() const noexcept;

		/**
		 * @brief Get the State metadata tag
		 * @return Optional containing the State tag if set, nullopt otherwise
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& state() const noexcept;

		/**
		 * @brief Get the Command metadata tag
		 * @return Optional containing the Command tag if set, nullopt otherwise
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& command() const noexcept;

		/**
		 * @brief Get the Type metadata tag
		 * @return Optional containing the Type tag if set, nullopt otherwise
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& type() const noexcept;

		/**
		 * @brief Get the Position metadata tag
		 * @return Optional containing the Position tag if set, nullopt otherwise
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& position() const noexcept;

		/**
		 * @brief Get the Detail metadata tag
		 * @return Optional containing the Detail tag if set, nullopt otherwise
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& detail() const noexcept;

		/**
		 * @brief Check if LocalId has any custom (non-standard) metadata tags
		 * @return True if any metadata tag is custom
		 */
		[[nodiscard]] inline bool hasCustomTag() const noexcept;

		/**
		 * @brief Get the builder that created this LocalId
		 * @return Const reference to the internal builder
		 */
		[[nodiscard]] inline const LocalIdBuilder& builder() const noexcept;

		/**
		 * @brief Convert to string representation
		 * @return String representation of the LocalId in dnv-v2 format
		 */
		[[nodiscard]] virtual std::string toString() const;

		/**
		 * @brief Create LocalId from string representation
		 * @param localIdStr String representation of LocalId in dnv-v2 format
		 * @return Optional containing LocalId if parsing succeeded, nullopt otherwise
		 * @note This method silently fails on parse errors. Use the overload with ParsingErrors for diagnostics.
		 * @note VIS version is extracted from the localIdStr (e.g., /dnv-v2/vis-3-4a/...)
		 */
		[[nodiscard]] static std::optional<LocalId> fromString( std::string_view localIdStr ) noexcept;

		/**
		 * @brief Create LocalId from string representation with error reporting
		 * @param localIdStr String representation of LocalId in dnv-v2 format
		 * @param errors Output parameter filled with parsing errors if parsing fails
		 * @return Optional containing LocalId if parsing succeeded, nullopt otherwise
		 * @note VIS version is extracted from the localIdStr (e.g., /dnv-v2/vis-3-4a/...)
		 */
		[[nodiscard]] static std::optional<LocalId> fromString( std::string_view localIdStr,
			ParsingErrors& errors ) noexcept;

	protected:
		LocalIdBuilder m_builder; ///< Internal builder containing all state
	};
} // namespace dnv::vista::sdk

#include "detail/LocalId.inl"
