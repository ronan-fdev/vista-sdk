/**
 * @file DateTimeISO8601.h
 * @brief Cross-platform UTC DateTime class with ISO 8601 support
 * @details Provides UTC-only datetime operations with 100-nanosecond precision,
 *          ISO 8601 parsing/formatting, and compatibility with system clocks
 *
 * @note Design inspired by .NET DateTime/DateTimeOffset and TimeSpan semantics
 *       (100 ns ticks, ranges, and formatting) to match the original C# SDK API.
 */

#pragma once

namespace dnv::vista::sdk::datatypes
{
	//=====================================================================
	// TimeSpan class
	//=====================================================================

	/**
	 * @brief Represents a time interval in 100-nanosecond ticks
	 */
	class TimeSpan final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Construct from ticks (100-nanosecond units) */
		explicit inline constexpr TimeSpan( std::int64_t ticks = 0 ) noexcept;

		/** @brief Copy constructor */
		TimeSpan( const TimeSpan& ) = default;

		/** @brief Move constructor */
		TimeSpan( TimeSpan&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~TimeSpan() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		TimeSpan& operator=( const TimeSpan& ) = default;

		/** @brief Move assignment operator */
		TimeSpan& operator=( TimeSpan&& ) noexcept = default;

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		constexpr inline bool operator==( const TimeSpan& other ) const noexcept;
		constexpr inline bool operator!=( const TimeSpan& other ) const noexcept;
		constexpr inline bool operator<( const TimeSpan& other ) const noexcept;
		constexpr inline bool operator<=( const TimeSpan& other ) const noexcept;
		constexpr inline bool operator>( const TimeSpan& other ) const noexcept;
		constexpr inline bool operator>=( const TimeSpan& other ) const noexcept;

		//----------------------------------------------
		// Arithmetic operators
		//----------------------------------------------

		constexpr inline TimeSpan operator+( const TimeSpan& other ) const noexcept;
		constexpr inline TimeSpan operator-( const TimeSpan& other ) const noexcept;
		constexpr inline TimeSpan operator-() const noexcept;
		constexpr inline TimeSpan& operator+=( const TimeSpan& other ) noexcept;
		constexpr inline TimeSpan& operator-=( const TimeSpan& other ) noexcept;

		//----------------------------------------------
		// Property accessors
		//----------------------------------------------

		/** @brief Get tick count */
		[[nodiscard]] constexpr inline std::int64_t ticks() const noexcept;

		/** @brief Get total days */
		[[nodiscard]] constexpr inline double totalDays() const noexcept;

		/** @brief Get total hours */
		[[nodiscard]] constexpr inline double totalHours() const noexcept;

		/** @brief Get total minutes */
		[[nodiscard]] constexpr inline double totalMinutes() const noexcept;

		/** @brief Get total seconds */
		[[nodiscard]] constexpr inline double totalSeconds() const noexcept;

		/** @brief Get total milliseconds */
		[[nodiscard]] constexpr inline double totalMilliseconds() const noexcept;

		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/** @brief Static factory: Create from days */
		[[nodiscard]] inline static constexpr TimeSpan fromDays( double days ) noexcept;

		/** @brief Static factory: Create from hours */
		[[nodiscard]] inline static constexpr TimeSpan fromHours( double hours ) noexcept;

		/** @brief Static factory: Create from minutes */
		[[nodiscard]] inline static constexpr TimeSpan fromMinutes( double minutes ) noexcept;

		/** @brief Static factory: Create from seconds */
		[[nodiscard]] inline static constexpr TimeSpan fromSeconds( double seconds ) noexcept;

		/** @brief Static factory: Create from milliseconds */
		[[nodiscard]] inline static constexpr TimeSpan fromMilliseconds( double milliseconds ) noexcept;

	private:
		/** @brief 100-nanosecond ticks since January 1, 0001 UTC */
		std::int64_t m_ticks;
	};

	//=====================================================================
	// DateTime class
	//=====================================================================

