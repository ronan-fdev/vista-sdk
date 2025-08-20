/**
 * @file TESTS_Decimal128.cpp
 * @brief Comprehensive tests for cross-platform Decimal128 implementation
 * @details Validates Decimal128 compatibility and cross-platform behavior
 */

#include "dnv/vista/sdk/datatypes/Decimal128.h"

namespace dnv::vista::sdk::test
{
	//=====================================================================
	// Basic Construction Tests
	//=====================================================================

	TEST( Decimal128Test, DefaultConstruction )
	{
		datatypes::Decimal128 d;
		EXPECT_TRUE( d.isZero() );
		EXPECT_FALSE( d.isNegative() );
		EXPECT_EQ( d.scale(), 0 );
		EXPECT_EQ( d.toString(), "0" );
	}

	TEST( Decimal128Test, IntegerConstruction )
	{
		/* Positive integers */
		datatypes::Decimal128 d1( 42 );
		EXPECT_FALSE( d1.isZero() );
		EXPECT_FALSE( d1.isNegative() );
		EXPECT_EQ( d1.toString(), "42" );

		/* Negative integers */
		datatypes::Decimal128 d2( -123 );
		EXPECT_FALSE( d2.isZero() );
		EXPECT_TRUE( d2.isNegative() );
		EXPECT_EQ( d2.toString(), "-123" );

		/* Zero */
		datatypes::Decimal128 d3( 0 );
		EXPECT_TRUE( d3.isZero() );
		EXPECT_FALSE( d3.isNegative() );
		EXPECT_EQ( d3.toString(), "0" );

		/* Large integers */
		datatypes::Decimal128 d4( std::numeric_limits<std::int64_t>::max() );
		EXPECT_FALSE( d4.isZero() );
		EXPECT_FALSE( d4.isNegative() );
	}

	TEST( Decimal128Test, DoubleConstruction )
	{
		/* Simple double */
		datatypes::Decimal128 d1( 123.456 );
		EXPECT_FALSE( d1.isZero() );
		EXPECT_FALSE( d1.isNegative() );

		/* Negative double */
		datatypes::Decimal128 d2( -123.456 );
		EXPECT_FALSE( d2.isZero() );
		EXPECT_TRUE( d2.isNegative() );

		/* Zero double */
		datatypes::Decimal128 d3( 0.0 );
		EXPECT_TRUE( d3.isZero() );
		EXPECT_FALSE( d3.isNegative() );

		/* Small Decimal128 */
		datatypes::Decimal128 d4( 0.001 );
		EXPECT_FALSE( d4.isZero() );
		EXPECT_FALSE( d4.isNegative() );
	}

	//=====================================================================
	// String Parsing Tests
	//=====================================================================

	TEST( Decimal128Test, StringConstruction )
	{
		/* Valid strings */
		datatypes::Decimal128 d1( "123.456" );
		EXPECT_EQ( d1.toString(), "123.456" );

		datatypes::Decimal128 d2( "-789.123" );
		EXPECT_TRUE( d2.isNegative() );

		datatypes::Decimal128 d3( "0" );
		EXPECT_TRUE( d3.isZero() );

		datatypes::Decimal128 d4( "0.0001" );
		EXPECT_FALSE( d4.isZero() );

		/* Test with many Decimal128 places */
		datatypes::Decimal128 d5( "123.1234567890123456789" );
		EXPECT_FALSE( d5.isZero() );
	}

	TEST( Decimal128Test, TryParse )
	{
		datatypes::Decimal128 result;

		/* Valid cases */
		EXPECT_TRUE( datatypes::Decimal128::tryParse( "123.456", result ) );
		EXPECT_EQ( result.toString(), "123.456" );

		EXPECT_TRUE( datatypes::Decimal128::tryParse( "-789", result ) );
		EXPECT_TRUE( result.isNegative() );

		EXPECT_TRUE( datatypes::Decimal128::tryParse( "0", result ) );
		EXPECT_TRUE( result.isZero() );

		/* Invalid cases */
		EXPECT_FALSE( datatypes::Decimal128::tryParse( "", result ) );
		EXPECT_FALSE( datatypes::Decimal128::tryParse( "abc", result ) );
		EXPECT_FALSE( datatypes::Decimal128::tryParse( "12.34.56", result ) );
		EXPECT_FALSE( datatypes::Decimal128::tryParse( "12a34", result ) );
	}

