/**
 * @file DateTimeISO8601.cpp
 * @brief Implementation of DateTime and TimeSpan classes
 * @details Provides UTC-only datetime operations with 100-nanosecond precision,
 *          ISO 8601 parsing/formatting, and cross-platform compatibility
 */

#include "dnv/vista/sdk/config/Platform.h"

#include "dnv/vista/sdk/datatypes/DateTimeISO8601.h"

namespace dnv::vista::sdk::datatypes
{
	//=====================================================================
	// TimeSpan class
	//=====================================================================

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	TimeSpan TimeSpan::parse( std::string_view str )
	{
		TimeSpan result;
		if ( !tryParse( str, result ) )
		{
			throw std::invalid_argument( "Invalid TimeSpan string format" );
		}
		return result;
	}

	bool TimeSpan::tryParse( std::string_view str, TimeSpan& result ) noexcept
	{
		try
		{
			/*
			 * TimeSpan parsing supports multiple formats:
			 *
			 * 1. ISO 8601 Duration Format:
			 *    - Full: P[n]Y[n]M[n]DT[n]H[n]M[n]S (e.g., "P3Y6M4DT12H30M5S" = 3 years, 6 months, 4 days, 12h 30m 5s)
			 *    - Time-only: PT[n]H[n]M[n]S (e.g., "PT1H30M45S" = 1 hour, 30 minutes, 45 seconds)
			 *    - Components optional: "PT2H30M", "PT45S", "PT1.5H" (fractional values allowed)
			 *
			 * 2. Time-of-Day Format (H:M:S):
			 *    - "01:30:45" = 1 hour, 30 minutes, 45 seconds
			 *    - "00:05:30.5" = 5 minutes, 30.5 seconds
			 *    - Validates 0-23 hours, 0-59 minutes/seconds (time-of-day constraints)
			 *
			 * 3. Numeric Seconds Format:
			 *    - "123.45" = 123.45 seconds
			 *    - "60" = 60 seconds (1 minute)
			 *    - Any numeric value interpreted as seconds
			 */

			if ( str.empty() )
			{
				return false;
			}

			/* Handle simple numeric format (assume seconds) */
			if ( str.find_first_not_of( "0123456789.-" ) == std::string_view::npos )
			{
				try
				{
					double seconds = std::stod( std::string( str ) );
					result = TimeSpan::fromSeconds( seconds );
					return true;
				}
				catch ( ... )
				{
					return false;
				}
			}

			/* Handle H:M:S format */
			if ( str.find( ':' ) != std::string_view::npos )
			{
				std::vector<std::string> parts;
				std::string temp( str );
				size_t pos = 0;
				while ( ( pos = temp.find( ':' ) ) != std::string::npos )
				{
					parts.push_back( temp.substr( 0, pos ) );
					temp.erase( 0, pos + 1 );
				}
				parts.push_back( temp );

				if ( parts.size() == 3 )
				{
					int hours = std::stoi( parts[0] );
					int minutes = std::stoi( parts[1] );
					double seconds = std::stod( parts[2] );

					/* Validate time components - TimeSpan can represent longer durations but H:M:S format implies time-of-day constraints */
					if ( hours >= 0 && hours < 24 && minutes >= 0 && minutes < 60 && seconds >= 0 && seconds < 60 )
					{
						result = TimeSpan::fromHours( hours ) + TimeSpan::fromMinutes( minutes ) + TimeSpan::fromSeconds( seconds );

						return true;
					}
				}
			}

			/* Handle ISO 8601 duration format (simplified) */
			if ( str.length() > 1 && str[0] == 'P' )
			{
				/* Simple PT[n]H[n]M[n]S format parsing */
				auto pos = str.find( 'T' );
				if ( pos != std::string_view::npos )
				{
					auto timePart = str.substr( pos + 1 );
					double totalSeconds = 0.0;

					/* Parse hours */
					auto hPos = timePart.find( 'H' );
					if ( hPos != std::string_view::npos )
					{
						auto hours = std::stod( std::string( timePart.substr( 0, hPos ) ) );
						totalSeconds += hours * 3600.0;
						timePart = timePart.substr( hPos + 1 );
					}

					/* Parse minutes */
					auto mPos = timePart.find( 'M' );
					if ( mPos != std::string_view::npos )
					{
						auto minutes = std::stod( std::string( timePart.substr( 0, mPos ) ) );
						totalSeconds += minutes * 60.0;
						timePart = timePart.substr( mPos + 1 );
					}

					/* Parse seconds */
					auto sPos = timePart.find( 'S' );
					if ( sPos != std::string_view::npos )
					{
						auto seconds = std::stod( std::string( timePart.substr( 0, sPos ) ) );
						totalSeconds += seconds;
					}

					result = TimeSpan::fromSeconds( totalSeconds );

					return true;
				}
			}

			return false;
		}
		catch ( ... )
		{
			return false;
		}
	}

	//=====================================================================
	// std::chrono interoperability limits
	//=====================================================================

	/**
	 * @brief Check if a DateTime value can safely round-trip through std::chrono::system_clock
	 * @details This is platform-dependent; on 64-bit systems with nanosecond precision,
	 *          the safe range is approximately years 1677-2262.
	 * @return true if this DateTime is within the safe chrono range
	 */
	inline constexpr bool isChronoSafeTicks( std::int64_t ticks ) noexcept
	{
		return ticks >= constants::datetime::MIN_CHRONO_SAFE_TICKS && ticks <= constants::datetime::MAX_CHRONO_SAFE_TICKS;
	}

