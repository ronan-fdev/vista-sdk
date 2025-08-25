/**
 * @file Decimal128.cpp
 * @brief Implementation of cross-platform 128-bit decimal arithmetic
 * @details Provides exact decimal arithmetic with portable 128-bit operations
 */

#include "dnv/vista/sdk/constants/Decimal128Constants.h"
#include "dnv/vista/sdk/internal/StringBuilderPool.h"

#include "dnv/vista/sdk/datatypes/Decimal128.h"

namespace dnv::vista::sdk::datatypes
{
	//=====================================================================
	// Decimal128 class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Decimal128::Decimal128( double value ) noexcept
		: m_mantissa{ { 0, 0, 0 } },
		  m_flags{ 0 }
	{
		if ( std::isnan( value ) || std::isinf( value ) )
		{
			/* Invalid values become zero */
			return;
		}

		if ( value == 0.0 )
		{
			return;
		}

		/* Extract sign */
		bool negative = value < 0.0;
		if ( negative )
		{
			value = -value;
			m_flags |= constants::decimal128::SIGN_MASK;
		}

		/* Convert double to decimal with limited precision to match IEEE 754 behavior
		   Find the appropriate scale but limit to what double can accurately represent */
		std::uint8_t currentScale = 0;
		double scaledValue = value;

		/* Scale up while we have fractional parts and maintain precision */
		while ( currentScale < 15 && currentScale < constants::decimal128::MAXIMUM_PLACES && scaledValue != std::floor( scaledValue ) )
		{
			scaledValue *= 10.0;
			++currentScale;
		}

		/* Convert to integer mantissa */
		std::uint64_t mantissaValue = static_cast<std::uint64_t>( std::round( scaledValue ) );

		/* Set scale */
		m_flags |= ( static_cast<std::uint32_t>( currentScale ) << constants::decimal128::SCALE_SHIFT );

		/* Store mantissa */
		m_mantissa[0] = static_cast<std::uint32_t>( mantissaValue );
		m_mantissa[1] = static_cast<std::uint32_t>( mantissaValue >> 32 );
		m_mantissa[2] = 0;

		normalize();
	}

	Decimal128::Decimal128( std::int32_t value ) noexcept
		: m_mantissa{ { 0, 0, 0 } },
		  m_flags{ 0 }
	{
		if ( value < 0 )
		{
			m_flags |= constants::decimal128::SIGN_MASK;
			value = -value;
		}

		m_mantissa[0] = static_cast<std::uint32_t>( value );
	}

	Decimal128::Decimal128( std::int64_t value ) noexcept
		: m_mantissa{ { 0, 0, 0 } },
		  m_flags{ 0 }
	{
		if ( value < 0 )
		{
			m_flags |= constants::decimal128::SIGN_MASK;
			value = -value;
		}

		m_mantissa[0] = static_cast<std::uint32_t>( value );
		m_mantissa[1] = static_cast<std::uint32_t>( value >> 32 );
	}

	Decimal128::Decimal128( std::uint32_t value ) noexcept
		: m_mantissa{ { 0, 0, 0 } },
		  m_flags{ 0 } { m_mantissa[0] = value; }

	Decimal128::Decimal128( std::uint64_t value ) noexcept
		: m_mantissa{ { 0, 0, 0 } },
		  m_flags{ 0 }
	{
		m_mantissa[0] = static_cast<std::uint32_t>( value );
		m_mantissa[1] = static_cast<std::uint32_t>( value >> 32 );
	}

	Decimal128::Decimal128( std::string_view str )
		: m_mantissa{ { 0, 0, 0 } },
		  m_flags{ 0 }
	{
		if ( !tryParse( str, *this ) )
		{
			throw std::invalid_argument( "Invalid decimal string format" );
		}
	}

	//----------------------------------------------
	// Arithmetic operators
	//----------------------------------------------

