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
 * @file ImoNumber.cpp
 * @brief Implementation of ImoNumber class methods
 * @details Provides constructors, validation logic, parsing, and string conversion for IMO numbers
 */

#include "dnv/vista/sdk/ImoNumber.h"

#include <nfx/StringUtils.h>

#include <stdexcept>

namespace dnv::vista::sdk
{
	ImoNumber::ImoNumber( int value )
	{
		if ( !isValid( value ) )
		{
			throw std::invalid_argument{ "Invalid IMO number: " + std::to_string( value ) };
		}

		m_value = value;
	}

	ImoNumber::ImoNumber( std::string_view value )
	{
		if ( nfx::string::isEmpty( value ) )
		{
			throw std::invalid_argument{ "Invalid IMO number: " + std::string{ value } };
		}

		if ( nfx::string::isNullOrWhiteSpace( value ) )
		{
			throw std::invalid_argument{ "Invalid IMO number: " + std::string{ value } };
		}

		std::string_view sv = value;
		if ( sv.length() >= 3 && nfx::string::iequals( sv.substr( 0, 3 ), "IMO" ) )
		{
			sv = sv.substr( 3 );
		}

		int num = 0;
		if ( !nfx::string::fromString<int>( sv, num ) )
		{
			throw std::invalid_argument{ "Invalid IMO number: " + std::string{ value } };
		}
		if ( num == 0 || !isValid( num ) )
		{
			throw std::invalid_argument{ "Invalid IMO number: " + std::string{ value } };
		}

		m_value = num;
	}

	/*
		IMO number validation according to the standard:
		https://en.wikipedia.org/wiki/IMO_number
		An IMO number is made of the three letters "IMO" followed by a seven-digit number.
		This consists of a six-digit sequential unique number followed by a check digit.
		The integrity of an IMO number can be verified using its check digit.
		This is done by multiplying each of the first six digits by a factor
		of 2 to 7 corresponding to their position from right to left.
		The rightmost digit of this sum is the check digit.
		For example, for IMO 9074729: (9×7) + (0×6) + (7×5) + (4×4) + (7×3) + (2×2) = 139
		The rightmost digit (9) must equal checksum mod 10 (139 % 10 = 9)
	*/
	bool ImoNumber::isValid( int imoNumber ) noexcept
	{
		if ( imoNumber < 1000000 || imoNumber > 9999999 )
		{
			return false;
		}

		int digits[7];
		int temp = imoNumber;
		for ( int i = 0; i < 7; ++i )
		{
			digits[i] = temp % 10;
			temp /= 10;
		}

		int checkSum = 0;
		for ( int i = 1; i < 7; ++i )
		{
			checkSum += digits[i] * ( i + 1 );
		}

		int calculatedCheckDigit = checkSum % 10;
		int providedCheckDigit = digits[0];

		return providedCheckDigit == calculatedCheckDigit;
	}

	std::optional<ImoNumber> ImoNumber::fromString( std::string_view value ) noexcept
	{
		if ( nfx::string::isNullOrWhiteSpace( value ) )
		{
			return std::nullopt;
		}

		std::string_view sv = value;
		if ( sv.length() >= 3 && nfx::string::iequals( sv.substr( 0, 3 ), "IMO" ) )
		{
			sv = sv.substr( 3 );
		}

		int num = 0;
		if ( !nfx::string::fromString<int>( sv, num ) )
		{
			return std::nullopt;
		}

		if ( num == 0 || !isValid( num ) )
		{
			return std::nullopt;
		}

		return ImoNumber{ num };
	}

	std::string ImoNumber::toString() const
	{
		return "IMO" + std::to_string( m_value );
	}
} // namespace dnv::vista::sdk
