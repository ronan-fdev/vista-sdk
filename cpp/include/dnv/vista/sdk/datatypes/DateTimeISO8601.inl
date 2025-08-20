/**
 * @file DateTimeISO8601.inl
 * @brief Inline implementations for DateTime and TimeSpan classes
 * @details Performance-critical methods implemented inline for optimal speed
 */

#pragma once

#include "dnv/vista/sdk/constants/DateTimeConstants.h"

namespace dnv::vista::sdk::datatypes
{
	//=====================================================================
	// TimeSpan class - inline implementations
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline constexpr TimeSpan::TimeSpan( std::int64_t ticks ) noexcept
		: m_ticks{ ticks }
	{
	}

	//----------------------------------------------
	// Comparison operators
	//----------------------------------------------

	constexpr inline bool TimeSpan::operator==( const TimeSpan& other ) const noexcept
	{
		return m_ticks == other.m_ticks;
	}

	constexpr inline bool TimeSpan::operator!=( const TimeSpan& other ) const noexcept
	{
		return m_ticks != other.m_ticks;
	}

	constexpr inline bool TimeSpan::operator<( const TimeSpan& other ) const noexcept
	{
		return m_ticks < other.m_ticks;
	}

	constexpr inline bool TimeSpan::operator<=( const TimeSpan& other ) const noexcept
	{
		return m_ticks <= other.m_ticks;
	}

	constexpr inline bool TimeSpan::operator>( const TimeSpan& other ) const noexcept
	{
		return m_ticks > other.m_ticks;
	}

	constexpr inline bool TimeSpan::operator>=( const TimeSpan& other ) const noexcept
	{
		return m_ticks >= other.m_ticks;
	}

	//----------------------------------------------
	// Arithmetic operators
	//----------------------------------------------

	constexpr inline TimeSpan TimeSpan::operator+( const TimeSpan& other ) const noexcept
	{
		return TimeSpan{ m_ticks + other.m_ticks };
	}

	constexpr inline TimeSpan TimeSpan::operator-( const TimeSpan& other ) const noexcept
	{
		return TimeSpan{ m_ticks - other.m_ticks };
	}

	constexpr inline TimeSpan TimeSpan::operator-() const noexcept
	{
		return TimeSpan{ -m_ticks };
	}

	constexpr inline TimeSpan& TimeSpan::operator+=( const TimeSpan& other ) noexcept
	{
		m_ticks += other.m_ticks;

		return *this;
	}

	constexpr inline TimeSpan& TimeSpan::operator-=( const TimeSpan& other ) noexcept
	{
		m_ticks -= other.m_ticks;

		return *this;
	}

	//----------------------------------------------
	// Property accessors
	//----------------------------------------------

	constexpr inline std::int64_t TimeSpan::ticks() const noexcept
	{
		return m_ticks;
	}

	constexpr inline double TimeSpan::totalDays() const noexcept
	{
		return static_cast<double>( m_ticks ) / constants::datetime::TICKS_PER_DAY;
	}

	constexpr inline double TimeSpan::totalHours() const noexcept
	{
		return static_cast<double>( m_ticks ) / constants::datetime::TICKS_PER_HOUR;
	}

	constexpr inline double TimeSpan::totalMinutes() const noexcept
	{
		return static_cast<double>( m_ticks ) / constants::datetime::TICKS_PER_MINUTE;
	}

	constexpr inline double TimeSpan::totalSeconds() const noexcept
	{
		return static_cast<double>( m_ticks ) / constants::datetime::TICKS_PER_SECOND;
	}