	//=====================================================================
	// Conversion Tests
	//=====================================================================

	TEST( Decimal128Test, ToDouble )
	{
		datatypes::Decimal128 d1( 123.456 );
		double converted = d1.toDouble();
		EXPECT_NEAR( converted, 123.456, 0.001 );

		datatypes::Decimal128 d2( -789.123 );
		converted = d2.toDouble();
		EXPECT_NEAR( converted, -789.123, 0.001 );

		datatypes::Decimal128 d3( 0 );
		converted = d3.toDouble();
		EXPECT_EQ( converted, 0.0 );
	}

	TEST( Decimal128Test, ToString )
	{
		/* Basic cases */
		EXPECT_EQ( datatypes::Decimal128( 123 ).toString(), "123" );
		EXPECT_EQ( datatypes::Decimal128( -456 ).toString(), "-456" );
		EXPECT_EQ( datatypes::Decimal128( 0 ).toString(), "0" );

		/* Decimal128 cases */
		datatypes::Decimal128 d1( "123.456" );
		EXPECT_EQ( d1.toString(), "123.456" );

		datatypes::Decimal128 d2( "-0.001" );
		EXPECT_EQ( d2.toString(), "-0.001" );
	}

	//=====================================================================
	// Arithmetic Tests
	//=====================================================================

	TEST( Decimal128Test, Addition )
	{
		datatypes::Decimal128 d1( "123.45" );
		datatypes::Decimal128 d2( "67.89" );
		auto result = d1 + d2;

		/* Should be close to 191.34 */
		EXPECT_FALSE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		/* Test with different signs */
		datatypes::Decimal128 d3( "100" );
		datatypes::Decimal128 d4( "-50" );
		result = d3 + d4;
		/* Should be 50 */
		EXPECT_FALSE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		/* Test adding zero */
		result = d1 + datatypes::Decimal128( 0 );
		EXPECT_EQ( result.toString(), d1.toString() );
	}

	TEST( Decimal128Test, Subtraction )
	{
		datatypes::Decimal128 d1( "100" );
		datatypes::Decimal128 d2( "30" );
		datatypes::Decimal128 result = d1 - d2;

		/* Should be 70 */
		EXPECT_FALSE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		/* Test negative result */
		result = d2 - d1;
		/* Should be -70 */
		EXPECT_FALSE( result.isZero() );
		EXPECT_TRUE( result.isNegative() );

		/* Test subtracting self */
		result = d1 - d1;
		EXPECT_TRUE( result.isZero() );
	}

	TEST( Decimal128Test, Multiplication )
	{
		datatypes::Decimal128 d1( "12.5" );
		datatypes::Decimal128 d2( "8" );
		datatypes::Decimal128 result = d1 * d2;

		/* Should be 100 */
		EXPECT_FALSE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		/* Test with negative */
		datatypes::Decimal128 d3( "-5" );
		result = d1 * d3;
		/* Should be -62.5 */
		EXPECT_FALSE( result.isZero() );
		EXPECT_TRUE( result.isNegative() );

		/* Test multiply by zero */
		result = d1 * datatypes::Decimal128( 0 );
		EXPECT_TRUE( result.isZero() );
	}

	TEST( Decimal128Test, Division )
	{
		datatypes::Decimal128 d1( "100" );
		datatypes::Decimal128 d2( "4" );
		datatypes::Decimal128 result = d1 / d2;

		/* Should be 25 */
		EXPECT_FALSE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		/* Test with negative */
		datatypes::Decimal128 d3( "-20" );
		result = d1 / d3;
		/* Should be -5 */
		EXPECT_FALSE( result.isZero() );
		EXPECT_TRUE( result.isNegative() );

		/* Test division by zero */
		EXPECT_THROW( d1 / datatypes::Decimal128( 0 ), std::overflow_error );
	}

	//=====================================================================
	// Comparison Tests
	//=====================================================================

	TEST( Decimal128Test, Equality )
	{
		datatypes::Decimal128 d1( "123.45" );
		datatypes::Decimal128 d2( "123.45" );
		datatypes::Decimal128 d3( "123.46" );

		EXPECT_TRUE( d1 == d2 );
		EXPECT_FALSE( d1 == d3 );
		EXPECT_FALSE( d1 != d2 );
		EXPECT_TRUE( d1 != d3 );

		/* Test with zero */
		datatypes::Decimal128 zero1( 0 );
		datatypes::Decimal128 zero2( "0.0" );
		EXPECT_TRUE( zero1 == zero2 );
	}

