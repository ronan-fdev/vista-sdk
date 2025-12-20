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
 * @file ImoNumber.h
 * @brief IMO number representation and validation
 * @details Represents an IMO (International Maritime Organization) number with validation.
 *          IMO numbers are unique identifiers for ships, consisting of the prefix "IMO"
 *          followed by a seven-digit number with checksum validation.
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace dnv::vista::sdk
{
	/**
	 * @brief Represents an International Maritime Organization (IMO) number.
	 *
	 * An IMO number is a unique seven-digit identifier assigned to maritime vessels,
	 * featuring a check digit for validation according to the IMO standard.
	 * This class provides functionality to create, validate, parse, and represent IMO numbers.
	 * Instances of this class are immutable once constructed.
	 */
	class ImoNumber final
	{
	public:
		/** @brief Default constructor */
		ImoNumber() = delete;

		/**
		 * @brief Construct IMO number from integer with validation
		 * @param value Seven-digit IMO number (e.g., 9074729)
		 * @throws std::invalid_argument if value fails checksum validation
		 */
		explicit ImoNumber( int value );

		/**
		 * @brief Construct IMO number from string with validation
		 * @param value IMO number string with or without "IMO" prefix (e.g., "IMO9074729" or "9074729")
		 * @throws std::invalid_argument if string format is invalid or checksum fails
		 */
		explicit ImoNumber( std::string_view value );

		/** @brief Copy constructor */
		ImoNumber( const ImoNumber& ) = default;

		/** @brief Move constructor */
		ImoNumber( ImoNumber&& ) noexcept = default;

		/** @brief Destructor */
		~ImoNumber() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		ImoNumber& operator=( const ImoNumber& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		ImoNumber& operator=( ImoNumber&& ) noexcept = default;

		/**
		 * @brief Equality comparison
		 * @param other ImoNumber to compare with
		 * @return True if both represent same IMO number
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator==( const ImoNumber& other ) const noexcept;

		/**
		 * @brief Inequality comparison
		 * @param other ImoNumber to compare with
		 * @return True if IMO numbers differ
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator!=( const ImoNumber& other ) const noexcept;

		/**
		 * @brief Checks if an integer value represents a valid IMO number.
		 *
		 * This method validates the 7-digit structure and the checksum.
		 * @param imoNumber The integer value to check (e.g., 9074729).
		 * @return True if the integer is a valid IMO number, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool isValid( int imoNumber ) noexcept;

		/**
		 * @brief Parse IMO number from string and return optional
		 * @param value String to parse (e.g., "IMO9074729" or "9074729")
		 * @return Optional ImoNumber if valid, nullopt otherwise
		 * @details Accepts strings with or without "IMO" prefix and validates checksum.
		 *          Convenience wrapper around the reference version of fromString().
		 */
		[[nodiscard]] static std::optional<ImoNumber> fromString( std::string_view value ) noexcept;

		/**
		 * @brief Convert to string with "IMO" prefix
		 * @return String in format "IMO" followed by 7 digits (e.g., "IMO9074729")
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

	private:
		int m_value; ///< The IMO number value (7-digit integer)
	};
} // namespace dnv::vista::sdk

#include "detail/ImoNumber.inl"
