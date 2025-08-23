/**
 * @file TESTS_Int128.cpp
 * @brief Comprehensive tests for Int128 128-bit signed integer class
 * @details Tests covering construction, arithmetic, comparison, and edge cases
 */

#include "dnv/vista/sdk/datatypes/Int128.h"

namespace dnv::vista::sdk::test
{
	/**
	 * @brief Test fixture for Int128 operations
	 */
	class Int128Test : public ::testing::Test
	{
	protected:
		void SetUp() override {}
		void TearDown() override {}
	};

	//----------------------------------------------
	// Construction Tests
	//----------------------------------------------

	TEST_F( Int128Test, DefaultConstruction )
	{
		datatypes::Int128 zero;
		EXPECT_TRUE( zero.isZero() );
		EXPECT_FALSE( zero.isNegative() );
		EXPECT_EQ( 0ULL, zero.toLow() );
		EXPECT_EQ( 0ULL, zero.toHigh() );
	}

	TEST_F( Int128Test, ConstructionFromUint64 )
	{
		const std::uint64_t value = 0x123456789ABCDEFULL;
		datatypes::Int128 num( value );

		EXPECT_FALSE( num.isZero() );
		EXPECT_FALSE( num.isNegative() );
		EXPECT_EQ( value, num.toLow() );
		EXPECT_EQ( 0ULL, num.toHigh() );
	}

	TEST_F( Int128Test, ConstructionFromInt64Positive )
	{
		const std::int64_t value = 0x123456789ABCDEFLL;
		datatypes::Int128 num( value );

		EXPECT_FALSE( num.isZero() );
		EXPECT_FALSE( num.isNegative() );
		EXPECT_EQ( static_cast<std::uint64_t>( value ), num.toLow() );
		EXPECT_EQ( 0ULL, num.toHigh() );
	}

	TEST_F( Int128Test, ConstructionFromInt64Negative )
	{
		const std::int64_t value = -0x123456789ABCDEFLL;
		datatypes::Int128 num( value );

		EXPECT_FALSE( num.isZero() );
		EXPECT_TRUE( num.isNegative() );
		EXPECT_EQ( static_cast<std::uint64_t>( value ), num.toLow() );
		EXPECT_EQ( 0xFFFFFFFFFFFFFFFFULL, num.toHigh() ); /* Sign extension */
	}

	TEST_F( Int128Test, ConstructionFromUint32 )
	{
		const std::uint32_t value = 0x12345678U;
		datatypes::Int128 num( value );

		EXPECT_FALSE( num.isZero() );
		EXPECT_FALSE( num.isNegative() );
		EXPECT_EQ( static_cast<std::uint64_t>( value ), num.toLow() );
		EXPECT_EQ( 0ULL, num.toHigh() );
	}

	TEST_F( Int128Test, ConstructionFromIntPositive )
	{
		const int value = 0x12345678;
		datatypes::Int128 num( value );

		EXPECT_FALSE( num.isZero() );
		EXPECT_FALSE( num.isNegative() );
		EXPECT_EQ( static_cast<std::uint64_t>( value ), num.toLow() );
		EXPECT_EQ( 0ULL, num.toHigh() );
	}

	TEST_F( Int128Test, ConstructionFromIntNegative )
	{
		const int value = -0x12345678;
		datatypes::Int128 num( value );

		EXPECT_FALSE( num.isZero() );
		EXPECT_TRUE( num.isNegative() );
		EXPECT_EQ( static_cast<std::uint64_t>( value ), num.toLow() );
		/* Sign extension */
		EXPECT_EQ( 0xFFFFFFFFFFFFFFFFULL, num.toHigh() );
	}

	TEST_F( Int128Test, ConstructionFromLowHigh )
	{
		const std::uint64_t low = 0x123456789ABCDEFULL;
		const std::uint64_t high = 0xFEDCBA9876543210ULL;
		datatypes::Int128 num( low, high );

		EXPECT_FALSE( num.isZero() );
		/* High bit set */
		EXPECT_TRUE( num.isNegative() );
		EXPECT_EQ( low, num.toLow() );
		EXPECT_EQ( high, num.toHigh() );
	}

	//----------------------------------------------
	// Arithmetic Tests
	//----------------------------------------------

