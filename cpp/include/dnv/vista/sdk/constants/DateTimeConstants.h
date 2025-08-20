/**
 * @file DateTimeConstants.h
 * @brief DateTime and DateTimeOffset constants for cross-platform compatibility and ISO 8601 support
 * @details Defines boundary values, timezone limits, and time unit constants for
 *          cross-platform DateTime operations with ISO 8601 standard compliance
 * @todo Development-only file - remove before production release
 */

#pragma once

namespace dnv::vista::sdk::constants::datetime
{
	//=====================================================================
	// DateTime value boundaries
	//=====================================================================

	/** @brief Minimum DateTime value: January 1, 0001 00:00:00.000 UTC (100-nanosecond ticks) */
	inline constexpr std::int64_t MIN_DATETIME_TICKS = 0LL;

	/** @brief Maximum DateTime value: December 31, 9999 23:59:59.999 UTC (100-nanosecond ticks) */
	inline constexpr std::int64_t MAX_DATETIME_TICKS = 3155378975999999999LL;

	/** @brief Unix epoch: January 1, 1970 00:00:00.000 UTC (100-nanosecond ticks) */
	inline constexpr std::int64_t UNIX_EPOCH_TICKS = 621355968000000000LL;

	/** @brief Microsoft Windows FILETIME epoch: January 1, 1601 00:00:00.000 UTC (100-nanosecond ticks) */
	inline constexpr std::int64_t MICROSOFT_FILETIME_EPOCH_TICKS = 504911232000000000LL;

	//=====================================================================
	// Time unit conversion constants
	//=====================================================================

	/** @brief Ticks per millisecond (100-nanosecond units) */
	inline constexpr std::int64_t TICKS_PER_MILLISECOND = 10000LL;

	/** @brief Ticks per second */
	inline constexpr std::int64_t TICKS_PER_SECOND = TICKS_PER_MILLISECOND * 1000LL;

	/** @brief Ticks per minute */
	inline constexpr std::int64_t TICKS_PER_MINUTE = TICKS_PER_SECOND * 60LL;

	/** @brief Ticks per hour */
	inline constexpr std::int64_t TICKS_PER_HOUR = TICKS_PER_MINUTE * 60LL;

	/** @brief Ticks per day */
	inline constexpr std::int64_t TICKS_PER_DAY = TICKS_PER_HOUR * 24LL;

	//=====================================================================
	// Timezone offset limits (ISO 8601)
	//=====================================================================

	/** @brief Minimum timezone offset in minutes: -14:00 (UTC-14) */
	inline constexpr std::int32_t MIN_OFFSET_MINUTES = -14 * 60;

	/** @brief Maximum timezone offset in minutes: +14:00 (UTC+14) */
	inline constexpr std::int32_t MAX_OFFSET_MINUTES = 14 * 60;

	/** @brief Minimum timezone offset in seconds: -14:00:00 (UTC-14) */
	inline constexpr std::int32_t MIN_OFFSET_SECONDS = MIN_OFFSET_MINUTES * 60;

	/** @brief Maximum timezone offset in seconds: +14:00:00 (UTC+14) */
	inline constexpr std::int32_t MAX_OFFSET_SECONDS = MAX_OFFSET_MINUTES * 60;

	/** @brief UTC offset (zero) */
	inline constexpr std::int32_t UTC_OFFSET = 0;

	//=====================================================================
	// Calendar system constants (Gregorian)
	//=====================================================================

	//----------------------------------------------
	// Year boundaries
	//----------------------------------------------

	/** @brief Minimum year value */
	inline constexpr std::int32_t MIN_YEAR = 1;

	/** @brief Maximum year value */
	inline constexpr std::int32_t MAX_YEAR = 9999;

	//----------------------------------------------
	// Calendar cycle constants for date calculations
	//----------------------------------------------

	/** @brief Days per 400-year cycle (Gregorian calendar) */
	inline constexpr std::int32_t DAYS_PER_400_YEARS = 146097;

	/** @brief Days per 100-year cycle (Gregorian calendar) */
	inline constexpr std::int32_t DAYS_PER_100_YEARS = 36524;

	/** @brief Days per 4-year cycle (Gregorian calendar) */
	inline constexpr std::int32_t DAYS_PER_4_YEARS = 1461;

	/** @brief Days per year (non-leap) */
	inline constexpr std::int32_t DAYS_PER_YEAR = 365;

	//=====================================================================
	// ISO 8601 string formats
	//=====================================================================

	//----------------------------------------------
	// String format patterns
	//----------------------------------------------

	/** @brief Basic ISO 8601 datetime format pattern */
	inline constexpr std::string_view ISO8601_BASIC_PATTERN = "yyyy-MM-ddTHH:mm:ssZ";

	/** @brief Extended ISO 8601 datetime format pattern with fractional seconds */
	inline constexpr std::string_view ISO8601_EXTENDED_PATTERN = "yyyy-MM-ddTHH:mm:ss.fffffffK";

	/** @brief ISO 8601 date-only format pattern */
	inline constexpr std::string_view ISO8601_DATE_PATTERN = "yyyy-MM-dd";

	/** @brief ISO 8601 time-only format pattern */
	inline constexpr std::string_view ISO8601_TIME_PATTERN = "HH:mm:ss";

	//----------------------------------------------
	// String format limits (for buffer allocation)
	//----------------------------------------------

	/**
	 * @brief Maximum length of ISO 8601 datetime string
	 * @details "2024-12-31T23:59:59.9999999+14:00" format
	 */
	inline constexpr std::size_t MAX_ISO8601_LENGTH = 35;

	/**
	 * @brief Minimum length of ISO 8601 datetime string
	 * @details "2024-01-01T00:00:00Z" format
	 */
	inline constexpr std::size_t MIN_ISO8601_LENGTH = 20;

	//=====================================================================
	// Unix epoch format limits
	//=====================================================================

	/** @brief Maximum length of Unix timestamp in seconds (10 digits for year 2286) */
	inline constexpr std::size_t MAX_UNIX_SECONDS_LENGTH = 10;

	/** @brief Maximum length of Unix timestamp in milliseconds (13 digits) */
	inline constexpr std::size_t MAX_UNIX_MILLISECONDS_LENGTH = 13;

	/** @brief Current typical length of Unix timestamp in seconds (10 digits until 2286) */
	inline constexpr std::size_t CURRENT_UNIX_SECONDS_LENGTH = 10;
}