	Decimal128 Decimal128::operator+( const Decimal128& other ) const
	{
		if ( isZero() )
		{
			return other;
		}
		if ( other.isZero() )
		{
			return *this;
		}

		Decimal128 result;
		auto [left, right] = alignScale( other );

		result.setMantissa( left + right );
		result.m_flags = ( m_flags & ~constants::decimal128::SCALE_MASK ) |
						 ( std::max( scale(), other.scale() ) << constants::decimal128::SCALE_SHIFT );

		/* Handle sign */
		if ( isNegative() == other.isNegative() )
		{
			if ( isNegative() )
			{
				result.m_flags |= constants::decimal128::SIGN_MASK;
			}
		}
		else
		{
			/* Different signs - need subtraction logic */
			if ( left > right )
			{
				result.setMantissa( left - right );
				if ( isNegative() )
				{
					result.m_flags |= constants::decimal128::SIGN_MASK;
				}
			}
			else
			{
				result.setMantissa( right - left );
				if ( other.isNegative() )
				{
					result.m_flags |= constants::decimal128::SIGN_MASK;
				}
			}
		}

		result.normalize();

		return result;
	}

	Decimal128 Decimal128::operator-( const Decimal128& other ) const
	{
		Decimal128 negatedOther = other;

		negatedOther.m_flags ^= constants::decimal128::SIGN_MASK;

		return *this + negatedOther;
	}

	Decimal128 Decimal128::operator*( const Decimal128& other ) const
	{
		if ( isZero() || other.isZero() )
		{
			return Decimal128{};
		}

		Decimal128 result;
		Int128 left = getMantissa();
		Int128 right = other.getMantissa();

		result.setMantissa( left * right );

		/* Combine scales */
		std::uint8_t newScale = static_cast<std::uint8_t>( scale() + other.scale() );
		if ( newScale > constants::decimal128::MAXIMUM_PLACES )
		{
			/* Reduce precision to fit scale limit */
			std::uint8_t reduce = static_cast<std::uint8_t>( newScale - constants::decimal128::MAXIMUM_PLACES );
			for ( std::uint8_t i = 0U; i < reduce; ++i )
			{
				result.divideByPowerOf10( 1U );
			}
			newScale = constants::decimal128::MAXIMUM_PLACES;
		}

		result.m_flags = ( static_cast<std::uint32_t>( newScale ) << constants::decimal128::SCALE_SHIFT );

		/* Combine signs */
		if ( isNegative() != other.isNegative() )
		{
			result.m_flags |= constants::decimal128::SIGN_MASK;
		}

		result.normalize();

		return result;
	}

	Decimal128 Decimal128::operator/( const Decimal128& other ) const
	{
		if ( other.isZero() )
		{
			throw std::overflow_error( "Division by zero" );
		}

		if ( isZero() )
		{
			return Decimal128{};
		}

		Decimal128 result;
		Int128 dividend = getMantissa();
		Int128 divisor = other.getMantissa();

		/* Scale adjustment for division:
		   If dividend has scale d and divisor has scale s,
		   result should have scale (d - s)
		   To maintain precision, we may need to scale up the dividend */
		std::int32_t targetScale = static_cast<std::int32_t>( scale() ) - static_cast<std::int32_t>( other.scale() );

		/* If target scale would be negative, we need to scale up the dividend */
		if ( targetScale < 0 )
		{
			std::uint8_t scaleUp = static_cast<std::uint8_t>( -targetScale );
			for ( std::uint8_t i = 0U; i < scaleUp && i < constants::decimal128::MAXIMUM_PLACES; ++i )
			{
				dividend = dividend * Int128{ 10 };
			}
			targetScale = 0;
		}

		result.setMantissa( dividend / divisor );
		result.m_flags = ( static_cast<std::uint32_t>( targetScale ) << constants::decimal128::SCALE_SHIFT );

		/* Combine signs */
		if ( isNegative() != other.isNegative() )
		{
			result.m_flags |= constants::decimal128::SIGN_MASK;
		}

		result.normalize();

		return result;
	}

