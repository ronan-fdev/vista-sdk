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
 * @file LocationBuilder.cpp
 * @brief Implementation of VIS location parsing and validation
 * @details Provides implementations for Location, RelativeLocation, and Locations classes
 *          for parsing and validating VIS location strings.
 */

#include "dnv/vista/sdk/LocationBuilder.h"

#include <nfx/string/StringBuilder.h>
#include <nfx/string/Utils.h>

namespace dnv::vista::sdk
{
	LocationBuilder::LocationBuilder( VisVersion visVersion, const std::unordered_map<char, LocationGroup>* reversedGroups )
		: m_visVersion{ visVersion },
		  m_reversedGroups{ reversedGroups }
	{
	}

	LocationBuilder LocationBuilder::create( const Locations& locations )
	{
		return LocationBuilder{ locations.version(), &locations.m_reversedGroups };
	}

	LocationBuilder LocationBuilder::withLocation( const Location& location ) const
	{
		LocationBuilder builder{ *this };

		std::string locationStr = location.value();
		std::string_view span = locationStr;
		std::optional<int> number;

		for ( size_t i = 0; i < span.length(); ++i )
		{
			char ch = span[i];

			if ( std::isdigit( ch ) )
			{
				if ( !number.has_value() )
				{
					// Convert ASCII digit character to numeric value: '0'=48, '1'=49, ..., '9'=57
					// Subtracting '0' (ASCII 48) gives: '0'-'0'=0, '1'-'0'=1, ..., '9'-'0'=9
					number = ch - '0';
				}
				else
				{
					// Extract substring from start to current position (i+1)
					std::string_view numberSubstr = span.substr( 0, i + 1 );
					int parsedNumber;
					if ( !nfx::string::fromString<int>( numberSubstr, parsedNumber ) )
					{
						throw std::invalid_argument{ "Should include a valid number" };
					}
					number = parsedNumber;
				}
				continue;
			}

			builder = builder.withCode( ch );
		}

		if ( number.has_value() )
		{
			builder = builder.withNumber( number.value() );
		}

		return builder;
	}

	LocationBuilder LocationBuilder::withLocation( const std::optional<Location>& location ) const
	{
		if ( !location.has_value() )
		{
			throw std::invalid_argument{ "Location is empty" };
		}
		return withLocation( location.value() );
	}

	LocationBuilder LocationBuilder::withNumber( int number ) const
	{
		if ( number < 1 )
		{
			throw std::invalid_argument{ "Value should be greater than 0" };
		}

		LocationBuilder result = *this;
		result.m_number = number;
		return result;
	}

	LocationBuilder LocationBuilder::withCode( char code ) const
	{
		// Find which group this character belongs to
		auto it = m_reversedGroups->find( code );
		if ( it == m_reversedGroups->end() )
		{
			throw std::invalid_argument{ std::string( "The value " ) + code + " is an invalid Locations value" };
		}

		// Delegate to the appropriate with* method based on the group
		LocationGroup group = it->second;
		switch ( group )
		{
			case LocationGroup::Side:
				return withSide( code );
			case LocationGroup::Vertical:
				return withVertical( code );
			case LocationGroup::Transverse:
				return withTransverse( code );
			case LocationGroup::Longitudinal:
				return withLongitudinal( code );
			default:
				throw std::invalid_argument{ "Unsupported LocationGroup" };
		}
	}

	LocationBuilder LocationBuilder::withSide( char side ) const
	{
		// Validate that the character is valid for Side group
		auto it = m_reversedGroups->find( side );
		if ( it == m_reversedGroups->end() || it->second != LocationGroup::Side )
		{
			throw std::invalid_argument{ std::string( "The value " ) + side + " is an invalid Side value" };
		}

		LocationBuilder result = *this;
		result.m_side = side;
		return result;
	}

	LocationBuilder LocationBuilder::withVertical( char vertical ) const
	{
		// Validate that the character is valid for Vertical group
		auto it = m_reversedGroups->find( vertical );
		if ( it == m_reversedGroups->end() || it->second != LocationGroup::Vertical )
		{
			throw std::invalid_argument{ std::string( "The value " ) + vertical + " is an invalid Vertical value" };
		}

		LocationBuilder result = *this;
		result.m_vertical = vertical;
		return result;
	}

	LocationBuilder LocationBuilder::withTransverse( char transverse ) const
	{
		// Validate that the character is valid for Transverse group
		auto it = m_reversedGroups->find( transverse );
		if ( it == m_reversedGroups->end() || it->second != LocationGroup::Transverse )
		{
			throw std::invalid_argument{ std::string( "The value " ) + transverse + " is an invalid Transverse value" };
		}

		LocationBuilder result = *this;
		result.m_transverse = transverse;
		return result;
	}

	LocationBuilder LocationBuilder::withLongitudinal( char longitudinal ) const
	{
		// Validate that the character is valid for Longitudinal group
		auto it = m_reversedGroups->find( longitudinal );
		if ( it == m_reversedGroups->end() || it->second != LocationGroup::Longitudinal )
		{
			throw std::invalid_argument{ std::string( "The value " ) + longitudinal + " is an invalid Longitudinal value" };
		}

		LocationBuilder result = *this;
		result.m_longitudinal = longitudinal;
		return result;
	}

	LocationBuilder LocationBuilder::withoutValue( LocationGroup group ) const
	{
		LocationBuilder result = *this;

		switch ( group )
		{
			case LocationGroup::Number:
			{
				result.m_number = std::nullopt;
				break;
			}
			case LocationGroup::Side:
			{
				result.m_side = std::nullopt;
				break;
			}
			case LocationGroup::Vertical:
			{
				result.m_vertical = std::nullopt;
				break;
			}
			case LocationGroup::Transverse:
			{
				result.m_transverse = std::nullopt;
				break;
			}
			case LocationGroup::Longitudinal:
			{
				result.m_longitudinal = std::nullopt;
				break;
			}
			default:
			{
				throw std::invalid_argument{ "Unsupported LocationGroup" };
			}
		}

		return result;
	}

	Location LocationBuilder::build() const
	{
		return Location{ toString() };
	}

	std::string LocationBuilder::toString() const
	{
		auto sb = nfx::string::StringBuilder();

		if ( m_side.has_value() )
		{
			sb.append( m_side.value() );
		}
		if ( m_vertical.has_value() )
		{
			sb.append( m_vertical.value() );
		}
		if ( m_transverse.has_value() )
		{
			sb.append( m_transverse.value() );
		}
		if ( m_longitudinal.has_value() )
		{
			sb.append( m_longitudinal.value() );
		}

		// Sort alphabetically
		std::sort( sb.begin(), sb.end() );

		if ( m_number.has_value() )
		{
			sb.prepend( m_number.value() );
		}

		return sb.toString();
	}
} // namespace dnv::vista::sdk
