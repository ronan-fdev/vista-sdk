/**
 * @file TESTS_DateTime8601.cpp
 * @brief Comprehensive tests for DateTime, TimeSpan, and DateTimeOffset classes
 * @details Tests UTC-only datetime operations with 100-nanosecond precision,
 *          timezone-aware DateTimeOffset operations, ISO 8601 parsing/formatting,
 *          and cross-platform compatibility
 */

#include "dnv/vista/sdk/constants/DateTimeConstants.h"
#include "dnv/vista/sdk/datatypes/DateTimeISO8601.h"

namespace dnv::vista::sdk::test
{
	//=====================================================================
	// TimeSpan Tests
	//=====================================================================

	class TimeSpanTest : public ::testing::Test
	{
	protected:
		void SetUp() override {}
		void TearDown() override {}
	};

	TEST_F( TimeSpanTest, Construction )
	{
		/* Default constructor */
		datatypes::TimeSpan ts1;
		EXPECT_EQ( 0, ts1.ticks() );

		/* Constructor with ticks*/
		datatypes::TimeSpan ts2{ 1234567890 };
		EXPECT_EQ( 1234567890, ts2.ticks() );

		/* Copy constructor*/
		datatypes::TimeSpan ts3{ ts2 };
		EXPECT_EQ( ts2.ticks(), ts3.ticks() );

		/* Move constructor */
		datatypes::TimeSpan ts4{ datatypes::TimeSpan{ 9876543210 } };
		EXPECT_EQ( 9876543210, ts4.ticks() );
	}

	TEST_F( TimeSpanTest, ComparisonOperators )
	{
		datatypes::TimeSpan ts1{ 1000 };
		datatypes::TimeSpan ts2{ 2000 };
		datatypes::TimeSpan ts3{ 1000 };

		/* Equality */
		EXPECT_TRUE( ts1 == ts3 );
		EXPECT_FALSE( ts1 == ts2 );
		EXPECT_TRUE( ts1 != ts2 );
		EXPECT_FALSE( ts1 != ts3 );

		/* Ordering*/
		EXPECT_TRUE( ts1 < ts2 );
		EXPECT_FALSE( ts2 < ts1 );
		EXPECT_TRUE( ts1 <= ts2 );
		EXPECT_TRUE( ts1 <= ts3 );
		EXPECT_TRUE( ts2 > ts1 );
		EXPECT_FALSE( ts1 > ts2 );
		EXPECT_TRUE( ts2 >= ts1 );
		EXPECT_TRUE( ts3 >= ts1 );
	}

	TEST_F( TimeSpanTest, ArithmeticOperators )
	{
		datatypes::TimeSpan ts1{ 1000 };
		datatypes::TimeSpan ts2{ 500 };

		/* Addition */
		datatypes::TimeSpan sum = ts1 + ts2;
		EXPECT_EQ( 1500, sum.ticks() );

		/* Subtraction */
		datatypes::TimeSpan diff = ts1 - ts2;
		EXPECT_EQ( 500, diff.ticks() );

		/* Unary minus */
		datatypes::TimeSpan neg = -ts1;
		EXPECT_EQ( -1000, neg.ticks() );

		/* In-place addition */
		ts1 += ts2;
		EXPECT_EQ( 1500, ts1.ticks() );

		/* In-place subtraction */
		ts1 -= ts2;
		EXPECT_EQ( 1000, ts1.ticks() );
	}

	TEST_F( TimeSpanTest, PropertyAccessors )
	{
		/* 1 day = 24 hours = 1440 minutes = 86400 seconds = 864000000000 ticks */
		datatypes::TimeSpan oneDay{ constants::datetime::TICKS_PER_DAY };

		EXPECT_DOUBLE_EQ( 1.0, oneDay.totalDays() );
		EXPECT_DOUBLE_EQ( 24.0, oneDay.totalHours() );
		EXPECT_DOUBLE_EQ( 1440.0, oneDay.totalMinutes() );
		EXPECT_DOUBLE_EQ( 86400.0, oneDay.totalSeconds() );
		EXPECT_DOUBLE_EQ( 86400000.0, oneDay.totalMilliseconds() );

		/* Test fractional values */
		datatypes::TimeSpan halfDay{ constants::datetime::TICKS_PER_DAY / 2 };
		EXPECT_DOUBLE_EQ( 0.5, halfDay.totalDays() );
		EXPECT_DOUBLE_EQ( 12.0, halfDay.totalHours() );
	}

	TEST_F( TimeSpanTest, StaticFactoryMethods )
	{
		auto fromDays = datatypes::TimeSpan::fromDays( 2.5 );
		EXPECT_EQ( static_cast<std::int64_t>( 2.5 * constants::datetime::TICKS_PER_DAY ), fromDays.ticks() );

		auto fromHours = datatypes::TimeSpan::fromHours( 3.0 );
		EXPECT_EQ( 3 * constants::datetime::TICKS_PER_HOUR, fromHours.ticks() );

		auto fromMinutes = datatypes::TimeSpan::fromMinutes( 90.0 );
		EXPECT_EQ( 90 * constants::datetime::TICKS_PER_MINUTE, fromMinutes.ticks() );

		auto fromSeconds = datatypes::TimeSpan::fromSeconds( 30.5 );
		EXPECT_EQ( static_cast<std::int64_t>( 30.5 * constants::datetime::TICKS_PER_SECOND ), fromSeconds.ticks() );

		auto fromMs = datatypes::TimeSpan::fromMilliseconds( 1500.0 );
		EXPECT_EQ( 1500 * constants::datetime::TICKS_PER_MILLISECOND, fromMs.ticks() );
	}

	//=====================================================================
	// DateTime Tests
	//=====================================================================

	class DateTimeTest : public ::testing::Test
	{
	protected:
		void SetUp() override {}
		void TearDown() override {}
	};

	TEST_F( DateTimeTest, Construction )
	{
		/* Default constructor */
		datatypes::DateTime dt1;
		EXPECT_EQ( constants::datetime::MIN_DATETIME_TICKS, dt1.ticks() );

		/* Constructor with ticks */
		datatypes::TimeSpan dt2{ constants::datetime::UNIX_EPOCH_TICKS };
		EXPECT_EQ( constants::datetime::UNIX_EPOCH_TICKS, dt2.ticks() );

		/* Constructor from date components */
		datatypes::DateTime dt3{ 2024, 1, 15 };
		EXPECT_EQ( 2024, dt3.year() );
		EXPECT_EQ( 1, dt3.month() );
		EXPECT_EQ( 15, dt3.day() );
		EXPECT_EQ( 0, dt3.hour() );
		EXPECT_EQ( 0, dt3.minute() );
		EXPECT_EQ( 0, dt3.second() );

		/* Constructor from date and time components */
		datatypes::DateTime dt4{ 2024, 6, 15, 14, 30, 45 };
		EXPECT_EQ( 2024, dt4.year() );
		EXPECT_EQ( 6, dt4.month() );
		EXPECT_EQ( 15, dt4.day() );
		EXPECT_EQ( 14, dt4.hour() );
		EXPECT_EQ( 30, dt4.minute() );
		EXPECT_EQ( 45, dt4.second() );

		/* Constructor with milliseconds */
		datatypes::DateTime dt5{ 2024, 12, 25, 23, 59, 59, 999 };
		EXPECT_EQ( 2024, dt5.year() );
		EXPECT_EQ( 12, dt5.month() );
		EXPECT_EQ( 25, dt5.day() );
		EXPECT_EQ( 23, dt5.hour() );
		EXPECT_EQ( 59, dt5.minute() );
		EXPECT_EQ( 59, dt5.second() );
		EXPECT_EQ( 999, dt5.millisecond() );
	}

	TEST_F( DateTimeTest, SystemClockConstructor )
	{
		auto now = std::chrono::system_clock::now();
		datatypes::DateTime dt{ now };

		/* Should be within reasonable range of current time */
		auto currentTime = datatypes::DateTime::now();
		datatypes::TimeSpan diff = currentTime - dt;

		/* Should be within 1 second difference */
		EXPECT_LT( std::abs( diff.totalSeconds() ), 1.0 );
	}

	TEST_F( DateTimeTest, ISO8601StringConstructor )
	{
		/* Basic ISO 8601 format */
		datatypes::DateTime dt1{ "2024-06-15T14:30:45Z" };
		EXPECT_EQ( 2024, dt1.year() );
		EXPECT_EQ( 6, dt1.month() );
		EXPECT_EQ( 15, dt1.day() );
		EXPECT_EQ( 14, dt1.hour() );
		EXPECT_EQ( 30, dt1.minute() );
		EXPECT_EQ( 45, dt1.second() );

		/* ISO 8601 with fractional seconds */
		datatypes::DateTime dt2{ "2024-01-01T00:00:00.1234567Z" };
		EXPECT_EQ( 2024, dt2.year() );
		EXPECT_EQ( 1, dt2.month() );
		EXPECT_EQ( 1, dt2.day() );
		EXPECT_EQ( 0, dt2.hour() );
		EXPECT_EQ( 0, dt2.minute() );
		EXPECT_EQ( 0, dt2.second() );

		/* Invalid format should throw */
		EXPECT_THROW( datatypes::DateTime( "invalid-date-string" ), std::invalid_argument );
		EXPECT_THROW( datatypes::DateTime( "2024-13-01T00:00:00Z" ), std::invalid_argument );
	}