	constexpr inline double TimeSpan::totalMilliseconds() const noexcept
	{
		return static_cast<double>( m_ticks ) / constants::datetime::TICKS_PER_MILLISECOND;
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	inline constexpr TimeSpan TimeSpan::fromDays( double days ) noexcept
	{
		return TimeSpan{ static_cast<std::int64_t>( days * constants::datetime::TICKS_PER_DAY ) };
	}

	inline constexpr TimeSpan TimeSpan::fromHours( double hours ) noexcept
	{
		return TimeSpan{ static_cast<std::int64_t>( hours * constants::datetime::TICKS_PER_HOUR ) };
	}

	inline constexpr TimeSpan TimeSpan::fromMinutes( double minutes ) noexcept
	{
		return TimeSpan{ static_cast<std::int64_t>( minutes * constants::datetime::TICKS_PER_MINUTE ) };
	}

	inline constexpr TimeSpan TimeSpan::fromSeconds( double seconds ) noexcept
	{
		return TimeSpan{ static_cast<std::int64_t>( seconds * constants::datetime::TICKS_PER_SECOND ) };
	}

	inline constexpr TimeSpan TimeSpan::fromMilliseconds( double milliseconds ) noexcept
	{
		return TimeSpan{ static_cast<std::int64_t>( milliseconds * constants::datetime::TICKS_PER_MILLISECOND ) };
	}

	//=====================================================================
	// DateTime class - inline implementations
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline constexpr DateTime::DateTime() noexcept
		: m_ticks{ constants::datetime::MIN_DATETIME_TICKS }
	{
	}

	inline constexpr DateTime::DateTime( std::int64_t ticks ) noexcept
		: m_ticks{ ticks }
	{
	}

	//----------------------------------------------
	// Comparison operators
	//----------------------------------------------

	inline bool DateTime::operator==( const DateTime& other ) const noexcept
	{
		return m_ticks == other.m_ticks;
	}

	inline bool DateTime::operator!=( const DateTime& other ) const noexcept
	{
		return m_ticks != other.m_ticks;
	}

	inline bool DateTime::operator<( const DateTime& other ) const noexcept
	{
		return m_ticks < other.m_ticks;
	}

	inline bool DateTime::operator<=( const DateTime& other ) const noexcept
	{
		return m_ticks <= other.m_ticks;
	}

	inline bool DateTime::operator>( const DateTime& other ) const noexcept
	{
		return m_ticks > other.m_ticks;
	}

	inline bool DateTime::operator>=( const DateTime& other ) const noexcept
	{
		return m_ticks >= other.m_ticks;
	}

	//----------------------------------------------
	// Arithmetic operators
	//----------------------------------------------

	inline DateTime DateTime::operator+( const TimeSpan& duration ) const noexcept
	{
		return DateTime{ m_ticks + duration.ticks() };
	}

	inline DateTime DateTime::operator-( const TimeSpan& duration ) const noexcept
	{
		return DateTime{ m_ticks - duration.ticks() };
	}

	inline TimeSpan DateTime::operator-( const DateTime& other ) const noexcept
	{
		return TimeSpan{ m_ticks - other.m_ticks };
	}

	inline DateTime& DateTime::operator+=( const TimeSpan& duration ) noexcept
	{
		m_ticks += duration.ticks();

		return *this;
	}

	inline DateTime& DateTime::operator-=( const TimeSpan& duration ) noexcept
	{
		m_ticks -= duration.ticks();

		return *this;
	}

	//----------------------------------------------
	// Property accessors
	//----------------------------------------------

	inline constexpr std::int64_t DateTime::ticks() const noexcept
	{
		return m_ticks;
	}

	//----------------------------------------------
	// Conversion methods
	//----------------------------------------------

	inline std::int64_t DateTime::toUnixSeconds() const noexcept
	{
		return ( m_ticks - constants::datetime::UNIX_EPOCH_TICKS ) / constants::datetime::TICKS_PER_SECOND;
	}

	inline std::int64_t DateTime::toUnixMilliseconds() const noexcept
	{
		return ( m_ticks - constants::datetime::UNIX_EPOCH_TICKS ) / constants::datetime::TICKS_PER_MILLISECOND;
	}

	//----------------------------------------------
	// Validation methods
	//----------------------------------------------

	inline bool DateTime::isValid() const noexcept
	{
		return m_ticks >= constants::datetime::MIN_DATETIME_TICKS && m_ticks <= constants::datetime::MAX_DATETIME_TICKS;
	}

	inline constexpr bool DateTime::isLeapYear( std::int32_t year ) noexcept
	{
		return ( year % 4 == 0 && year % 100 != 0 ) || ( year % 400 == 0 );
	}

	inline constexpr std::int32_t DateTime::daysInMonth( std::int32_t year, std::int32_t month ) noexcept
	{
		if ( month < 1 || month > 12 )
		{
			return 0;
		}

		constexpr std::int32_t daysPerMonth[]{ 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

		if ( month == 2 && isLeapYear( year ) )
		{
			return 29;
		}

		return daysPerMonth[month];
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	inline constexpr DateTime DateTime::minValue() noexcept
	{
		return DateTime{ constants::datetime::MIN_DATETIME_TICKS };
	}

	inline constexpr DateTime DateTime::maxValue() noexcept
	{
		return DateTime{ constants::datetime::MAX_DATETIME_TICKS };
	}

	inline constexpr DateTime DateTime::epoch() noexcept
	{
		return DateTime{ constants::datetime::UNIX_EPOCH_TICKS };
	}

	//=====================================================================
	// DateTimeOffset class - Inline implementations
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	constexpr inline DateTimeOffset::DateTimeOffset() noexcept
		: m_dateTime{ DateTime::minValue() },
		  m_offset{ 0 }
	{
	}

	constexpr inline DateTimeOffset::DateTimeOffset( const DateTime& dateTime, const TimeSpan& offset ) noexcept
		: m_dateTime{ dateTime },
		  m_offset{ offset } {}

	constexpr inline DateTimeOffset::DateTimeOffset( std::int64_t ticks, const TimeSpan& offset ) noexcept
		: m_dateTime{ ticks },
		  m_offset{ offset } {}

	//----------------------------------------------
	// Comparison operators
	//----------------------------------------------

	inline bool DateTimeOffset::operator==( const DateTimeOffset& other ) const noexcept
	{
		/* Compare UTC times */
		return utcTicks() == other.utcTicks();
	}

	inline bool DateTimeOffset::operator!=( const DateTimeOffset& other ) const noexcept
	{
		return !( *this == other );
	}

	inline bool DateTimeOffset::operator<( const DateTimeOffset& other ) const noexcept
	{
		/* Compare UTC times */
		return utcTicks() < other.utcTicks();
	}

	inline bool DateTimeOffset::operator<=( const DateTimeOffset& other ) const noexcept
	{
		return utcTicks() <= other.utcTicks();
	}

	inline bool DateTimeOffset::operator>( const DateTimeOffset& other ) const noexcept
	{
		return utcTicks() > other.utcTicks();
	}

	inline bool DateTimeOffset::operator>=( const DateTimeOffset& other ) const noexcept
	{
		return utcTicks() >= other.utcTicks();
	}

	//----------------------------------------------
	// Arithmetic operators
	//----------------------------------------------

	inline DateTimeOffset DateTimeOffset::operator+( const TimeSpan& duration ) const noexcept
	{
		return DateTimeOffset{ m_dateTime + duration, m_offset };
	}

	inline DateTimeOffset DateTimeOffset::operator-( const TimeSpan& duration ) const noexcept
	{
		return DateTimeOffset{ m_dateTime - duration, m_offset };
	}

	inline TimeSpan DateTimeOffset::operator-( const DateTimeOffset& other ) const noexcept
	{
		/* Compare UTC times by converting both to UTC ticks and subtracting */
		return TimeSpan{ utcTicks() - other.utcTicks() };
	}

	inline DateTimeOffset& DateTimeOffset::operator+=( const TimeSpan& duration ) noexcept
	{
		m_dateTime += duration;

		return *this;
	}

	inline DateTimeOffset& DateTimeOffset::operator-=( const TimeSpan& duration ) noexcept
	{
		m_dateTime -= duration;

		return *this;
	}

	//----------------------------------------------
	// Property accessors
	//----------------------------------------------

	constexpr inline const DateTime& DateTimeOffset::dateTime() const noexcept
	{
		return m_dateTime;
	}

	constexpr inline const TimeSpan& DateTimeOffset::offset() const noexcept
	{
		return m_offset;
	}

	constexpr inline std::int64_t DateTimeOffset::ticks() const noexcept
	{
		return m_dateTime.ticks();
	}

	inline std::int64_t DateTimeOffset::utcTicks() const noexcept
	{
		return m_dateTime.ticks() - m_offset.ticks();
	}

	inline std::int32_t DateTimeOffset::year() const noexcept
	{
		return m_dateTime.year();
	}

	inline std::int32_t DateTimeOffset::month() const noexcept
	{
		return m_dateTime.month();
	}

	inline std::int32_t DateTimeOffset::day() const noexcept
	{
		return m_dateTime.day();
	}

	inline std::int32_t DateTimeOffset::hour() const noexcept
	{
		return m_dateTime.hour();
	}

	inline std::int32_t DateTimeOffset::minute() const noexcept
	{
		return m_dateTime.minute();
	}

	inline std::int32_t DateTimeOffset::second() const noexcept
	{
		return m_dateTime.second();
	}

	inline std::int32_t DateTimeOffset::millisecond() const noexcept
	{
		return m_dateTime.millisecond();
	}

	inline std::int32_t DateTimeOffset::microsecond() const noexcept
	{
		/* Extract microseconds from ticks (1 microsecond = 10 ticks) */
		const auto remainderTicks = m_dateTime.ticks() % 10000;

		return static_cast<std::int32_t>( remainderTicks / 10 );
	}

	inline std::int32_t DateTimeOffset::nanosecond() const noexcept
	{
		/* Extract nanosecond component (in hundreds of nanoseconds since ticks are 100ns units) */
		const auto remainderTicks = m_dateTime.ticks() % 10;

		return static_cast<std::int32_t>( remainderTicks * 100 );
	}

	inline std::int32_t DateTimeOffset::dayOfWeek() const noexcept
	{
		return m_dateTime.dayOfWeek();
	}

	inline std::int32_t DateTimeOffset::dayOfYear() const noexcept
	{
		return m_dateTime.dayOfYear();
	}

	inline std::int32_t DateTimeOffset::totalOffsetMinutes() const noexcept
	{
		return static_cast<std::int32_t>( m_offset.totalMinutes() );
	}

	//----------------------------------------------
	// Conversion methods
	//----------------------------------------------

	inline std::int64_t DateTimeOffset::toUnixSeconds() const noexcept
	{
		return utcDateTime().toUnixSeconds();
	}

	inline std::int64_t DateTimeOffset::toUnixMilliseconds() const noexcept
	{
		return utcDateTime().toUnixMilliseconds();
	}

	inline TimeSpan DateTimeOffset::timeOfDay() const noexcept
	{
		return m_dateTime.timeOfDay();
	}

	//----------------------------------------------
	// Arithmetic methods
	//----------------------------------------------

	inline DateTimeOffset DateTimeOffset::add( const TimeSpan& value ) const noexcept
	{
		return *this + value;
	}

	inline TimeSpan DateTimeOffset::subtract( const DateTimeOffset& value ) const noexcept
	{
		return *this - value;
	}

	inline DateTimeOffset DateTimeOffset::subtract( const TimeSpan& value ) const noexcept
	{
		return *this - value;
	}

	inline DateTimeOffset DateTimeOffset::addTicks( std::int64_t ticks ) const noexcept
	{
		return *this + TimeSpan{ ticks };
	}

	//----------------------------------------------
	// Comparison methods
	//----------------------------------------------

	inline std::int32_t DateTimeOffset::compareTo( const DateTimeOffset& other ) const noexcept
	{
		const auto utcThis = utcTicks();
		const auto utcOther = other.utcTicks();

		if ( utcThis < utcOther )
		{
			return -1;
		}
		if ( utcThis > utcOther )
		{
			return 1;
		}

		return 0;
	}

	inline bool DateTimeOffset::equals( const DateTimeOffset& other ) const noexcept
	{
		return *this == other;
	}

	inline bool DateTimeOffset::equalsExact( const DateTimeOffset& other ) const noexcept
	{
		/* Check both local time and offset are identical */
		return m_dateTime == other.m_dateTime && m_offset == other.m_offset;
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	constexpr inline DateTimeOffset DateTimeOffset::minValue() noexcept
	{
		return DateTimeOffset{ DateTime::minValue(), TimeSpan{ 0 } };
	}

	constexpr inline DateTimeOffset DateTimeOffset::maxValue() noexcept
	{
		return DateTimeOffset{ DateTime::maxValue(), TimeSpan{ 0 } };
	}

	constexpr inline DateTimeOffset DateTimeOffset::unixEpoch() noexcept
	{
		return DateTimeOffset{ DateTime::epoch(), TimeSpan{ 0 } };
	}

	inline std::int32_t DateTimeOffset::compare( const DateTimeOffset& left, const DateTimeOffset& right ) noexcept
	{
		return left.compareTo( right );
	}
}