	/**
	 * @brief Cross-platform UTC DateTime type with 100-nanosecond precision
	 * @details Implements UTC-only datetime operations with:
	 *          - 100-nanosecond tick precision (matching .NET DateTime)
	 *          - Range: January 1, 0001 to December 31, 9999 UTC
	 *          - ISO 8601 string parsing and formatting
	 *          - System clock interoperability
	 *          - Arithmetic operations with time intervals
	 *
	 *          Note: This is UTC-only. For timezone-aware operations, use DateTimeOffset.
	 */
	class DateTime final
	{
	public:
		//----------------------------------------------
		// Enumerations
		//----------------------------------------------

		/**
		 * @brief DateTime string format options
		 * @details Provides type-safe format selection with self-documenting format names
		 */
		enum class Format : std::uint8_t
		{
			/** @brief ISO 8601 basic format: "2024-01-01T12:00:00Z" */
			Iso8601Basic,

			/** @brief ISO 8601 extended format with fractional seconds: "2024-01-01T12:00:00.1234567Z" */
			Iso8601Extended,

			/** @brief Date and time with timezone: "2024-01-01T12:00:00+02:00" */
			Iso8601WithOffset,

			/** @brief Date only format: "2024-01-01" */
			DateOnly,

			/** @brief Time only: "12:00:00" */
			TimeOnly,

			/** @brief Unix timestamp format: "1704110400" (seconds since epoch) */
			UnixSeconds,

			/** @brief Unix timestamp with milliseconds: "1704110400123" */
			UnixMilliseconds,
		};

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor (minimum DateTime value) */
		constexpr DateTime() noexcept;

		/** @brief Construct from tick count (100-nanosecond units since year 1) */
		explicit constexpr DateTime( std::int64_t ticks ) noexcept;

		/** @brief Construct from system clock time point */
		explicit DateTime( std::chrono::system_clock::time_point timePoint ) noexcept;

		/** @brief Construct from date components (UTC) */
		DateTime( std::int32_t year, std::int32_t month, std::int32_t day ) noexcept;

		/** @brief Construct from date and time components (UTC) */
		DateTime( std::int32_t year, std::int32_t month, std::int32_t day,
			std::int32_t hour, std::int32_t minute, std::int32_t second ) noexcept;

		/** @brief Construct from date and time components with milliseconds (UTC) */
		DateTime( std::int32_t year, std::int32_t month, std::int32_t day,
			std::int32_t hour, std::int32_t minute, std::int32_t second,
			std::int32_t millisecond ) noexcept;

		/** @brief Parse from ISO 8601 string */
		explicit DateTime( std::string_view iso8601String );

		/** @brief Copy constructor */
		DateTime( const DateTime& ) = default;

