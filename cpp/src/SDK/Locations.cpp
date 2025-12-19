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
 * @file Locations.cpp
 * @brief Implementation of VIS location parsing and validation
 * @details Provides implementations for Location, RelativeLocation, and Locations classes
 *          for parsing and validating VIS location strings.
 */

#include "dnv/vista/sdk/Locations.h"

#include "dnv/vista/sdk/ParsingErrors.h"
#include "dto/LocationsDto.h"
#include "parsing/LocationParsingErrorBuilder.h"

#include <cctype>
#include <stdexcept>

namespace dnv::vista::sdk
{
	Locations::Locations( VisVersion version, const LocationsDto& dto )
		: m_visVersion{ version }
	{
		m_relativeLocations.reserve( dto.items.size() );
		m_groups.reserve( 5 );

		std::unordered_map<LocationGroup, std::vector<RelativeLocation>> tempGroups;

		for ( const auto& item : dto.items )
		{
			// Create Location from single character
			std::string locStr( 1, item.code );
			Location location( locStr );

			// Create RelativeLocation
			RelativeLocation relLoc(
				item.code,
				item.name,
				location,
				item.definition );

			m_relativeLocations.push_back( std::move( relLoc ) );
			m_locationCodes.insert( item.code );

			// Skip H and V for grouping
			if ( item.code == 'H' || item.code == 'V' )
				continue;

			// Map code to group
			LocationGroup group;
			if ( item.code == 'N' )
			{
				group = LocationGroup::Number;
			}
			else if ( item.code == 'P' || item.code == 'C' || item.code == 'S' )
			{
				group = LocationGroup::Side;
			}
			else if ( item.code == 'U' || item.code == 'M' || item.code == 'L' )
			{
				group = LocationGroup::Vertical;
			}
			else if ( item.code == 'I' || item.code == 'O' )
			{
				group = LocationGroup::Transverse;
			}
			else if ( item.code == 'F' || item.code == 'A' )
			{
				group = LocationGroup::Longitudinal;
			}
			else
			{
				throw std::runtime_error( "Unsupported location code" );
			}

			// Skip N for reversedGroups and group values
			if ( group == LocationGroup::Number )
			{
				continue;
			}
			m_reversedGroups[item.code] = group;
			tempGroups[group].push_back( m_relativeLocations.back() );
		}

		m_groups = std::move( tempGroups );
	}

	std::optional<Location> Locations::fromString( std::string_view locationStr ) const noexcept
	{
		if ( locationStr.empty() )
		{
			return std::nullopt;
		}

		bool allWhitespace = true;
		for ( char ch : locationStr )
		{
			if ( !std::isspace( static_cast<unsigned char>( ch ) ) )
			{
				allWhitespace = false;
				break;
			}
		}

		if ( allWhitespace )
		{
			return std::nullopt;
		}

		std::optional<std::size_t> prevDigitIndex;
		std::optional<std::size_t> digitStartIndex;
		std::optional<std::size_t> charsStartIndex;

		// Track which groups have been seen (4 groups: Side, Vertical, Transverse, Longitudinal)
		std::array<std::optional<char>, 4> seenGroups{};

		for ( std::size_t i = 0; i < locationStr.size(); ++i )
		{
			const char ch = locationStr[i];

			if ( std::isdigit( static_cast<unsigned char>( ch ) ) )
			{
				if ( !digitStartIndex.has_value() && i != 0 )
				{
					return std::nullopt;
				}

				if ( prevDigitIndex.has_value() && prevDigitIndex.value() != ( i - 1 ) )
				{
					return std::nullopt;
				}

				if ( !digitStartIndex.has_value() )
				{
					digitStartIndex = i;
				}

				prevDigitIndex = i;
			}
			else
			{
				// Check if code exists in reversedGroups
				auto groupIt = m_reversedGroups.find( ch );
				if ( groupIt == m_reversedGroups.end() )
				{
					if ( m_locationCodes.find( ch ) == m_locationCodes.end() )
					{
						return std::nullopt;
					}

					if ( !charsStartIndex.has_value() )
					{
						charsStartIndex = i;
					}
					continue;
				}

				const LocationGroup group = groupIt->second;

				const std::size_t groupIndex = static_cast<std::size_t>( group ) - 1;
				if ( seenGroups[groupIndex].has_value() )
				{
					return std::nullopt;
				}

				seenGroups[groupIndex] = ch;

				// Check alphabetical order
				if ( charsStartIndex.has_value() && i > 0 )
				{
					const char prevCh = locationStr[i - 1];
					if ( !std::isdigit( static_cast<unsigned char>( prevCh ) ) && ch < prevCh )
					{
						return std::nullopt;
					}
				}

				if ( !charsStartIndex.has_value() )
				{
					charsStartIndex = i;
				}
			}
		}

		return Location( locationStr );
	}

