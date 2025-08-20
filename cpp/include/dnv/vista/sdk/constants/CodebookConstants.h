/**
 * @file CodebookConstants.h
 * @brief Codebook-specific constants for Vista SDK codebook operations
 * @details Contains codebook names, prefixes, validation groups, and result constants
 *          for efficient codebook processing and position validation
 * @todo Development-only file - remove before production release
 */

#pragma once

namespace dnv::vista::sdk::constants::codebook
{
	//----------------------------------------------
	// Codebooks names
	//----------------------------------------------

	/** @brief Standard position codebook name identifier. */
	inline constexpr std::string_view CODEBOOK_NAME_POSITION = "positions";

	/** @brief Standard calculation codebook name identifier. */
	inline constexpr std::string_view CODEBOOK_NAME_CALCULATION = "calculations";

	/** @brief Standard quantity codebook name identifier. */
	inline constexpr std::string_view CODEBOOK_NAME_QUANTITY = "quantities";

	/** @brief Standard state codebook name identifier. */
	inline constexpr std::string_view CODEBOOK_NAME_STATE = "states";

	/** @brief Standard content codebook name identifier. */
	inline constexpr std::string_view CODEBOOK_NAME_CONTENT = "contents";

	/** @brief Standard command codebook name identifier. */
	inline constexpr std::string_view CODEBOOK_NAME_COMMAND = "commands";

	/** @brief Standard type codebook name identifier. */
	inline constexpr std::string_view CODEBOOK_NAME_TYPE = "types";

	/** @brief Functional services codebook name identifier. */
	inline constexpr std::string_view CODEBOOK_NAME_FUNCTIONAL_SERVICES = "functional_services";

	/** @brief Maintenance category codebook name identifier. */
	inline constexpr std::string_view CODEBOOK_NAME_MAINTENANCE_CATEGORY = "maintenance_category";

	/** @brief Activity type codebook name identifier. */
	inline constexpr std::string_view CODEBOOK_NAME_ACTIVITY_TYPE = "activity_type";

	/** @brief Detail codebook name identifier. */
	inline constexpr std::string_view CODEBOOK_NAME_DETAIL = "detail";

	//----------------------------------------------
	// Codebooks prefix constants
	//----------------------------------------------

	/** @brief Prefix identifier for position-related codebook entries. */
	inline constexpr std::string_view CODEBOOK_PREFIX_POSITION = "pos";

	/** @brief Prefix identifier for quantity-related codebook entries. */
	inline constexpr std::string_view CODEBOOK_PREFIX_QUANTITY = "qty";

	/** @brief Prefix identifier for calculation-related codebook entries. */
	inline constexpr std::string_view CODEBOOK_PREFIX_CALCULATION = "calc";

	/** @brief Prefix identifier for state-related codebook entries. */
	inline constexpr std::string_view CODEBOOK_PREFIX_STATE = "state";

	/** @brief Prefix identifier for content-related codebook entries. */
	inline constexpr std::string_view CODEBOOK_PREFIX_CONTENT = "cnt";

	/** @brief Prefix identifier for command-related codebook entries. */
	inline constexpr std::string_view CODEBOOK_PREFIX_COMMAND = "cmd";

	/** @brief Prefix identifier for type-related codebook entries. */
	inline constexpr std::string_view CODEBOOK_PREFIX_TYPE = "type";

	/** @brief Prefix identifier for functional services codebook entries. */
	inline constexpr std::string_view CODEBOOK_PREFIX_FUNCTIONAL_SERVICES = "funct.svc";

	/** @brief Prefix identifier for maintenance category codebook entries. */
	inline constexpr std::string_view CODEBOOK_PREFIX_MAINTENANCE_CATEGORY = "maint.cat";

	/** @brief Prefix identifier for activity type codebook entries. */
	inline constexpr std::string_view CODEBOOK_PREFIX_ACTIVITY_TYPE = "act.type";

	/** @brief Prefix identifier for detail-related codebook entries. */
	inline constexpr std::string_view CODEBOOK_PREFIX_DETAIL = "detail";

	//----------------------------------------------
	// Position validation group constants
	//----------------------------------------------

	/** @brief Special group identifier for numeric-only values in VISTA codebooks. */
	inline constexpr std::string_view CODEBOOK_GROUP_NUMBER = "<number>";

	/** @brief Default group name for ungrouped position components. */
	inline constexpr std::string_view CODEBOOK_GROUP_DEFAULT = "DEFAULT_GROUP";

	/** @brief Fallback group identifier for unrecognized position components. */
	inline constexpr std::string_view CODEBOOK_GROUP_UNKNOWN = "UNKNOWN";

	//----------------------------------------------
	// Position validation results constants
	//----------------------------------------------

	/** @brief Result constant indicating invalid position validation. */
	inline constexpr std::string_view CODEBOOK_POSITION_VALIDATION_INVALID = "invalid";

	/** @brief Result constant indicating invalid position component ordering. */
	inline constexpr std::string_view CODEBOOK_POSITION_VALIDATION_INVALID_ORDER = "invalidorder";

	/** @brief Result constant indicating invalid position component grouping. */
	inline constexpr std::string_view CODEBOOK_POSITION_VALIDATION_INVALID_GROUPING = "invalidgrouping";

	/** @brief Result constant indicating valid position validation. */
	inline constexpr std::string_view CODEBOOK_POSITION_VALIDATION_VALID = "valid";

	/** @brief Result constant indicating custom position validation rules applied. */
	inline constexpr std::string_view CODEBOOK_POSITION_VALIDATION_CUSTOM = "custom";
}
