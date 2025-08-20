/**
 * @file AlgorithmConstants.h
 * @brief Core algorithm constants for hashing and data structure optimization
 * @details FNV hash constants, CHD perfect hashing tuning parameters,
 *          and string processing constants for maximum performance
 * @todo Development-only file - remove before production release
 */

#pragma once

namespace dnv::vista::sdk::constants::algorithm
{
	//=====================================================================
	// Hash algorithm constants
	//=====================================================================

	/** @brief FNV offset basis constant for hash calculations. */
	inline constexpr uint32_t FNV_OFFSET_BASIS{ 0x811C9DC5 };

	/** @brief FNV prime constant for hash calculations. */
	inline constexpr uint32_t FNV_PRIME{ 0x01000193 };

	//=====================================================================
	// CHD dictionary configuration
	//=====================================================================

	/** @brief Maximum multiplier for seed search iterations in CHD construction. */
	inline constexpr uint32_t MAX_SEED_SEARCH_MULTIPLIER = 100;

	//=====================================================================
	// String parsing constants
	//=====================================================================

	/** @brief Character set for null or whitespace detection in string parsing operations. */
	inline constexpr std::string_view NULL_OR_WHITESPACE = " \t\n\r\f\v";
}