		/** @brief Move constructor */
		DateTime( DateTime&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DateTime() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		DateTime& operator=( const DateTime& ) = default;

		/** @brief Move assignment operator */
		DateTime& operator=( DateTime&& ) noexcept = default;

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		/** @brief Equality comparison */
		bool operator==( const DateTime& other ) const noexcept;

		/** @brief Inequality comparison */
		bool operator!=( const DateTime& other ) const noexcept;

		/** @brief Less than comparison */
		bool operator<( const DateTime& other ) const noexcept;

		/** @brief Less than or equal comparison */
		bool operator<=( const DateTime& other ) const noexcept;

		/** @brief Greater than comparison */
		bool operator>( const DateTime& other ) const noexcept;

		/** @brief Greater than or equal comparison */
		bool operator>=( const DateTime& other ) const noexcept;

		//----------------------------------------------
		// Arithmetic operators
		//----------------------------------------------

		/** @brief Add time duration */
		DateTime operator+( const TimeSpan& duration ) const noexcept;

		/** @brief Subtract time duration */
		DateTime operator-( const TimeSpan& duration ) const noexcept;

		/** @brief Get time difference between DateTimes */
		TimeSpan operator-( const DateTime& other ) const noexcept;

		/** @brief Add time duration (in-place) */
		DateTime& operator+=( const TimeSpan& duration ) noexcept;

		/** @brief Subtract time duration (in-place) */
		DateTime& operator-=( const TimeSpan& duration ) noexcept;

		//----------------------------------------------
		// Property accessors
		//----------------------------------------------

		/** @brief Get tick count (100-nanosecond units since year 1) */
		[[nodiscard]] constexpr std::int64_t ticks() const noexcept;

		/** @brief Get year component (1-9999) */
		[[nodiscard]] std::int32_t year() const noexcept;

		/** @brief Get month component (1-12) */
		[[nodiscard]] std::int32_t month() const noexcept;

		/** @brief Get day component (1-31) */
		[[nodiscard]] std::int32_t day() const noexcept;

		/** @brief Get hour component (0-23) */
		[[nodiscard]] std::int32_t hour() const noexcept;

		/** @brief Get minute component (0-59) */
		[[nodiscard]] std::int32_t minute() const noexcept;

		/** @brief Get second component (0-59) */
		[[nodiscard]] std::int32_t second() const noexcept;

		/** @brief Get millisecond component (0-999) */
		[[nodiscard]] std::int32_t millisecond() const noexcept;

		/** @brief Get day of week (0=Sunday, 6=Saturday) */
		[[nodiscard]] std::int32_t dayOfWeek() const noexcept;

		/** @brief Get day of year (1-366) */
		[[nodiscard]] std::int32_t dayOfYear() const noexcept;

		//----------------------------------------------
		// Conversion methods
		//----------------------------------------------

		/** @brief Convert to Unix timestamp (seconds since epoch) */
		[[nodiscard]] std::int64_t toUnixSeconds() const noexcept;

		/** @brief Convert to Unix timestamp (milliseconds since epoch) */
		[[nodiscard]] std::int64_t toUnixMilliseconds() const noexcept;

		/** @brief Get date component (time set to 00:00:00) */
		[[nodiscard]] DateTime date() const noexcept;

		/** @brief Get time of day as duration since midnight */
		[[nodiscard]] TimeSpan timeOfDay() const noexcept;

		//----------------------------------------------
		// String formatting
		//----------------------------------------------

		/** @brief Convert to ISO 8601 string (basic format) */
		[[nodiscard]] std::string toString() const;

		/** @brief Convert to string using specified format */
		[[nodiscard]] std::string toString( Format format ) const;

		/** @brief Convert to ISO 8601 extended format with full precision */
		[[nodiscard]] std::string toIso8601Extended() const;

		//----------------------------------------------
		// Validation methods
		//----------------------------------------------

		/** @brief Check if this DateTime is valid */
		[[nodiscard]] inline bool isValid() const noexcept;

		/** @brief Check if given year is a leap year */
		[[nodiscard]] inline static constexpr bool isLeapYear( std::int32_t year ) noexcept;

		/** @brief Get days in month for given year and month */
		[[nodiscard]] inline static constexpr std::int32_t daysInMonth( std::int32_t year, std::int32_t month ) noexcept;

		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/** @brief Get current UTC time */
		[[nodiscard]] static DateTime now() noexcept;

		/** @brief Get current UTC date (time set to 00:00:00) */
		[[nodiscard]] static DateTime today() noexcept;

		/** @brief Get minimum DateTime value */
		[[nodiscard]] static constexpr DateTime minValue() noexcept;

		/** @brief Get maximum DateTime value */
		[[nodiscard]] static constexpr DateTime maxValue() noexcept;

		/** @brief Get Unix epoch DateTime (January 1, 1970 00:00:00 UTC) */
		[[nodiscard]] static constexpr DateTime epoch() noexcept;

		/** @brief Try parse ISO 8601 string without throwing */
		[[nodiscard]] static bool tryParse( std::string_view str, DateTime& result ) noexcept;

		/** @brief Create from Unix timestamp (seconds since epoch) */
		[[nodiscard]] static DateTime sinceEpochSeconds( std::int64_t seconds ) noexcept;

		/** @brief Create from Unix timestamp (milliseconds since epoch) */
		[[nodiscard]] static DateTime sinceEpochMilliseconds( std::int64_t milliseconds ) noexcept;

		//----------------------------------------------
		// std::chrono interoperability
		//----------------------------------------------

		/** @brief Convert to std::chrono::system_clock::time_point */
		[[nodiscard]] std::chrono::system_clock::time_point toChrono() const noexcept;

		/** @brief Create DateTime from std::chrono::system_clock::time_point */
		[[nodiscard]] static DateTime fromChrono( const std::chrono::system_clock::time_point& timePoint ) noexcept;

	private:
		//----------------------------------------------
		// Internal data
		//----------------------------------------------

		/** @brief 100-nanosecond ticks since January 1, 0001 UTC */
		std::int64_t m_ticks;

		//----------------------------------------------
		// Internal helper methods
		//----------------------------------------------

		/** @brief Convert ticks to date components */
		void getDateComponents( std::int32_t& year, std::int32_t& month, std::int32_t& day ) const noexcept;

		/** @brief Convert ticks to time components */
		void getTimeComponents( std::int32_t& hour, std::int32_t& minute, std::int32_t& second, std::int32_t& millisecond ) const noexcept;

		/** @brief Convert date components to ticks */
		static std::int64_t dateToTicks( std::int32_t year, std::int32_t month, std::int32_t day ) noexcept;

		/** @brief Convert time components to ticks */
		static std::int64_t timeToTicks( std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t millisecond ) noexcept;

		/** @brief Validate date components */
		static bool isValidDate( std::int32_t year, std::int32_t month, std::int32_t day ) noexcept;

		/** @brief Validate time components */
		static bool isValidTime( std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t millisecond ) noexcept;
	};