	TEST_F( DateTimeTest, ComparisonOperators )
	{
		datatypes::DateTime dt1{ 2024, 1, 1, 12, 0, 0 };
		datatypes::DateTime dt2{ 2024, 1, 1, 13, 0, 0 };
		datatypes::DateTime dt3{ 2024, 1, 1, 12, 0, 0 };

		EXPECT_TRUE( dt1 == dt3 );
		EXPECT_FALSE( dt1 == dt2 );
		EXPECT_TRUE( dt1 != dt2 );
		EXPECT_FALSE( dt1 != dt3 );

		EXPECT_TRUE( dt1 < dt2 );
		EXPECT_FALSE( dt2 < dt1 );
		EXPECT_TRUE( dt1 <= dt2 );
		EXPECT_TRUE( dt1 <= dt3 );
		EXPECT_TRUE( dt2 > dt1 );
		EXPECT_FALSE( dt1 > dt2 );
		EXPECT_TRUE( dt2 >= dt1 );
		EXPECT_TRUE( dt3 >= dt1 );
	}

	TEST_F( DateTimeTest, ArithmeticOperators )
	{
		datatypes::DateTime dt{ 2024, 6, 15, 12, 0, 0 };
		auto oneHour = datatypes::TimeSpan::fromHours( 1.0 );
		auto oneDay = datatypes::TimeSpan::fromDays( 1.0 );

		/* Addition */
		datatypes::DateTime dtPlusHour = dt + oneHour;
		EXPECT_EQ( 13, dtPlusHour.hour() );

		/* Subtraction */
		datatypes::DateTime dtMinusHour = dt - oneHour;
		EXPECT_EQ( 11, dtMinusHour.hour() );

		/* DateTime difference */
		datatypes::TimeSpan diff = dtPlusHour - dt;
		EXPECT_DOUBLE_EQ( 1.0, diff.totalHours() );

		/* In-place operations */
		dt += oneDay;
		EXPECT_EQ( 16, dt.day() );

		dt -= oneDay;
		EXPECT_EQ( 15, dt.day() );
	}

	TEST_F( DateTimeTest, PropertyAccessors )
	{
		datatypes::DateTime dt{ 2024, 6, 15, 14, 30, 45, 123 };

		EXPECT_EQ( 2024, dt.year() );
		EXPECT_EQ( 6, dt.month() );
		EXPECT_EQ( 15, dt.day() );
		EXPECT_EQ( 14, dt.hour() );
		EXPECT_EQ( 30, dt.minute() );
		EXPECT_EQ( 45, dt.second() );
		EXPECT_EQ( 123, dt.millisecond() );

		/* Day of week (0=Sunday, 6=Saturday) - June 15, 2024 is a Saturday */
		EXPECT_EQ( 6, dt.dayOfWeek() );

		/* Day of year calculation - Jan+Feb(leap)+Mar+Apr+May+15days */
		int expectedDayOfYear = 31 + 29 + 31 + 30 + 31 + 15;
		EXPECT_EQ( expectedDayOfYear, dt.dayOfYear() );
	}

	TEST_F( DateTimeTest, ConversionMethods )
	{
		datatypes::DateTime epoch = datatypes::DateTime::epoch();

		/* Unix timestamp conversions */
		EXPECT_EQ( 0, epoch.toUnixSeconds() );
		EXPECT_EQ( 0, epoch.toUnixMilliseconds() );

		/* 2021-01-01T00:00:00Z */
		datatypes::DateTime dt = datatypes::DateTime::sinceEpochSeconds( 1609459200 );
		EXPECT_EQ( 2021, dt.year() );
		EXPECT_EQ( 1, dt.month() );
		EXPECT_EQ( 1, dt.day() );

		/* Date extraction */
		datatypes::DateTime dtWithTime{ 2024, 6, 15, 14, 30, 45 };
		datatypes::DateTime dateOnly = dtWithTime.date();
		EXPECT_EQ( 2024, dateOnly.year() );
		EXPECT_EQ( 6, dateOnly.month() );
		EXPECT_EQ( 15, dateOnly.day() );
		EXPECT_EQ( 0, dateOnly.hour() );
		EXPECT_EQ( 0, dateOnly.minute() );
		EXPECT_EQ( 0, dateOnly.second() );

		/* Time of day extraction */
		datatypes::TimeSpan timeOfDay = dtWithTime.timeOfDay();
		EXPECT_DOUBLE_EQ( 14.0, timeOfDay.totalHours() - 30.0 / 60.0 - 45.0 / 3600.0 );
	}

	TEST_F( DateTimeTest, StringFormatting )
	{
		datatypes::DateTime dt{ 2024, 6, 15, 14, 30, 45, 123 };

		/* Basic ISO 8601 */
		std::string basic = dt.toString();
		EXPECT_EQ( "2024-06-15T14:30:45Z", basic );

		/* Specific formats */
		EXPECT_EQ( "2024-06-15T14:30:45Z", dt.toString( datatypes::DateTime::Format::Iso8601Basic ) );
		EXPECT_EQ( "2024-06-15", dt.toString( datatypes::DateTime::Format::DateOnly ) );
		EXPECT_EQ( "14:30:45", dt.toString( datatypes::DateTime::Format::TimeOnly ) );
		EXPECT_EQ( "2024-06-15T14:30:45+00:00", dt.toString( datatypes::DateTime::Format::Iso8601WithOffset ) );

		/* Extended format with fractional seconds */
		std::string extended = dt.toString( datatypes::DateTime::Format::Iso8601Extended );
		EXPECT_TRUE( extended.find( "2024-06-15T14:30:45." ) != std::string::npos );
		EXPECT_TRUE( extended.find( "Z" ) != std::string::npos );

		/* Unix timestamps */
		datatypes::DateTime epoch = datatypes::DateTime::epoch();
		EXPECT_EQ( "0", epoch.toString( datatypes::DateTime::Format::UnixSeconds ) );
		EXPECT_EQ( "0", epoch.toString( datatypes::DateTime::Format::UnixMilliseconds ) );
	}

	TEST_F( DateTimeTest, ValidationMethods )
	{
		/* Valid DateTime */
		datatypes::DateTime validDt{ 2024, 6, 15, 12, 30, 45 };
		EXPECT_TRUE( validDt.isValid() );

		/* Leap year tests */
		EXPECT_TRUE( datatypes::DateTime::isLeapYear( 2024 ) );
		EXPECT_TRUE( datatypes::DateTime::isLeapYear( 2000 ) );
		EXPECT_FALSE( datatypes::DateTime::isLeapYear( 1900 ) );
		EXPECT_FALSE( datatypes::DateTime::isLeapYear( 2023 ) );

		/* Days in month tests */
		EXPECT_EQ( 31, datatypes::DateTime::daysInMonth( 2024, 1 ) );
		EXPECT_EQ( 29, datatypes::DateTime::daysInMonth( 2024, 2 ) );
		EXPECT_EQ( 28, datatypes::DateTime::daysInMonth( 2023, 2 ) );
		EXPECT_EQ( 30, datatypes::DateTime::daysInMonth( 2024, 4 ) );
		EXPECT_EQ( 31, datatypes::DateTime::daysInMonth( 2024, 12 ) );
		EXPECT_EQ( 0, datatypes::DateTime::daysInMonth( 2024, 13 ) );
	}

