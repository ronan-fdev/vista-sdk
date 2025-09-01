/**
 * @file ImoNumber.cpp
 * @brief Implementation of the ImoNumber class
 */

#include <charconv>
#include <stdexcept>
#include <system_error>

#include "dnv/vista/sdk/ImoNumber.h"

#include "dnv/vista/sdk/constants/AlgorithmConstants.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// ImoNumber class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

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
		auto result = tryParse( value );
		if ( !result )
		{
			throw std::invalid_argument{ "Invalid IMO number: " + std::string{ value } };
		}

		m_value = result->m_value;
	}

	//----------------------------------------------
	// String conversion
	//----------------------------------------------

	std::string ImoNumber::toString() const
	{
		return "IMO" + std::to_string( m_value );
	}

	//----------------------------------------------
	// State inspection
	//----------------------------------------------

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

	//----------------------------------------------
	// Parsing
	//----------------------------------------------

	ImoNumber ImoNumber::parse( std::string_view value )
	{
		if ( value.empty() )
		{
			throw std::invalid_argument( "Empty IMO number string" );
		}

		auto result = tryParse( value );
		if ( !result )
		{
			throw std::invalid_argument( "Failed to parse ImoNumber: " + std::string{ value } );
		}

		return *result;
	}

	std::optional<ImoNumber> ImoNumber::tryParse( std::string_view value )
	{
		if ( value.empty() )
		{
			return std::nullopt;
		}

		if ( value.find_first_of( constants::algorithm::NULL_OR_WHITESPACE ) != std::string::npos )
		{
			return std::nullopt;
		}

		std::string_view sv = value;
		bool hasImoPrefix = sv.length() >= 3 &&
							( std::toupper( static_cast<unsigned char>( sv[0] ) ) == 'I' ) &&
							( std::toupper( static_cast<unsigned char>( sv[1] ) ) == 'M' ) &&
							( std::toupper( static_cast<unsigned char>( sv[2] ) ) == 'O' );

		if ( hasImoPrefix )
		{
			sv = sv.substr( 3 );
		}

		/* Parse the numeric part using std::from_chars for fast, locale-independent conversion */
		int num = 0;
		auto [ptr, ec] = std::from_chars( sv.data(), sv.data() + sv.length(), num );

		if ( ec != std::errc() )
		{
			return std::nullopt;
		}

		if ( num == 0 || !isValid( num ) )
		{
			return std::nullopt;
		}

		return ImoNumber{ num };
	}
}