	//=====================================================================
	// DateTimeOffset class
	//=====================================================================

	/**
	 * @brief Timezone-aware DateTime with UTC offset
	 * @details Represents a point in time, typically expressed as a date and time of day,
	 *          relative to Coordinated Universal Time (UTC). Composed of a DateTime value
	 *          and a TimeSpan offset that defines the difference from UTC.
	 *
	 *          Key features:
	 *          - 100-nanosecond tick precision for maximum accuracy
	 *          - Range: January 1, 0001 to December 31, 9999 with offset ±14:00:00
	 *          - ISO 8601 string parsing and formatting with timezone offset
	 *          - Cross-platform timezone-aware operations
	 *          - Arithmetic operations that account for timezone offsets
	 *          - Unix timestamp support with timezone awareness
	 */
	class DateTimeOffset final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor (minimum DateTimeOffset value with zero offset) */
		inline constexpr DateTimeOffset() noexcept;

		/** @brief Construct from DateTime and offset */
		inline constexpr DateTimeOffset( const DateTime& dateTime, const TimeSpan& offset ) noexcept;

		/** @brief Construct from DateTime (assumes local timezone offset) */
		explicit DateTimeOffset( const DateTime& dateTime ) noexcept;

		/** @brief Construct from tick count and offset */
		inline constexpr DateTimeOffset( std::int64_t ticks, const TimeSpan& offset ) noexcept;

		/** @brief Construct from date components and offset (UTC) */
		DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, const TimeSpan& offset ) noexcept;

