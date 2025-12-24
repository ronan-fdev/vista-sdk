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
 * @file LocalIdBuilder.h
 * @brief Builder pattern for constructing VIS LocalId instances
 * @details Provides a fluent API for building LocalId objects according to the dnv-v2 naming rule.
 *          LocalId combines GmodPath(s) with metadata tags to create unique sensor identifiers.
 *          Format: /dnv-v2/vis-3-4a/411.1/C101.62/S205/meta/qty-pressure/cnt-lubricating.oil/pos-inlet
 */

#pragma once

#include "GmodPath.h"
#include "MetadataTag.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace dnv::vista::sdk
{
	class LocalId;

	/**
	 * @class LocalIdBuilder
	 * @brief Builder for constructing VIS LocalId instances with fluent API
	 * @details Provides immutable builder pattern for creating LocalId objects with validation.
	 *          Each with* method returns a new builder instance with the modified value,
	 *          preserving immutability. Values are validated against the VIS version specification.
	 *          Use build() to construct the final LocalId object from the builder state.
	 *
	 * @note LocalId follows the dnv-v2 naming rule and supports the following codebooks:
	 *       - Quantity, Content, Calculation, State, Command, Type, Position, Detail
	 */
	class LocalIdBuilder final
	{
	public:
		/** @brief Naming rule identifier for LocalId */
		static constexpr const char* NamingRule = "dnv-v2";

	private:
		/** @brief Default constructor. */
		LocalIdBuilder() = default;

	public:
		/** @brief Copy constructor */
		LocalIdBuilder( const LocalIdBuilder& ) = default;

		/** @brief Move constructor */
		LocalIdBuilder( LocalIdBuilder&& ) noexcept = default;

		/** @brief Destructor */
		~LocalIdBuilder() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		LocalIdBuilder& operator=( const LocalIdBuilder& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		LocalIdBuilder& operator=( LocalIdBuilder&& ) noexcept = default;

		/**
		 * @brief Check equality with another builder
		 * @param other Builder to compare with
		 * @return True if all components are equal
		 */
		[[nodiscard]] inline bool operator==( const LocalIdBuilder& other ) const noexcept;

		/**
		 * @brief Check inequality with another builder
		 * @param other Builder to compare with
		 * @return True if any component differs
		 */
		[[nodiscard]] inline bool operator!=( const LocalIdBuilder& other ) const noexcept;

		/**
		 * @brief Get the VIS version for this builder
		 * @return Optional containing the VisVersion if set, nullopt otherwise
		 */
		[[nodiscard]] inline std::optional<VisVersion> version() const noexcept;

		/**
		 * @brief Get the verbose mode flag
		 * @return True if verbose mode is enabled
		 */
		[[nodiscard]] inline bool isVerboseMode() const noexcept;

		/**
		 * @brief Check if builder state is valid for building a LocalId
		 * @return True if VisVersion, primary item, and at least one metadata tag are set
		 */
		[[nodiscard]] inline bool isValid() const noexcept;

		/**
		 * @brief Check if builder is empty (no components set)
		 * @return True if no primary item, secondary item, or metadata tags are set
		 */
		[[nodiscard]] inline bool isEmpty() const noexcept;

		/**
		 * @brief Checks if the builder has no metadata tags set
		 * @return True if no metadata tags (quantity, content, etc.) are present, false otherwise
		 */
		[[nodiscard]] inline bool isEmptyMetadata() const noexcept;

		/**
		 * @brief Check if builder has any custom (non-standard) metadata tags
		 * @return True if any metadata tag is custom
		 */
		[[nodiscard]] inline bool hasCustomTag() const noexcept;

		/**
		 * @brief Get the primary item (GMOD path)
		 * @return Optional containing the primary GmodPath if set, nullopt otherwise
		 */
		[[nodiscard]] inline const std::optional<GmodPath>& primaryItem() const noexcept;

		/**
		 * @brief Get the secondary item (GMOD path)
		 * @return Optional containing the secondary GmodPath if set, nullopt otherwise
		 */
		[[nodiscard]] inline const std::optional<GmodPath>& secondaryItem() const noexcept;

		/**
		 * @brief Get all metadata tags that are set
		 * @return Vector containing all set metadata tags in the order: Quantity, Content, Calculation, State, Command, Type, Position, Detail
		 */
		[[nodiscard]] inline std::vector<MetadataTag> metadataTags() const noexcept;

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
		 * @brief Create a new LocalIdBuilder for the given VIS version
		 * @param visVersion VIS version for this LocalId
		 * @return New LocalIdBuilder instance with the specified VIS version
		 */
		[[nodiscard]] static LocalIdBuilder create( VisVersion visVersion ) noexcept;

		/**
		 * @brief Set the VIS version
		 * @param version VIS version enum value
		 * @return New LocalIdBuilder with VIS version set
		 */
		[[nodiscard]] LocalIdBuilder withVisVersion( VisVersion version ) const noexcept;

		/**
		 * @brief Set the VIS version from string
		 * @param visVersionStr String representation of VIS version (e.g., "3-4a")
		 * @return New LocalIdBuilder with VIS version set
		 * @throws std::invalid_argument if version string is invalid
		 */
		[[nodiscard]] LocalIdBuilder withVisVersion( std::string_view visVersionStr ) const;

		/**
		 * @brief Remove the VIS version
		 * @return New LocalIdBuilder with VIS version cleared
		 */
		[[nodiscard]] LocalIdBuilder withoutVisVersion() const noexcept;

		/**
		 * @brief Set the primary item (GMOD path)
		 * @param path Primary GmodPath
		 * @return New LocalIdBuilder with primary item set
		 */
		[[nodiscard]] LocalIdBuilder withPrimaryItem( const GmodPath& path ) const;

		/**
		 * @brief Set the primary item from optional GmodPath
		 * @param path Optional primary GmodPath
		 * @return New LocalIdBuilder with primary item set
		 * @throws std::invalid_argument if path is nullopt
		 */
		[[nodiscard]] LocalIdBuilder withPrimaryItem( const std::optional<GmodPath>& path ) const;

		/**
		 * @brief Remove the primary item
		 * @return New LocalIdBuilder with primary item cleared
		 */
		[[nodiscard]] LocalIdBuilder withoutPrimaryItem() const noexcept;

		/**
		 * @brief Set the secondary item (GMOD path)
		 * @param path Secondary GmodPath
		 * @return New LocalIdBuilder with secondary item set
		 */
		[[nodiscard]] LocalIdBuilder withSecondaryItem( const GmodPath& path ) const;

		/**
		 * @brief Set the secondary item from optional GmodPath
		 * @param path Optional secondary GmodPath
		 * @return New LocalIdBuilder with secondary item set
		 * @throws std::invalid_argument if path is nullopt
		 */
		[[nodiscard]] LocalIdBuilder withSecondaryItem( const std::optional<GmodPath>& path ) const;

		/**
		 * @brief Remove the secondary item
		 * @return New LocalIdBuilder with secondary item cleared
		 */
		[[nodiscard]] LocalIdBuilder withoutSecondaryItem() const noexcept;

		/**
		 * @brief Set a metadata tag
		 * @param tag MetadataTag to set (codebook name determines which slot)
		 * @return New LocalIdBuilder with the metadata tag set
		 */
		[[nodiscard]] LocalIdBuilder withMetadataTag( const MetadataTag& tag ) const;

		/**
		 * @brief Set a metadata tag from optional
		 * @param tag Optional MetadataTag to set
		 * @return New LocalIdBuilder with the metadata tag set if tag has value, unchanged copy if nullopt
		 * @note If tag is nullopt, returns unchanged copy
		 */
		[[nodiscard]] LocalIdBuilder withMetadataTag( const std::optional<MetadataTag>& tag ) const;

		/**
		 * @brief Remove a metadata tag by codebook name
		 * @param name Codebook name identifying which tag to remove
		 * @return New LocalIdBuilder with the specified metadata tag cleared
		 */
		[[nodiscard]] LocalIdBuilder withoutMetadataTag( CodebookName name ) const noexcept;

		/**
		 * @brief Set verbose mode
		 * @param verbose True to enable verbose mode (include common names in string representation)
		 * @return New LocalIdBuilder with verbose mode set
		 */
		[[nodiscard]] LocalIdBuilder withVerboseMode( bool verbose ) const noexcept;

		/**
		 * @brief Build the final LocalId object from builder state
		 * @return LocalId instance constructed from current builder state
		 * @throws std::invalid_argument if builder state is invalid (no primary item)
		 */
		[[nodiscard]] LocalId build() const;

		/**
		 * @brief Convert builder state to string representation
		 * @return String representation of the LocalId in dnv-v2 format
		 * @note Format: /dnv-v2/vis-{version}/{primary-item}[/sec/{secondary-item}]/meta[/qty-{value}][/cnt-{value}]...
		 */
		[[nodiscard]] std::string toString() const;

		/**
		 * @brief Convert builder state to string representation (in-place)
		 * @param builder StringBuilder to append the LocalId string representation to
		 * @note Format: /dnv-v2/vis-{version}/{primary-item}[/sec/{secondary-item}]/meta[/qty-{value}][/cnt-{value}]...
		 */
		void toString( StringBuilder& builder ) const;

		/**
		 * @brief Create LocalId from string representation
		 * @param localIdStr String representation of LocalId in dnv-v2 format (e.g., "/dnv-v2/vis-3-4a/411.1/H101/meta/qty-power")
		 * @return Optional containing LocalId if parsing succeeded, nullopt otherwise
		 * @note This method does not throw exceptions. Use the overload with ParsingErrors for diagnostics.
		 */
		[[nodiscard]] static std::optional<LocalId> fromString( std::string_view localIdStr ) noexcept;

		/**
		 * @brief Create LocalId from string representation with error reporting
		 * @param localIdStr String representation of LocalId in dnv-v2 format
		 * @param errors Output parameter filled with parsing errors if parsing fails
		 * @return Optional containing LocalId if parsing succeeded, nullopt otherwise
		 * @note This method does not throw exceptions. Check the errors parameter for failure details.
		 */
		[[nodiscard]] static std::optional<LocalId> fromString( std::string_view localIdStr,
			ParsingErrors& errors ) noexcept;

	private:
		std::optional<VisVersion> m_visVersion;	  ///< VIS version for this LocalId
		bool m_verboseMode{ false };			  ///< Verbose mode flag (include common names)
		std::optional<GmodPath> m_primaryItem;	  ///< Primary item (required)
		std::optional<GmodPath> m_secondaryItem;  ///< Secondary item (optional)
		std::optional<MetadataTag> m_quantity;	  ///< Quantity metadata tag
		std::optional<MetadataTag> m_content;	  ///< Content metadata tag
		std::optional<MetadataTag> m_calculation; ///< Calculation metadata tag
		std::optional<MetadataTag> m_state;		  ///< State metadata tag
		std::optional<MetadataTag> m_command;	  ///< Command metadata tag
		std::optional<MetadataTag> m_type;		  ///< Type metadata tag
		std::optional<MetadataTag> m_position;	  ///< Position metadata tag
		std::optional<MetadataTag> m_detail;	  ///< Detail metadata tag
	};
} // namespace dnv::vista::sdk

#include "detail/LocalIdBuilder.inl"