	Decimal128& Decimal128::operator+=( const Decimal128& other )
	{
		*this = *this + other;
		return *this;
	}

	Decimal128& Decimal128::operator-=( const Decimal128& other )
	{
		*this = *this - other;
		return *this;
	}

	Decimal128& Decimal128::operator*=( const Decimal128& other )
	{
		*this = *this * other;
		return *this;
	}

	Decimal128& Decimal128::operator/=( const Decimal128& other )
	{
		*this = *this / other;
		return *this;
	}

	Decimal128 Decimal128::operator-() const noexcept
	{
		Decimal128 result = *this;

		result.m_flags ^= constants::decimal128::SIGN_MASK;
		return result;
	}

	//----------------------------------------------
	// Comparison operators
	//----------------------------------------------

	bool Decimal128::operator==( const Decimal128& other ) const noexcept
	{
		if ( isZero() && other.isZero() )
		{
			return true;
		}

		if ( isNegative() != other.isNegative() )
		{
			return false;
		}

		auto [left, right] = alignScale( other );

		return left == right;
	}

	bool Decimal128::operator!=( const Decimal128& other ) const noexcept
	{
		return !( *this == other );
	}

	bool Decimal128::operator<( const Decimal128& other ) const noexcept
	{
		if ( isNegative() != other.isNegative() )
		{
			return isNegative();
		}

		auto [left, right] = alignScale( other );

		if ( isNegative() )
		{
			return left > right;
		}
		else
		{
			return left < right;
		}
	}

	bool Decimal128::operator<=( const Decimal128& other ) const noexcept
	{
		return *this < other || *this == other;
	}

	bool Decimal128::operator>( const Decimal128& other ) const noexcept
	{
		return !( *this <= other );
	}

	bool Decimal128::operator>=( const Decimal128& other ) const noexcept
	{
		return !( *this < other );
	}

	//----------------------------------------------
	// String parsing and conversion
	//----------------------------------------------

	Decimal128 Decimal128::parse( std::string_view str )
	{
		Decimal128 result;
		if ( !tryParse( str, result ) )
		{
			throw std::invalid_argument( "Invalid decimal string format" );
		}
		return result;
	}

	bool Decimal128::tryParse( std::string_view str, Decimal128& result ) noexcept
	{
		try
		{
			result = Decimal128{};

			if ( str.empty() )
			{
				return false;
			}

			/* Handle sign */
			bool negative = false;
			size_t pos = 0;
			if ( str[0] == '-' )
			{
				negative = true;
				pos = 1;
			}
			else if ( str[0] == '+' )
			{
				pos = 1;
			}

			/* Check if we have at least one character after sign */
			if ( pos >= str.length() )
			{
				return false;
			}

			/* Find decimal point and validate there's only one */
			size_t decimalPos = std::string_view::npos;
			std::uint8_t currentScale = 0;
			size_t decimalCount = 0;

			for ( size_t i = pos; i < str.length(); ++i )
			{
				if ( str[i] == '.' )
				{
					decimalCount++;
					if ( decimalCount > 1 )
					{
						return false;
					}

					decimalPos = i;
				}
			}

			if ( decimalPos != std::string_view::npos )
			{
				currentScale = static_cast<std::uint8_t>( str.length() - decimalPos - 1 );
				if ( currentScale > constants::decimal128::MAXIMUM_PLACES )
				{
					/* Too many decimal places */
					return false;
				}
			}

			/* Optimized digit accumulation */
			Int128 mantissaValue;
			const Int128 ten{ 10 };
			bool hasDigits = false;

			for ( size_t i = pos; i < str.length(); ++i )
			{
				if ( str[i] == '.' )
				{
					continue;
				}

				if ( str[i] < '0' || str[i] > '9' )
				{
					/* Invalid character */
					return false;
				}

				hasDigits = true;
				std::uint64_t digit = static_cast<std::uint64_t>( str[i] - '0' );

				/* Fast overflow check and accumulation */
				try
				{
					/* Optimize for common case: multiply by 10 and add digit */
					mantissaValue = mantissaValue * ten + Int128{ digit };
				}
				catch ( ... )
				{
					/* Overflow */
					return false;
				}
			}

			/* Ensure we have at least one digit (prevents parsing ".", "+", "-", etc.) */
			if ( !hasDigits )
			{
				return false;
			}

			/* Check if mantissa fits in our 96-bit storage */
			if ( mantissaValue.toHigh() > 0xFFFFFFFF )
			{
				/* Value too large for decimal */
				return false;
			}

			/* Set result */
			if ( negative )
			{
				result.m_flags |= constants::decimal128::SIGN_MASK;
			}

			result.m_flags |= ( static_cast<std::uint32_t>( currentScale ) << constants::decimal128::SCALE_SHIFT );

			/* Store the 96-bit mantissa */
			std::uint64_t low = mantissaValue.toLow();
			std::uint64_t high = mantissaValue.toHigh();

			result.m_mantissa[0] = static_cast<std::uint32_t>( low );
			result.m_mantissa[1] = static_cast<std::uint32_t>( low >> 32 );
			result.m_mantissa[2] = static_cast<std::uint32_t>( high );

			return true;
		}
		catch ( ... )
		{
			return false;
		}
	}