		/** @brief Construct from date and time components and offset (UTC) */
		DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second,
			const TimeSpan& offset ) noexcept;

		/** @brief Construct from date and time components with milliseconds and offset (UTC) */
		DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second,
			std::int32_t millisecond, const TimeSpan& offset ) noexcept;

		/** @brief Construct from date and time components with microseconds and offset (UTC) */
		DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second,
			std::int32_t millisecond, std::int32_t microsecond, const TimeSpan& offset ) noexcept;

		/** @brief Parse from ISO 8601 string with timezone offset */
		explicit DateTimeOffset( std::string_view iso8601String );

		/** @brief Copy constructor */
		DateTimeOffset( const DateTimeOffset& ) = default;

		/** @brief Move constructor */
		DateTimeOffset( DateTimeOffset&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DateTimeOffset() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		DateTimeOffset& operator=( const DateTimeOffset& ) = default;

		/** @brief Move assignment operator */
		DateTimeOffset& operator=( DateTimeOffset&& ) noexcept = default;

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		/** @brief Equality comparison (compares UTC values) */
		inline bool operator==( const DateTimeOffset& other ) const noexcept;

		/** @brief Inequality comparison (compares UTC values) */
		inline bool operator!=( const DateTimeOffset& other ) const noexcept;

		/** @brief Less than comparison (compares UTC values) */
		inline bool operator<( const DateTimeOffset& other ) const noexcept;

		/** @brief Less than or equal comparison (compares UTC values) */
		inline bool operator<=( const DateTimeOffset& other ) const noexcept;

		/** @brief Greater than comparison (compares UTC values) */
		inline bool operator>( const DateTimeOffset& other ) const noexcept;

		/** @brief Greater than or equal comparison (compares UTC values) */
		inline bool operator>=( const DateTimeOffset& other ) const noexcept;

		//----------------------------------------------
		// Arithmetic operators
		//----------------------------------------------

		/** @brief Add time duration */
		inline DateTimeOffset operator+( const TimeSpan& duration ) const noexcept;

		/** @brief Subtract time duration */
		inline DateTimeOffset operator-( const TimeSpan& duration ) const noexcept;

		/** @brief Get time difference between DateTimeOffsets (in UTC) */
		inline TimeSpan operator-( const DateTimeOffset& other ) const noexcept;

		/** @brief Add time duration (in-place) */
		inline DateTimeOffset& operator+=( const TimeSpan& duration ) noexcept;

		/** @brief Subtract time duration (in-place) */
		inline DateTimeOffset& operator-=( const TimeSpan& duration ) noexcept;

		//----------------------------------------------
		// Property accessors
		//----------------------------------------------

		/** @brief Get the DateTime component (local time) */
		[[nodiscard]] inline constexpr const DateTime& dateTime() const noexcept;

		/** @brief Get the offset from UTC */
		[[nodiscard]] inline constexpr const TimeSpan& offset() const noexcept;

		/** @brief Get UTC DateTime equivalent */
		[[nodiscard]] DateTime utcDateTime() const noexcept;

		/** @brief Get local DateTime equivalent */
		[[nodiscard]] DateTime localDateTime() const noexcept;

		/** @brief Get tick count (100-nanosecond units of local time) */
		[[nodiscard]] inline constexpr std::int64_t ticks() const noexcept;

		/** @brief Get UTC tick count */
		[[nodiscard]] inline std::int64_t utcTicks() const noexcept;

		/** @brief Get year component (1-9999) */
		[[nodiscard]] inline std::int32_t year() const noexcept;

		/** @brief Get month component (1-12) */
		[[nodiscard]] inline std::int32_t month() const noexcept;

		/** @brief Get day component (1-31) */
		[[nodiscard]] inline std::int32_t day() const noexcept;

		/** @brief Get hour component (0-23) */
		[[nodiscard]] inline std::int32_t hour() const noexcept;

		/** @brief Get minute component (0-59) */
		[[nodiscard]] inline std::int32_t minute() const noexcept;

		/** @brief Get second component (0-59) */
		[[nodiscard]] inline std::int32_t second() const noexcept;

		/** @brief Get millisecond component (0-999) */
		[[nodiscard]] inline std::int32_t millisecond() const noexcept;

		/** @brief Get microsecond component (0-999) */
		[[nodiscard]] inline std::int32_t microsecond() const noexcept;

		/** @brief Get nanosecond component (0-900, in hundreds) */
		[[nodiscard]] inline std::int32_t nanosecond() const noexcept;

		/** @brief Get day of week (0=Sunday, 6=Saturday) */
		[[nodiscard]] inline std::int32_t dayOfWeek() const noexcept;

		/** @brief Get day of year (1-366) */
		[[nodiscard]] inline std::int32_t dayOfYear() const noexcept;

		/** @brief Get offset in total minutes */
		[[nodiscard]] inline std::int32_t totalOffsetMinutes() const noexcept;

		//----------------------------------------------
		// Conversion methods
		//----------------------------------------------

		/** @brief Convert to Unix timestamp (seconds since epoch) */
		[[nodiscard]] inline std::int64_t toUnixSeconds() const noexcept;

		/** @brief Convert to Unix timestamp (milliseconds since epoch) */
		[[nodiscard]] inline std::int64_t toUnixMilliseconds() const noexcept;

		/** @brief Get date component (time set to 00:00:00) */
		[[nodiscard]] DateTimeOffset date() const noexcept;

		/** @brief Get time of day as duration since midnight */
		[[nodiscard]] inline TimeSpan timeOfDay() const noexcept;

		/** @brief Convert to specified offset */
		[[nodiscard]] DateTimeOffset toOffset( const TimeSpan& newOffset ) const noexcept;

		/** @brief Convert to UTC (offset = 00:00:00) */
		[[nodiscard]] DateTimeOffset toUniversalTime() const noexcept;

		/** @brief Convert to local time (system timezone) */
		[[nodiscard]] DateTimeOffset toLocalTime() const noexcept;

		/**
		 * @brief Convert to Windows FILETIME format
		 * @details Works on any platform - performs mathematical epoch conversion only.
		 *          Useful for interoperability with Windows-originated data on any system.
		 * @return 100-nanosecond intervals since January 1, 1601 UTC
		 */
		[[nodiscard]] std::int64_t toFILETIME() const noexcept;

		//----------------------------------------------
		// Arithmetic methods
		//----------------------------------------------

		/** @brief Add time span */
		[[nodiscard]] inline DateTimeOffset add( const TimeSpan& value ) const noexcept;

		/** @brief Add days */
		[[nodiscard]] DateTimeOffset addDays( double days ) const noexcept;

		/** @brief Add hours */
		[[nodiscard]] DateTimeOffset addHours( double hours ) const noexcept;

		/** @brief Add milliseconds */
		[[nodiscard]] DateTimeOffset addMilliseconds( double milliseconds ) const noexcept;

		/** @brief Add minutes */
		[[nodiscard]] DateTimeOffset addMinutes( double minutes ) const noexcept;

		/** @brief Add months */
		[[nodiscard]] DateTimeOffset addMonths( std::int32_t months ) const noexcept;

		/** @brief Add seconds */
		[[nodiscard]] DateTimeOffset addSeconds( double seconds ) const noexcept;

		/** @brief Add ticks */
		[[nodiscard]] inline DateTimeOffset addTicks( std::int64_t ticks ) const noexcept;

		/** @brief Add years */
		[[nodiscard]] DateTimeOffset addYears( std::int32_t years ) const noexcept;

		/** @brief Subtract DateTimeOffset and return TimeSpan */
		[[nodiscard]] inline TimeSpan subtract( const DateTimeOffset& value ) const noexcept;

		/** @brief Subtract TimeSpan and return DateTimeOffset */
		[[nodiscard]] inline DateTimeOffset subtract( const TimeSpan& value ) const noexcept;

		//----------------------------------------------
		// String formatting
		//----------------------------------------------

		/** @brief Convert to ISO 8601 string with offset */
		[[nodiscard]] std::string toString() const;

		/** @brief Convert to string using specified format */
		[[nodiscard]] std::string toString( DateTime::Format format ) const;

		/** @brief Convert to ISO 8601 extended format with full precision and offset */
		[[nodiscard]] std::string toIso8601Extended() const;

		//----------------------------------------------
		// Comparison methods
		//----------------------------------------------

		/** @brief Compare to another DateTimeOffset */
		[[nodiscard]] inline std::int32_t compareTo( const DateTimeOffset& other ) const noexcept;

		/** @brief Check if this DateTimeOffset equals another (same UTC time) */
		[[nodiscard]] inline bool equals( const DateTimeOffset& other ) const noexcept;

		/** @brief Check if this DateTimeOffset equals another exactly (same local time and offset) */
		[[nodiscard]] inline bool equalsExact( const DateTimeOffset& other ) const noexcept;

		//----------------------------------------------
		// Validation methods
		//----------------------------------------------

		/** @brief Check if this DateTimeOffset is valid */
		[[nodiscard]] bool isValid() const noexcept;

		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/** @brief Get current local time with system timezone offset */
		[[nodiscard]] static DateTimeOffset now() noexcept;

		/** @brief Get current UTC time (offset = 00:00:00) */
		[[nodiscard]] static DateTimeOffset utcNow() noexcept;

		/** @brief Get current local date (time set to 00:00:00) */
		[[nodiscard]] static DateTimeOffset today() noexcept;

		/** @brief Get minimum DateTimeOffset value */
		[[nodiscard]] inline static constexpr DateTimeOffset minValue() noexcept;

		/** @brief Get maximum DateTimeOffset value */
		[[nodiscard]] inline static constexpr DateTimeOffset maxValue() noexcept;

		/** @brief Get Unix epoch DateTimeOffset (January 1, 1970 00:00:00 UTC) */
		[[nodiscard]] inline static constexpr DateTimeOffset unixEpoch() noexcept;

		/** @brief Compare two DateTimeOffset values */
		[[nodiscard]] inline static std::int32_t compare( const DateTimeOffset& left, const DateTimeOffset& right ) noexcept;

		/** @brief Try parse ISO 8601 string without throwing */
		[[nodiscard]] static bool tryParse( std::string_view str, DateTimeOffset& result ) noexcept;

		/** @brief Create from Unix timestamp seconds with UTC offset */
		[[nodiscard]] static DateTimeOffset fromUnixTimeSeconds( std::int64_t seconds ) noexcept;

		/** @brief Create from Unix timestamp milliseconds with UTC offset */
		[[nodiscard]] static DateTimeOffset fromUnixTimeMilliseconds( std::int64_t milliseconds ) noexcept;

		/**
		 * @brief Create DateTimeOffset from Windows FILETIME format
		 * @details Works on any platform - performs mathematical epoch conversion only.
		 *          Useful for processing Windows-originated data on any system.
		 * @param fileTime 100-nanosecond intervals since January 1, 1601 UTC
		 * @return DateTimeOffset representing the same instant in UTC (offset = 00:00:00)
		 */
		[[nodiscard]] static DateTimeOffset fromFileTime( std::int64_t fileTime ) noexcept;

	private:
		//----------------------------------------------
		// Internal data
		//----------------------------------------------

		/** @brief Local date and time */
		DateTime m_dateTime;

		/** @brief Offset from UTC */
		TimeSpan m_offset;

		//----------------------------------------------
		// Internal helper methods
		//----------------------------------------------

		/** @brief Validate offset range (±14:00:00) */
		static bool isValidOffset( const TimeSpan& offset ) noexcept;

		/** @brief Get system timezone offset for given DateTime */
		static TimeSpan systemTimezoneOffset( const DateTime& dateTime ) noexcept;

		/** @brief Validate DateTimeOffset components */
		bool isValidInternal() const noexcept;
	};

	//=====================================================================
	// Stream operators
	//=====================================================================

	/** @brief Output stream operator (ISO 8601 format) */
	std::ostream& operator<<( std::ostream& os, const DateTime& dateTime );

	/** @brief Input stream operator (ISO 8601 format) */
	std::istream& operator>>( std::istream& is, DateTime& dateTime );

	/** @brief Output stream operator (ISO 8601 format with offset) */
	std::ostream& operator<<( std::ostream& os, const DateTimeOffset& dateTimeOffset );

	/** @brief Input stream operator (ISO 8601 format with offset) */
	std::istream& operator>>( std::istream& is, DateTimeOffset& dateTimeOffset );
}

#include "DateTimeISO8601.inl"
