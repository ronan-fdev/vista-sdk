/**
 * @file ISO19848Constants.h
 * @brief Constants for ISO 19848 standard data types and validation
 * @details Compile-time constants for format type names and validation messages
 * @todo Development-only file - remove before production release
 */

#pragma once

#include <cstdint>
#include <string_view>

namespace dnv::vista::sdk::constants::iso19848
{
	//=====================================================================
	// ISO 19848 Annex C naming constants
	//=====================================================================

	/** @brief ISO 19848 Annex C naming entity */
	inline constexpr std::string_view ANNEX_C_NAMING_ENTITY = "data.dnv.com";

	/** @brief Default reference URL for ISO 19848 documentation */
	inline constexpr std::string_view ANNEX_C_REFERENCE_URL = "https://docs.vista.dnv.com";

	/** @brief ISO 19848 Annex C naming rule identifier */
	inline constexpr std::string_view ANNEX_C_NAMING_RULE_PREFIX = "dnv";

	/** @brief ISO 19848 Annex C naming scheme version */
	inline constexpr std::string_view ANNEX_C_NAMING_SCHEME_VERSION = "v2";

	/** @brief ISO 19848 Annex C naming rule */
	inline constexpr std::string_view ANNEX_C_NAMING_RULE = "dnv-v2";

	//=====================================================================
	// Version constants
	//=====================================================================

	/** @brief ISO 19848 version 2018 identifier */
	inline constexpr std::string_view VERSION_2018 = "v2018";

	/** @brief ISO 19848 version 2024 identifier */
	inline constexpr std::string_view VERSION_2024 = "v2024";

	//=====================================================================
	// Format type constants
	//=====================================================================

	/** @brief String format type identifier */
	inline constexpr std::string_view FORMAT_TYPE_STRING = "String";

	/** @brief Character format type identifier */
	inline constexpr std::string_view FORMAT_TYPE_CHAR = "Char";

	/** @brief Boolean format type identifier */
	inline constexpr std::string_view FORMAT_TYPE_BOOLEAN = "Boolean";

	/** @brief Integer format type identifier */
	inline constexpr std::string_view FORMAT_TYPE_INTEGER = "Integer";

	/** @brief UnsignedInteger format type identifier */
	inline constexpr std::string_view FORMAT_TYPE_UNSIGNED_INTEGER = "UnsignedInteger";

	/** @brief Long format type identifier */
	inline constexpr std::string_view FORMAT_TYPE_LONG = "Long";

	/** @brief Double format type identifier */
	inline constexpr std::string_view FORMAT_TYPE_DOUBLE = "Double";

	/** @brief Decimal format type identifier */
	inline constexpr std::string_view FORMAT_TYPE_DECIMAL = "Decimal";

	/** @brief DateTime format type identifier */
	inline constexpr std::string_view FORMAT_TYPE_DATETIME = "DateTime";

	//=====================================================================
	// Boolean value constants
	//=====================================================================

	/** @brief Boolean true value string */
	inline constexpr std::string_view BOOLEAN_TRUE_VALUE = "true";

	/** @brief Boolean false value string */
	inline constexpr std::string_view BOOLEAN_FALSE_VALUE = "false";

	//=====================================================================
	// DateTime format constants
	//=====================================================================

	/** @brief ISO 8601 datetime format pattern */
	inline constexpr std::string_view DATETIME_FORMAT_PATTERN = "%Y-%m-%dT%H:%M:%SZ";
}