	TEST_F( DateTimeTest, StaticFactoryMethods )
	{
		/* Min and max values */
		auto minVal = datatypes::DateTime::minValue();
		auto maxVal = datatypes::DateTime::maxValue();
		EXPECT_EQ( constants::datetime::MIN_DATETIME_TICKS, minVal.ticks() );
		EXPECT_EQ( constants::datetime::MAX_DATETIME_TICKS, maxVal.ticks() );

		/* Epoch */
		auto epoch = datatypes::DateTime::epoch();
		EXPECT_EQ( constants::datetime::UNIX_EPOCH_TICKS, epoch.ticks() );
		EXPECT_EQ( 1970, epoch.year() );
		EXPECT_EQ( 1, epoch.month() );
		EXPECT_EQ( 1, epoch.day() );

		/* Now and today (basic smoke tests)  */
		auto now = datatypes::DateTime::now();
		auto today = datatypes::DateTime::today();
		EXPECT_TRUE( now.isValid() );
		EXPECT_TRUE( today.isValid() );
		EXPECT_EQ( 0, today.hour() );
		EXPECT_EQ( 0, today.minute() );
		EXPECT_EQ( 0, today.second() );

		/* Factory from timestamps */
		auto fromSeconds = datatypes::DateTime::sinceEpochSeconds( 86400 );
		EXPECT_EQ( 1970, fromSeconds.year() );
		EXPECT_EQ( 1, fromSeconds.month() );
		EXPECT_EQ( 2, fromSeconds.day() );

		auto fromMs = datatypes::DateTime::sinceEpochMilliseconds( 86400000 );
		EXPECT_EQ( 1970, fromMs.year() );
		EXPECT_EQ( 1, fromMs.month() );
		EXPECT_EQ( 2, fromMs.day() );
	}

