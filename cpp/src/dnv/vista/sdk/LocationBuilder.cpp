/**
 * @file LocationBuilder.cpp
 * @brief Implementation of the LocationBuilder class
 */

#include "pch.h"

#include "dnv/vista/sdk/LocationBuilder.h"

#include "dnv/vista/sdk/constants/LocationsConstants.h"
#include "dnv/vista/sdk/internal/StringBuilderPool.h"

#include "dnv/vista/sdk/Exceptions.h"
#include "dnv/vista/sdk/Locations.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// LocationBuilder class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	LocationBuilder::LocationBuilder( VisVersion visVersion, const std::map<char, LocationGroup>* reversedGroups )
		: m_visVersion{ visVersion },
		  m_reversedGroups{ reversedGroups }
	{
	}

	//----------------------------------------------
	// Conversion and comparison
	//----------------------------------------------

	std::string LocationBuilder::toString() const
	{
		auto lease = internal::StringBuilderPool::instance();
		auto builder = lease.builder();

		if ( m_number.has_value() )
		{
			builder.append( std::to_string( m_number.value() ) );
		}

		if ( m_side.has_value() )
		{
			builder.push_back( m_side.value() );
		}
		if ( m_vertical.has_value() )
		{
			builder.push_back( m_vertical.value() );
		}
		if ( m_transverse.has_value() )
		{
			builder.push_back( m_transverse.value() );
		}
		if ( m_longitudinal.has_value() )
		{
			builder.push_back( m_longitudinal.value() );
		}

		std::string result = lease.toString();
		std::sort( result.begin(), result.end() );

		return result;
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	LocationBuilder LocationBuilder::create( const Locations& locations )
	{
		return LocationBuilder( locations.visVersion(), &locations.reversedGroups() );
	}

	//----------------------------------------------
	// Build methods (Immutable fluent interface)
	//----------------------------------------------

	//----------------------------
	// Build
	//----------------------------

	Location LocationBuilder::build() const
	{
		return Location( toString() );
	}

	//----------------------------
	// Location
	//----------------------------

	LocationBuilder LocationBuilder::withLocation( const Location& location ) const
	{
		LocationBuilder builder{ m_visVersion, m_reversedGroups };

		std::string locationStr = location.toString();
		std::string_view span = locationStr;
		std::optional<int> number;

		for ( size_t i = 0; i < span.length(); ++i )
		{
			char ch = span[i];

			if ( std::isdigit( ch ) )
			{
				if ( !number.has_value() )
				{
					/* Convert ASCII digit character to numeric value: '0'=48, '1'=49, ..., '9'=57
					 * Subtracting '0' (ASCII 48) gives: '0'-'0'=0, '1'-'0'=1, ..., '9'-'0'=9 */
					number = ch - '0';
				}
				else
				{
					int parsedNumber;
					if ( !Locations::tryParseInt( span, 0, static_cast<int>( i + 1 ), parsedNumber ) )
					{
						throw std::invalid_argument( "Should include a valid number" );
					}
					number = parsedNumber;
				}
				continue;
			}

			builder = builder.withValue( ch );
		}

		if ( number.has_value() )
		{
			builder = builder.withNumber( number.value() );
		}

		return builder;
	}

	//----------------------------
	// Number
	//----------------------------

	LocationBuilder LocationBuilder::withNumber( int number ) const
	{
		return withValueInternal( LocationGroup::Number, number );
	}

	LocationBuilder LocationBuilder::withoutNumber() const
	{
		return withoutValue( LocationGroup::Number );
	}

	//----------------------------
	// Side
	//----------------------------

	LocationBuilder LocationBuilder::withSide( char side ) const
	{
		return withValueInternal( LocationGroup::Side, side );
	}

	LocationBuilder LocationBuilder::withoutSide() const
	{
		return withoutValue( LocationGroup::Side );
	}

	//----------------------------
	// Vertical
	//----------------------------

	LocationBuilder LocationBuilder::withVertical( char vertical ) const
	{
		return withValueInternal( LocationGroup::Vertical, vertical );
	}

	LocationBuilder LocationBuilder::withoutVertical() const
	{
		return withoutValue( LocationGroup::Vertical );
	}

	//----------------------------
	// Transverse
	//----------------------------

	LocationBuilder LocationBuilder::withTransverse( char transverse ) const
	{
		return withValueInternal( LocationGroup::Transverse, transverse );
	}

	LocationBuilder LocationBuilder::withoutTransverse() const
	{
		return withoutValue( LocationGroup::Transverse );
	}

	//----------------------------
	// Longitudinal
	//----------------------------

	LocationBuilder LocationBuilder::withLongitudinal( char longitudinal ) const
	{
		return withValueInternal( LocationGroup::Longitudinal, longitudinal );
	}

	LocationBuilder LocationBuilder::withoutLongitudinal() const
	{
		return withoutValue( LocationGroup::Longitudinal );
	}

	//----------------------------
	// Internal implementation
	//----------------------------

	LocationBuilder LocationBuilder::withValueInternal( LocationGroup group, int value ) const
	{
		if ( group != LocationGroup::Number )
		{
			throw ValidationException( "Integer values are only valid for Number group" );
		}

		if ( value < 1 )
		{
			throw ValidationException( "Value should be greater than 0" );
		}

		LocationBuilder result = *this;
		result.m_number = value;

		return result;
	}

	LocationBuilder LocationBuilder::withValueInternal( LocationGroup group, char value ) const
	{
		if ( group == LocationGroup::Number )
		{
			throw std::invalid_argument( "Character values are not valid for Number group" );
		}

		const auto it = m_reversedGroups->find( value );
		if ( it == m_reversedGroups->end() || it->second != group )
		{
			const std::string_view groupName = [group]() {
				switch ( group )
				{
					case LocationGroup::Side:
					{
						return constants::locations::GROUP_NAME_SIDE;
					}
					case LocationGroup::Vertical:
					{
						return constants::locations::GROUP_NAME_VERTICAL;
					}
					case LocationGroup::Transverse:
					{
						return constants::locations::GROUP_NAME_TRANSVERSE;
					}
					case LocationGroup::Longitudinal:
					{
						return constants::locations::GROUP_NAME_LONGITUDINAL;
					}
					case LocationGroup::Number:
					{
						return constants::locations::GROUP_NAME_NUMBER;
					}
					default:
					{
						return constants::locations::GROUP_NAME_UNKNOWN;
					}
				}
			}();

			throw ValidationException(
				fmt::format( "The value '{}' is an invalid {} value",
					value,
					groupName ) );
		}

		LocationBuilder result = *this;
		switch ( group )
		{
			case LocationGroup::Side:
			{
				result.m_side = value;
				break;
			}
			case LocationGroup::Vertical:
			{
				result.m_vertical = value;
				break;
			}
			case LocationGroup::Transverse:
			{
				result.m_transverse = value;
				break;
			}
			case LocationGroup::Longitudinal:
			{
				result.m_longitudinal = value;
				break;
			}
			case LocationGroup::Number:
			{
				throw std::invalid_argument( "Number group should not contain character values" );
			}
			default:
			{
				throw std::invalid_argument( "Unsupported LocationGroup" );
			}
		}
		return result;
	}

	//----------------------------
	// Value
	//----------------------------

	LocationBuilder LocationBuilder::withValue( int value ) const
	{
		return withValueInternal( LocationGroup::Number, value );
	}

	LocationBuilder LocationBuilder::withValue( char value ) const
	{
		const auto it = m_reversedGroups->find( value );
		if ( it == m_reversedGroups->end() )
		{
			throw ValidationException( fmt::format( "The value '{}' is an invalid Locations value", value ) );
		}

		return withValueInternal( it->second, value );
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
				throw std::invalid_argument( "Unsupported LocationGroup" );
			}
		}

		return result;
	}
}
