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
 * @file UniversalId.h
 * @brief Immutable VIS Universal ID representation
 * @details Provides an immutable UniversalId class representing a globally unique sensor identifier
 *          combining vessel identification (IMO number) with sensor identification (LocalId).
 *          Format: data.dnv.com/IMO1234567/dnv-v2/vis-3-4a/621.21/S90/sec/411.1/C101/meta/qty-mass/cnt-fuel.oil/pos-inlet
 */

#pragma once

#include "ImoNumber.h"
#include "LocalId.h"
#include "UniversalIdBuilder.h"

#include <optional>
#include <string>
#include <string_view>

namespace dnv::vista::sdk
{
	class ParsingErrors;

	/**
	 * @class UniversalId
	 * @brief Immutable representation of a VIS Universal ID
	 * @details Represents a globally unique sensor identifier combining IMO number and LocalId.
	 *          UniversalId is constructed from a UniversalIdBuilder and provides read-only access to components.
	 *          Once constructed, a UniversalId cannot be modified. To create variations, access the builder
	 *          via builder() and create a new UniversalId instance.
	 *
	 * @note UniversalId follows the format: {naming-entity}/{imo-number}{local-id}
	 *       Example: data.dnv.com/9999999/dnv-v2/vis-3-4a/411.1/H101/meta/qty-power
	 */
	class UniversalId final
	{
		friend class UniversalIdBuilder;

	public:
		/** @brief Naming entity identifier for UniversalId */
		static constexpr const char* NamingEntity = "data.dnv.com";

	protected:
		/**
		 * @brief Construct UniversalId from builder
		 * @param builder UniversalIdBuilder with validated state
		 * @throws std::invalid_argument if builder is invalid (missing IMO number or LocalId)
		 */
		explicit UniversalId( const UniversalIdBuilder& builder );

	public:
		/** @brief Default constructor */
		UniversalId() = delete;

		/** @brief Copy constructor */
		UniversalId( const UniversalId& ) = default;

		/** @brief Move constructor */
		UniversalId( UniversalId&& ) noexcept = default;

		/** @brief Destructor */
		~UniversalId() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		UniversalId& operator=( const UniversalId& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		UniversalId& operator=( UniversalId&& ) noexcept = default;

		/**
		 * @brief Check equality with another UniversalId
		 * @param other UniversalId to compare with
		 * @return True if both IMO number and LocalId are equal
		 */
		[[nodiscard]] inline bool operator==( const UniversalId& other ) const noexcept;

		/**
		 * @brief Check inequality with another UniversalId
		 * @param other UniversalId to compare with
		 * @return True if IMO number or LocalId differ
		 */
		[[nodiscard]] inline bool operator!=( const UniversalId& other ) const noexcept;

		/**
		 * @brief Get the IMO number
		 * @return Reference to the ImoNumber
		 */
		[[nodiscard]] inline const ImoNumber& imoNumber() const noexcept;

		/**
		 * @brief Get the LocalId
		 * @return Reference to the LocalId
		 */
		[[nodiscard]] inline const LocalId& localId() const noexcept;

		/**
		 * @brief Get the builder that created this UniversalId
		 * @return Const reference to the internal builder
		 */
		[[nodiscard]] inline const UniversalIdBuilder& builder() const noexcept;

		/**
		 * @brief Convert to string representation
		 * @return String representation in format: data.dnv.com/{imo-number}{local-id}
		 */
		[[nodiscard]] std::string toString() const;

		/**
		 * @brief Create UniversalId from string representation
		 * @param universalIdStr String representation of UniversalId
		 * @return Optional containing UniversalId if parsing succeeded, nullopt otherwise
		 * @note This method silently fails on parse errors. Use the overload with ParsingErrors for diagnostics.
		 */
		[[nodiscard]] static std::optional<UniversalId> fromString( std::string_view universalIdStr ) noexcept;

		/**
		 * @brief Create UniversalId from string representation with error reporting
		 * @param universalIdStr String representation of UniversalId
		 * @param errors Output parameter filled with parsing errors if parsing fails
		 * @return Optional containing UniversalId if parsing succeeded, nullopt otherwise
		 */
		[[nodiscard]] static std::optional<UniversalId> fromString( std::string_view universalIdStr,
			ParsingErrors& errors ) noexcept;

	protected:
		UniversalIdBuilder m_builder; ///< Internal builder containing all state
		LocalId m_localId;			  ///< Pre-built LocalId for efficient access
	};
} // namespace dnv::vista::sdk

#include "detail/UniversalId.inl"
