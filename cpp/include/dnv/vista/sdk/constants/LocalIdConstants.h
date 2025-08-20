/**
 * @file LocalIdConstants.h
 * @brief LocalId parsing and string building constants
 * @details Contains prefix constants used for LocalId parsing state transitions
 *          and string serialization operations
 * @todo Development-only file - remove before production release
 */

#pragma once

namespace dnv::vista::sdk::constants::localId
{
	//----------------------------------------------
	// LocalId prefix constants
	//----------------------------------------------

	/** @brief Standard naming rule prefix for Local IDs */
	inline constexpr std::string_view NAMING_RULE = "dnv-v2";

	/** @brief VIS version prefix */
	inline constexpr std::string_view PREFIX_VIS = "vis-";

	/** @brief Secondary item prefix */
	inline constexpr std::string_view PREFIX_SEC = "sec";

	/** @brief Initial secondary verbose prefix */
	inline constexpr std::string_view PREFIX_FOR = "~for.";

	/** @brief Subsequent secondary verbose prefix */
	inline constexpr std::string_view PREFIX_TILDE = "~";

	/** @brief Dash separator for metadata tag values */
	inline constexpr std::string_view PREFIX_DASH = "-";

	/** @brief Metadata section prefix */
	inline constexpr std::string_view PREFIX_META = "meta";

	//----------------------------------------------
	// LocalId char constants
	//----------------------------------------------

	/** @brief Tilde character for custom values and verbose mode */
	inline constexpr char CHAR_TILDE = '~';

	/** @brief Dash character for metadata separators */
	inline constexpr char CHAR_DASH = '-';

	//----------------------------------------------
	// LocalId path constants
	//----------------------------------------------

	/** @brief Metadata path with leading slash */
	inline constexpr std::string_view PATH_META = "/meta";

	/** @brief Secondary path with leading slash */
	inline constexpr std::string_view PATH_SEC = "/sec";

	//----------------------------------------------
	// Metadata prefix mappings
	//----------------------------------------------

	/** @brief Short quantity prefix */
	inline constexpr std::string_view META_QTY_SHORT = "q";
	/** @brief Long quantity prefix */
	inline constexpr std::string_view META_QTY_LONG = "qty";

	/** @brief Short content prefix */
	inline constexpr std::string_view META_CNT_SHORT = "c";
	/** @brief Long content prefix */
	inline constexpr std::string_view META_CNT_LONG = "cnt";

	/** @brief Short calculation prefix */
	inline constexpr std::string_view META_CALC_SHORT = "cal";
	/** @brief Long calculation prefix */
	inline constexpr std::string_view META_CALC_LONG = "calc";

	/** @brief Short state prefix */
	inline constexpr std::string_view META_STATE_SHORT = "s";
	/** @brief Long state prefix */
	inline constexpr std::string_view META_STATE_LONG = "state";

	/** @brief Short command prefix */
	inline constexpr std::string_view META_CMD_SHORT = "cmd";
	/** @brief Long command prefix */
	inline constexpr std::string_view META_CMD_LONG = "command";

	/** @brief Short type prefix */
	inline constexpr std::string_view META_TYPE_SHORT = "t";
	/** @brief Long type prefix */
	inline constexpr std::string_view META_TYPE_LONG = "type";

	/** @brief Position prefix */
	inline constexpr std::string_view META_POS_SHORT = "pos";
	/** @brief Long position prefix */
	inline constexpr std::string_view META_POS_LONG = "position";

	/** @brief Short detail prefix */
	inline constexpr std::string_view META_DETAIL_SHORT = "d";
	/** @brief Long detail prefix */
	inline constexpr std::string_view META_DETAIL_LONG = "detail";
}