	//=====================================================================
	// DateTime class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	DateTime::DateTime( std::chrono::system_clock::time_point timePoint ) noexcept
	{
		auto duration = timePoint.time_since_epoch();

		using ticks_duration = std::chrono::duration<std::int64_t, std::ratio<1, 10000000>>;
		auto ticks = std::chrono::duration_cast<ticks_duration>( duration );

		m_ticks = constants::datetime::UNIX_EPOCH_TICKS + ticks.count();

		/* Clamp to valid range */
		if ( m_ticks < constants::datetime::MIN_DATETIME_TICKS )
		{
			m_ticks = constants::datetime::MIN_DATETIME_TICKS;
		}
		else if ( m_ticks > constants::datetime::MAX_DATETIME_TICKS )
		{
			m_ticks = constants::datetime::MAX_DATETIME_TICKS;
		}
	}

	DateTime::DateTime( std::int32_t year, std::int32_t month, std::int32_t day ) noexcept
	{
		if ( !isValidDate( year, month, day ) )
		{
			m_ticks = constants::datetime::MIN_DATETIME_TICKS;
			return;
		}
		m_ticks = dateToTicks( year, month, day );
	}

	DateTime::DateTime( std::int32_t year, std::int32_t month, std::int32_t day,
		std::int32_t hour, std::int32_t minute, std::int32_t second ) noexcept
	{
		if ( !isValidDate( year, month, day ) || !isValidTime( hour, minute, second, 0 ) )
		{
			m_ticks = constants::datetime::MIN_DATETIME_TICKS;
			return;
		}
		m_ticks = dateToTicks( year, month, day ) + timeToTicks( hour, minute, second, 0 );
	}

	DateTime::DateTime( std::int32_t year, std::int32_t month, std::int32_t day,
		std::int32_t hour, std::int32_t minute, std::int32_t second,
		std::int32_t millisecond ) noexcept
	{
		if ( !isValidDate( year, month, day ) || !isValidTime( hour, minute, second, millisecond ) )
		{
			m_ticks = constants::datetime::MIN_DATETIME_TICKS;
			return;
		}
		m_ticks = dateToTicks( year, month, day ) + timeToTicks( hour, minute, second, millisecond );
	}

	DateTime::DateTime( std::string_view iso8601String )
	{
		DateTime result;
		if ( !tryParse( iso8601String, result ) )
		{
			throw std::invalid_argument( "Invalid ISO 8601 DateTime string" );
		}
		m_ticks = result.m_ticks;
	}

	//----------------------------------------------
	// Property accessors
	//----------------------------------------------

	std::int32_t DateTime::year() const noexcept
	{
		std::int32_t year, month, day;
		getDateComponents( year, month, day );

		return year;
	}

	std::int32_t DateTime::month() const noexcept
	{
		std::int32_t year, month, day;
		getDateComponents( year, month, day );

		return month;
	}

	std::int32_t DateTime::day() const noexcept
	{
		std::int32_t year, month, day;
		getDateComponents( year, month, day );

		return day;
	}

	std::int32_t DateTime::hour() const noexcept
	{
		std::int32_t hour, minute, second, millisecond;
		getTimeComponents( hour, minute, second, millisecond );

		return hour;
	}

	std::int32_t DateTime::minute() const noexcept
	{
		std::int32_t hour, minute, second, millisecond;
		getTimeComponents( hour, minute, second, millisecond );

		return minute;
	}

	std::int32_t DateTime::second() const noexcept
	{
		std::int32_t hour, minute, second, millisecond;
		getTimeComponents( hour, minute, second, millisecond );

		return second;
	}

	std::int32_t DateTime::millisecond() const noexcept
	{
		std::int32_t hour, minute, second, millisecond;
		getTimeComponents( hour, minute, second, millisecond );

		return millisecond;
	}

	std::int32_t DateTime::dayOfWeek() const noexcept
	{
		/* January 1, 0001 was a Monday (day 1), so we need to adjust */
		std::int64_t days = m_ticks / constants::datetime::TICKS_PER_DAY;

		/* 0=Sunday, 6=Saturday */
		return static_cast<std::int32_t>( ( days + 1 ) % 7 );
	}

	std::int32_t DateTime::dayOfYear() const noexcept
	{
		std::int32_t year, month, day;
		getDateComponents( year, month, day );

		std::int32_t dayCount = 0;
		for ( std::int32_t m = 1; m < month; ++m )
		{
			dayCount += daysInMonth( year, m );
		}

		return dayCount + day;
	}

	//----------------------------------------------
	// Conversion methods
	//----------------------------------------------

	DateTime DateTime::date() const noexcept
	{
		std::int64_t dayTicks = ( m_ticks / constants::datetime::TICKS_PER_DAY ) * constants::datetime::TICKS_PER_DAY;

		return DateTime{ dayTicks };
	}

	TimeSpan DateTime::timeOfDay() const noexcept
	{
		std::int64_t timeTicks = m_ticks % constants::datetime::TICKS_PER_DAY;

		return TimeSpan{ timeTicks };
	}

	//----------------------------------------------
	// String formatting
	//----------------------------------------------

	std::string DateTime::toString() const
	{
		return toString( Format::Iso8601Basic );
	}