	std::optional<Location> Locations::fromString( std::string_view locationStr, ParsingErrors& errors ) const noexcept
	{
		using namespace internal;

		LocationParsingErrorBuilder errorBuilder;

		if ( locationStr.empty() )
		{
			errorBuilder.addError(
				LocationValidationResult::NullOrWhiteSpace,
				"Invalid location: contains only whitespace" );
			errors = errorBuilder.build();
			return std::nullopt;
		}

		bool allWhitespace = true;
		for ( char ch : locationStr )
		{
			if ( !std::isspace( static_cast<unsigned char>( ch ) ) )
			{
				allWhitespace = false;
				break;
			}
		}

		if ( allWhitespace )
		{
			errorBuilder.addError(
				LocationValidationResult::NullOrWhiteSpace,
				"Invalid location: contains only whitespace" );
			errors = errorBuilder.build();
			return std::nullopt;
		}

		std::optional<std::size_t> prevDigitIndex;
		std::optional<std::size_t> digitStartIndex;
		std::optional<std::size_t> charsStartIndex;

		// Track which groups have been seen (4 groups: Side, Vertical, Transverse, Longitudinal)
		std::array<std::optional<char>, 4> seenGroups{};

		for ( std::size_t i = 0; i < locationStr.size(); ++i )
		{
			const char ch = locationStr[i];

			if ( std::isdigit( static_cast<unsigned char>( ch ) ) )
			{
				if ( !digitStartIndex.has_value() && i != 0 )
				{
					std::string errorMsg = "Invalid location: numeric location should start before location code(s) in location: '" +
										   std::string( locationStr ) + "'";
					errorBuilder.addError( LocationValidationResult::Invalid, errorMsg );
					errors = errorBuilder.build();
					return std::nullopt;
				}

				if ( prevDigitIndex.has_value() && prevDigitIndex.value() != ( i - 1 ) )
				{
					std::string errorMsg = "Invalid location: cannot have multiple separated digits in location: '" +
										   std::string( locationStr ) + "'";
					errorBuilder.addError( LocationValidationResult::Invalid, errorMsg );
					errors = errorBuilder.build();
					return std::nullopt;
				}

				if ( !digitStartIndex.has_value() )
				{
					digitStartIndex = i;
				}

				prevDigitIndex = i;
			}
			else
			{
				// Check if code exists in reversedGroups
				auto groupIt = m_reversedGroups.find( ch );
				if ( groupIt == m_reversedGroups.end() )
				{
					if ( m_locationCodes.find( ch ) == m_locationCodes.end() )
					{
						// Collect all invalid characters
						std::string invalidChars;
						for ( std::size_t j = 0; j < locationStr.size(); ++j )
						{
							const char c = locationStr[j];
							if ( !std::isdigit( static_cast<unsigned char>( c ) ) &&
								 ( c == 'N' || m_locationCodes.find( c ) == m_locationCodes.end() ) )
							{
								if ( !invalidChars.empty() )
								{
									invalidChars += ",";
								}
								invalidChars += "'";
								invalidChars += c;
								invalidChars += "'";
							}
						}

						std::string errorMsg = "Invalid location code: '" + std::string( locationStr ) +
											   "' with invalid location code(s): " + invalidChars;
						errorBuilder.addError( LocationValidationResult::InvalidCode, errorMsg );
						errors = errorBuilder.build();
						return std::nullopt;
					}
					if ( !charsStartIndex.has_value() )
					{
						charsStartIndex = i;
					}
					continue;
				}

				const LocationGroup group = groupIt->second;

				const std::size_t groupIndex = static_cast<std::size_t>( group ) - 1;
				if ( seenGroups[groupIndex].has_value() )
				{
					const char* groupName = "";
					switch ( group )
					{
						case LocationGroup::Side:
							groupName = "Side";
							break;
						case LocationGroup::Vertical:
							groupName = "Vertical";
							break;
						case LocationGroup::Transverse:
							groupName = "Transverse";
							break;
						case LocationGroup::Longitudinal:
							groupName = "Longitudinal";
							break;
						default:
							break;
					}
					std::string errorMsg = "Invalid location: Multiple '" + std::string( groupName ) + "' values. Got both '" +
										   std::string( 1, seenGroups[groupIndex].value() ) + "' and '" + std::string( 1, ch ) +
										   "' in '" + std::string( locationStr ) + "'";
					errorBuilder.addError( LocationValidationResult::Invalid, errorMsg );
					errors = errorBuilder.build();
					return std::nullopt;
				}

				seenGroups[groupIndex] = ch;

				// Check alphabetical order
				if ( charsStartIndex.has_value() && i > 0 )
				{
					const char prevCh = locationStr[i - 1];
					if ( !std::isdigit( static_cast<unsigned char>( prevCh ) ) && ch < prevCh )
					{
						std::string errorMsg = "Invalid location: '" + std::string( locationStr ) + "' not alphabetically sorted";
						errorBuilder.addError( LocationValidationResult::InvalidOrder, errorMsg );
						errors = errorBuilder.build();
						return std::nullopt;
					}
				}

				if ( !charsStartIndex.has_value() )
				{
					charsStartIndex = i;
				}
			}
		}

		errors = errorBuilder.build();
		return Location{ locationStr };
	}
} // namespace dnv::vista::sdk