	//----------------------------------------------
	// Type conversion
	//----------------------------------------------

	double Decimal128::toDouble() const noexcept
	{
		Int128 mantissa = getMantissa();

		double result;
#if VISTA_SDK_CPP_HAS_INT128
		result = static_cast<double>( mantissa.m_value );
#else
		/* Convert 128-bit to double (approximate) */
		result = static_cast<double>( mantissa.toHigh() ) * ( 1ULL << 32 ) * ( 1ULL << 32 ) +
				 static_cast<double>( mantissa.toLow() );
#endif /* Apply scale */
		std::uint8_t currentScale = scale();
		for ( std::uint8_t i = 0; i < currentScale; ++i )
		{
			result /= 10.0;
		}

		/* Apply sign */
		if ( isNegative() )
		{
			result = -result;
		}

		return result;
	}

	std::string Decimal128::toString() const
	{
		if ( isZero() )
		{
			return "0";
		}

		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();
		Int128 mantissa = getMantissa().abs();
		std::uint8_t currentScale = scale();

		/* Optimized digit extraction with fast division */
		std::array<char, constants::decimal128::MAX_STRING_LENGTH> digits;
		size_t digitCount = 0;

		/* Fast path for values that fit in 64-bit */
#if VISTA_SDK_CPP_HAS_INT128
		if ( mantissa.m_value <= UINT64_MAX )
		{
			std::uint64_t value = static_cast<std::uint64_t>( mantissa.m_value );
			while ( value > 0 && digitCount < digits.size() )
			{
				digits[digitCount++] = static_cast<char>( constants::decimal128::DIGIT_OFFSET + ( value % 10 ) );
				value /= 10;
			}
		}
		else
		{
			/* Full 128-bit extraction */
			while ( !mantissa.isZero() && digitCount < digits.size() )
			{
				digits[digitCount++] = static_cast<char>( constants::decimal128::DIGIT_OFFSET +
														  ( mantissa.m_value % constants::decimal128::BASE ) );
				mantissa = Int128{ mantissa.m_value / constants::decimal128::BASE };
			}
		}
#else
		if ( mantissa.toHigh() == 0 )
		{
			/* Fast 64-bit path */
			std::uint64_t value = mantissa.toLow();
			while ( value > 0 && digitCount < digits.size() )
			{
				digits[digitCount++] = static_cast<char>( constants::decimal128::DIGIT_OFFSET + ( value % 10 ) );
				value /= 10;
			}
		}
		else
		{
			/* Manual 128-bit extraction */
			while ( !mantissa.isZero() && digitCount < digits.size() )
			{
				if ( mantissa.toHigh() == 0 )
				{
					/* Switched to 64-bit range */
					std::uint64_t value = mantissa.toLow();
					while ( value > 0 && digitCount < digits.size() )
					{
						digits[digitCount++] = static_cast<char>( constants::decimal128::DIGIT_OFFSET + ( value % 10 ) );
						value /= 10;
					}
					break;
				}
				digits[digitCount++] = static_cast<char>( constants::decimal128::DIGIT_OFFSET +
														  ( mantissa.toLow() % constants::decimal128::BASE ) );

				mantissa = mantissa / Int128{ constants::decimal128::BASE };
			}
		}
#endif

		if ( digitCount == 0 )
		{
			digitCount = 1;
			digits[0] = constants::decimal128::ZERO_CHAR;
		}

		/* Handle sign */
		if ( isNegative() )
		{
			builder.push_back( constants::decimal128::MINUS_SIGN );
		}

		/* Apply decimal point formatting */
		if ( currentScale > 0 )
		{
			if ( currentScale >= digitCount )
			{
				/* Need leading zeros: "0.00123" */
				builder.push_back( constants::decimal128::ZERO_CHAR );
				builder.push_back( constants::decimal128::POINT );

				/* Add leading zeros */
				for ( size_t i = 0; i < currentScale - digitCount; ++i )
				{
					builder.push_back( constants::decimal128::ZERO_CHAR );
				}

				/* Add digits in reverse order */
				for ( size_t i = digitCount; i > 0; --i )
				{
					builder.push_back( digits[i - 1] );
				}
			}
			else
			{
				/* Add integer part (reverse order) */
				for ( size_t i = digitCount; i > currentScale; --i )
				{
					builder.push_back( digits[i - 1] );
				}

				builder.push_back( constants::decimal128::POINT );

				/* Add fractional part (reverse order) */
				for ( size_t i = currentScale; i > 0; --i )
				{
					builder.push_back( digits[i - 1] );
				}
			}
		}
		else
		{
			/* No decimal point, just add digits in reverse order */
			for ( size_t i = digitCount; i > 0; --i )
			{
				builder.push_back( digits[i - 1] );
			}
		}

		return lease.toString();
	}

