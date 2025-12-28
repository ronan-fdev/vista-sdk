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
 * @file ShipId.h
 * @brief Ship identifier representation for ISO 19848
 * @details Represents a ship identifier that can be either an IMO number or an alternative identifier.
 *          This is used in the Header of DataChannelList and TimeSeriesData packages.
 */

#pragma once

#include "dnv/vista/sdk/ImoNumber.h"

#include <optional>
#include <string>
#include <string_view>
#include <variant>

namespace dnv::vista::sdk
{
	/**
	 * @brief Represents a ship identifier in ISO 19848 format.
	 *
	 * A ShipId is a discriminated union that can hold either:
	 * - An IMO number (preferred, validated identifier)
	 * - An alternative string identifier (for vessels without IMO numbers)
	 *
	 * In ISO 19848, IMO numbers as ShipID should be prefixed with "IMO".
	 */
	class ShipId final
	{
	public:
		/** @brief Default constructor*/
		ShipId() = delete;

		/**
		 * @brief Construct ShipId from IMO number
		 * @param imoNumber Valid IMO number
		 */
		explicit ShipId( const ImoNumber& imoNumber );

		/**
		 * @brief Construct ShipId from alternative identifier
		 * @param otherId Alternative ship identifier string
		 * @throws std::invalid_argument if otherId is empty
		 */
		explicit ShipId( std::string otherId );

		/** @brief Copy constructor */
		ShipId( const ShipId& ) = default;

		/** @brief Move constructor */
		ShipId( ShipId&& ) noexcept = default;

		/** @brief Destructor */
		~ShipId() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		ShipId& operator=( const ShipId& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		ShipId& operator=( ShipId&& ) noexcept = default;

		/**
		 * @brief Equality comparison operator
		 * @param other ShipId to compare with
		 * @return true if both ShipIds represent the same identifier
		 */
		[[nodiscard]] bool operator==( const ShipId& other ) const noexcept;

		/**
		 * @brief Inequality comparison operator
		 * @param other ShipId to compare with
		 * @return true if ShipIds represent different identifiers
		 */
		[[nodiscard]] bool operator!=( const ShipId& other ) const noexcept;

		/**
		 * @brief Check if ShipId contains an IMO number
		 * @return true if this ShipId holds an IMO number
		 */
		[[nodiscard]] bool isImoNumber() const noexcept;

		/**
		 * @brief Check if ShipId contains an alternative identifier
		 * @return true if this ShipId holds an alternative identifier
		 */
		[[nodiscard]] bool isOtherId() const noexcept;

		/**
		 * @brief Get IMO number if available
		 * @return Optional containing IMO number, or nullopt if ShipId holds alternative ID
		 */
		[[nodiscard]] std::optional<ImoNumber> imoNumber() const noexcept;

		/**
		 * @brief Get alternative identifier if available
		 * @return Optional containing alternative ID, or nullopt if ShipId holds IMO number
		 */
		[[nodiscard]] std::optional<std::string_view> otherId() const noexcept;

		/**
		 * @brief Pattern matching visitor for ShipId
		 * @tparam F1 Function type for IMO number case
		 * @tparam F2 Function type for other ID case
		 * @param onImoNumber Function to call if ShipId contains IMO number
		 * @param onOtherId Function to call if ShipId contains alternative ID
		 * @return Result of the matching function
		 */
		template <typename F1, typename F2>
		[[nodiscard]] auto match( F1&& onImoNumber, F2&& onOtherId ) const;

		/**
		 * @brief Visit ShipId with side effects (no return value)
		 * @tparam F1 Function type for IMO number case
		 * @tparam F2 Function type for other ID case
		 * @param onImoNumber Function to call if ShipId contains IMO number
		 * @param onOtherId Function to call if ShipId contains alternative ID
		 */
		template <typename F1, typename F2>
		void visit( F1&& onImoNumber, F2&& onOtherId ) const;

		/**
		 * @brief Convert ShipId to string representation
		 * @return String representation (IMO number with "IMO" prefix, or alternative ID)
		 */
		[[nodiscard]] std::string toString() const;

		/**
		 * @brief Create ShipId from string representation
		 * @param value String containing "IMO" prefix followed by IMO number, or alternative ID
		 * @return Optional containing ShipId if parsing succeeded, nullopt otherwise
		 * @details If the string starts with "IMO" (case-insensitive) and is followed by a valid
		 *          IMO number, creates a ShipId with ImoNumber. Otherwise, creates a ShipId with
		 *          the alternative identifier. Returns nullopt if value is empty.
		 */
		[[nodiscard]] static std::optional<ShipId> fromString( std::string_view value ) noexcept;

	private:
		std::variant<ImoNumber, std::string> m_value;
	};

	template <typename F1, typename F2>
	inline auto ShipId::match( F1&& onImoNumber, F2&& onOtherId ) const
	{
		return std::visit(
			[&]( const auto& value ) -> decltype( auto ) {
				using T = std::decay_t<decltype( value )>;
				if constexpr ( std::is_same_v<T, ImoNumber> )
				{
					return std::forward<F1>( onImoNumber )( value );
				}
				else
				{
					return std::forward<F2>( onOtherId )( value );
				}
			},
			m_value );
	}

	template <typename F1, typename F2>
	inline void ShipId::visit( F1&& onImoNumber, F2&& onOtherId ) const
	{
		std::visit(
			[&]( const auto& value ) {
				using T = std::decay_t<decltype( value )>;
				if constexpr ( std::is_same_v<T, ImoNumber> )
				{
					std::forward<F1>( onImoNumber )( value );
				}
				else
				{
					std::forward<F2>( onOtherId )( value );
				}
			},
			m_value );
	}
} // namespace dnv::vista::sdk