	TEST_F( DateTimeTest, TryParseMethod )
	{
		datatypes::DateTime result;

		/* Valid parsing */
		EXPECT_TRUE( datatypes::DateTime::tryParse( "2024-06-15T14:30:45Z", result ) );
		EXPECT_EQ( 2024, result.year() );
		EXPECT_EQ( 6, result.month() );
		EXPECT_EQ( 15, result.day() );
		EXPECT_EQ( 14, result.hour() );
		EXPECT_EQ( 30, result.minute() );
		EXPECT_EQ( 45, result.second() );

		/* Valid parsing with fractional seconds */
		EXPECT_TRUE( datatypes::DateTime::tryParse( "2024-01-01T00:00:00.123Z", result ) );
		EXPECT_EQ( 2024, result.year() );

		/* Invalid parsing */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "invalid-date", result ) );
		EXPECT_FALSE( datatypes::DateTime::tryParse( "2024-13-01T00:00:00Z", result ) );
		EXPECT_FALSE( datatypes::DateTime::tryParse( "2024-01-32T00:00:00Z", result ) );
		EXPECT_FALSE( datatypes::DateTime::tryParse( "2024-01-01T25:00:00Z", result ) );
	}

	TEST_F( DateTimeTest, StreamOperators )
	{
		datatypes::DateTime dt{ 2024, 6, 15, 14, 30, 45 };

		/* Output stream operator */
		std::ostringstream oss;
		oss << dt;
		EXPECT_EQ( "2024-06-15T14:30:45Z", oss.str() );

		/* Input stream operator */
		std::istringstream iss( "2024-12-25T23:59:59Z" );
		datatypes::DateTime parsed;
		iss >> parsed;
		EXPECT_EQ( 2024, parsed.year() );
		EXPECT_EQ( 12, parsed.month() );
		EXPECT_EQ( 25, parsed.day() );
		EXPECT_EQ( 23, parsed.hour() );
		EXPECT_EQ( 59, parsed.minute() );
		EXPECT_EQ( 59, parsed.second() );

		/* Invalid input should set fail bit */
		std::istringstream issInvalid( "invalid-date" );
		datatypes::DateTime invalidParsed;
		issInvalid >> invalidParsed;
		EXPECT_TRUE( issInvalid.fail() );
	}

	TEST_F( DateTimeTest, EdgeCases )
	{
		/* Boundary values */
		auto minDateTime = datatypes::DateTime::minValue();
		auto maxDateTime = datatypes::DateTime::maxValue();

		EXPECT_EQ( constants::datetime::MIN_YEAR, minDateTime.year() );
		EXPECT_EQ( constants::datetime::MAX_YEAR, maxDateTime.year() );

		/* Invalid date construction should clamp to minimum */
		datatypes::DateTime invalidDate{ 0, 0, 0 };
		EXPECT_EQ( constants::datetime::MIN_DATETIME_TICKS, invalidDate.ticks() );

		/* Invalid time construction should clamp to minimum */
		datatypes::DateTime invalidTime{ 2024, 1, 1, -1, -1, -1, -1 };
		EXPECT_EQ( constants::datetime::MIN_DATETIME_TICKS, invalidTime.ticks() );

		/* Leap year edge cases */
		datatypes::DateTime leapDay2024{ 2024, 2, 29 };
		EXPECT_EQ( 29, leapDay2024.day() );

		/* End of year */
		datatypes::DateTime endOfYear{ 2024, 12, 31, 23, 59, 59, 999 };
		EXPECT_EQ( 366, endOfYear.dayOfYear() );
	}

	//=====================================================================
	// Integration Tests
	//=====================================================================

	TEST_F( DateTimeTest, DateTimeTimeSpanIntegration )
	{
		datatypes::DateTime start{ 2024, 1, 1, 0, 0, 0 };
		datatypes::DateTime end{ 2024, 1, 2, 0, 0, 0 };

		datatypes::TimeSpan duration = end - start;
		EXPECT_DOUBLE_EQ( 1.0, duration.totalDays() );
		EXPECT_DOUBLE_EQ( 24.0, duration.totalHours() );

		datatypes::DateTime calculated = start + duration;
		EXPECT_EQ( end.ticks(), calculated.ticks() );
	}

	TEST_F( DateTimeTest, RoundTripSerialization )
	{
		datatypes::DateTime original{ 2024, 6, 15, 14, 30, 45, 123 };

		/* Round trip through ISO 8601 string */
		std::string serialized = original.toString( datatypes::DateTime::Format::Iso8601Extended );
		datatypes::DateTime deserialized;
		EXPECT_TRUE( datatypes::DateTime::tryParse( serialized, deserialized ) );

		/* Should be very close (within millisecond precision) */
		datatypes::TimeSpan diff = original - deserialized;
		EXPECT_LT( std::abs( diff.totalMilliseconds() ), 1.0 );
	}

	TEST_F( DateTimeTest, CrossPlatformConsistency )
	{
		/* Test that key values are consistent across platforms */
		auto epoch = datatypes::DateTime::epoch();
		EXPECT_EQ( 1970, epoch.year() );
		EXPECT_EQ( 1, epoch.month() );
		EXPECT_EQ( 1, epoch.day() );
		EXPECT_EQ( 0, epoch.hour() );
		EXPECT_EQ( 0, epoch.minute() );
		EXPECT_EQ( 0, epoch.second() );

		/* Known Unix timestamp - 2021-01-01T00:00:00Z*/
		auto knownDate = datatypes::DateTime::sinceEpochSeconds( 1609459200 );
		EXPECT_EQ( 2021, knownDate.year() );
		EXPECT_EQ( 1, knownDate.month() );
		EXPECT_EQ( 1, knownDate.day() );
	}

	TEST_F( DateTimeTest, ISO8601FormatValidationEdgeCases )
	{
		datatypes::DateTime result;

		/* Invalid ISO 8601 format variations that ACTUALLY fail based on parser implementation */

		/* Wrong separators - parser expects exact dashes for dates and colons for times */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994/11/20T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994/11/20T10:25:33Z" ), std::invalid_argument );

		/* 2-digit year - parser expects exactly 4 digits */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "94-11-20T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "94-11-20T10:25:33Z" ), std::invalid_argument );

		/* Missing date separators - parser expects exact format */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "19941120T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "19941120T10:25:33Z" ), std::invalid_argument );

		/* Wrong time datatypes::separators - parser expects colons */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994-11-20T10.25.33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994-11-20T10.25.33Z" ), std::invalid_argument );

		/* Missing time separators */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994-11-20T102533Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994-11-20T102533Z" ), std::invalid_argument );

		/* Non-numeric characters in date/time components */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "ABCD-11-20T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "ABCD-11-20T10:25:33Z" ), std::invalid_argument );

		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994-AB-20T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994-AB-20T10:25:33Z" ), std::invalid_argument );

		/* Empty or too short strings */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "", result ) );
		EXPECT_THROW( datatypes::DateTime( "" ), std::invalid_argument );

		EXPECT_FALSE( datatypes::DateTime::tryParse( "123", result ) );
		EXPECT_THROW( datatypes::DateTime( "123" ), std::invalid_argument );

		/* Completely malformed strings */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "not-a-date", result ) );
		EXPECT_THROW( datatypes::DateTime( "not-a-date" ), std::invalid_argument );

		EXPECT_FALSE( datatypes::DateTime::tryParse( "random text", result ) );
		EXPECT_THROW( datatypes::DateTime( "random text" ), std::invalid_argument );
	}

	TEST_F( DateTimeTest, DateTimeBoundaryViolations )
	{
		datatypes::DateTime result;

		/* Date/time boundary violations */

		/* Invalid month - months must be 1-12 */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994-13-20T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994-13-20T10:25:33Z" ), std::invalid_argument );
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994-00-20T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994-00-20T10:25:33Z" ), std::invalid_argument );

		/* Invalid day - February 30th doesn't exist */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994-02-30T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994-02-30T10:25:33Z" ), std::invalid_argument );
		/* Day 32 doesn't exist in any month */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994-01-32T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994-01-32T10:25:33Z" ), std::invalid_argument );
		/* Day 0 is invalid */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994-11-00T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994-11-00T10:25:33Z" ), std::invalid_argument );

		/* Invalid hour - hours must be 0-23 */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994-11-20T25:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994-11-20T25:25:33Z" ), std::invalid_argument );
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994-11-20T24:00:00Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994-11-20T24:00:00Z" ), std::invalid_argument );

		/* Invalid minute/second - minutes and seconds must be 0-59 */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994-11-20T10:60:33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994-11-20T10:60:33Z" ), std::invalid_argument );
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994-11-20T10:25:60Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994-11-20T10:25:60Z" ), std::invalid_argument );
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994-11-20T10:61:33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994-11-20T10:61:33Z" ), std::invalid_argument );

		/* Additional boundary tests */
		/* Leap year edge cases */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1900-02-29T10:25:33Z", result ) ); /* 1900 is not a leap year */
		EXPECT_THROW( datatypes::DateTime( "1900-02-29T10:25:33Z" ), std::invalid_argument );
		EXPECT_FALSE( datatypes::DateTime::tryParse( "2100-02-29T10:25:33Z", result ) ); /* 2100 is not a leap year */
		EXPECT_THROW( datatypes::DateTime( "2100-02-29T10:25:33Z" ), std::invalid_argument );

		/* Valid leap year should work */
		EXPECT_TRUE( datatypes::DateTime::tryParse( "2024-02-29T10:25:33Z", result ) ); /* 2024 is a leap year */
		EXPECT_NO_THROW( datatypes::DateTime( "2024-02-29T10:25:33Z" ) );

		/* April has only 30 days */
		EXPECT_FALSE( datatypes::DateTime::tryParse( "1994-04-31T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTime( "1994-04-31T10:25:33Z" ), std::invalid_argument );
	}

	//=====================================================================
	// DateTimeOffset Tests
	//=====================================================================

	class DateTimeOffsetTest : public ::testing::Test
	{
	protected:
		void SetUp() override {}
		void TearDown() override {}
	};

	TEST_F( DateTimeOffsetTest, Construction )
	{
		/* Default constructor */
		datatypes::DateTimeOffset dto1;
		EXPECT_EQ( constants::datetime::MIN_DATETIME_TICKS, dto1.ticks() );
		EXPECT_EQ( 0, dto1.totalOffsetMinutes() );

		/* Constructor from DateTime and offset */
		datatypes::DateTime dt( 2024, 6, 15, 14, 30, 45 );
		auto offset = datatypes::TimeSpan::fromHours( 2.0 );
		datatypes::DateTimeOffset dto2{ dt, offset };
		EXPECT_EQ( dt.ticks(), dto2.ticks() );
		EXPECT_EQ( 120, dto2.totalOffsetMinutes() );

		/* Constructor from ticks and offset */
		datatypes::DateTimeOffset dto3{ constants::datetime::UNIX_EPOCH_TICKS, datatypes::TimeSpan::fromHours( -5.0 ) };
		EXPECT_EQ( constants::datetime::UNIX_EPOCH_TICKS, dto3.ticks() );
		EXPECT_EQ( -300, dto3.totalOffsetMinutes() );

		/* Constructor from date components and offset */
		datatypes::DateTimeOffset dto4{ 2024, 1, 15, datatypes::TimeSpan::fromMinutes( 90 ) };
		EXPECT_EQ( 2024, dto4.year() );
		EXPECT_EQ( 1, dto4.month() );
		EXPECT_EQ( 15, dto4.day() );
		EXPECT_EQ( 90, dto4.totalOffsetMinutes() );

		/* Constructor with time components */
		datatypes::DateTimeOffset dto5{ 2024, 6, 15, 14, 30, 45, datatypes::TimeSpan::fromHours( -8.0 ) };
		EXPECT_EQ( 2024, dto5.year() );
		EXPECT_EQ( 6, dto5.month() );
		EXPECT_EQ( 15, dto5.day() );
		EXPECT_EQ( 14, dto5.hour() );
		EXPECT_EQ( 30, dto5.minute() );
		EXPECT_EQ( 45, dto5.second() );
		EXPECT_EQ( -480, dto5.totalOffsetMinutes() );

		/* Constructor with milliseconds */
		datatypes::DateTimeOffset dto6{ 2024, 12, 25, 23, 59, 59, 999, datatypes::TimeSpan::fromHours( 9.0 ) };
		EXPECT_EQ( 2024, dto6.year() );
		EXPECT_EQ( 999, dto6.millisecond() );
		EXPECT_EQ( 540, dto6.totalOffsetMinutes() );

		/* Constructor with microseconds */
		datatypes::DateTimeOffset dto7{ 2024, 1, 1, 0, 0, 0, 0, 500, datatypes::TimeSpan::fromMinutes( 30 ) };
		EXPECT_EQ( 500, dto7.microsecond() );
		EXPECT_EQ( 30, dto7.totalOffsetMinutes() );
	}

	TEST_F( DateTimeOffsetTest, ISO8601StringConstructor )
	{
		/* Basic ISO 8601 format with UTC */
		datatypes::DateTimeOffset dto1{ "2024-06-15T14:30:45Z" };
		EXPECT_EQ( 2024, dto1.year() );
		EXPECT_EQ( 6, dto1.month() );
		EXPECT_EQ( 15, dto1.day() );
		EXPECT_EQ( 14, dto1.hour() );
		EXPECT_EQ( 30, dto1.minute() );
		EXPECT_EQ( 45, dto1.second() );
		EXPECT_EQ( 0, dto1.totalOffsetMinutes() );

		/* ISO 8601 with positive offset */
		datatypes::DateTimeOffset dto2{ "2024-01-01T12:00:00+02:00" };
		EXPECT_EQ( 2024, dto2.year() );
		EXPECT_EQ( 12, dto2.hour() );
		EXPECT_EQ( 120, dto2.totalOffsetMinutes() );

		/* ISO 8601 with negative offset */
		datatypes::DateTimeOffset dto3{ "2024-07-04T16:00:00-05:00" };
		EXPECT_EQ( 2024, dto3.year() );
		EXPECT_EQ( 7, dto3.month() );
		EXPECT_EQ( 4, dto3.day() );
		EXPECT_EQ( 16, dto3.hour() );
		EXPECT_EQ( -300, dto3.totalOffsetMinutes() );

		/* Invalid format should throw */
		EXPECT_THROW( datatypes ::DateTimeOffset( "invalid-date-string" ), std::invalid_argument );
		EXPECT_THROW( datatypes ::DateTimeOffset( "2024-13-01T00:00:00Z" ), std::invalid_argument );
	}

	TEST_F( DateTimeOffsetTest, ComparisonOperators )
	{
		/* Same UTC time with different offsets should be equal */
		datatypes::DateTimeOffset dto1{ 2024, 1, 1, 14, 0, 0, datatypes::TimeSpan::fromHours( 2.0 ) };
		datatypes::DateTimeOffset dto2{ 2024, 1, 1, 12, 0, 0, datatypes::TimeSpan::fromHours( 0.0 ) };
		datatypes::DateTimeOffset dto3{ 2024, 1, 1, 7, 0, 0, datatypes::TimeSpan::fromHours( -5.0 ) };

		/* All represent 12:00 UTC */
		EXPECT_TRUE( dto1 == dto2 );
		EXPECT_TRUE( dto2 == dto3 );
		EXPECT_TRUE( dto1 == dto3 );

		/* Different UTC times */
		datatypes::DateTimeOffset dto4{ 2024, 1, 1, 13, 0, 0, datatypes::TimeSpan::fromHours( 0.0 ) };
		EXPECT_FALSE( dto1 == dto4 );
		EXPECT_TRUE( dto1 != dto4 );
		EXPECT_TRUE( dto1 < dto4 );
		EXPECT_TRUE( dto4 > dto1 );

		/* Ordering based on UTC time */
		EXPECT_TRUE( dto1 <= dto2 );
		EXPECT_TRUE( dto4 >= dto1 );
	}

	TEST_F( DateTimeOffsetTest, ArithmeticOperators )
	{
		datatypes::DateTimeOffset dto{ 2024, 6, 15, 12, 0, 0, datatypes::TimeSpan::fromHours( 3.0 ) };
		auto oneHour = datatypes::TimeSpan::fromHours( 1.0 );
		auto oneDay = datatypes::TimeSpan::fromDays( 1.0 );

		/* Addition preserves offset */
		datatypes::DateTimeOffset dtoPlusHour = dto + oneHour;
		EXPECT_EQ( 13, dtoPlusHour.hour() );
		EXPECT_EQ( 180, dtoPlusHour.totalOffsetMinutes() );

		datatypes::/* Subtraction preserves offset */
			DateTimeOffset dtoMinusHour = dto - oneHour;
		EXPECT_EQ( 11, dtoMinusHour.hour() );
		EXPECT_EQ( 180, dtoMinusHour.totalOffsetMinutes() );

		/* DateTimeOffset difference (based on UTC time) */
		datatypes::TimeSpan diff = dtoPlusHour - dto;
		EXPECT_DOUBLE_EQ( 1.0, diff.totalHours() );

		/* In-place operations */
		dto += oneDay;
		EXPECT_EQ( 16, dto.day() );
		EXPECT_EQ( 180, dto.totalOffsetMinutes() );

		dto -= oneDay;
		EXPECT_EQ( 15, dto.day() );
	}

	TEST_F( DateTimeOffsetTest, PropertyAccessors )
	{
		datatypes::DateTimeOffset dto{ 2024, 6, 15, 14, 30, 45, 123, 456, datatypes::TimeSpan::fromMinutes( 90 ) };

		/* Date and time components */
		EXPECT_EQ( 2024, dto.year() );
		EXPECT_EQ( 6, dto.month() );
		EXPECT_EQ( 15, dto.day() );
		EXPECT_EQ( 14, dto.hour() );
		EXPECT_EQ( 30, dto.minute() );
		EXPECT_EQ( 45, dto.second() );
		EXPECT_EQ( 123, dto.millisecond() );
		EXPECT_EQ( 456, dto.microsecond() );

		/* Offset properties */
		EXPECT_EQ( 90, dto.totalOffsetMinutes() );
		auto offset = dto.offset();
		EXPECT_DOUBLE_EQ( 1.5, offset.totalHours() );

		/* DateTime properties */
		auto localDt = dto.localDateTime();
		auto utcDt = dto.utcDateTime();
		EXPECT_EQ( dto.ticks(), localDt.ticks() );
		EXPECT_NE( localDt.ticks(), utcDt.ticks() );

		/* Day calculations */
		EXPECT_EQ( 6, dto.dayOfWeek() );
		int expectedDayOfYear = 31 + 29 + 31 + 30 + 31 + 15;
		EXPECT_EQ( expectedDayOfYear, dto.dayOfYear() );
	}

	TEST_F( DateTimeOffsetTest, ConversionMethods )
	{
		datatypes::DateTimeOffset dto{ 2024, 6, 15, 14, 30, 45, datatypes::TimeSpan::fromHours( 2.0 ) };

		/* Date extraction */
		datatypes::DateTimeOffset dateOnly = dto.date();
		EXPECT_EQ( 2024, dateOnly.year() );
		EXPECT_EQ( 6, dateOnly.month() );
		EXPECT_EQ( 15, dateOnly.day() );
		EXPECT_EQ( 0, dateOnly.hour() );
		EXPECT_EQ( 0, dateOnly.minute() );
		EXPECT_EQ( 0, dateOnly.second() );
		EXPECT_EQ( 120, dateOnly.totalOffsetMinutes() );

		/* Offset conversion */
		auto dtoUtc = dto.toUniversalTime();
		EXPECT_EQ( 0, dtoUtc.totalOffsetMinutes() );
		EXPECT_EQ( 12, dtoUtc.hour() );

		auto dtoNewOffset = dto.toOffset( datatypes::TimeSpan::fromHours( -5.0 ) );
		EXPECT_EQ( -300, dtoNewOffset.totalOffsetMinutes() );
		EXPECT_EQ( 7, dtoNewOffset.hour() );

		/* Unix timestamp conversions */
		auto epoch = datatypes::DateTimeOffset::unixEpoch();
		EXPECT_EQ( 0, epoch.toUnixSeconds() );
		EXPECT_EQ( 0, epoch.toUnixMilliseconds() );

		/* File time conversion */
		auto fileTime = dto.toFILETIME();
		auto fromFileTime = datatypes::DateTimeOffset::fromFileTime( fileTime );
		EXPECT_EQ( dto.utcDateTime().ticks(), fromFileTime.utcDateTime().ticks() );

		/* Time of day */
		datatypes::TimeSpan timeOfDay = dto.timeOfDay();
		EXPECT_DOUBLE_EQ( 14.5125, timeOfDay.totalHours() );
	}

	TEST_F( DateTimeOffsetTest, StringFormatting )
	{
		datatypes::DateTimeOffset dto{ 2024, 6, 15, 14, 30, 45, 123, datatypes::TimeSpan::fromHours( 2.0 ) };

		/* Basic ISO 8601 */
		auto basic = dto.toString();
		EXPECT_EQ( "2024-06-15T14:30:45+02:00", basic );

		/* Specific formats */
		EXPECT_EQ( "2024-06-15T14:30:45+02:00", dto.toString( datatypes::DateTime::Format::Iso8601Basic ) );
		EXPECT_EQ( "2024-06-15T14:30:45+02:00", dto.toString( datatypes::DateTime::Format::Iso8601WithOffset ) );
		EXPECT_EQ( "2024-06-15", dto.toString( datatypes::DateTime::Format::DateOnly ) );
		EXPECT_EQ( "14:30:45+02:00", dto.toString( datatypes::DateTime::Format::TimeOnly ) );

		/* Extended format with fractional seconds */
		auto extended = dto.toString( datatypes::DateTime::Format::Iso8601Extended );
		EXPECT_TRUE( extended.find( "2024-06-15T14:30:45." ) != std::string::npos );
		EXPECT_TRUE( extended.find( "+02:00" ) != std::string::npos );

		/* UTC offset formatting */
		datatypes::DateTimeOffset utc{ 2024, 1, 1, 12, 0, 0, datatypes::TimeSpan::fromHours( 0.0 ) };
		EXPECT_EQ( "2024-01-01T12:00:00Z", utc.toString() );

		/* Negative offset */
		datatypes::DateTimeOffset negative{ 2024, 1, 1, 8, 0, 0, datatypes::TimeSpan::fromHours( -5.0 ) };
		EXPECT_EQ( "2024-01-01T08:00:00-05:00", negative.toString() );

		/* Unix timestamps */
		auto epoch = datatypes::DateTimeOffset::unixEpoch();
		EXPECT_EQ( "0", epoch.toString( datatypes ::DateTime::Format::UnixSeconds ) );
		EXPECT_EQ( "0", epoch.toString( datatypes ::DateTime::Format::UnixMilliseconds ) );

		/* ISO 8601 extended method */
		auto iso8601Ext = dto.toIso8601Extended();
		EXPECT_EQ( extended, iso8601Ext );
	}

	TEST_F( DateTimeOffsetTest, ValidationMethods )
	{
		/* Valid DateTimeOffset */
		datatypes::DateTimeOffset validDto{ 2024, 6, 15, 12, 30, 45, datatypes::TimeSpan::fromHours( 3.0 ) };
		EXPECT_TRUE( validDto.isValid() );
	}

	TEST_F( DateTimeOffsetTest, StaticFactoryMethods )
	{
		/* Min and max values */
		auto minVal = datatypes::DateTimeOffset::minValue();
		auto maxVal = datatypes::DateTimeOffset::maxValue();
		EXPECT_EQ( constants::datetime::MIN_DATETIME_TICKS, minVal.ticks() );
		EXPECT_EQ( constants::datetime::MAX_DATETIME_TICKS, maxVal.ticks() );
		EXPECT_EQ( 0, minVal.totalOffsetMinutes() );
		EXPECT_EQ( 0, maxVal.totalOffsetMinutes() );

		/* Unix epoch */
		auto epoch = datatypes::DateTimeOffset::unixEpoch();
		EXPECT_EQ( constants::datetime::UNIX_EPOCH_TICKS, epoch.ticks() );
		EXPECT_EQ( 1970, epoch.year() );
		EXPECT_EQ( 1, epoch.month() );
		EXPECT_EQ( 1, epoch.day() );
		EXPECT_EQ( 0, epoch.totalOffsetMinutes() );

		/* Now, utcNow, and today (basic smoke tests) */
		auto now = datatypes::DateTimeOffset::now();
		auto utcNow = datatypes::DateTimeOffset::utcNow();
		auto today = datatypes::DateTimeOffset::today();

		EXPECT_TRUE( now.isValid() );
		EXPECT_TRUE( utcNow.isValid() );
		EXPECT_TRUE( today.isValid() );

		EXPECT_EQ( 0, utcNow.totalOffsetMinutes() );
		EXPECT_EQ( 0, today.hour() );
		EXPECT_EQ( 0, today.minute() );
		EXPECT_EQ( 0, today.second() );

		/* Factory from timestamps */
		auto fromSeconds = datatypes::DateTimeOffset::fromUnixTimeSeconds( 86400 );
		EXPECT_EQ( 1970, fromSeconds.year() );
		EXPECT_EQ( 1, fromSeconds.month() );
		EXPECT_EQ( 2, fromSeconds.day() );
		EXPECT_EQ( 0, fromSeconds.totalOffsetMinutes() );

		auto fromMs = datatypes::DateTimeOffset::fromUnixTimeMilliseconds( 86400000 );
		EXPECT_EQ( 1970, fromMs.year() );
		EXPECT_EQ( 1, fromMs.month() );
		EXPECT_EQ( 2, fromMs.day() );
		EXPECT_EQ( 0, fromMs.totalOffsetMinutes() );

		/* From file time */
		std::int64_t fileTime = 132679392000000000LL;
		auto fromFileTime = datatypes ::DateTimeOffset::fromFileTime( fileTime );
		EXPECT_TRUE( fromFileTime.isValid() );
		EXPECT_EQ( 0, fromFileTime.totalOffsetMinutes() );
	}

	TEST_F( DateTimeOffsetTest, ArithmeticMethods )
	{
		datatypes::DateTimeOffset dto{ 2024, 6, 15, 12, 0, 0, datatypes ::TimeSpan::fromHours( 3.0 ) };

		/* Add methods */
		auto plusDays = dto.addDays( 5.5 );
		EXPECT_EQ( 21, plusDays.day() );
		EXPECT_EQ( 0, plusDays.hour() );
		EXPECT_EQ( 180, plusDays.totalOffsetMinutes() );

		auto plusHours = dto.addHours( 25.0 );
		EXPECT_EQ( 16, plusHours.day() );
		EXPECT_EQ( 13, plusHours.hour() );

		auto plusMinutes = dto.addMinutes( 90.0 );
		EXPECT_EQ( 13, plusMinutes.hour() );
		EXPECT_EQ( 30, plusMinutes.minute() );

		auto plusSeconds = dto.addSeconds( 3665.0 );
		EXPECT_EQ( 13, plusSeconds.hour() );
		EXPECT_EQ( 1, plusSeconds.minute() );
		EXPECT_EQ( 5, plusSeconds.second() );

		auto plusMs = dto.addMilliseconds( 2500.0 );
		EXPECT_EQ( 2, plusMs.second() );
		EXPECT_EQ( 500, plusMs.millisecond() );

		/* Add months with calendar logic */
		datatypes::DateTimeOffset jan31{ 2024, 1, 31, 12, 0, 0, datatypes::TimeSpan::fromHours( 2.0 ) };
		datatypes::DateTimeOffset feb = jan31.addMonths( 1 );
		EXPECT_EQ( 2, feb.month() );
		EXPECT_EQ( 29, feb.day() );

		datatypes::DateTimeOffset plusYear = dto.addYears( 1 );
		EXPECT_EQ( 2025, plusYear.year() );
		EXPECT_EQ( 6, plusYear.month() );
		EXPECT_EQ( 15, plusYear.day() );

		/* Add ticks */
		datatypes::DateTimeOffset plusTicks = dto.addTicks( 10000000LL );
		EXPECT_EQ( 1, plusTicks.second() );
	}

	TEST_F( DateTimeOffsetTest, ComparisonMethods )
	{
		datatypes::DateTimeOffset dto1{ 2024, 1, 1, 14, 0, 0, datatypes::TimeSpan::fromHours( 2.0 ) };
		datatypes::DateTimeOffset dto2{ 2024, 1, 1, 12, 0, 0, datatypes::TimeSpan::fromHours( 0.0 ) };
		datatypes::DateTimeOffset dto3{ 2024, 1, 1, 13, 0, 0, datatypes::TimeSpan::fromHours( 0.0 ) };

		/* Compare to method (based on UTC time) */
		EXPECT_EQ( 0, dto1.compareTo( dto2 ) );
		EXPECT_LT( 0, dto3.compareTo( dto1 ) );
		EXPECT_GT( 0, dto1.compareTo( dto3 ) );

		/* Equals methods */
		EXPECT_TRUE( dto1.equals( dto2 ) );
		EXPECT_FALSE( dto1.equals( dto3 ) );

		/* Exact equals (checks both local time and offset) */
		EXPECT_FALSE( dto1.equalsExact( dto2 ) );
		datatypes::DateTimeOffset dto1Copy{ 2024, 1, 1, 14, 0, 0, datatypes::TimeSpan::fromHours( 2.0 ) };
		EXPECT_TRUE( dto1.equalsExact( dto1Copy ) );

		/* Static compare method */
		EXPECT_EQ( 0, datatypes::DateTimeOffset::compare( dto1, dto2 ) );
		EXPECT_LT( 0, datatypes::DateTimeOffset::compare( dto3, dto1 ) );
	}

	TEST_F( DateTimeOffsetTest, TryParseMethod )
	{
		datatypes::DateTimeOffset result;

		/* Valid parsing with UTC */
		EXPECT_TRUE( datatypes::DateTimeOffset::tryParse( "2024-06-15T14:30:45Z", result ) );
		EXPECT_EQ( 2024, result.year() );
		EXPECT_EQ( 6, result.month() );
		EXPECT_EQ( 15, result.day() );
		EXPECT_EQ( 14, result.hour() );
		EXPECT_EQ( 30, result.minute() );
		EXPECT_EQ( 45, result.second() );
		EXPECT_EQ( 0, result.totalOffsetMinutes() );

		/* Valid parsing with positive offset */
		EXPECT_TRUE( datatypes::DateTimeOffset::tryParse( "2024-01-01T12:00:00+02:30", result ) );
		EXPECT_EQ( 12, result.hour() );
		EXPECT_EQ( 150, result.totalOffsetMinutes() );

		/* Valid parsing with negative offset */
		EXPECT_TRUE( datatypes::DateTimeOffset::tryParse( "2024-07-04T16:00:00-05:00", result ) );
		EXPECT_EQ( 16, result.hour() );
		EXPECT_EQ( -300, result.totalOffsetMinutes() );

		/* Invalid parsing */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "invalid-date", result ) );
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "2024-13-01T00:00:00Z", result ) );
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "2024-01-01T25:00:00Z", result ) );
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "2024-01-01T12:00:00+15:00", result ) );
	}

	TEST_F( DateTimeOffsetTest, StreamOperators )
	{
		datatypes::DateTimeOffset dto{ 2024, 6, 15, 14, 30, 45, datatypes::TimeSpan::fromHours( 2.0 ) };

		/* Output stream operator */
		std::ostringstream oss;
		oss << dto;
		EXPECT_EQ( "2024-06-15T14:30:45+02:00", oss.str() );

		/* Input stream operator */
		std::istringstream iss( "2024-12-25T23:59:59-08:00" );
		datatypes::DateTimeOffset parsed;
		iss >> parsed;
		EXPECT_EQ( 2024, parsed.year() );
		EXPECT_EQ( 12, parsed.month() );
		EXPECT_EQ( 25, parsed.day() );
		EXPECT_EQ( 23, parsed.hour() );
		EXPECT_EQ( 59, parsed.minute() );
		EXPECT_EQ( 59, parsed.second() );
		EXPECT_EQ( -480, parsed.totalOffsetMinutes() );

		/* Invalid input should set fail bit */
		std::istringstream issInvalid( "invalid-date" );
		datatypes::DateTimeOffset invalidParsed;
		issInvalid >> invalidParsed;
		EXPECT_TRUE( issInvalid.fail() );
	}

	TEST_F( DateTimeOffsetTest, EdgeCases )
	{
		/* Maximum positive offset */
		auto maxOffset = datatypes::TimeSpan::fromHours( 14.0 );
		datatypes::DateTimeOffset maxOffsetDto{ 2024, 1, 1, 12, 0, 0, maxOffset };
		EXPECT_TRUE( maxOffsetDto.isValid() );
		EXPECT_EQ( 840, maxOffsetDto.totalOffsetMinutes() );

		/* Maximum negative offset */
		auto minOffset = datatypes::TimeSpan::fromHours( -14.0 );
		datatypes::DateTimeOffset minOffsetDto{ 2024, 1, 1, 12, 0, 0, minOffset };
		EXPECT_TRUE( minOffsetDto.isValid() );
		EXPECT_EQ( -840, minOffsetDto.totalOffsetMinutes() );

		/* Boundary datetime values */
		auto minDateTime = datatypes::DateTimeOffset::minValue();
		auto maxDateTime = datatypes::DateTimeOffset::maxValue();
		EXPECT_TRUE( minDateTime.isValid() );
		EXPECT_TRUE( maxDateTime.isValid() );

		/* Cross day boundary with offset */
		datatypes::DateTimeOffset endOfDay{ 2024, 1, 1, 23, 30, 0, datatypes::TimeSpan::fromHours( -6.0 ) };
		auto nextDayUtc = endOfDay.toUniversalTime();
		EXPECT_EQ( 2, nextDayUtc.day() );
		EXPECT_EQ( 5, nextDayUtc.hour() );

		/* Leap year with offset */
		datatypes::DateTimeOffset leapDay{ 2024, 2, 29, 12, 0, 0, datatypes::TimeSpan::fromHours( 1.0 ) };
		EXPECT_EQ( 29, leapDay.day() );
		EXPECT_TRUE( leapDay.isValid() );
	}

	//=====================================================================
	// DateTimeOffset Integration Tests
	//=====================================================================

	TEST_F( DateTimeOffsetTest, DateTimeIntegration )
	{
		/* Conversion between DateTime and DateTimeOffset */
		datatypes::DateTime dt{ 2024, 6, 15, 14, 30, 45 };
		datatypes::DateTimeOffset dto{ dt, datatypes::TimeSpan::fromHours( 3.0 ) };

		EXPECT_EQ( dt.ticks(), dto.ticks() );
		EXPECT_EQ( dt.year(), dto.year() );
		EXPECT_EQ( dt.hour(), dto.hour() );

		/* Extract DateTime from DateTimeOffset */
		auto localDt = dto.localDateTime();
		auto utcDt = dto.utcDateTime();

		EXPECT_EQ( dt.ticks(), localDt.ticks() );
		EXPECT_NE( dt.ticks(), utcDt.ticks() );
	}

	TEST_F( DateTimeOffsetTest, RoundTripSerialization )
	{
		datatypes::DateTimeOffset original{ 2024, 6, 15, 14, 30, 45, 123, datatypes::TimeSpan::fromMinutes( 150 ) };

		/* Round trip through ISO 8601 string */
		std::string serialized = original.toString( datatypes::DateTime::Format::Iso8601Extended );
		datatypes::DateTimeOffset deserialized;
		EXPECT_TRUE( datatypes::DateTimeOffset::tryParse( serialized, deserialized ) );

		/* Should be very close (within millisecond precision) */
		auto diff = original - deserialized;
		EXPECT_LT( std::abs( diff.totalMilliseconds() ), 1.0 );
		EXPECT_EQ( original.totalOffsetMinutes(), deserialized.totalOffsetMinutes() );
	}

	TEST_F( DateTimeOffsetTest, CrossPlatformConsistency )
	{
		/* Test that key values are consistent across platforms */
		auto epoch = datatypes::DateTimeOffset::unixEpoch();
		EXPECT_EQ( 1970, epoch.year() );
		EXPECT_EQ( 1, epoch.month() );
		EXPECT_EQ( 1, epoch.day() );
		EXPECT_EQ( 0, epoch.hour() );
		EXPECT_EQ( 0, epoch.minute() );
		EXPECT_EQ( 0, epoch.second() );
		EXPECT_EQ( 0, epoch.totalOffsetMinutes() );

		/* Known Unix timestamp with offset */
		auto knownDate = datatypes::DateTimeOffset::fromUnixTimeSeconds( 1609459200 );
		EXPECT_EQ( 2021, knownDate.year() );
		EXPECT_EQ( 1, knownDate.month() );
		EXPECT_EQ( 1, knownDate.day() );
		EXPECT_EQ( 0, knownDate.totalOffsetMinutes() );

		/* File time round trip */
		datatypes::DateTimeOffset testDto{ 2024, 6, 15, 14, 30, 45, datatypes::TimeSpan::fromHours( 2.0 ) };
		std::int64_t fileTime = testDto.toFILETIME();
		auto fromFileTime = datatypes::DateTimeOffset::fromFileTime( fileTime );

		/* Should match in UTC time */
		EXPECT_EQ( testDto.utcDateTime().ticks(), fromFileTime.utcDateTime().ticks() );
	}

	TEST_F( DateTimeOffsetTest, ISO8601FormatValidationEdgeCases )
	{
		datatypes::DateTimeOffset result;

		/* Invalid ISO 8601 format variations for DateTimeOffset - based on actual parser behavior */

		/* Wrong separators - should use dashes and colons, not slashes */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994/11/20T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994/11/20T10:25:33Z" ), std::invalid_argument );

		/* Invalid timezone offsets - beyond ±14:00 range (841+ minutes) */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-11-20T10:25:33+25:00", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-11-20T10:25:33+25:00" ), std::invalid_argument );
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-11-20T10:25:33-15:30", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-11-20T10:25:33-15:30" ), std::invalid_argument );
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-11-20T10:25:33+14:01", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-11-20T10:25:33+14:01" ), std::invalid_argument );

		/* Invalid timezone minute values - must be 0-59 */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-11-20T10:25:33+02:60", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-11-20T10:25:33+02:60" ), std::invalid_argument );
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-11-20T10:25:33-05:75", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-11-20T10:25:33-05:75" ), std::invalid_argument );

		/* Malformed timezone format with invalid characters */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-11-20T10:25:33+AB:CD", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-11-20T10:25:33+AB:CD" ), std::invalid_argument );

		/* 2-digit year - parser expects exactly 4 digits */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "94-11-20T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "94-11-20T10:25:33Z" ), std::invalid_argument );

		/* Missing date separators */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "19941120T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "19941120T10:25:33Z" ), std::invalid_argument );

		/* Wrong time separators */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-11-20T10.25.33Z", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-11-20T10.25.33Z" ), std::invalid_argument );

		/* Non-numeric characters in date/time components */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "ABCD-11-20T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "ABCD-11-20T10:25:33Z" ), std::invalid_argument );

		/* Empty or too short strings */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "" ), std::invalid_argument );

		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "123", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "123" ), std::invalid_argument );

		/* Completely malformed strings */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "not-a-date", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "not-a-date" ), std::invalid_argument );
	}

	TEST_F( DateTimeOffsetTest, DateTimeBoundaryViolations )
	{
		datatypes::DateTimeOffset result;

		/* Date/time boundary violations for DateTimeOffset */

		/* Invalid month - months must be 1-12 */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-13-20T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-13-20T10:25:33Z" ), std::invalid_argument );
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-00-20T10:25:33+02:00", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-00-20T10:25:33+02:00" ), std::invalid_argument );

		/* Invalid day - February 30th doesn't exist */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-02-30T10:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-02-30T10:25:33Z" ), std::invalid_argument );
		/* Day 32 doesn't exist in any month */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-01-32T10:25:33-05:00", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-01-32T10:25:33-05:00" ), std::invalid_argument );
		/* Day 0 is invalid */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-11-00T10:25:33+08:00", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-11-00T10:25:33+08:00" ), std::invalid_argument );

		/* Invalid hour - hours must be 0-23 */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-11-20T25:25:33Z", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-11-20T25:25:33Z" ), std::invalid_argument );
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-11-20T24:00:00+02:00", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-11-20T24:00:00+02:00" ), std::invalid_argument );

		/* Invalid minute/second - minutes and seconds must be 0-59 */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-11-20T10:60:33Z", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-11-20T10:60:33Z" ), std::invalid_argument );
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-11-20T10:25:60-03:00", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-11-20T10:25:60-03:00" ), std::invalid_argument );
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-11-20T10:61:33+07:00", result ) );
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-11-20T10:61:33+07:00" ), std::invalid_argument );

		/* Leap year edge cases with timezone offsets */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1900-02-29T10:25:33+01:00", result ) ); /* 1900 is not a leap year */
		EXPECT_THROW( datatypes::DateTimeOffset( "1900-02-29T10:25:33+01:00" ), std::invalid_argument );
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "2100-02-29T10:25:33-02:00", result ) ); /* 2100 is not a leap year */
		EXPECT_THROW( datatypes::DateTimeOffset( "2100-02-29T10:25:33-02:00" ), std::invalid_argument );

		/* Valid leap year should work with timezone */
		EXPECT_TRUE( datatypes::DateTimeOffset::tryParse( "2024-02-29T10:25:33+05:30", result ) ); /* 2024 is a leap year */
		EXPECT_NO_THROW( datatypes::DateTimeOffset( "2024-02-29T10:25:33+05:30" ) );

		/* Month-specific day validation with timezones */
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-04-31T10:25:33-08:00", result ) ); /* April has only 30 days */
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-04-31T10:25:33-08:00" ), std::invalid_argument );
		EXPECT_FALSE( datatypes::DateTimeOffset::tryParse( "1994-06-31T10:25:33+09:00", result ) ); /* June has only 30 days */
		EXPECT_THROW( datatypes::DateTimeOffset( "1994-06-31T10:25:33+09:00" ), std::invalid_argument );
	}

	//=====================================================================
	// std::chrono interoperability Tests
	//=====================================================================

	class DateTimeChronoTest : public ::testing::Test
	{
	protected:
		void SetUp() override {}
		void TearDown() override {}
	};

	TEST_F( DateTimeChronoTest, ConstructorFromTimePoint )
	{
		/* Test construction from std::chrono::system_clock::time_point */
		auto now = std::chrono::system_clock::now();
		datatypes::DateTime dt{ now };

		/* Should be valid and close to current time */
		EXPECT_TRUE( dt.isValid() );

		/* Convert back and compare */
		auto converted = dt.toChrono();
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>( converted - now ).count();

		/* Within 1 second due to precision differences */
		EXPECT_LT( std::abs( diff ), 1000 );
	}

	TEST_F( DateTimeChronoTest, ToChronoConversion )
	{
		/* Test known DateTime to chrono conversion */
		auto epoch = datatypes::DateTime::epoch();
		auto chronoEpoch = epoch.toChrono();

		/* Should match Unix epoch */
		auto duration = chronoEpoch.time_since_epoch();
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>( duration ).count();
		EXPECT_EQ( 0, seconds );

		/* Test specific date */
		datatypes::DateTime specificDate{ 2024, 6, 15, 14, 30, 45, 123 };
		auto chronoPoint = specificDate.toChrono();

		/* Convert back to verify round-trip */
		datatypes::DateTime roundTrip{ chronoPoint };
		EXPECT_EQ( specificDate.year(), roundTrip.year() );
		EXPECT_EQ( specificDate.month(), roundTrip.month() );
		EXPECT_EQ( specificDate.day(), roundTrip.day() );
		EXPECT_EQ( specificDate.hour(), roundTrip.hour() );
		EXPECT_EQ( specificDate.minute(), roundTrip.minute() );
		EXPECT_EQ( specificDate.second(), roundTrip.second() );

		/* Millisecond precision might have small differences */
		EXPECT_LT( std::abs( specificDate.millisecond() - roundTrip.millisecond() ), 2 );
	}

	TEST_F( DateTimeChronoTest, FromChronoStaticMethod )
	{
		/* Test static fromChrono method */
		auto now = std::chrono::system_clock::now();
		datatypes::DateTime dt = datatypes::DateTime::fromChrono( now );

		EXPECT_TRUE( dt.isValid() );

		datatypes::/* Should be equivalent to constructor */
			DateTime dtConstructor{ now };
		EXPECT_EQ( dt.ticks(), dtConstructor.ticks() );
	}

	TEST_F( DateTimeChronoTest, RoundTripConversions )
	{
		/* Test round-trip: DateTime -> chrono -> DateTime */
		datatypes::DateTime original{ 2024, 1, 1, 12, 0, 0, 0 };
		auto chronoPoint = original.toChrono();
		auto roundTrip = datatypes::DateTime::fromChrono( chronoPoint );

		EXPECT_EQ( original.ticks(), roundTrip.ticks() );
		EXPECT_EQ( original.year(), roundTrip.year() );
		EXPECT_EQ( original.month(), roundTrip.month() );
		EXPECT_EQ( original.day(), roundTrip.day() );
		EXPECT_EQ( original.hour(), roundTrip.hour() );
		EXPECT_EQ( original.minute(), roundTrip.minute() );
		EXPECT_EQ( original.second(), roundTrip.second() );

		/* Test with fractional seconds */
		datatypes::DateTime withMs{ 2024, 6, 15, 14, 30, 45, 999 };
		auto chronoMs = withMs.toChrono();
		auto roundTripMs = datatypes::DateTime::fromChrono( chronoMs );

		/* Should preserve most precision */
		datatypes::TimeSpan diff = withMs - roundTripMs;
		EXPECT_LT( std::abs( diff.totalMilliseconds() ), 1.0 );
	}

	TEST_F( DateTimeChronoTest, PrecisionHandling )
	{
		/* Test precision boundaries */
		datatypes::DateTime dt{ 2024, 1, 1, 12, 0, 0, 0 };

		/* Add specific number of ticks (100-nanosecond precision) */
		/* 123.4567 milliseconds */
		std::int64_t extraTicks = 1234567;
		datatypes::DateTime dtWithTicks{ dt.ticks() + extraTicks };

		auto chronoPoint = dtWithTicks.toChrono();
		datatypes::DateTime converted{ chronoPoint };

		/* Should preserve tick-level precision */
		EXPECT_EQ( dtWithTicks.ticks(), converted.ticks() );
	}

	TEST_F( DateTimeChronoTest, BoundaryValues )
	{
		/* Test minimum DateTime value */
		auto minDt = datatypes::DateTime::minValue();
		auto minChrono = minDt.toChrono();
		datatypes::DateTime minRoundTrip{ minChrono };
		EXPECT_EQ( minDt.ticks(), minRoundTrip.ticks() );

		/* Test maximum DateTime value */
		datatypes::DateTime maxDt = datatypes::DateTime::maxValue();
		auto maxChrono = maxDt.toChrono();
		datatypes::DateTime maxRoundTrip{ maxChrono };
		EXPECT_EQ( maxDt.ticks(), maxRoundTrip.ticks() );

		/* Test Unix epoch specifically */
		auto epoch = datatypes::DateTime::epoch();
		auto epochChrono = epoch.toChrono();
		auto epochDuration = epochChrono.time_since_epoch();
		auto epochSeconds = std::chrono::duration_cast<std::chrono::seconds>( epochDuration ).count();
		EXPECT_EQ( 0, epochSeconds );
	}

	TEST_F( DateTimeChronoTest, ChronoArithmetic )
	{
		/* Test compatibility with chrono arithmetic */
		datatypes::DateTime dt{ 2024, 6, 15, 12, 0, 0 };
		auto chronoPoint = dt.toChrono();

		/* Add time using chrono */
		auto chronoPlus1Hour = chronoPoint + std::chrono::hours( 1 );
		datatypes::DateTime dtPlus1Hour{ chronoPlus1Hour };

		EXPECT_EQ( 13, dtPlus1Hour.hour() );
		EXPECT_EQ( dt.year(), dtPlus1Hour.year() );
		EXPECT_EQ( dt.month(), dtPlus1Hour.month() );
		EXPECT_EQ( dt.day(), dtPlus1Hour.day() );

		/* Add fractional time */
		auto chronoPlus30Min = chronoPoint + std::chrono::minutes( 30 );
		datatypes::DateTime dtPlus30Min{ chronoPlus30Min };

		EXPECT_EQ( 12, dtPlus30Min.hour() );
		EXPECT_EQ( 30, dtPlus30Min.minute() );
	}

	TEST_F( DateTimeChronoTest, ChronoCompatibilityWithStandardLibrary )
	{
		/* Test that our DateTime works with standard chrono utilities */
		datatypes::DateTime dt1{ 2024, 1, 1, 12, 0, 0 };
		datatypes::DateTime dt2{ 2024, 1, 1, 13, 0, 0 };

		auto chrono1 = dt1.toChrono();
		auto chrono2 = dt2.toChrono();

		/* Test chrono comparison */
		EXPECT_TRUE( chrono1 < chrono2 );
		EXPECT_FALSE( chrono1 > chrono2 );
		EXPECT_TRUE( chrono1 != chrono2 );

		/* Test chrono duration calculation */
		auto chronoDiff = chrono2 - chrono1;
		auto hoursDiff = std::chrono::duration_cast<std::chrono::hours>( chronoDiff ).count();
		EXPECT_EQ( 1, hoursDiff );

		/* Test with standard time formatting (C++20 feature when available) */
		auto timeT = std::chrono::system_clock::to_time_t( chrono1 );

		/* Should be a valid time_t */
		EXPECT_GT( timeT, 0 );
	}

	TEST_F( DateTimeChronoTest, TimeZoneIndependence )
	{
		/* DateTime is UTC-only, chrono conversions should maintain UTC semantics */
		datatypes::DateTime utcTime{ 2024, 6, 15, 14, 30, 45 };
		auto chronoPoint = utcTime.toChrono();

		/* Convert to time_t and back to verify UTC handling */
		auto timeT = std::chrono::system_clock::to_time_t( chronoPoint );
		auto chronoFromTimeT = std::chrono::system_clock::from_time_t( timeT );
		datatypes::DateTime convertedBack{ chronoFromTimeT };

		/* Should match original (within second precision due to time_t) */
		EXPECT_EQ( utcTime.year(), convertedBack.year() );
		EXPECT_EQ( utcTime.month(), convertedBack.month() );
		EXPECT_EQ( utcTime.day(), convertedBack.day() );
		EXPECT_EQ( utcTime.hour(), convertedBack.hour() );
		EXPECT_EQ( utcTime.minute(), convertedBack.minute() );
		EXPECT_EQ( utcTime.second(), convertedBack.second() );
	}
}