	std::array<std::int32_t, 4> Decimal128::toBits() const noexcept
	{
		std::array<std::int32_t, 4> bits{};

		/* First three elements are the 96-bit mantissa */
		bits[0] = static_cast<std::int32_t>( m_mantissa[0] );
		bits[1] = static_cast<std::int32_t>( m_mantissa[1] );
		bits[2] = static_cast<std::int32_t>( m_mantissa[2] );

		/* Fourth element contains scale and sign information */
		bits[3] = static_cast<std::int32_t>( m_flags );

		return bits;
	}

	//----------------------------------------------
	// Mathematical operations
	//----------------------------------------------

	Decimal128 Decimal128::truncate() const noexcept
	{
		if ( scale() == 0 || isZero() )
		{
			return *this;
		}

		Decimal128 result = *this;
		std::uint8_t currentScale = scale();

		/* Remove all fractional digits */
		for ( std::uint8_t i = 0; i < currentScale; ++i )
		{
			result.divideByPowerOf10( 1U );
		}

		/* Clear the scale */
		result.m_flags &= ~constants::decimal128::SCALE_MASK;

		return result;
	}

	Decimal128 Decimal128::floor() const noexcept
	{
		if ( scale() == 0 || isZero() )
		{
			return *this;
		}

		Decimal128 truncated = truncate();

		/* If negative and had fractional part, subtract 1 */
		if ( isNegative() && !( *this == truncated ) )
		{
			truncated = truncated - Decimal128::one();
		}

		return truncated;
	}