	std::string DateTime::toString( Format format ) const
	{
		std::int32_t y, mon, d, h, min, s, ms;
		getDateComponents( y, mon, d );
		getTimeComponents( h, min, s, ms );

		std::ostringstream oss;

		switch ( format )
		{
			case Format::Iso8601Basic:
			{
				oss << std::setfill( '0' ) << std::setw( 4 ) << y << "-" << std::setw( 2 ) << mon << "-" << std::setw( 2 ) << d << "T" << std::setw( 2 ) << h << ":"
					<< std::setw( 2 ) << min << ":" << std::setw( 2 ) << s << "Z";

				break;
			}
			case Format::Iso8601Extended:
			{
				std::int32_t fractionalTicks = static_cast<std::int32_t>( m_ticks % constants::datetime::TICKS_PER_SECOND );
				oss << std::setfill( '0' ) << std::setw( 4 ) << y << "-" << std::setw( 2 ) << mon << "-" << std::setw( 2 ) << d << "T" << std::setw( 2 ) << h
					<< ":" << std::setw( 2 ) << min << ":" << std::setw( 2 ) << s << "." << std::setw( 7 ) << fractionalTicks << "Z";

				break;
			}
			case Format::Iso8601WithOffset:
			{
				/* UTC DateTime always has +00:00 offset */
				oss << std::setfill( '0' ) << std::setw( 4 ) << y << "-" << std::setw( 2 ) << mon << "-" << std::setw( 2 ) << d << "T" << std::setw( 2 ) << h
					<< ":" << std::setw( 2 ) << min << ":" << std::setw( 2 ) << s << "+00:00";

				break;
			}
			case Format::DateOnly:
			{
				oss << std::setfill( '0' ) << std::setw( 4 ) << y << "-" << std::setw( 2 ) << mon << "-" << std::setw( 2 ) << d;

				break;
			}
			case Format::TimeOnly:
			{
				oss << std::setfill( '0' ) << std::setw( 2 ) << h << ":" << std::setw( 2 ) << min << ":" << std::setw( 2 ) << s;

				break;
			}
			case Format::UnixSeconds:
			{
				oss << toUnixSeconds();

				break;
			}
			case Format::UnixMilliseconds:
			{
				oss << toUnixMilliseconds();

				break;
			}
			default:
			{
				oss << toString( Format::Iso8601Basic );

				break;
			}
		}

		return oss.str();
	}

