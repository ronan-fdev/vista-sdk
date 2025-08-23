/**
 * @file Decimal128.inl
 * @brief Inline implementations for cross-platform Decimal128 class
 */

#pragma once

#include "dnv/vista/sdk/constants/Decimal128Constants.h"

namespace dnv::vista::sdk::datatypes
{
	//=====================================================================
	// Decimal128 class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline constexpr Decimal128::Decimal128() noexcept
		: m_mantissa{ { 0, 0, 0 } },
		  m_flags{ 0 }
	{
	}

	//----------------------------------------------
	// Decimal constants
	//----------------------------------------------

	inline constexpr Decimal128 Decimal128::zero() noexcept
	{
		return Decimal128{};
	}

	inline constexpr Decimal128 Decimal128::one() noexcept
	{
		Decimal128 result{};
		result.m_mantissa[0] = 1;

		return result;
	}

	inline constexpr Decimal128 Decimal128::minValue() noexcept
	{
		Decimal128 result{};
		result.m_mantissa[0] = 1;
		result.m_flags = ( constants::decimal128::MAXIMUM_PLACES << constants::decimal128::SCALE_SHIFT );

		return result;
	}

	inline constexpr Decimal128 Decimal128::maxValue() noexcept
	{
		Decimal128 result{};
		result.m_mantissa[0] = 0xFFFFFFFF;
		result.m_mantissa[1] = 0xFFFFFFFF;
		result.m_mantissa[2] = 0xFFFFFFFF;

		return result;
	}

	//----------------------------------------------
	// Property accessors
	//----------------------------------------------

	inline std::uint8_t Decimal128::scale() const noexcept
	{
		return static_cast<std::uint8_t>( ( m_flags & constants::decimal128::SCALE_MASK ) >> constants::decimal128::SCALE_SHIFT );
	}

	//----------------------------------------------
	// State checking
	//----------------------------------------------

	inline bool Decimal128::isZero() const noexcept
	{
		return m_mantissa[0] == 0 && m_mantissa[1] == 0 && m_mantissa[2] == 0;
	}

	inline bool Decimal128::isNegative() const noexcept
	{
		return ( m_flags & constants::decimal128::SIGN_MASK ) != 0;
	}

	//----------------------------------------------
	// Internal helper methods
	//----------------------------------------------

	inline Int128 Decimal128::getMantissa() const noexcept
	{
#if VISTA_SDK_CPP_HAS_INT128
		VISTA_SDK_CPP_INT128 value = static_cast<VISTA_SDK_CPP_INT128>( m_mantissa[2] ) << 64 | static_cast<VISTA_SDK_CPP_INT128>( m_mantissa[1] ) << 32 |
									 static_cast<VISTA_SDK_CPP_INT128>( m_mantissa[0] );

		return Int128( value );
#else
		std::uint64_t low = static_cast<std::uint64_t>( m_mantissa[1] ) << 32 | m_mantissa[0];
		std::uint64_t high = m_mantissa[2];

		return Int128( low, high );
#endif
	}

	inline void Decimal128::setMantissa( const Int128& value ) noexcept
	{
#if VISTA_SDK_CPP_HAS_INT128
		auto nativeValue = value.toNative();
		m_mantissa[0] = static_cast<std::uint32_t>( nativeValue );
		m_mantissa[1] = static_cast<std::uint32_t>( nativeValue >> 32 );
		m_mantissa[2] = static_cast<std::uint32_t>( nativeValue >> 64 );
#else
		m_mantissa[0] = static_cast<std::uint32_t>( value.toLow() );
		m_mantissa[1] = static_cast<std::uint32_t>( value.toLow() >> 32 );
		m_mantissa[2] = static_cast<std::uint32_t>( value.toHigh() );
#endif
	}
}