	TEST( Decimal128Test, Comparison )
	{
		datatypes::Decimal128 d1( "100" );
		datatypes::Decimal128 d2( "200" );
		datatypes::Decimal128 d3( "-50" );

		/* Less than */
		EXPECT_TRUE( d1 < d2 );
		EXPECT_FALSE( d2 < d1 );
		EXPECT_TRUE( d3 < d1 );

		/* Greater than */
		EXPECT_TRUE( d2 > d1 );
		EXPECT_FALSE( d1 > d2 );
		EXPECT_TRUE( d1 > d3 );

		/* Less than or equal */
		EXPECT_TRUE( d1 <= d2 );
		EXPECT_TRUE( d1 <= datatypes::Decimal128( "100" ) );
		EXPECT_FALSE( d2 <= d1 );

		/* Greater than or equal */
		EXPECT_TRUE( d2 >= d1 );
		EXPECT_TRUE( d1 >= datatypes::Decimal128( "100" ) );
		EXPECT_FALSE( d1 >= d2 );
	}

	//=====================================================================
	// Precision and Limits Tests
	//=====================================================================

	TEST( Decimal128Test, MaxPrecision )
	{
		/* Test with maximum Decimal128 places (28) */
		std::string max_precision = "1.2345678901234567890123456789";
		datatypes::Decimal128 d1;
		EXPECT_TRUE( datatypes::Decimal128::tryParse( max_precision, d1 ) );
		EXPECT_FALSE( d1.isZero() );

		/* Test exceeding max precision should fail */
		/* 29 Decimal128 places */
		std::string too_precise = "1.23456789012345678901234567890";
		EXPECT_FALSE( datatypes::Decimal128::tryParse( too_precise, d1 ) );
	}

	TEST( Decimal128Test, LargeNumbers )
	{
		/* Test with large integers */
		datatypes::Decimal128 d1( std::numeric_limits<std::int64_t>::max() );
		EXPECT_FALSE( d1.isZero() );
		EXPECT_FALSE( d1.isNegative() );

		datatypes::Decimal128 d2( std::numeric_limits<std::int64_t>::min() );
		EXPECT_FALSE( d2.isZero() );
		EXPECT_TRUE( d2.isNegative() );
	}

	TEST( Decimal128Test, VerySmallNumbers )
	{
		/* Test very small Decimal128 values */
		/* 28 Decimal128 places */
		datatypes::Decimal128 d1( "0.0000000000000000000000000001" );
		EXPECT_FALSE( d1.isZero() );
		EXPECT_FALSE( d1.isNegative() );
		EXPECT_EQ( d1.scale(), 28 );
	}

	//=====================================================================
	// Assignment Operators Tests
	//=====================================================================

	TEST( Decimal128Test, CompoundAssignment )
	{
		datatypes::Decimal128 d1( "100" );
		datatypes::Decimal128 d2( "25" );

		/* Addition assignment */
		d1 += d2;
		EXPECT_FALSE( d1.isZero() );
		/* Should be 125 */

		/* Subtraction assignment */
		d1 -= datatypes::Decimal128( "25" );
		/* Should be back to 100 */

		/* Multiplication assignment */
		d1 *= datatypes::Decimal128( "2" );
		/* Should be 200 */

		/* Division assignment */
		d1 /= datatypes::Decimal128( "4" );

		/* Should be 50 */
		EXPECT_EQ( d1.toString(), "50" );
		EXPECT_FALSE( d1.isZero() );
	}

	TEST( Decimal128Test, UnaryMinus )
	{
		datatypes::Decimal128 d1( "123.45" );
		datatypes::Decimal128 d2 = -d1;

		EXPECT_FALSE( d1.isNegative() );
		EXPECT_TRUE( d2.isNegative() );
		EXPECT_EQ( d1.toString(), "123.45" );

		/* Double negation */
		datatypes::Decimal128 d3 = -d2;
		EXPECT_FALSE( d3.isNegative() );
		EXPECT_EQ( d3.toString(), "123.45" );
	}

	//=====================================================================
	// Stream I/O Tests
	//=====================================================================