	std::string DateTime::toIso8601Extended() const
	{
		return toString( Format::Iso8601Extended );
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	DateTime DateTime::now() noexcept
	{
		return DateTime{ std::chrono::system_clock::now() };
	}

	DateTime DateTime::today() noexcept
	{
		return now().date();
	}

	DateTime DateTime::parse( std::string_view str )
	{
		DateTime result;
		if ( !tryParse( str, result ) )
		{
			throw std::invalid_argument( "Invalid ISO 8601 DateTime string" );
		}
		return result;
	}

	bool DateTime::tryParse( std::string_view str, DateTime& result ) noexcept
	{
		try
		{
			/* ISO 8601 parser
			 * Supports: YYYY-MM-DD, YYYY-MM-DDTHH:mm:ss, YYYY-MM-DDTHH:mm:ssZ,
			 * YYYY-MM-DDTHH:mm:ss.f, YYYY-MM-DDTHH:mm:ss.fffffffZ, etc. */

			if ( str.empty() || str.length() < 4 )
			{
				return false;
			}

			/* Remove trailing 'Z' if present */
			if ( str.back() == 'Z' )
			{
				str.remove_suffix( 1 );
			}

			/* Remove timezone offset for DateTime parsing */
			auto tzPos = str.find_last_of( "+-" );

			/* Ensure it's not in date part */
			if ( tzPos != std::string_view::npos && tzPos > 10 )
			{
				str = str.substr( 0, tzPos );
			}

			/* Parse year (required) */
			if ( str.length() < 4 )
			{
				return false;
			}

			auto pos = size_t{ 0 };
			std::int32_t year = 0;
			for ( size_t i = 0; i < 4; ++i )
			{
				char c = str[pos++];
				if ( c < '0' || c > '9' )
				{
					return false;
				}
				year = year * 10 + ( c - '0' );
			}

			/* Expect separator */
			if ( pos >= str.length() || str[pos++] != '-' )
			{
				return false;
			}

			/* Parse month (required) */
			if ( pos >= str.length() )
			{
				return false;
			}

			std::int32_t month = 0;
			char c = str[pos++];
			if ( c < '0' || c > '9' )
			{
				return false;
			}

			month = c - '0';

			if ( pos < str.length() && str[pos] >= '0' && str[pos] <= '9' )
				month = month * 10 + ( str[pos++] - '0' );

			/* Expect separator */
			if ( pos >= str.length() || str[pos++] != '-' )
			{
				return false;
			}

			/* Parse day (required) */
			if ( pos >= str.length() )
			{
				return false;
			}

			std::int32_t day = 0;
			c = str[pos++];
			if ( c < '0' || c > '9' )
			{
				return false;
			}

			day = c - '0';

			if ( pos < str.length() && str[pos] >= '0' && str[pos] <= '9' )
				day = day * 10 + ( str[pos++] - '0' );

			/* Time part is optional */
			std::int32_t hour = 0, minute = 0, second = 0;
			std::int32_t fractionalTicks = 0;

			if ( pos < str.length() && str[pos] == 'T' )
			{
				++pos; /* Skip 'T' */

				/* Parse hour */
				if ( pos + 1 >= str.length() )
				{
					return false;
				}

				c = str[pos++];
				if ( c < '0' || c > '9' )
				{
					return false;
				}

				hour = c - '0';

				if ( pos < str.length() && str[pos] >= '0' && str[pos] <= '9' )
				{
					hour = hour * 10 + ( str[pos++] - '0' );
				}

				/* Expect colon */
				if ( pos >= str.length() || str[pos++] != ':' )
				{
					return false;
				}

				/* Parse minute */
				if ( pos + 1 >= str.length() )
				{
					return false;
				}

				c = str[pos++];
				if ( c < '0' || c > '9' )
				{
					return false;
				}

				minute = c - '0';

				if ( pos < str.length() && str[pos] >= '0' && str[pos] <= '9' )
				{
					minute = minute * 10 + ( str[pos++] - '0' );
				}

				/* Expect colon */
				if ( pos >= str.length() || str[pos++] != ':' )
				{
					return false;
				}

				/* Parse second */
				if ( pos >= str.length() )
				{
					return false;
				}

				c = str[pos++];
				if ( c < '0' || c > '9' )
				{
					return false;
				}

				second = c - '0';

				if ( pos < str.length() && str[pos] >= '0' && str[pos] <= '9' )
				{
					second = second * 10 + ( str[pos++] - '0' );
				}

				/* Parse fractional seconds if present */
				if ( pos < str.length() && str[pos] == '.' )
				{
					++pos; /* Skip '.' */

					std::int32_t fractionDigits = 0;
					std::int32_t fractionValue = 0;

					while ( pos < str.length() && str[pos] >= '0' && str[pos] <= '9' && fractionDigits < 7 )
					{
						fractionValue = fractionValue * 10 + ( str[pos++] - '0' );
						++fractionDigits;
					}

					/* Skip any extra fractional digits */
					while ( pos < str.length() && str[pos] >= '0' && str[pos] <= '9' )
					{
						++pos;
					}

					/* Convert to ticks (pad to 7 digits) */
					while ( fractionDigits < 7 )
					{
						fractionValue *= 10;
						++fractionDigits;
					}

					fractionalTicks = fractionValue;
				}
			}

			/* Validate components */
			if ( !isValidDate( year, month, day ) || !isValidTime( hour, minute, second, 0 ) )
			{
				return false;
			}

			/* Calculate ticks */
			std::int64_t ticks = dateToTicks( year, month, day ) + timeToTicks( hour, minute, second, 0 ) + fractionalTicks;

			result = DateTime{ ticks };

			return true;
		}
		catch ( ... )
		{
			return false;
		}
	}

	DateTime DateTime::sinceEpochSeconds( std::int64_t seconds ) noexcept
	{
		std::int64_t ticks = constants::datetime::UNIX_EPOCH_TICKS + ( seconds * constants::datetime::TICKS_PER_SECOND );

		return DateTime{ ticks };
	}

	DateTime DateTime::sinceEpochMilliseconds( std::int64_t milliseconds ) noexcept
	{
		std::int64_t ticks = constants::datetime::UNIX_EPOCH_TICKS + ( milliseconds * constants::datetime::TICKS_PER_MILLISECOND );

		return DateTime{ ticks };
	}

	//----------------------------------------------
	// std::chrono interoperability
	//----------------------------------------------

	std::chrono::system_clock::time_point DateTime::toChrono() const noexcept
	{
		/* Clamp to chrono-safe range */
		std::int64_t safeTicks = m_ticks;
		if ( safeTicks < constants::datetime::MIN_CHRONO_SAFE_TICKS )
		{
			safeTicks = constants::datetime::MIN_CHRONO_SAFE_TICKS;
		}
		else if ( safeTicks > constants::datetime::MAX_CHRONO_SAFE_TICKS )
		{
			safeTicks = constants::datetime::MAX_CHRONO_SAFE_TICKS;
		}

		/* Calculate duration since Unix epoch in 100-nanosecond ticks */
		std::int64_t ticksSinceEpoch = safeTicks - constants::datetime::UNIX_EPOCH_TICKS;

		/* Convert to std::chrono duration (100ns precision) */
		using ticks_duration = std::chrono::duration<std::int64_t, std::ratio<1, 10000000>>;
		auto duration = ticks_duration{ ticksSinceEpoch };

		return std::chrono::system_clock::time_point{ duration };
	}

	DateTime DateTime::fromChrono( const std::chrono::system_clock::time_point& timePoint ) noexcept
	{
		return DateTime{ timePoint };
	}

	//----------------------------------------------
	// Internal helper methods
	//----------------------------------------------

	void DateTime::getDateComponents( std::int32_t& year, std::int32_t& month, std::int32_t& day ) const noexcept
	{
		/* Extract date components from ticks using simple, reliable iteration */
		std::int64_t totalDays = m_ticks / constants::datetime::TICKS_PER_DAY;

		/* Find the year by iterating from year 1 */
		year = 1;
		while ( true )
		{
			std::int32_t daysInCurrentYear = isLeapYear( year )
												 ? 366
												 : 365;
			if ( totalDays < daysInCurrentYear )
			{
				break;
			}

			totalDays -= daysInCurrentYear;
			++year;
		}

		/* Find the month by iterating through months */
		month = 1;
		while ( month <= 12 )
		{
			std::int32_t daysInCurrentMonth = daysInMonth( year, month );
			if ( totalDays < daysInCurrentMonth )
			{
				break;
			}

			totalDays -= daysInCurrentMonth;
			++month;
		}

		/* Remaining days is the day of month (add 1 because day is 1-based) */
		day = static_cast<std::int32_t>( totalDays ) + 1;
	}

	void DateTime::getTimeComponents( std::int32_t& hour, std::int32_t& minute, std::int32_t& second, std::int32_t& millisecond ) const noexcept
	{
		std::int64_t timeTicks = m_ticks % constants::datetime::TICKS_PER_DAY;

		hour = static_cast<std::int32_t>( timeTicks / constants::datetime::TICKS_PER_HOUR );
		timeTicks %= constants::datetime::TICKS_PER_HOUR;

		minute = static_cast<std::int32_t>( timeTicks / constants::datetime::TICKS_PER_MINUTE );
		timeTicks %= constants::datetime::TICKS_PER_MINUTE;

		second = static_cast<std::int32_t>( timeTicks / constants::datetime::TICKS_PER_SECOND );
		timeTicks %= constants::datetime::TICKS_PER_SECOND;

		millisecond = static_cast<std::int32_t>( timeTicks / constants::datetime::TICKS_PER_MILLISECOND );
	}

	std::int64_t DateTime::dateToTicks( std::int32_t year, std::int32_t month, std::int32_t day ) noexcept
	{
		/* Calculate total days since January 1, year 1 using simple, reliable method */
		std::int64_t totalDays = 0;

		/* Add days for complete years before the given year */
		for ( std::int32_t y = 1; y < year; ++y )
		{
			totalDays += isLeapYear( y )
							 ? 366
							 : 365;
		}

		/* Add days for complete months in the given year */
		for ( std::int32_t m = 1; m < month; ++m )
		{
			totalDays += daysInMonth( year, m );
		}

		/* Add days in the current month (subtract 1 because day is 1-based) */
		totalDays += day - 1;

		return totalDays * constants::datetime::TICKS_PER_DAY;
	}

	std::int64_t DateTime::timeToTicks( std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t millisecond ) noexcept
	{
		return ( static_cast<std::int64_t>( hour ) * constants::datetime::TICKS_PER_HOUR ) +
			   ( static_cast<std::int64_t>( minute ) * constants::datetime::TICKS_PER_MINUTE ) +
			   ( static_cast<std::int64_t>( second ) * constants::datetime::TICKS_PER_SECOND ) +
			   ( static_cast<std::int64_t>( millisecond ) * constants::datetime::TICKS_PER_MILLISECOND );
	}

	bool DateTime::isValidDate( std::int32_t year, std::int32_t month, std::int32_t day ) noexcept
	{
		if ( year < constants::datetime::MIN_YEAR || year > constants::datetime::MAX_YEAR )
		{
			return false;
		}
		if ( month < 1 || month > 12 )
		{
			return false;
		}
		if ( day < 1 || day > daysInMonth( year, month ) )
		{
			return false;
		}

		return true;
	}

	bool DateTime::isValidTime( std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t millisecond ) noexcept
	{
		return hour >= 0 && hour <= 23 &&
			   minute >= 0 && minute <= 59 &&
			   second >= 0 && second <= 59 &&
			   millisecond >= 0 && millisecond <= 999;
	}

	//=====================================================================
	// DateTimeOffset class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	DateTimeOffset::DateTimeOffset( const DateTime& dateTime ) noexcept
		: m_dateTime{ dateTime },
		  m_offset{ systemTimezoneOffset( dateTime ) } {}

	DateTimeOffset::DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, const TimeSpan& offset ) noexcept
		: m_dateTime{ year, month, day },
		  m_offset{ offset }
	{
	}