	TEST_F( Int128Test, Addition )
	{
		datatypes::Int128 a( 100 );
		datatypes::Int128 b( 200 );
		datatypes::Int128 result = a + b;

		EXPECT_EQ( 300ULL, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
	}

	TEST_F( Int128Test, AdditionWithCarry )
	{
		/* Test overflow from low to high word */
		datatypes::Int128 a( 0xFFFFFFFFFFFFFFFFULL, 0 );
		datatypes::Int128 b( 1 );
		datatypes::Int128 result = a + b;

		EXPECT_EQ( 0ULL, result.toLow() );
		EXPECT_EQ( 1ULL, result.toHigh() );
	}

	TEST_F( Int128Test, Subtraction )
	{
		datatypes::Int128 a( 300 );
		datatypes::Int128 b( 100 );
		datatypes::Int128 result = a - b;

		EXPECT_EQ( 200ULL, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
	}

	TEST_F( Int128Test, SubtractionWithBorrow )
	{
		/* Test borrow from high to low word */
		datatypes::Int128 a( 0, 1 );
		datatypes::Int128 b( 1 );
		datatypes::Int128 result = a - b;

		EXPECT_EQ( 0xFFFFFFFFFFFFFFFFULL, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
	}

	TEST_F( Int128Test, Multiplication )
	{
		datatypes::Int128 a( 123 );
		datatypes::Int128 b( 456 );
		datatypes::Int128 result = a * b;

		EXPECT_EQ( 123ULL * 456ULL, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
	}

	TEST_F( Int128Test, MultiplicationLarge )
	{
		/* Test multiplication that requires high word */
		const std::uint64_t a_val = 0x123456789ABCDEFULL;
		/* 2^32 */
		const std::uint64_t b_val = 0x100000000ULL;

		datatypes::Int128 a( a_val );
		datatypes::Int128 b( b_val );
		datatypes::Int128 result = a * b;

		/* Result should be a_val shifted left by 32 bits */
		EXPECT_EQ( ( a_val << 32 ) & 0xFFFFFFFFFFFFFFFFULL, result.toLow() );
		EXPECT_EQ( a_val >> 32, result.toHigh() );
	}

	TEST_F( Int128Test, Division )
	{
		datatypes::Int128 a( 456 );
		datatypes::Int128 b( 123 );
		datatypes::Int128 result = a / b;

		EXPECT_EQ( 3ULL, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
	}

	TEST_F( Int128Test, DivisionByZero )
	{
		datatypes::Int128 a( 123 );
		datatypes::Int128 zero;

		EXPECT_THROW( a / zero, std::overflow_error );
	}

	TEST_F( Int128Test, Modulo )
	{
		datatypes::Int128 a( 456 );
		datatypes::Int128 b( 123 );
		datatypes::Int128 result = a % b;

		/* 456 % 123 = 87 */
		EXPECT_EQ( 87ULL, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
	}

	TEST_F( Int128Test, ModuloByZero )
	{
		datatypes::Int128 a( 123 );
		datatypes::Int128 zero;

		EXPECT_THROW( a % zero, std::overflow_error );
	}

	TEST_F( Int128Test, UnaryMinus )
	{
		datatypes::Int128 positive( 123 );
		datatypes::Int128 negative = -positive;

		EXPECT_TRUE( negative.isNegative() );
		EXPECT_FALSE( negative.isZero() );

		/* Two's complement: ~123 + 1 */
		EXPECT_EQ( static_cast<std::uint64_t>( -123 ), negative.toLow() );
		EXPECT_EQ( 0xFFFFFFFFFFFFFFFFULL, negative.toHigh() );
	}

	TEST_F( Int128Test, UnaryMinusZero )
	{
		datatypes::Int128 zero;
		datatypes::Int128 negated = -zero;

		EXPECT_TRUE( negated.isZero() );
		EXPECT_FALSE( negated.isNegative() );
	}

	//----------------------------------------------
	// Comparison Tests
	//----------------------------------------------

	TEST_F( Int128Test, Equality )
	{
		datatypes::Int128 a( 123 );
		datatypes::Int128 b( 123 );
		datatypes::Int128 c( 456 );

		EXPECT_TRUE( a == b );
		EXPECT_FALSE( a == c );
	}

	TEST_F( Int128Test, Inequality )
	{
		datatypes::Int128 a( 123 );
		datatypes::Int128 b( 123 );
		datatypes::Int128 c( 456 );

		EXPECT_FALSE( a != b );
		EXPECT_TRUE( a != c );
	}

	TEST_F( Int128Test, LessThan )
	{
		datatypes::Int128 a( 123 );
		datatypes::Int128 b( 456 );

		EXPECT_TRUE( a < b );
		EXPECT_FALSE( b < a );
		EXPECT_FALSE( a < a );
	}

	TEST_F( Int128Test, LessThanSignedComparison )
	{
		datatypes::Int128 positive( 123 );
		datatypes::Int128 negative( -456 );

		EXPECT_TRUE( negative < positive );
		EXPECT_FALSE( positive < negative );
	}

	TEST_F( Int128Test, LessEqual )
	{
		datatypes::Int128 a( 123 );
		datatypes::Int128 b( 456 );
		datatypes::Int128 c( 123 );

		EXPECT_TRUE( a <= b );
		EXPECT_TRUE( a <= c );
		EXPECT_FALSE( b <= a );
	}

	TEST_F( Int128Test, GreaterThan )
	{
		datatypes::Int128 a( 123 );
		datatypes::Int128 b( 456 );

		EXPECT_FALSE( a > b );
		EXPECT_TRUE( b > a );
		EXPECT_FALSE( a > a );
	}

	TEST_F( Int128Test, GreaterEqual )
	{
		datatypes::Int128 a( 123 );
		datatypes::Int128 b( 456 );
		datatypes::Int128 c( 123 );

		EXPECT_FALSE( a >= b );
		EXPECT_TRUE( a >= c );
		EXPECT_TRUE( b >= a );
	}

	TEST_F( Int128Test, ComparisonHighWord )
	{
		/* Test comparison when high words differ */
		datatypes::Int128 a( 0xFFFFFFFFFFFFFFFFULL, 0 );
		datatypes::Int128 b( 0, 1 );

		EXPECT_TRUE( a < b );
		EXPECT_FALSE( b < a );
	}

	//----------------------------------------------
	// State Checking Tests
	//----------------------------------------------

	TEST_F( Int128Test, IsZero )
	{
		datatypes::Int128 zero;
		datatypes::Int128 nonZero( 1 );

		EXPECT_TRUE( zero.isZero() );
		EXPECT_FALSE( nonZero.isZero() );
	}

	TEST_F( Int128Test, IsNegative )
	{
		datatypes::Int128 positive( 123 );
		datatypes::Int128 negative( -123 );
		datatypes::Int128 zero;

		EXPECT_FALSE( positive.isNegative() );
		EXPECT_TRUE( negative.isNegative() );
		EXPECT_FALSE( zero.isNegative() );
	}

	TEST_F( Int128Test, IsNegativeHighBit )
	{
		/* Test negativity based on high bit */
		datatypes::Int128 negative( 0, 0x8000000000000000ULL );
		datatypes::Int128 positive( 0, 0x7FFFFFFFFFFFFFFFULL );

		EXPECT_TRUE( negative.isNegative() );
		EXPECT_FALSE( positive.isNegative() );
	}

	//----------------------------------------------
	// Mathematical Operation Tests
	//----------------------------------------------

	TEST_F( Int128Test, AbsoluteValue )
	{
		datatypes::Int128 positive( 123 );
		datatypes::Int128 negative( -123 );
		datatypes::Int128 zero;

		EXPECT_EQ( positive, positive.abs() );
		EXPECT_EQ( positive, negative.abs() );
		EXPECT_EQ( zero, zero.abs() );
	}

	//----------------------------------------------
	// String parsing tests
	//----------------------------------------------

	TEST_F( Int128Test, TryParseMethod )
	{
		datatypes::Int128 result;

		/* Valid positive parsing */
		EXPECT_TRUE( datatypes::Int128::tryParse( "12345", result ) );
		EXPECT_EQ( 12345ULL, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
		EXPECT_FALSE( result.isNegative() );

		/* Valid negative parsing */
		EXPECT_TRUE( datatypes::Int128::tryParse( "-9876543210", result ) );
		EXPECT_TRUE( result.isNegative() );

		/* Zero parsing */
		EXPECT_TRUE( datatypes::Int128::tryParse( "0", result ) );
		EXPECT_TRUE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		/* Large positive number */
		EXPECT_TRUE( datatypes::Int128::tryParse( "123456789012345678901234567890", result ) );
		EXPECT_FALSE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		/* Large negative number */
		EXPECT_TRUE( datatypes::Int128::tryParse( "-123456789012345678901234567890", result ) );
		EXPECT_FALSE( result.isZero() );
		EXPECT_TRUE( result.isNegative() );

		/* Positive sign */
		EXPECT_TRUE( datatypes::Int128::tryParse( "+42", result ) );
		EXPECT_EQ( 42ULL, result.toLow() );
		EXPECT_FALSE( result.isNegative() );

		/* Invalid strings */
		EXPECT_FALSE( datatypes::Int128::tryParse( "", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "abc", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "123abc", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "12.34", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "+", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "-", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "123 456", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "1e10", result ) );
	}

	TEST_F( Int128Test, ParseMethod )
	{
		/* Valid positive parsing */
		auto result1 = datatypes::Int128::parse( "12345" );
		EXPECT_EQ( 12345ULL, result1.toLow() );
		EXPECT_FALSE( result1.isNegative() );

		/* Valid negative parsing */
		auto result2 = datatypes::Int128::parse( "-9876543210" );
		EXPECT_TRUE( result2.isNegative() );

		/* Zero parsing */
		auto result3 = datatypes::Int128::parse( "0" );
		EXPECT_TRUE( result3.isZero() );

		/* Large number parsing */
		auto result4 = datatypes::Int128::parse( "123456789012345678901234567890" );
		EXPECT_FALSE( result4.isZero() );
		EXPECT_FALSE( result4.isNegative() );

		/* Positive sign */
		auto result5 = datatypes::Int128::parse( "+42" );
		EXPECT_EQ( 42ULL, result5.toLow() );
		EXPECT_FALSE( result5.isNegative() );

		/* Invalid parsing should throw */
		EXPECT_THROW( datatypes::Int128::parse( "" ), std::invalid_argument );
		EXPECT_THROW( datatypes::Int128::parse( "abc" ), std::invalid_argument );
		EXPECT_THROW( datatypes::Int128::parse( "123abc" ), std::invalid_argument );
		EXPECT_THROW( datatypes::Int128::parse( "12.34" ), std::invalid_argument );
		EXPECT_THROW( datatypes::Int128::parse( "+" ), std::invalid_argument );
		EXPECT_THROW( datatypes::Int128::parse( "-" ), std::invalid_argument );
		EXPECT_THROW( datatypes::Int128::parse( "123 456" ), std::invalid_argument );
		EXPECT_THROW( datatypes::Int128::parse( "1e10" ), std::invalid_argument );
	}

	//----------------------------------------------
	// Edge Case Tests
	//----------------------------------------------

	TEST_F( Int128Test, MaxValues )
	{
		/* Test with maximum possible values */
		datatypes::Int128 maxVal( 0xFFFFFFFFFFFFFFFFULL, 0x7FFFFFFFFFFFFFFFULL );

		EXPECT_FALSE( maxVal.isZero() );
		EXPECT_FALSE( maxVal.isNegative() );
		EXPECT_EQ( 0xFFFFFFFFFFFFFFFFULL, maxVal.toLow() );
		EXPECT_EQ( 0x7FFFFFFFFFFFFFFFULL, maxVal.toHigh() );
	}

	TEST_F( Int128Test, MinValue )
	{
		/* Test with minimum possible value (most negative) */
		datatypes::Int128 minVal( 0, 0x8000000000000000ULL );

		EXPECT_FALSE( minVal.isZero() );
		EXPECT_TRUE( minVal.isNegative() );
		EXPECT_EQ( 0ULL, minVal.toLow() );
		EXPECT_EQ( 0x8000000000000000ULL, minVal.toHigh() );
	}

	TEST_F( Int128Test, OverflowAddition )
	{
		/* Test addition overflow */
		datatypes::Int128 maxPositive( 0xFFFFFFFFFFFFFFFFULL, 0x7FFFFFFFFFFFFFFFULL );
		datatypes::Int128 one( 1 );
		datatypes::Int128 result = maxPositive + one;

		/* Should wrap to minimum negative value */
		EXPECT_TRUE( result.isNegative() );
		EXPECT_EQ( 0ULL, result.toLow() );
		EXPECT_EQ( 0x8000000000000000ULL, result.toHigh() );
	}
}