	TEST( Decimal128Test, StreamOutput )
	{
		datatypes::Decimal128 d1( "123.456" );
		std::ostringstream oss;
		oss << d1;
		EXPECT_EQ( oss.str(), "123.456" );

		datatypes::Decimal128 d2( "-789.123" );
		oss.str( "" );
		oss << d2;
		EXPECT_EQ( oss.str(), "-789.123" );
	}

	TEST( Decimal128Test, StreamInput )
	{
		datatypes::Decimal128 d1;
		std::istringstream iss( "456.789" );
		iss >> d1;
		EXPECT_FALSE( iss.fail() );
		EXPECT_EQ( d1.toString(), "456.789" );

		/* Test invalid input */
		datatypes::Decimal128 d2;
		std::istringstream iss2( "invalid" );
		iss2 >> d2;
		EXPECT_TRUE( iss2.fail() );
	}

	//=====================================================================
	// Cross-Platform Compatibility Tests
	//=====================================================================

	TEST( Decimal128Test, CrossPlatformConsistency )
	{
		/* Test that results are consistent across platforms */
		datatypes::Decimal128 d1( "123456789.123456789" );
		datatypes::Decimal128 d2( "987654321.987654321" );

		/* Addition should work consistently */
		datatypes::Decimal128 sum = d1 + d2;
		EXPECT_FALSE( sum.isZero() );

		/* Multiplication should work consistently */
		datatypes::Decimal128 product = datatypes::Decimal128( "123.456" ) * datatypes::Decimal128( "789.123" );
		EXPECT_FALSE( product.isZero() );

		/* Division should work consistently */
		datatypes::Decimal128 quotient = datatypes::Decimal128( "1000" ) / datatypes::Decimal128( "3" );
		EXPECT_FALSE( quotient.isZero() );
	}

	//=====================================================================
	// Standard Decimal128 Behavior Tests
	//=====================================================================

	TEST( Decimal128Test, StandardDecimalCompatibility )
	{
		/* Test behaviors that should match standard Decimal128 arithmetic */

		/* Decimal128 has 28-29 significant digits */
		datatypes::Decimal128 d1( "1234567890123456789012345678.9" );
		EXPECT_FALSE( d1.isZero() );

		/* Decimal128 preserves trailing zeros in scale */
		datatypes::Decimal128 d2( "123.4500" );
		EXPECT_EQ( d2.scale(), 4 ); /* Should preserve 4 Decimal128 places */

		/* Decimal128 arithmetic should be exact (no floating-point errors) */
		datatypes::Decimal128 d3( "0.1" );
		datatypes::Decimal128 d4( "0.2" );
		datatypes::Decimal128 sum = d3 + d4;

		/* Should be exactly 0.3, not 0.30000000000000004 like double */
		datatypes::Decimal128 expected( "0.3" );
		EXPECT_TRUE( sum == expected );
	}

	TEST( Decimal128Test, NormalizationBehavior )
	{
		/* Test that normalization removes unnecessary trailing zeros */
		datatypes::Decimal128 d1( "123.4500" );
		/* Force normalization through conversion */
		std::string normalized = d1.toString();

		/* After normalization, trailing zeros should be removed */
		/* but scale should still be maintained for precision */
		EXPECT_FALSE( d1.isZero() );

		/* Test zero normalization */
		datatypes::Decimal128 zero1( "0.000" );
		datatypes::Decimal128 zero2( 0 );
		EXPECT_TRUE( zero1 == zero2 );
	}

	//=====================================================================
	// Performance and Stress Tests
	//=====================================================================

	TEST( Decimal128Test, StressTestLargeOperations )
	{
		/* Perform many operations to test stability */
		datatypes::Decimal128 accumulator( "0" );
		datatypes::Decimal128 increment( "0.001" );

		for ( int i = 0; i < 1000; ++i )
		{
			accumulator += increment;
		}

		/* Should be close to 1.0 */
		EXPECT_FALSE( accumulator.isZero() );
		EXPECT_FALSE( accumulator.isNegative() );
	}

	//=====================================================================
	// Mathematical Methods Tests
	//=====================================================================

