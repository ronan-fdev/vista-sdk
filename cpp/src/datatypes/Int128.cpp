/**
 * @file Int128.cpp
 * @brief Implementation of cross-platform 128-bit integer arithmetic
 * @details Provides exact 128-bit integer arithmetic with portable operations
 */

#include "dnv/vista/sdk/datatypes/Int128.h"

namespace dnv::vista::sdk::datatypes
{
	//=====================================================================
	// Int128 class
	//=====================================================================

	//----------------------------------------------
	// String parsing and conversion
	//----------------------------------------------

	Int128 Int128::parse( std::string_view str )
	{
		Int128 result;
		if ( !tryParse( str, result ) )
		{
			throw std::invalid_argument( "Invalid Int128 string format" );
		}

		return result;
	}

	bool Int128::tryParse( std::string_view str, Int128& result ) noexcept
	{
		try
		{
			if ( str.empty() )
			{
				return false;
			}

			/* Handle sign */
			bool isNegative = false;
			size_t pos = 0;

			if ( str[0] == '-' )
			{
				isNegative = true;
				pos = 1;
			}
			else if ( str[0] == '+' )
			{
				pos = 1;
			}

			/* Check if we have digits after sign */
			if ( pos >= str.length() )
			{
				return false;
			}

			/* Parse digits and build the number */
			result = Int128{ 0 };
			Int128 ten{ 10 };

			for ( size_t i = pos; i < str.length(); ++i )
			{
				char c = str[i];
				if ( c < '0' || c > '9' )
				{
					return false; /* Invalid character */
				}

				int digit = c - '0';

				/* Check for overflow before multiplication and addition */
				/* For 128-bit signed integer, max value is approximately 1.7 * 10^38 */
				/* We use a simple overflow detection strategy */
				try
				{
					Int128 temp = result * ten;
					temp = temp + Int128{ digit };
					result = temp;
				}
				catch ( ... )
				{
					return false; /* Overflow occurred */
				}
			}

			/* Apply sign */
			if ( isNegative )
			{
				result = -result;
			}

			return true;
		}
		catch ( ... )
		{
			return false;
		}
	}

	//----------------------------------------------
	// Stream operators
	//----------------------------------------------

	std::ostream& operator<<( std::ostream& os, const Int128& value )
	{
		/* Simple string conversion for display purposes */
		if ( value.isZero() )
		{
			return os << "0";
		}

		Int128 temp = value.abs();
		std::string result;

		/* Extract digits by repeated division by 10 */
		while ( !temp.isZero() )
		{
			Int128 quotient = temp / Int128{ 10 };
			Int128 remainder = temp % Int128{ 10 };

			/* remainder should be 0-9, extract as single digit */
			char digit = static_cast<char>( '0' + remainder.toLow() );
			result = digit + result;

			temp = quotient;
		}

		if ( value.isNegative() )
		{
			result = "-" + result;
		}

		return os << result;
	}
}
