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
 * @file Locations.h
 * @brief VIS location representation with validation and parsing
 * @details Provides Location, RelativeLocation, and Locations classes for parsing and validating
 *          VIS location strings. Locations are used to identify physical positions on vessels
 *          with numeric prefixes and alphabetically sorted location codes.
 */

#pragma once

#include "VisVersions.h"

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace dnv::vista::sdk
{
	struct LocationsDto;
	class ParsingErrors;

	/**
	 * @brief Classification of VIS location codes into functional groups.
	 * @details Groups location codes by their semantic meaning (Number, Side, Vertical,
	 *          Transverse, Longitudinal). Used for organizing and validating location strings.
	 */
	enum class LocationGroup : std::uint8_t
	{
		Number = 0,	 ///< Numeric location prefix group
		Side,		 ///< Port/Center/Starboard group
		Vertical,	 ///< Upper/Middle/Lower group
		Transverse,	 ///< Inboard/Outboard group
		Longitudinal ///< Forward/Aft group
	};

	/**
	 * @brief Represents a VIS location value.
	 * @details Lightweight value type that owns the canonical string representation of a VIS
	 *          location (for example "1PS"). Instances are constructed by `Locations` after
	 *          validation and are intended to be used as an immutable value object.
	 */
	class Location final
	{
		friend class Locations;

	private:
		/**
		 * @brief Constructs a Location from a validated string.
		 * @param value Validated location string
		 */
		inline explicit Location( std::string_view value ) noexcept;

	public:
		/** @brief Default constructor */
		Location() = delete;

		/** @brief Copy constructor */
		Location( const Location& ) noexcept = default;

		/** @brief Move constructor */
		Location( Location&& ) noexcept = default;

		/** @brief Destructor */
		~Location() = default;

		/**
		 * @brief Copy assignment operator
		 * @param other Location to copy from
		 * @return Reference to this Location
		 */
		Location& operator=( const Location& other ) noexcept = default;

		/**
		 * @brief Move assignment operator
		 * @param other Location to move from
		 * @return Reference to this Location
		 */
		Location& operator=( Location&& other ) noexcept = default;

		/**
		 * @brief Equality comparison operator
		 * @param other Location to compare with
		 * @return true if location values are equal
		 */
		[[nodiscard]] inline bool operator==( const Location& other ) const noexcept;

		/**
		 * @brief Inequality comparison operator
		 * @param other Location to compare with
		 * @return true if location values are not equal
		 */
		[[nodiscard]] inline bool operator!=( const Location& other ) const noexcept;

		/**
		 * @brief Get the location string value
		 * @return Const reference to the location string
		 */
		[[nodiscard]] inline const std::string& value() const noexcept;

	private:
		std::string m_value; ///< Canonical location string representation
	};

	/**
	 * @brief Metadata for a relative location code.
	 * @details Associates a single-character location code with a human readable name,
	 *          its canonical `Location` representation and an optional definition.
	 */
	class RelativeLocation final
	{
		friend class Locations;

	private:
		/**
		 * @brief Constructs a RelativeLocation
		 * @param code Single character location code
		 * @param name Human-readable name
		 * @param location Canonical Location representation
		 * @param definition Optional definition text
		 */
		inline explicit RelativeLocation(
			char code,
			std::string_view name,
			const Location& location,
			std::optional<std::string> definition = std::nullopt ) noexcept;

	public:
		/** @brief Default constructor */
		RelativeLocation() = delete;

		/** @brief Copy constructor */
		RelativeLocation( const RelativeLocation& ) = default;

		/** @brief Move constructor */
		RelativeLocation( RelativeLocation&& ) noexcept = default;

		/** @brief Destructor */
		~RelativeLocation() = default;

		/**
		 * @brief Copy assignment operator
		 * @param other RelativeLocation to copy from
		 * @return Reference to this RelativeLocation
		 */
		RelativeLocation& operator=( const RelativeLocation& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other RelativeLocation to move from
		 * @return Reference to this RelativeLocation
		 */
		RelativeLocation& operator=( RelativeLocation&& other ) noexcept = default;

		/**
		 * @brief Equality comparison based on code
		 * @param other RelativeLocation to compare with
		 * @return true if codes are equal
		 */
		[[nodiscard]] inline bool operator==( const RelativeLocation& other ) const noexcept;

		/**
		 * @brief Inequality comparison based on code
		 * @param other RelativeLocation to compare with
		 * @return true if codes are not equal
		 */
		[[nodiscard]] inline bool operator!=( const RelativeLocation& other ) const noexcept;

		/**
		 * @brief Get the single-character location code
		 * @return Location code character
		 */
		[[nodiscard]] inline char code() const noexcept;

		/**
		 * @brief Get the human-readable name
		 * @return Const reference to name string
		 */
		[[nodiscard]] inline const std::string& name() const noexcept;

		/**
		 * @brief Get the optional definition
		 * @return Const reference to optional definition string
		 */
		[[nodiscard]] inline const std::optional<std::string>& definition() const noexcept;

		/**
		 * @brief Get the canonical Location representation
		 * @return Const reference to Location
		 */
		[[nodiscard]] inline const Location& location() const noexcept;

	private:
		char m_code;							 ///< Single-character location code
		std::string m_name;						 ///< Human-readable name
		Location m_location;					 ///< Canonical Location representation
		std::optional<std::string> m_definition; ///< Optional definition text
	};

	/**
	 * @brief Collection and parser for VIS locations.
	 * @details Constructs a set of known `RelativeLocation` items for a given VIS version,
	 *          provides grouping and parsing utilities (see `fromString`).
	 */
	class Locations final
	{
		friend class VIS;

	private:
		/**
		 * @brief Constructs Locations from DTO
		 * @param version VIS version
		 * @param dto LocationsDto containing location definitions
		 */
		explicit Locations( VisVersion version, const LocationsDto& dto );

	public:
		/** @brief Default constructor */
		Locations() = delete;

		/** @brief Copy constructor */
		Locations( const Locations& ) = default;

		/** @brief Move constructor */
		Locations( Locations&& ) noexcept = default;

		/** @brief Destructor */
		~Locations() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this Locations
		 */
		Locations& operator=( const Locations& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this Locations
		 */
		Locations& operator=( Locations&& ) noexcept = default;

		/**
		 * @brief Get the VIS version
		 * @return VIS version enum value
		 */
		inline VisVersion version() const noexcept;

		/**
		 * @brief Get all relative locations
		 * @return Const reference to vector of RelativeLocation items
		 */
		inline const std::vector<RelativeLocation>& relativeLocations() const noexcept;

		/**
		 * @brief Get location groups
		 * @return Const reference to map of LocationGroup to RelativeLocation vectors
		 */
		inline const std::unordered_map<LocationGroup, std::vector<RelativeLocation>>& groups() const noexcept;

		/**
		 * @brief Parse location string without error details
		 * @param locationStr Location string to parse
		 * @return Optional Location if valid, nullopt otherwise
		 */
		std::optional<Location> fromString( std::string_view locationStr ) const noexcept;

		/**
		 * @brief Parse location string with error details
		 * @param locationStr Location string to parse
		 * @param errors Output ParsingErrors containing detailed error information
		 * @return Optional Location if valid, nullopt otherwise
		 */
		std::optional<Location> fromString( std::string_view locationStr, ParsingErrors& errors ) const noexcept;

	private:
		std::unordered_map<LocationGroup, std::vector<RelativeLocation>> m_groups; ///< Location groups map
		std::unordered_map<char, LocationGroup> m_reversedGroups;				   ///< Code to group mapping
		std::unordered_set<char> m_locationCodes;								   ///< All valid location codes
		std::vector<RelativeLocation> m_relativeLocations;						   ///< All relative locations
		VisVersion m_visVersion;												   ///< VIS version
	};
} // namespace dnv::vista::sdk

#include "detail/Locations.inl"
