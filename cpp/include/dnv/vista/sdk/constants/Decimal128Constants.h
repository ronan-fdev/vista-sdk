/**
 * @file Decimal128Constants.h
 * @brief Mathematical and formatting constants for high-precision decimal arithmetic
 * @details Compile-time constants for decimal operations, string conversion,
 *          and arithmetic optimization
 * @todo Development-only file - remove before production release
 */

#pragma once

namespace dnv::vista::sdk::constants::decimal128
{
	//=====================================================================
	// Decimal arithmetic constants
	//=====================================================================

	/** @brief Maximum number of decimal places supported. */
	inline constexpr std::uint8_t MAXIMUM_PLACES = 28U;

	/** @brief Bit mask for extracting scale from flags field (bits 16-23). */
	inline constexpr std::uint32_t SCALE_MASK = 0x00FF0000U;

	/** @brief Bit mask for sign detection (bit 31). */
	inline constexpr std::uint32_t SIGN_MASK = 0x80000000U;

	/** @brief Bit position for scale field in flags. */
	inline constexpr std::uint8_t SCALE_SHIFT = 16U;

	/** @brief Maximum value that fits in 96-bit mantissa high word. */
	inline constexpr std::uint64_t MAX_HIGH_WORD = 0xFFFFFFFFULL;

	//=====================================================================
	// String conversion constants
	//=====================================================================

	/** @brief Base for decimal digit conversion. */
	inline constexpr std::uint64_t BASE = 10ULL;

	/** @brief ASCII offset for digit conversion ('0' character). */
	inline constexpr char DIGIT_OFFSET = '0';

	/** @brief Decimal point character for string formatting. */
	inline constexpr char POINT = '.';

	/** @brief Minus sign character for negative values. */
	inline constexpr char MINUS_SIGN = '-';

	/** @brief Plus sign character for positive values. */
	inline constexpr char PLUS_SIGN = '+';

	/** @brief Zero character for padding operations. */
	inline constexpr char ZERO_CHAR = '0';

	/** @brief Maximum expected length for decimal string representation. */
	inline constexpr size_t MAX_STRING_LENGTH = 64UL;

	//=====================================================================
	// Performance optimization constants
	//=====================================================================

	/** @brief Power of 10 lookup table size for fast scaling operations. */
	inline constexpr std::uint8_t POWER_TABLE_SIZE = 29U;

	//=====================================================================
	// Pre-computed power of 10 lookup table
	//=====================================================================

	/** @brief Powers of 10 lookup table for efficient scaling operations. */
	inline constexpr std::array<std::uint64_t, POWER_TABLE_SIZE> POWERS_OF_10{ {
		1ULL,												 /* 10^0 */
		10ULL,												 /* 10^1 */
		100ULL,												 /* 10^2 */
		1000ULL,											 /* 10^3 */
		10000ULL,											 /* 10^4 */
		100000ULL,											 /* 10^5 */
		1000000ULL,											 /* 10^6 */
		10000000ULL,										 /* 10^7 */
		100000000ULL,										 /* 10^8 */
		1000000000ULL,										 /* 10^9 */
		10000000000ULL,										 /* 10^10 */
		100000000000ULL,									 /* 10^11 */
		1000000000000ULL,									 /* 10^12 */
		10000000000000ULL,									 /* 10^13 */
		100000000000000ULL,									 /* 10^14 */
		1000000000000000ULL,								 /* 10^15 */
		10000000000000000ULL,								 /* 10^16 */
		100000000000000000ULL,								 /* 10^17 */
		1000000000000000000ULL,								 /* 10^18 */
		10000000000000000000ULL,							 /* 10^19 (max uint64_t support) */
		0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL /* 10^20-28 (require 128-bit) */
	} };
}
