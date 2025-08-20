/**
 * @file ISO19848Constants.h
 * @brief Constants for ISO 19848 standard data types and validation
 * @details Compile-time constants for format type names and validation messages
 * @todo Development-only file - remove before production release
 */

#pragma once

namespace dnv::vista::sdk::constants::iso19848
{
	//=====================================================================
	// Version constants
	//=====================================================================

	/** @brief ISO 19848 version 2018 identifier */
	inline constexpr std::string_view ISO19848_VERSION_2018 = "v2018";

	/** @brief ISO 19848 version 2024 identifier */
	inline constexpr std::string_view ISO19848_VERSION_2024 = "v2024";

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
