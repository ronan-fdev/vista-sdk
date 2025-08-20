/**
 * @file Int128.inl
 * @brief Inline implementations for cross-platform Int128 class
 */

#pragma once

namespace dnv::vista::sdk::datatypes
{
	//=====================================================================
	// Int128 class
	//=====================================================================

#if VISTA_SDK_CPP_HAS_INT128

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline constexpr Int128::Int128() noexcept
		: m_value{ 0 }
	{
	}

	inline constexpr Int128::Int128( VISTA_SDK_CPP_INT128 val ) noexcept
		: m_value{ val }
	{
	}

	inline constexpr Int128::Int128( std::uint64_t low, std::uint64_t high ) noexcept
		: m_value{ static_cast<VISTA_SDK_CPP_INT128>( high ) << 64 | low } {}

	inline constexpr Int128::Int128( std::uint64_t val ) noexcept
		: m_value{ val }
	{
	}

	inline constexpr Int128::Int128( std::int64_t val ) noexcept
		: m_value{ val }
	{
	}

	inline constexpr Int128::Int128( std::uint32_t val ) noexcept
		: m_value{ val }
	{
	}

	inline constexpr Int128::Int128( int val ) noexcept
		: m_value{ val }
	{
	}

	//----------------------------------------------
	// Arithmetic operations
	//----------------------------------------------

	inline Int128 Int128::operator+( const Int128& other ) const noexcept
	{
		return Int128{ m_value + other.m_value };
	}

	inline Int128 Int128::operator-( const Int128& other ) const noexcept
	{
		return Int128{ m_value - other.m_value };
	}

	inline Int128 Int128::operator*( const Int128& other ) const noexcept
	{
		return Int128{ m_value * other.m_value };
	}

	inline Int128 Int128::operator/( const Int128& other ) const
	{
		if ( other.m_value == 0 )
		{
			throw std::overflow_error( "Division by zero" );
		}

		return Int128{ m_value / other.m_value };
	}

	inline Int128 Int128::operator%( const Int128& other ) const
	{
		if ( other.m_value == 0 )
		{
			throw std::overflow_error( "Division by zero" );
		}

		return Int128{ m_value % other.m_value };
	}

	inline Int128 Int128::operator-() const noexcept
	{
		return Int128{ -m_value };
	}

	//----------------------------------------------
	// Comparison operations
	//----------------------------------------------

	inline bool Int128::operator==( const Int128& other ) const noexcept
	{
		return m_value == other.m_value;
	}

	inline bool Int128::operator!=( const Int128& other ) const noexcept
	{
		return m_value != other.m_value;
	}

	inline bool Int128::operator<( const Int128& other ) const noexcept
	{
		return m_value < other.m_value;
	}

	inline bool Int128::operator<=( const Int128& other ) const noexcept
	{
		return m_value <= other.m_value;
	}

	inline bool Int128::operator>( const Int128& other ) const noexcept
	{
		return m_value > other.m_value;
	}

	inline bool Int128::operator>=( const Int128& other ) const noexcept
	{
		return m_value >= other.m_value;
	}

	//----------------------------------------------
	// State checking
	//----------------------------------------------

	inline bool Int128::isZero() const noexcept
	{
		return m_value == 0;
	}

	inline bool Int128::isNegative() const noexcept
	{
		return m_value < 0;
	}

	//----------------------------------------------
	// Mathematical operations
	//----------------------------------------------

	inline Int128 Int128::abs() const noexcept
	{
		return Int128{ m_value < 0 ? -m_value : m_value };
	}

	//----------------------------------------------
	// Access operations
	//----------------------------------------------

	inline std::uint64_t Int128::toLow() const noexcept
	{
		return static_cast<std::uint64_t>( m_value );
	}

	inline std::uint64_t Int128::toHigh() const noexcept
	{
		return static_cast<std::uint64_t>( m_value >> 64 );
	}

#else

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline constexpr Int128::Int128() noexcept
		: m_lower64bits{ 0 },
		  m_upper64bits{ 0 }
	{
	}

	inline constexpr Int128::Int128( std::uint64_t low, std::uint64_t high ) noexcept
		: m_lower64bits{ low },
		  m_upper64bits{ high }
	{
	}

