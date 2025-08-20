/**
 * @file Int128.cpp
 * @brief Implementation of cross-platform 128-bit integer arithmetic
 * @details Provides exact 128-bit integer arithmetic with portable operations
 */

#include "dnv/vista/sdk/pch.h"

#include "dnv/vista/sdk/datatypes/Int128.h"

namespace dnv::vista::sdk::datatypes
{
	//=====================================================================
	// Int128 class
	//=====================================================================

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