	Decimal128 Decimal128::ceiling() const noexcept
	{
		if ( scale() == 0 || isZero() )
		{
			return *this;
		}

		Decimal128 truncated = truncate();

		/* If positive and had fractional part, add 1 */
		if ( !isNegative() && !( *this == truncated ) )
		{
			truncated = truncated + Decimal128::one();
		}

		return truncated;
	}

	Decimal128 Decimal128::round() const noexcept
	{
		return round( 0 );
	}

	Decimal128 Decimal128::round( std::int32_t decimalsPlacesCount ) const noexcept
	{
		if ( decimalsPlacesCount < 0 )
		{
			decimalsPlacesCount = 0;
		}

		if ( decimalsPlacesCount >= static_cast<std::int32_t>( scale() ) || isZero() )
		{
			return *this;
		}

		Decimal128 result = *this;
		std::uint8_t currentScale = scale();
		std::uint8_t targetScale = static_cast<std::uint8_t>( decimalsPlacesCount );
		std::uint8_t digitsToRemove = static_cast<std::uint8_t>( currentScale - targetScale );

		/* Get the digit that determines rounding direction */
		Int128 mantissa = getMantissa();
		Int128 divisor{ 1 };
		if ( digitsToRemove > 1U )
		{
			std::uint8_t divisorPowers = digitsToRemove - 1U;
			for ( std::uint8_t i = std::uint8_t{ 0 }; i < divisorPowers; ++i )
			{
				divisor = divisor * Int128{ 10 };
			}
		}

		Int128 roundingDigit = ( mantissa / divisor ) % Int128{ 10 };

		/* Perform truncation to target scale */
		for ( std::uint8_t i = 0; i < digitsToRemove; ++i )
		{
			result.divideByPowerOf10( 1U );
		}

		result.m_flags =
			( result.m_flags & ~constants::decimal128::SCALE_MASK ) |
			( static_cast<std::uint32_t>( targetScale ) << constants::decimal128::SCALE_SHIFT );

		/* Round up if digit >= 5 */
		if ( roundingDigit.toLow() >= 5 )
		{
			Int128 resultMantissa = result.getMantissa();
			if ( isNegative() )
			{
				resultMantissa = resultMantissa - Int128{ 1 };
			}
			else
			{
				resultMantissa = resultMantissa + Int128{ 1 };
			}
			result.setMantissa( resultMantissa );
		}

		return result;
	}

	Decimal128 Decimal128::abs() const noexcept
	{
		if ( isNegative() )
		{
			return -*this;
		}

		return *this;
	}

	//----------------------------------------------
	// Static mathematical operations
	//----------------------------------------------

	Decimal128 Decimal128::truncate( const Decimal128& value ) noexcept
	{
		return value.truncate();
	}

	Decimal128 Decimal128::floor( const Decimal128& value ) noexcept
	{
		return value.floor();
	}

	Decimal128 Decimal128::ceiling( const Decimal128& value ) noexcept
	{
		return value.ceiling();
	}

	Decimal128 Decimal128::round( const Decimal128& value ) noexcept
	{
		return value.round();
	}

	Decimal128 Decimal128::round( const Decimal128& value, std::int32_t decimalsPlacesCount ) noexcept
	{
		return value.round( decimalsPlacesCount );
	}

	Decimal128 Decimal128::abs( const Decimal128& value ) noexcept
	{
		return value.abs();
	}

	//----------------------------------------------
	// Internal helper methods
	//----------------------------------------------

	void Decimal128::normalize() noexcept
	{
		/* Remove trailing zeros and reduce scale */
		while ( scale() > 0 && ( getMantissa() % Int128{ constants::decimal128::BASE } ) == Int128{ 0 } )
		{
			divideByPowerOf10( 1U );
			std::uint8_t currentScale = scale();
			m_flags = ( m_flags & ~constants::decimal128::SCALE_MASK ) |
					  ( static_cast<std::uint32_t>( currentScale - 1U )
						  << constants::decimal128::SCALE_SHIFT );
		}
	}