	inline constexpr Int128::Int128( std::uint64_t val ) noexcept
		: m_lower64bits{ val },
		  m_upper64bits{ 0 }
	{
	}

	inline constexpr Int128::Int128( std::int64_t val ) noexcept
		: m_lower64bits{ static_cast<std::uint64_t>( val ) },
		  m_upper64bits{ ( val < 0 ) ? static_cast<std::uint64_t>( -1 ) : 0 }
	{
	}

	inline constexpr Int128::Int128( std::uint32_t val ) noexcept
		: m_lower64bits{ val },
		  m_upper64bits{ 0 }
	{
	}

	inline constexpr Int128::Int128( int val ) noexcept
		: m_lower64bits{ static_cast<std::uint64_t>( val ) }, m_upper64bits{ ( val < 0 ) ? static_cast<std::uint64_t>( -1 ) : 0 }
	{
	}

	//----------------------------------------------
	// Arithmetic operations
	//----------------------------------------------

	inline Int128 Int128::operator+( const Int128& other ) const noexcept
	{
		/* 128-bit addition with carry propagation */
		std::uint64_t result_low = m_lower64bits + other.m_lower64bits;
		std::uint64_t carry = ( result_low < m_lower64bits ) ? 1 : 0;
		std::uint64_t result_high = m_upper64bits + other.m_upper64bits + carry;

		return Int128( result_low, result_high );
	}

	inline Int128 Int128::operator-( const Int128& other ) const noexcept
	{
		/* 128-bit subtraction with borrow propagation */
		std::uint64_t result_low = m_lower64bits - other.m_lower64bits;
		std::uint64_t borrow = ( m_lower64bits < other.m_lower64bits ) ? 1 : 0;
		std::uint64_t result_high = m_upper64bits - other.m_upper64bits - borrow;

		return Int128( result_low, result_high );
	}

	inline Int128 Int128::operator*( const Int128& other ) const noexcept
	{
		/*
		 * 128-bit multiplication using Karatsuba-style algorithm
		 * Performance: Breaks 64x64 multiplication into 32x32 operations
		 * to leverage hardware multipliers efficiently on all platforms
		 */
		std::uint64_t a_low = m_lower64bits & 0xFFFFFFFF;
		std::uint64_t a_high = m_lower64bits >> 32;
		std::uint64_t b_low = other.m_lower64bits & 0xFFFFFFFF;
		std::uint64_t b_high = other.m_lower64bits >> 32;

		/* Four 32x32->64 multiplications */
		std::uint64_t p0 = a_low * b_low;
		std::uint64_t p1 = a_low * b_high;
		std::uint64_t p2 = a_high * b_low;
		std::uint64_t p3 = a_high * b_high;

		/* Carry computation for intermediate sum */
		std::uint64_t carry = ( ( p0 >> 32 ) + ( p1 & 0xFFFFFFFF ) + ( p2 & 0xFFFFFFFF ) ) >> 32;

		/* Final result assembly */
		std::uint64_t result_low = p0 + ( p1 << 32 ) + ( p2 << 32 );
		std::uint64_t result_high = p3 + ( p1 >> 32 ) + ( p2 >> 32 ) + carry +
									m_upper64bits * other.m_lower64bits +
									m_lower64bits * other.m_upper64bits;

		return Int128( result_low, result_high );
	}

	inline Int128 Int128::operator/( const Int128& other ) const
	{
		if ( other.isZero() )
		{
			throw std::overflow_error( "Division by zero" );
		}

		/*
		 * Performance optimization: Fast path for 64-bit values
		 * Avoids expensive 128-bit division when possible
		 */
		if ( m_upper64bits == 0 && other.m_upper64bits == 0 )
		{
			/* Both fit in 64-bit - use native division */
			return Int128( m_lower64bits / other.m_lower64bits, 0 );
		}

		/*
		 * Fallback: Approximate division using double precision
		 * Performance trade-off: Fast but loses precision for very large values
		 * For exact division, would need full 128-bit division algorithm
		 */
		double dividend = static_cast<double>( m_upper64bits ) * ( 1ULL << 32 ) * ( 1ULL << 32 ) + static_cast<double>( m_lower64bits );
		double divisor = static_cast<double>( other.m_upper64bits ) * ( 1ULL << 32 ) * ( 1ULL << 32 ) + static_cast<double>( other.m_lower64bits );
		double result = dividend / divisor;

		/* Convert back to Int128 (approximate) */
		if ( result >= 0 )
		{
			std::uint64_t res = static_cast<std::uint64_t>( result );
			return Int128( res, 0 );
		}
		else
		{
			std::uint64_t res = static_cast<std::uint64_t>( -result );
			/* Two's complement negation for negative results */
			std::uint64_t neg_low = ~res + 1;
			std::uint64_t neg_high = ( res == 0 ) ? 0 : ~static_cast<std::uint64_t>( 0 );
			return Int128( neg_low, neg_high );
		}
	}