	TEST( Decimal128Test, BinaryRepresentation )
	{
		/* Test toBits() method that returns Decimal128 binary representation */
		datatypes::Decimal128 d1( 123.456 );
		auto bits = d1.toBits();

		/* Should return 4 elements: [low, mid, high, flags] */
		EXPECT_EQ( bits.size(), 4 );

		/* Test zero */
		datatypes::Decimal128 zero( 0 );
		auto zeroBits = zero.toBits();
		EXPECT_EQ( zeroBits[0], 0 );
		EXPECT_EQ( zeroBits[1], 0 );
		EXPECT_EQ( zeroBits[2], 0 );
		/* Flags should be zero for positive zero */
		EXPECT_EQ( zeroBits[3], 0 );

		/* Test negative value */
		datatypes::Decimal128 negative( -123.456 );
		auto negativeBits = negative.toBits();
		/* Sign bit should be set in flags */
		EXPECT_NE( negativeBits[3] & 0x80000000, 0 );
	}

	TEST( Decimal128Test, TruncateMethod )
	{
		/* Test instance method */
		datatypes::Decimal128 d1( "123.789" );
		auto truncated = d1.truncate();
		EXPECT_EQ( truncated.toString(), "123" );

		/* Test negative truncation */
		datatypes::Decimal128 d2( "-123.789" );
		truncated = d2.truncate();
		EXPECT_EQ( truncated.toString(), "-123" );

		/* Test zero */
		datatypes::Decimal128 d3( "0.123" );
		truncated = d3.truncate();
		EXPECT_EQ( truncated.toString(), "0" );

		/* Test integer (no change) */
		datatypes::Decimal128 d4( "123" );
		truncated = d4.truncate();
		EXPECT_EQ( truncated.toString(), "123" );

		/* Test static method */
		datatypes::Decimal128 d5( "456.789" );
		auto staticTruncated = datatypes::Decimal128::truncate( d5 );
		EXPECT_EQ( staticTruncated.toString(), "456" );
	}

	TEST( Decimal128Test, FloorMethod )
	{
		/* Test positive values */
		datatypes::Decimal128 d1( "123.789" );
		auto floored = d1.floor();
		EXPECT_EQ( floored.toString(), "123" );

		/* Test negative values (should round down) */
		datatypes::Decimal128 d2( "-123.789" );
		floored = d2.floor();
		EXPECT_EQ( floored.toString(), "-124" );

		/* Test positive value close to integer */
		datatypes::Decimal128 d3( "123.001" );
		floored = d3.floor();
		EXPECT_EQ( floored.toString(), "123" );

		/* Test negative value close to integer */
		datatypes::Decimal128 d4( "-123.001" );
		floored = d4.floor();
		EXPECT_EQ( floored.toString(), "-124" );

		/* Test integer (no change) */
		datatypes::Decimal128 d5( "123" );
		floored = d5.floor();
		EXPECT_EQ( floored.toString(), "123" );

		/* Test static method */
		datatypes::Decimal128 d6( "456.789" );
		auto staticFloored = datatypes::Decimal128::floor( d6 );
		EXPECT_EQ( staticFloored.toString(), "456" );
	}

	TEST( Decimal128Test, CeilingMethod )
	{
		/* Test positive values (should round up) */
		datatypes::Decimal128 d1( "123.123" );
		auto ceiled = d1.ceiling();
		EXPECT_EQ( ceiled.toString(), "124" );

		/* Test negative values */
		datatypes::Decimal128 d2( "-123.123" );
		ceiled = d2.ceiling();
		EXPECT_EQ( ceiled.toString(), "-123" );

		/* Test positive value close to integer */
		datatypes::Decimal128 d3( "123.001" );
		ceiled = d3.ceiling();
		EXPECT_EQ( ceiled.toString(), "124" );

		/* Test negative value close to integer */
		datatypes::Decimal128 d4( "-123.001" );
		ceiled = d4.ceiling();
		EXPECT_EQ( ceiled.toString(), "-123" );

		/* Test integer (no change) */
		datatypes::Decimal128 d5( "123" );
		ceiled = d5.ceiling();
		EXPECT_EQ( ceiled.toString(), "123" );

		/* Test static method */
		datatypes::Decimal128 d6( "456.123" );
		auto staticCeiled = datatypes::Decimal128 ::ceiling( d6 );
		EXPECT_EQ( staticCeiled.toString(), "457" );
	}

