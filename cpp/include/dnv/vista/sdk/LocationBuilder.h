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
 * @file LocationBuilder.h
 * @brief Builder pattern for constructing VIS Location instances
 * @details Provides a fluent API for building Location objects with validation.
 *          Supports setting individual location components (number, side, vertical, transverse, longitudinal)
 *          and validates values against the VIS version's location specification.
 */

#pragma once

#include "Locations.h"

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

namespace dnv::vista::sdk
{
	enum class VisVersion : std::uint8_t;

	/**
	 * @class LocationBuilder
	 * @brief Builder for constructing VIS Location instances with fluent API
	 * @details Provides immutable builder pattern for creating Location objects with validation.
	 *          Each with* method returns a new builder instance with the modified value,
	 *          preserving immutability. Values are validated against the VIS version specification.
	 *          Use build() to construct the final Location object from the builder state.
	 */
	class LocationBuilder final
	{
		friend class Locations;

	private:
		explicit LocationBuilder( VisVersion visVersion, const std::unordered_map<char, LocationGroup>* reversedGroups );

	public:
		/** @brief Default constructor */
		LocationBuilder() = delete;

		/** @brief Copy constructor */
		LocationBuilder( const LocationBuilder& ) = default;

		/** @brief Move constructor */
		LocationBuilder( LocationBuilder&& ) noexcept = default;

		/** @brief Destructor */
		~LocationBuilder() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		LocationBuilder& operator=( const LocationBuilder& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		LocationBuilder& operator=( LocationBuilder&& ) noexcept = default;

		/**
		 * @brief Create a new LocationBuilder for the given Locations instance
		 * @param locations Locations instance to build for
		 * @return New LocationBuilder instance
		 */
		[[nodiscard]] static LocationBuilder create( const Locations& locations );

		/**
		 * @brief Get the VIS version for this builder
		 * @return The VisVersion enum value
		 */
		[[nodiscard]] inline VisVersion version() const noexcept;

		/**
		 * @brief Get the location number component
		 * @return Optional containing the number if set, nullopt otherwise
		 */
		[[nodiscard]] inline std::optional<int> number() const noexcept;

		/**
		 * @brief Get the side location component
		 * @return Optional containing the side character if set, nullopt otherwise
		 */
		[[nodiscard]] inline std::optional<char> side() const noexcept;

		/**
		 * @brief Get the vertical location component
		 * @return Optional containing the vertical character if set, nullopt otherwise
		 */
		[[nodiscard]] inline std::optional<char> vertical() const noexcept;

		/**
		 * @brief Get the transverse location component
		 * @return Optional containing the transverse character if set, nullopt otherwise
		 */
		[[nodiscard]] inline std::optional<char> transverse() const noexcept;

		/**
		 * @brief Get the longitudinal location component
		 * @return Optional containing the longitudinal character if set, nullopt otherwise
		 */
		[[nodiscard]] inline std::optional<char> longitudinal() const noexcept;

		/**
		 * @brief Parse and set location components from existing Location
		 * @param location Location to parse and extract components from
		 * @return New LocationBuilder with components from the parsed location
		 */
		[[nodiscard]] LocationBuilder withLocation( const Location& location ) const;

		/**
		 * @brief Parse and set location components from optional Location
		 * @param location Optional location to parse and extract components from
		 * @return New LocationBuilder with components from the parsed location
		 * @throws std::invalid_argument if location is std::nullopt
		 */
		[[nodiscard]] LocationBuilder withLocation( const std::optional<Location>& location ) const;

		/**
		 * @brief Set the number component
		 * @param number Location number (must be > 0)
		 * @return New LocationBuilder with the number component set
		 * @throws std::invalid_argument if number is less than 1
		 */
		[[nodiscard]] LocationBuilder withNumber( int number ) const;

		/**
		 * @brief Remove the number component
		 * @return New LocationBuilder with the number component cleared
		 */
		[[nodiscard]] inline LocationBuilder withoutNumber() const;

		/**
		 * @brief Set location component by character code
		 * @param code Character code (e.g., 'S' for Side, 'U' for Upper)
		 * @return New LocationBuilder with the appropriate component set
		 * @throws std::invalid_argument if character is not valid for any location group
		 * @details Automatically determines which location group the character belongs to
		 *          and calls the appropriate with* method (withSide, withVertical, etc.)
		 */
		[[nodiscard]] LocationBuilder withCode( char code ) const;

		/**
		 * @brief Set the side component
		 * @param side Side character (must be valid for VIS version)
		 * @return New LocationBuilder with the side component set
		 * @throws std::invalid_argument if character is invalid for side group
		 */
		[[nodiscard]] LocationBuilder withSide( char side ) const;

		/**
		 * @brief Remove the side component
		 * @return New LocationBuilder with the side component cleared
		 */
		[[nodiscard]] inline LocationBuilder withoutSide() const;

		/**
		 * @brief Set the vertical component
		 * @param vertical Vertical character (must be valid for VIS version)
		 * @return New LocationBuilder with the vertical component set
		 * @throws std::invalid_argument if character is invalid for vertical group
		 */
		[[nodiscard]] LocationBuilder withVertical( char vertical ) const;

		/**
		 * @brief Remove the vertical component
		 * @return New LocationBuilder with the vertical component cleared
		 */
		[[nodiscard]] inline LocationBuilder withoutVertical() const;

		/**
		 * @brief Set the transverse component
		 * @param transverse Transverse character (must be valid for VIS version)
		 * @return New LocationBuilder with the transverse component set
		 * @throws std::invalid_argument if character is invalid for transverse group
		 */
		[[nodiscard]] LocationBuilder withTransverse( char transverse ) const;

		/**
		 * @brief Remove the transverse component
		 * @return New LocationBuilder with the transverse component cleared
		 */
		[[nodiscard]] inline LocationBuilder withoutTransverse() const;

		/**
		 * @brief Set the longitudinal component
		 * @param longitudinal Longitudinal character (must be valid for VIS version)
		 * @return New LocationBuilder with the longitudinal component set
		 * @throws std::invalid_argument if character is invalid for longitudinal group
		 */
		[[nodiscard]] LocationBuilder withLongitudinal( char longitudinal ) const;

		/**
		 * @brief Remove the longitudinal component
		 * @return New LocationBuilder with the longitudinal component cleared
		 */
		[[nodiscard]] inline LocationBuilder withoutLongitudinal() const;

		/**
		 * @brief Remove location component by group
		 * @param group The location group component to remove
		 * @return New LocationBuilder with the specified component cleared
		 */
		[[nodiscard]] LocationBuilder withoutValue( LocationGroup group ) const;

		/**
		 * @brief Build the final Location object from builder state
		 * @return Location instance constructed from current builder state
		 */
		[[nodiscard]] Location build() const;

		/**
		 * @brief Convert builder state to string representation
		 * @return String representation of the location with components in sorted order
		 */
		[[nodiscard]] std::string toString() const;

	private:
		std::optional<int> m_number;									 ///< Optional number component
		std::optional<char> m_side;										 ///< Optional side component
		std::optional<char> m_vertical;									 ///< Optional vertical component
		std::optional<char> m_transverse;								 ///< Optional transverse component
		std::optional<char> m_longitudinal;								 ///< Optional longitudinal component
		VisVersion m_visVersion;										 ///< VIS version for validation
		const std::unordered_map<char, LocationGroup>* m_reversedGroups; ///< Non-owning pointer to reversed groups map
	};
} // namespace dnv::vista::sdk

#include "detail/LocationBuilder.inl"