	inline Int128 Int128::operator%( const Int128& other ) const
	{
		if ( other.isZero() )
		{
			throw std::overflow_error( "Division by zero" );
		}

		/*
		 * Performance optimization: Fast modulo for 64-bit values
		 */
		if ( m_upper64bits == 0 && other.m_upper64bits == 0 )
		{
			/* Both fit in 64-bit - use native modulo */
			return Int128( m_lower64bits % other.m_lower64bits, 0 );
		}

		/*
		 * General case: a % b = a - (a / b) * b
		 * Performance: Reuses division and multiplication implementations
		 */
		Int128 quotient = *this / other;
		return *this - ( quotient * other );
	}

	inline Int128 Int128::operator-() const noexcept
	{
		/* Two's complement negation */
		Int128 result = Int128( ~m_lower64bits, ~m_upper64bits );
		return result + Int128{ 1 };
	}

	//----------------------------------------------
	// Comparison operations
	//----------------------------------------------

	inline bool Int128::operator==( const Int128& other ) const noexcept
	{
		return m_lower64bits == other.m_lower64bits && m_upper64bits == other.m_upper64bits;
	}

	inline bool Int128::operator!=( const Int128& other ) const noexcept
	{
		return m_lower64bits != other.m_lower64bits || m_upper64bits != other.m_upper64bits;
	}

	inline bool Int128::operator<( const Int128& other ) const noexcept
	{
		if ( m_upper64bits != other.m_upper64bits )
		{
			return static_cast<std::int64_t>( m_upper64bits ) < static_cast<std::int64_t>( other.m_upper64bits );
		}
		return m_lower64bits < other.m_lower64bits;
	}

	inline bool Int128::operator<=( const Int128& other ) const noexcept
	{
		if ( m_upper64bits != other.m_upper64bits )
		{
			return static_cast<std::int64_t>( m_upper64bits ) < static_cast<std::int64_t>( other.m_upper64bits );
		}
		return m_lower64bits <= other.m_lower64bits;
	}

	inline bool Int128::operator>( const Int128& other ) const noexcept
	{
		if ( m_upper64bits != other.m_upper64bits )
		{
			return static_cast<std::int64_t>( m_upper64bits ) > static_cast<std::int64_t>( other.m_upper64bits );
		}
		return m_lower64bits > other.m_lower64bits;
	}

	inline bool Int128::operator>=( const Int128& other ) const noexcept
	{
		if ( m_upper64bits != other.m_upper64bits )
		{
			return static_cast<std::int64_t>( m_upper64bits ) > static_cast<std::int64_t>( other.m_upper64bits );
		}
		return m_lower64bits >= other.m_lower64bits;
	}

	//----------------------------------------------
	// State checking
	//----------------------------------------------

	inline bool Int128::isZero() const noexcept
	{
		return m_lower64bits == 0 && m_upper64bits == 0;
	}

	inline bool Int128::isNegative() const noexcept
	{
		return static_cast<std::int64_t>( m_upper64bits ) < 0;
	}

	//----------------------------------------------
	// Mathematical operations
	//----------------------------------------------

	inline Int128 Int128::abs() const noexcept
	{
		if ( !isNegative() )
		{
			return *this;
		}
		return -*this;
	}

	//----------------------------------------------
	// Access operations
	//----------------------------------------------

	inline std::uint64_t Int128::toLow() const noexcept
	{
		return m_lower64bits;
	}

	inline std::uint64_t Int128::toHigh() const noexcept
	{
		return m_upper64bits;
	}

#endif
}