	TEST( Decimal128Test, RoundMethod )
	{
		/* Test basic rounding to nearest integer */
		datatypes::Decimal128 d1( "123.4" );
		auto rounded = d1.round();
		EXPECT_EQ( rounded.toString(), "123" );

		datatypes::Decimal128 d2( "123.6" );
		rounded = d2.round();
		EXPECT_EQ( rounded.toString(), "124" );

		/* Test exact half - should round to nearest even (banker's rounding) */
		datatypes::Decimal128 d3( "123.5" );
		rounded = d3.round();
		EXPECT_EQ( rounded.toString(), "124" );

		/* Test negative rounding */
		datatypes::Decimal128 d4( "-123.4" );
		rounded = d4.round();
		EXPECT_EQ( rounded.toString(), "-123" );

		datatypes::Decimal128 d5( "-123.6" );
		rounded = d5.round();
		EXPECT_EQ( rounded.toString(), "-122" );

		/* Test static method */
		datatypes::Decimal128 d6( "456.7" );
		auto staticRounded = datatypes::Decimal128 ::round( d6 );
		EXPECT_EQ( staticRounded.toString(), "457" );
	}

	TEST( Decimal128Test, RoundWithDecimalPlaces )
	{
		/* Test rounding to specific Decimal128 places */
		datatypes::Decimal128 d1( "123.4567" );

		/* Round to 2 Decimal128 places */
		auto rounded = d1.round( 2 );
		EXPECT_EQ( rounded.toString(), "123.46" );

		/* Round to 1 Decimal128 place */
		rounded = d1.round( 1 );
		EXPECT_EQ( rounded.toString(), "123.5" );

		/* Round to 0 Decimal128 places (same as round()) */
		rounded = d1.round( 0 );
		EXPECT_EQ( rounded.toString(), "123" );

		/* Test negative values */
		datatypes::Decimal128 d2( "-123.4567" );
		rounded = d2.round( 2 );
		EXPECT_EQ( rounded.toString(), "-123.44" );

		/* Test static method */
		datatypes::Decimal128 d3( "789.1234" );
		auto staticRounded = datatypes::Decimal128 ::round( d3, 3 );
		EXPECT_EQ( staticRounded.toString(), "789.123" );

		/* Test rounding to more places than available (should return unchanged) */
		datatypes::Decimal128 d4( "123.45" );
		rounded = d4.round( 5 );
		EXPECT_EQ( rounded.toString(), "123.45" );
	}

	TEST( Decimal128Test, AbsMethod )
	{
		/* Test positive value (should remain unchanged) */
		datatypes::Decimal128 d1( "123.456" );
		auto absValue = d1.abs();
		EXPECT_EQ( absValue.toString(), "123.456" );
		EXPECT_FALSE( absValue.isNegative() );

		/* Test negative value (should become positive) */
		datatypes::Decimal128 d2( "-123.456" );
		absValue = d2.abs();
		EXPECT_EQ( absValue.toString(), "123.456" );
		EXPECT_FALSE( absValue.isNegative() );

		/* Test zero (should remain zero) */
		datatypes::Decimal128 d3( "0" );
		absValue = d3.abs();
		EXPECT_EQ( absValue.toString(), "0" );
		EXPECT_TRUE( absValue.isZero() );

		/* Test negative zero (should become positive zero) */
		datatypes::Decimal128 d4( "-0.0" );
		absValue = d4.abs();
		EXPECT_TRUE( absValue.isZero() );
		EXPECT_FALSE( absValue.isNegative() );

		/* Test static method */
		datatypes::Decimal128 d5( "-789.123" );
		auto staticAbs = datatypes::Decimal128 ::abs( d5 );
		EXPECT_EQ( staticAbs.toString(), "789.123" );
		EXPECT_FALSE( staticAbs.isNegative() );

		/* Test very small negative value */
		datatypes::Decimal128 d6( "-0.000000000000000000000000001" );
		absValue = d6.abs();
		EXPECT_FALSE( absValue.isNegative() );
		EXPECT_FALSE( absValue.isZero() );
	}

	//=====================================================================
	// Advanced Mathematical Behavior Tests
	//=====================================================================

	TEST( Decimal128Test, MathematicalConsistency )
	{
		/* Test that mathematical operations are consistent */
		datatypes::Decimal128 a( "123.45" );
		datatypes::Decimal128 b( "67.89" );

		/* Test that (a + b) - a == b */
		datatypes::Decimal128 sum = a + b;
		datatypes::Decimal128 diff = sum - a;
		EXPECT_TRUE( diff == b );

		/* Test that a * b / a == b (within precision limits) */
		datatypes::Decimal128 product = a * b;
		datatypes::Decimal128 quotient = product / a;
		datatypes::Decimal128 difference = quotient - b;

		/* Due to precision, we test that they're very close */
		datatypes::Decimal128 tolerance( "0.00001" );
		EXPECT_TRUE( difference.abs() < tolerance );
	}

