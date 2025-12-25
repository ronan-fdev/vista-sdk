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
 * @file UniversalIdBuilder.h
 * @brief Builder pattern for constructing VIS UniversalId instances
 * @details Provides a fluent API for building UniversalId objects combining IMO numbers with LocalIds.
 *          UniversalId represents a globally unique sensor identifier across all vessels.
 *          Format: data.dnv.com/IMO1234567/dnv-v2/vis-3-4a/621.21/S90/sec/411.1/C101/meta/qty-mass/cnt-fuel.oil/pos-inlet
 */

#pragma once

#include "ImoNumber.h"
#include "LocalIdBuilder.h"

#include <optional>
#include <string>
#include <string_view>

namespace dnv::vista::sdk
{
	class UniversalId;

	/**
	 * @class UniversalIdBuilder
	 * @brief Builder for constructing VIS UniversalId instances with fluent API
	 * @details Provides immutable builder pattern for creating UniversalId objects with validation.
	 *          Each with* method returns a new builder instance with the modified value,
	 *          preserving immutability. A valid UniversalId requires both an IMO number and a valid LocalIdBuilder.
	 *          Use build() to construct the final UniversalId object from the builder state.
	 *
	 * @note UniversalId combines vessel identification (IMO number) with sensor identification (LocalId)
	 *       to create globally unique identifiers in the maritime domain.
	 */
	class UniversalIdBuilder final
	{
	public:
		/** @brief Naming entity identifier for UniversalId */
		static constexpr const char* NamingEntity = "data.dnv.com";

	private:
		/** @brief Default constructor */
		UniversalIdBuilder() = default;

	public:
		/** @brief Copy constructor */
		UniversalIdBuilder( const UniversalIdBuilder& ) = default;

		/** @brief Move constructor */
		UniversalIdBuilder( UniversalIdBuilder&& ) noexcept = default;

		/** @brief Destructor */
		~UniversalIdBuilder() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		UniversalIdBuilder& operator=( const UniversalIdBuilder& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		UniversalIdBuilder& operator=( UniversalIdBuilder&& ) noexcept = default;

		/**
		 * @brief Check equality with another builder
		 * @param other Builder to compare with
		 * @return True if IMO number and LocalIdBuilder are equal
		 */
		[[nodiscard]] inline bool operator==( const UniversalIdBuilder& other ) const noexcept;

		/**
		 * @brief Check inequality with another builder
		 * @param other Builder to compare with
		 * @return True if IMO number or LocalIdBuilder differ
		 */
		[[nodiscard]] inline bool operator!=( const UniversalIdBuilder& other ) const noexcept;

		/**
		 * @brief Get the IMO number
		 * @return Optional containing the ImoNumber if set, nullopt otherwise
		 */
		[[nodiscard]] inline const std::optional<ImoNumber>& imoNumber() const noexcept;

		/**
		 * @brief Get the LocalIdBuilder
		 * @return Optional containing the LocalIdBuilder if set, nullopt otherwise
		 */
		[[nodiscard]] inline const std::optional<LocalIdBuilder>& localId() const noexcept;

		/**
		 * @brief Check if builder state is valid for building a UniversalId
		 * @return True if both IMO number and valid LocalIdBuilder are set
		 */
		[[nodiscard]] inline bool isValid() const noexcept;

		/**
		 * @brief Create a new UniversalIdBuilder for the given VIS version
		 * @param visVersion VIS version for the LocalId component
		 * @return New UniversalIdBuilder instance with LocalIdBuilder initialized
		 */
		[[nodiscard]] static UniversalIdBuilder create( VisVersion visVersion ) noexcept;

		/**
		 * @brief Set the IMO number
		 * @param imoNumber IMO number for vessel identification
		 * @return New UniversalIdBuilder with IMO number set
		 */
		[[nodiscard]] UniversalIdBuilder withImoNumber( const ImoNumber& imoNumber ) const;

		/**
		 * @brief Set the IMO number from optional
		 * @param imoNumber Optional IMO number
		 * @return New UniversalIdBuilder with IMO number set
		 * @throws std::invalid_argument if imoNumber is nullopt
		 */
		[[nodiscard]] UniversalIdBuilder withImoNumber( const std::optional<ImoNumber>& imoNumber ) const;

		/**
		 * @brief Remove the IMO number
		 * @return New UniversalIdBuilder with IMO number cleared
		 */
		[[nodiscard]] UniversalIdBuilder withoutImoNumber() const noexcept;

		/**
		 * @brief Set the LocalIdBuilder
		 * @param localIdBuilder LocalIdBuilder containing sensor identification
		 * @return New UniversalIdBuilder with LocalIdBuilder set
		 */
		[[nodiscard]] UniversalIdBuilder withLocalId( const LocalIdBuilder& localIdBuilder ) const;

		/**
		 * @brief Set the LocalIdBuilder from optional
		 * @param localIdBuilder Optional LocalIdBuilder
		 * @return New UniversalIdBuilder with LocalIdBuilder set
		 * @throws std::invalid_argument if localIdBuilder is nullopt
		 */
		[[nodiscard]] UniversalIdBuilder withLocalId( const std::optional<LocalIdBuilder>& localIdBuilder ) const;

		/**
		 * @brief Remove the LocalIdBuilder
		 * @return New UniversalIdBuilder with LocalIdBuilder cleared
		 */
		[[nodiscard]] UniversalIdBuilder withoutLocalId() const noexcept;

		/**
		 * @brief Build the final UniversalId object from builder state
		 * @return UniversalId instance constructed from current builder state
		 * @throws std::invalid_argument if builder state is invalid (missing IMO number or LocalIdBuilder)
		 */
		[[nodiscard]] UniversalId build() const;

		/**
		 * @brief Convert builder state to string representation
		 * @return String representation in format: data.dnv.com/{imo-number}{local-id}
		 * @throws std::invalid_argument if IMO number or LocalIdBuilder is missing
		 */
		[[nodiscard]] std::string toString() const;

		/**
		 * @brief Create UniversalId from string representation
		 * @param universalIdStr String representation of UniversalId (e.g., "data.dnv.com/9999999/dnv-v2/vis-3-4a/411.1/H101/meta/qty-power")
		 * @return Optional containing UniversalId if parsing succeeded, nullopt otherwise
		 * @note This method does not throw exceptions. Use the overload with ParsingErrors for diagnostics.
		 */
		[[nodiscard]] static std::optional<UniversalId> fromString( std::string_view universalIdStr ) noexcept;

		/**
		 * @brief Create UniversalId from string representation with error reporting
		 * @param universalIdStr String representation of UniversalId
		 * @param errors Output parameter filled with parsing errors if parsing fails
		 * @return Optional containing UniversalId if parsing succeeded, nullopt otherwise
		 * @note This method does not throw exceptions. Check the errors parameter for failure details.
		 */
		[[nodiscard]] static std::optional<UniversalId> fromString( std::string_view universalIdStr,
			ParsingErrors& errors ) noexcept;

	private:
		std::optional<ImoNumber> m_imoNumber;			///< IMO number for vessel identification
		std::optional<LocalIdBuilder> m_localIdBuilder; ///< LocalIdBuilder for sensor identification
	};
} // namespace dnv::vista::sdk

#include "detail/UniversalIdBuilder.inl"
