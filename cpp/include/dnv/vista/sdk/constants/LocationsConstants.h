/**
 * @file LocationsConstants.h
 * @brief Location parsing and validation constants
 * @details Contains character codes, group names, and error message templates
 *          for location parsing and validation operations
 * @todo Development-only file - remove before production release
 */

#pragma once

namespace dnv::vista::sdk::constants::locations
{
	//----------------------------------------------
	// Location character constants
	//----------------------------------------------

	/* Special characters */
	inline constexpr char CHAR_HORIZONTAL = 'H';
	inline constexpr char CHAR_VERTICAL = 'V';
	inline constexpr char CHAR_NUMBER = 'N';

	/* Side group */
	inline constexpr char CHAR_PORT = 'P';
	inline constexpr char CHAR_CENTER = 'C';
	inline constexpr char CHAR_STARBOARD = 'S';

	/* Vertical group */
	inline constexpr char CHAR_UPPER = 'U';
	inline constexpr char CHAR_MIDDLE = 'M';
	inline constexpr char CHAR_LOWER = 'L';

	/* Transverse group */
	inline constexpr char CHAR_INBOARD = 'I';
	inline constexpr char CHAR_OUTBOARD = 'O';

	/* Longitudinal group */
	inline constexpr char CHAR_FORWARD = 'F';
	inline constexpr char CHAR_AFT = 'A';

	//----------------------------------------------
	// Group name constants
	//----------------------------------------------

	inline constexpr std::string_view GROUP_NAME_NUMBER = "Number";
	inline constexpr std::string_view GROUP_NAME_SIDE = "Side";
	inline constexpr std::string_view GROUP_NAME_VERTICAL = "Vertical";
	inline constexpr std::string_view GROUP_NAME_TRANSVERSE = "Transverse";
	inline constexpr std::string_view GROUP_NAME_LONGITUDINAL = "Longitudinal";
	inline constexpr std::string_view GROUP_NAME_UNKNOWN = "Unknown";
}