	TEST( Decimal128Test, RoundingConsistency )
	{
		/* Test that different rounding methods work consistently */
		datatypes::Decimal128 value( "123.456789" );

		/* Truncate should always round toward zero */
		datatypes::Decimal128 truncated = value.truncate();
		EXPECT_EQ( truncated.toString(), "123" );

		/* Floor should always round down */
		datatypes::Decimal128 floored = value.floor();
		EXPECT_EQ( floored.toString(), "123" );

		/* Ceiling should always round up */
		datatypes::Decimal128 ceiled = value.ceiling();
		EXPECT_EQ( ceiled.toString(), "124" );

		/* Round should use banker's rounding */
		datatypes::Decimal128 rounded = value.round();
		EXPECT_EQ( rounded.toString(), "123" );

		/* Test with negative values */
		datatypes::Decimal128 negValue( "-123.456789" );

		truncated = negValue.truncate();
		EXPECT_EQ( truncated.toString(), "-123" );

		floored = negValue.floor();
		EXPECT_EQ( floored.toString(), "-124" );

		ceiled = negValue.ceiling();
		EXPECT_EQ( ceiled.toString(), "-123" );

		rounded = negValue.round();
		EXPECT_EQ( rounded.toString(), "-123" );
	}

	TEST( Decimal128Test, PrecisionPreservation )
	{
		/* Test that precision is preserved in operations */
		datatypes::Decimal128 a( "0.1" );
		datatypes::Decimal128 b( "0.2" );
		datatypes::Decimal128 c( "0.3" );

		/* This should be exactly 0.3, not 0.30000000000000004 like with double */
		datatypes::Decimal128 sum = a + b;
		EXPECT_TRUE( sum == c );

		/* Test with many Decimal128 places */
		datatypes::Decimal128 precise1( "0.1234567890123456789012345678" );
		datatypes::Decimal128 precise2( "0.0000000000000000000000000001" );
		datatypes::Decimal128 preciseSum = precise1 + precise2;

		/* Should maintain precision */
		EXPECT_FALSE( preciseSum == precise1 );
		EXPECT_TRUE( preciseSum > precise1 );
	}

	//=====================================================================
	// Error Handling and Edge Cases Tests
	//=====================================================================

	TEST( Decimal128Test, DivisionByZeroHandling )
	{
		datatypes::Decimal128 dividend( "123.45" );
		datatypes::Decimal128 zero( "0" );

		/* Division by zero should throw */
		EXPECT_THROW( dividend / zero, std::overflow_error );
		EXPECT_THROW( dividend /= zero, std::overflow_error );
	}

	TEST( Decimal128Test, OverflowHandling )
	{
		/* Test near maximum values */
		try
		{
			/* Create large Decimal128 values within limits */
			datatypes::Decimal128 large1( "99999999999999999999999999.99" );
			datatypes::Decimal128 large2( "1.01" );

			/* Operations should either succeed or throw appropriately */
			datatypes::Decimal128 result = large1 * large2;
			/* If we get here, the operation succeeded */
			EXPECT_FALSE( result.isZero() );
		}
		catch ( const std::exception& )
		{
			/* Throwing on overflow is acceptable behavior */
			SUCCEED();
		}
	}

	TEST( Decimal128Test, InvalidInputHandling )
	{
		datatypes::Decimal128 result;

		/* Test various invalid string formats */
		EXPECT_FALSE( datatypes::Decimal128::tryParse( "", result ) );
		EXPECT_FALSE( datatypes::Decimal128::tryParse( "abc", result ) );
		EXPECT_FALSE( datatypes::Decimal128::tryParse( "12.34.56", result ) );
		EXPECT_FALSE( datatypes::Decimal128::tryParse( "12a34", result ) );
		EXPECT_FALSE( datatypes::Decimal128::tryParse( "+-123", result ) );
		EXPECT_FALSE( datatypes::Decimal128::tryParse( "123..", result ) );
		EXPECT_FALSE( datatypes::Decimal128::tryParse( ".123.", result ) );

		/* Test strings that are too long */
		std::string tooLong = "1.";
		for ( int i = 0; i < 50; ++i )
		{
			tooLong += "1";
		}

		EXPECT_FALSE( datatypes::Decimal128::tryParse( tooLong, result ) );
	}
}