	std::pair<Int128, Int128> Decimal128::alignScale( const Decimal128& other ) const
	{
		Int128 left = getMantissa();
		Int128 right = other.getMantissa();

		std::uint8_t leftScale = scale();
		std::uint8_t rightScale = other.scale();

		/* Optimized scaling using power-of-10 lookup table where possible */
		if ( leftScale < rightScale )
		{
			std::uint8_t scaleDiff = static_cast<std::uint8_t>( rightScale - leftScale );
			if ( scaleDiff < constants::decimal128::POWER_TABLE_SIZE &&
				 constants::decimal128::POWERS_OF_10[static_cast<size_t>( scaleDiff )] != 0 )
			{
				left = left * Int128{ constants::decimal128::POWERS_OF_10[static_cast<size_t>( scaleDiff )] };
			}
			else
			{
				/* Fall back to iterative scaling */
				while ( leftScale < rightScale )
				{
					left = left * Int128{ constants::decimal128::BASE };
					++leftScale;
				}
			}
		}
		else if ( rightScale < leftScale )
		{
			std::uint8_t scaleDiff = static_cast<std::uint8_t>( leftScale - rightScale );
			if ( scaleDiff < constants::decimal128::POWER_TABLE_SIZE &&
				 constants::decimal128::POWERS_OF_10[static_cast<size_t>( scaleDiff )] != 0 )
			{
				right = right * Int128{ constants::decimal128::POWERS_OF_10[static_cast<size_t>( scaleDiff )] };
			}
			else
			{
				/* Fall back to iterative scaling */
				while ( rightScale < leftScale )
				{
					right = right * Int128{ constants::decimal128::BASE };
					++rightScale;
				}
			}
		}

		return { std::move( left ), std::move( right ) };
	}

	void Decimal128::multiplyByPowerOf10( std::uint8_t power )
	{
		Int128 mantissa = getMantissa();

		/* Use lookup table for small powers, fall back to iteration for large ones */
		if ( power < constants::decimal128::POWER_TABLE_SIZE &&
			 constants::decimal128::POWERS_OF_10[power] != 0 )
		{
			mantissa = mantissa * Int128{ constants::decimal128::POWERS_OF_10[power] };
		}
		else
		{
			/* Fall back to iterative multiplication for powers > 19 or large values */
			for ( std::uint8_t i = 0U; i < power; ++i )
			{
				mantissa = mantissa * Int128{ constants::decimal128::BASE };
			}
		}

		setMantissa( mantissa );
	}

	void Decimal128::divideByPowerOf10( std::uint8_t power )
	{
		Int128 mantissa = getMantissa();

		/* Use lookup table for small powers, fall back to iteration for large ones */
		if ( power < constants::decimal128::POWER_TABLE_SIZE && constants::decimal128::POWERS_OF_10[power] != 0 )
		{
			mantissa = mantissa / Int128{ constants::decimal128::POWERS_OF_10[power] };
		}
		else
		{
			/* Fall back to iterative division for powers > 19 or large values */
			for ( std::uint8_t i = 0U; i < power; ++i )
			{
				mantissa = mantissa / Int128{ constants::decimal128::BASE };
			}
		}

		setMantissa( mantissa );
	}

	//=====================================================================
	// Stream operators
	//=====================================================================

	std::ostream& operator<<( std::ostream& os, const Decimal128& decimal )
	{
		return os << decimal.toString();
	}

	std::istream& operator>>( std::istream& is, Decimal128& decimal )
	{
		std::string str;
		is >> str;

		if ( !Decimal128::tryParse( str, decimal ) )
		{
			is.setstate( std::ios::failbit );
		}

		return is;
	}
}