	DateTimeOffset::DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day,
		std::int32_t hour, std::int32_t minute, std::int32_t second, const TimeSpan& offset ) noexcept
		: m_dateTime{ year, month, day, hour, minute, second },
		  m_offset{ offset }
	{
	}

	DateTimeOffset::DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second,
		std::int32_t millisecond, const TimeSpan& offset ) noexcept
		: m_dateTime{ year, month, day, hour, minute, second, millisecond },
		  m_offset{ offset }
	{
	}

	DateTimeOffset::DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second,
		std::int32_t millisecond, std::int32_t microsecond, const TimeSpan& offset ) noexcept
		: m_dateTime{ year, month, day, hour, minute, second, millisecond },
		  m_offset{ offset }
	{
		/* Add microseconds to the DateTime (1 microsecond = 10 ticks) */
		m_dateTime += TimeSpan{ microsecond * 10 };
	}

	DateTimeOffset::DateTimeOffset( std::string_view iso8601String )
		: m_dateTime{ DateTime::minValue() },
		  m_offset{ 0 }
	{
		if ( !tryParse( iso8601String, *this ) )
		{
			throw std::invalid_argument( "Invalid ISO 8601 DateTimeOffset string format" );
		}
	}

	//----------------------------------------------
	// Property accessors
	//----------------------------------------------

	DateTime DateTimeOffset::utcDateTime() const noexcept
	{
		return DateTime{ utcTicks() };
	}

	DateTime DateTimeOffset::localDateTime() const noexcept
	{
		/* Return the local DateTime component as-is */
		return m_dateTime;
	}

	//----------------------------------------------
	// Conversion methods
	//----------------------------------------------

	DateTimeOffset DateTimeOffset::date() const noexcept
	{
		return DateTimeOffset{ m_dateTime.date(), m_offset };
	}

	DateTimeOffset DateTimeOffset::toOffset( const TimeSpan& newOffset ) const noexcept
	{
		/* Convert to UTC, then apply new offset */
		const auto utcTime = utcDateTime();

		return DateTimeOffset{ utcTime + newOffset, newOffset };
	}

	DateTimeOffset DateTimeOffset::toUniversalTime() const noexcept
	{
		return DateTimeOffset{ utcDateTime(), TimeSpan{ 0 } };
	}

	DateTimeOffset DateTimeOffset::toLocalTime() const noexcept
	{
		const auto utcTime = utcDateTime();
		const auto localOffset = systemTimezoneOffset( utcTime );

		return DateTimeOffset{ utcTime + localOffset, localOffset };
	}

	std::int64_t DateTimeOffset::toFILETIME() const noexcept
	{
		/* Windows file time: 100-nanosecond intervals since January 1, 1601 UTC */
		const auto utcTicksValue = utcTicks();

		/* Guard against underflow for dates before FileTime epoch (Jan 1, 1601) */
		if ( utcTicksValue < constants::datetime::MICROSOFT_FILETIME_EPOCH_TICKS )
		{
			return 0; /* Return 0 for dates before FileTime epoch */
		}

		return utcTicksValue - constants::datetime::MICROSOFT_FILETIME_EPOCH_TICKS;
	}

	//----------------------------------------------
	// Arithmetic methods
	//----------------------------------------------

	DateTimeOffset DateTimeOffset::addDays( double days ) const noexcept
	{
		return DateTimeOffset{ m_dateTime + TimeSpan::fromDays( days ), m_offset };
	}

	DateTimeOffset DateTimeOffset::addHours( double hours ) const noexcept
	{
		return DateTimeOffset{ m_dateTime + TimeSpan::fromHours( hours ), m_offset };
	}

	DateTimeOffset DateTimeOffset::addMilliseconds( double milliseconds ) const noexcept
	{
		return DateTimeOffset{ m_dateTime + TimeSpan::fromMilliseconds( milliseconds ), m_offset };
	}

	DateTimeOffset DateTimeOffset::addMinutes( double minutes ) const noexcept
	{
		return DateTimeOffset{ m_dateTime + TimeSpan::fromMinutes( minutes ), m_offset };
	}

	DateTimeOffset DateTimeOffset::addMonths( std::int32_t months ) const noexcept
	{
		/* Extract date components */
		auto year = m_dateTime.year();
		auto month = m_dateTime.month();
		const auto day = m_dateTime.day();
		const auto timeOfDay = m_dateTime.timeOfDay();

		/* Add months with proper year overflow handling */
		month += months;
		while ( month > 12 )
		{
			month -= 12;
			year++;
		}
		while ( month < 1 )
		{
			month += 12;
			year--;
		}

		/* Handle day overflow (e.g., Jan 31 + 1 month = Feb 28/29) */
		const auto daysInTargetMonth = DateTime::daysInMonth( year, month );
		const auto adjustedDay = std::min( day, daysInTargetMonth );

		/* Create new DateTime and add back the time of day */
		const auto newDate = DateTime{ year, month, adjustedDay } + timeOfDay;

		return DateTimeOffset{ newDate, m_offset };
	}

	DateTimeOffset DateTimeOffset::addSeconds( double seconds ) const noexcept
	{
		return DateTimeOffset{ m_dateTime + TimeSpan::fromSeconds( seconds ), m_offset };
	}

	DateTimeOffset DateTimeOffset::addYears( std::int32_t years ) const noexcept
	{
		return addMonths( years * 12 );
	}

	//----------------------------------------------
	// String formatting
	//----------------------------------------------

	std::string DateTimeOffset::toString() const
	{
		return toString( DateTime::Format::Iso8601Basic );
	}

	std::string DateTimeOffset::toString( DateTime::Format format ) const
	{
		switch ( format )
		{
			case DateTime::Format::Iso8601Basic:
			case DateTime::Format::Iso8601Extended:
			case DateTime::Format::Iso8601WithOffset:
			{
				/* Format: "2024-01-01T12:00:00+02:00" or "2024-01-01T12:00:00.1234567+02:00" */
				std::ostringstream oss;
				oss << std::setfill( '0' );

				/* Date part */
				oss << std::setw( 4 ) << year() << '-' << std::setw( 2 ) << month() << '-' << std::setw( 2 ) << day() << 'T';

				/* Time part */
				oss << std::setw( 2 ) << hour() << ':' << std::setw( 2 ) << minute() << ':' << std::setw( 2 ) << second();

				/* Add fractional seconds for extended format */
				if ( format == DateTime::Format::Iso8601Extended )
				{
					const auto ms = millisecond();
					const auto us = microsecond();
					const auto ns = nanosecond();
					if ( ms > 0 || us > 0 || ns > 0 )
					{
						oss << '.' << std::setw( 3 ) << ms;
						if ( us > 0 || ns > 0 )
						{
							oss << std::setw( 3 ) << us;
							if ( ns > 0 )
							{
								/* Convert to single digit */
								oss << std::setw( 1 ) << ( ns / 100 );
							}
						}
					}
				}

				/* Offset part */
				const auto offsetMinutes = totalOffsetMinutes();
				if ( offsetMinutes == 0 )
				{
					oss << 'Z';
				}
				else
				{
					const auto absMinutes = std::abs( offsetMinutes );
					const auto offsetHours = absMinutes / 60;
					const auto offsetMins = absMinutes % 60;
					oss << ( offsetMinutes >= 0
								   ? '+'
								   : '-' )
						<< std::setw( 2 ) << offsetHours << ':' << std::setw( 2 ) << offsetMins;
				}

				return oss.str();
			}
			case DateTime::Format::DateOnly:
			{
				std::ostringstream oss;
				oss << std::setfill( '0' ) << std::setw( 4 ) << year() << '-' << std::setw( 2 ) << month() << '-' << std::setw( 2 ) << day();

				return oss.str();
			}
			case DateTime::Format::TimeOnly:
			{
				std::ostringstream oss;
				oss << std::setfill( '0' ) << std::setw( 2 ) << hour() << ':' << std::setw( 2 ) << minute() << ':' << std::setw( 2 ) << second();

				/* Add offset */
				const auto offsetMinutes = totalOffsetMinutes();
				if ( offsetMinutes == 0 )
				{
					oss << 'Z';
				}
				else
				{
					const auto absMinutes = std::abs( offsetMinutes );
					const auto offsetHours = absMinutes / 60;
					const auto offsetMins = absMinutes % 60;
					oss << ( offsetMinutes >= 0
								   ? '+'
								   : '-' )
						<< std::setw( 2 ) << offsetHours << ':' << std::setw( 2 ) << offsetMins;
				}

				return oss.str();
			}
			case DateTime::Format::UnixSeconds:
			{
				return std::to_string( toUnixSeconds() );
			}
			case DateTime::Format::UnixMilliseconds:
			{
				return std::to_string( toUnixMilliseconds() );
			}
			default:
			{
				return toString( DateTime::Format::Iso8601Basic );
			}
		}
	}

	std::string DateTimeOffset::toIso8601Extended() const
	{
		return toString( DateTime::Format::Iso8601Extended );
	}

	//----------------------------------------------
	// Validation methods
	//----------------------------------------------

	bool DateTimeOffset::isValid() const noexcept
	{
		return m_dateTime.isValid() && isValidOffset( m_offset );
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	DateTimeOffset DateTimeOffset::now() noexcept
	{
		const auto utcNow = DateTime::now();
		const auto localOffset = systemTimezoneOffset( utcNow );

		return DateTimeOffset{ utcNow + localOffset, localOffset };
	}

	DateTimeOffset DateTimeOffset::utcNow() noexcept
	{
		return DateTimeOffset{ DateTime::now(), TimeSpan{ 0 } };
	}

	DateTimeOffset DateTimeOffset::today() noexcept
	{
		/* Get current local time, then extract just the date part at midnight */
		const auto localNow = DateTimeOffset::now();

		/* Extract local date components */
		const auto year = localNow.year();
		const auto month = localNow.month();
		const auto day = localNow.day();

		/* Create local midnight for today with the same offset */
		return DateTimeOffset{ year, month, day, 0, 0, 0, localNow.offset() };
	}

	DateTimeOffset DateTimeOffset::parse( std::string_view str )
	{
		DateTimeOffset result;
		if ( !tryParse( str, result ) )
		{
			throw std::invalid_argument( "Invalid ISO 8601 DateTimeOffset string format" );
		}
		return result;
	}

	bool DateTimeOffset::tryParse( std::string_view str, DateTimeOffset& result ) noexcept
	{
		/*
			TODO: Parser is too permissive - needs stricter ISO 8601 compliance:
			1. Should REQUIRE timezone information (Z, +HH:MM, or -HH:MM)
			2. Should only accept standard ±HH:MM format, not +H, +HH, or +HHMM variations
			3. Should validate numeric characters manually instead of using std::stoi()
			4. Should enforce minimum string length for DateTimeOffset format
			5. Current behavior allows parsing without timezone = invalid for DateTimeOffset
		*/
		try
		{
			/* ISO 8601 parsing with comprehensive offset format support */
			std::string s( str );
			TimeSpan offset{ 0 };
			DateTime dateTime;

			/* Track if we successfully parsed an offset */
			bool offsetParsed = false;

			/* Find timezone indicator - search from right to avoid matching negative years */
			auto offsetPos = std::string::npos;
			for ( auto i = s.length(); i > 10; --i )
			{
				/* Skip date part (YYYY-MM-DD = 10 chars) */
				if ( s[i - 1] == 'Z' || s[i - 1] == '+' || s[i - 1] == '-' )
				{
					offsetPos = i - 1;
					break;
				}
			}

			if ( offsetPos != std::string::npos )
			{
				if ( s[offsetPos] == 'Z' )
				{
					/* UTC indicator */
					offset = TimeSpan{ 0 };
					offsetParsed = true;
					s = s.substr( 0, offsetPos );
				}
				else
				{
					/* Parse offset: supports +HH:MM, +HHMM, +HH formats */
					auto offsetStr = s.substr( offsetPos );
					s = s.substr( 0, offsetPos );

					/* Minimum: +H or -H */
					if ( offsetStr.length() >= 2 )
					{
						const bool isNegative = offsetStr[0] == '-';
						const auto numericPart = offsetStr.substr( 1 );

						std::int32_t hours = 0;
						std::int32_t minutes = 0;

						auto colonPos = numericPart.find( ':' );
						if ( colonPos != std::string::npos )
						{
							/* Format: +HH:MM or +H:MM */
							if ( colonPos > 0 && colonPos < numericPart.length() - 1 )
							{
								hours = std::stoi( numericPart.substr( 0, colonPos ) );
								minutes = std::stoi( numericPart.substr( colonPos + 1 ) );
							}
						}
						else if ( numericPart.length() == 4 )
						{
							/* Format: +HHMM */
							hours = std::stoi( numericPart.substr( 0, 2 ) );
							minutes = std::stoi( numericPart.substr( 2, 2 ) );
						}
						else if ( numericPart.length() == 2 || numericPart.length() == 1 )
						{
							/* Format: +HH or +H */
							hours = std::stoi( numericPart );
							minutes = 0;
						}

						/* Validate offset components - ISO 8601 allows ±14:00 maximum */
						if ( hours >= 0 && minutes >= 0 && minutes <= 59 )
						{
							const auto totalMinutes = hours * 60 + minutes;
							/* Maximum valid offset is ±14:00 (840 minutes) exactly */
							if ( totalMinutes <= 840 )
							{
								offset = TimeSpan::fromMinutes( isNegative
																	? -totalMinutes
																	: totalMinutes );
								offsetParsed = true;
							}
						}
					}
				}
			}
			else
			{
				/* No timezone indicator found - assume this is valid for DateTime parsing */
				offsetParsed = true;
			}

			/* Parse the datetime part */
			if ( DateTime::tryParse( s, dateTime ) && offsetParsed )
			{
				/* Validate offset range before creating DateTimeOffset */
				if ( isValidOffset( offset ) )
				{
					result = DateTimeOffset{ dateTime, offset };
					return true;
				}
			}
		}
		catch ( ... )
		{
			/* Parsing failed - tryParse methods should not throw exceptions */
		}

		return false;
	}

	DateTimeOffset DateTimeOffset::fromUnixTimeSeconds( std::int64_t seconds ) noexcept
	{
		return DateTimeOffset{ DateTime::sinceEpochSeconds( seconds ), TimeSpan{ 0 } };
	}

	DateTimeOffset DateTimeOffset::fromUnixTimeMilliseconds( std::int64_t milliseconds ) noexcept
	{
		return DateTimeOffset{ DateTime::sinceEpochMilliseconds( milliseconds ), TimeSpan{ 0 } };
	}

	DateTimeOffset DateTimeOffset::fromFileTime( std::int64_t fileTime ) noexcept
	{
		/* Windows file time: 100-nanosecond intervals since January 1, 1601 UTC */
		const auto ticks = fileTime + constants::datetime::MICROSOFT_FILETIME_EPOCH_TICKS;

		return DateTimeOffset{ DateTime{ ticks }, TimeSpan{ 0 } };
	}

	//----------------------------------------------
	// Internal helper methods
	//----------------------------------------------

	bool DateTimeOffset::isValidOffset( const TimeSpan& offset ) noexcept
	{
		/* Valid timezone offsets are ±14:00:00 (±840 minutes)
		   Use integer tick comparison to avoid floating-point precision issues */
		const auto offsetTicks = offset.ticks();

		/* ±14 hours = ±840 minutes = ±50,400 seconds = ±504,000,000,000 ticks */
		constexpr std::int64_t MAX_OFFSET_TICKS = 14LL * 60LL * 60LL * constants::datetime::TICKS_PER_SECOND;

		return offsetTicks >= -MAX_OFFSET_TICKS && offsetTicks <= MAX_OFFSET_TICKS;
	}

	TimeSpan DateTimeOffset::systemTimezoneOffset( const DateTime& dateTime ) noexcept
	{
		/* Get system timezone offset for the given UTC time
		   MinGW/GCC on Windows doesn't support C++20 timezone features */

#if defined( _MSC_VER ) && _MSC_VER >= 1929 && __cpp_lib_chrono >= 201907L
		/* C++20 timezone support - MSVC only */
		try
		{
			using namespace std::chrono;
			const auto sysTp = sys_time<nanoseconds>( nanoseconds( dateTime.ticks() * 100 ) );

			if ( const std::chrono::time_zone* zone = std::chrono::current_zone() )
			{
				const auto info = zone->get_info( sysTp );
				return TimeSpan::fromSeconds( static_cast<double>( info.offset.count() ) );
			}
		}
		catch ( ... )
		{
			/* Fall through to legacy implementation */
		}
#endif

		/* Legacy implementation for MinGW/GCC and fallback for MSVC */
		const auto timePoint = std::chrono::system_clock::time_point(
			std::chrono::duration_cast<std::chrono::system_clock::duration>(
				std::chrono::nanoseconds( dateTime.ticks() * 100 ) ) );

		const auto localTime = std::chrono::system_clock::to_time_t( timePoint );

		/* Use thread-safe time functions */
		std::tm utcTm{};
		std::tm localTm{};

		/* Use the cross-platform macros from Platform.h */
		if ( !VISTA_SDK_CPP_GMTIME_R( &localTime, &utcTm ) || !VISTA_SDK_CPP_LOCALTIME_R( &localTime, &localTm ) )
		{
			/* Fallback to UTC on error */
			return TimeSpan{ 0 };
		}

		/* Calculate offset in seconds */
		const auto utcSeconds = utcTm.tm_hour * 3600 + utcTm.tm_min * 60 + utcTm.tm_sec;
		const auto localSeconds = localTm.tm_hour * 3600 + localTm.tm_min * 60 + localTm.tm_sec;

		/* Handle day boundary crossings */
		auto offsetSeconds = localSeconds - utcSeconds;
		if ( localTm.tm_mday != utcTm.tm_mday )
		{
			if ( localTm.tm_mday > utcTm.tm_mday )
			{
				offsetSeconds += 24 * 3600;
			}
			else
			{
				offsetSeconds -= 24 * 3600;
			}
		}

		return TimeSpan::fromSeconds( offsetSeconds );
	}

	//=====================================================================
	// Stream operators
	//=====================================================================

	std::ostream& operator<<( std::ostream& os, const DateTime& dateTime )
	{
		os << dateTime.toString();

		return os;
	}

	std::istream& operator>>( std::istream& is, DateTime& dateTime )
	{
		std::string str;
		is >> str;
		if ( !DateTime::tryParse( str, dateTime ) )
		{
			is.setstate( std::ios::failbit );
		}

		return is;
	}

	std::ostream& operator<<( std::ostream& os, const DateTimeOffset& dateTimeOffset )
	{
		return os << dateTimeOffset.toString();
	}

	std::istream& operator>>( std::istream& is, DateTimeOffset& dateTimeOffset )
	{
		std::string str;
		is >> str;

		if ( !DateTimeOffset::tryParse( str, dateTimeOffset ) )
		{
			is.setstate( std::ios::failbit );
		}

		return is;
	}
}
