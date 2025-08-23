/**
 * @file TESTS_ISO19848.cpp
 * @brief ISO 19848 standard tests
 */

#include "dnv/vista/sdk/transport/ISO19848.h"
#include "dnv/vista/sdk/datatypes/DateTimeISO8601.h"

namespace dnv::vista::sdk::tests
{
	//=====================================================================
	// Test fixture for ISO19848 tests
	//=====================================================================

	class ISO19848Tests : public ::testing::Test
	{
	protected:
		void SetUp() override {}

		void TearDown() override {}

		/* Helper functions to reduce lambda verbosity */
		static void unexpectedDecimal()
		{
			static_cast<void>( 0 );
			ADD_FAILURE() << "Expected specific type, got decimal";
		}

		static void unexpectedInteger()
		{
			static_cast<void>( 0 );
			ADD_FAILURE() << "Expected specific type, got integer";
		}

		static void unexpectedBoolean()
		{
			static_cast<void>( 0 );
			ADD_FAILURE() << "Expected specific type, got boolean";
		}

		static void unexpectedString()
		{
			static_cast<void>( 0 );
			ADD_FAILURE() << "Expected specific type, got string";
		}

		static void unexpectedDateTime()
		{
			static_cast<void>( 0 );
			ADD_FAILURE() << "Expected specific type, got datetime";
		}

		/* Helper function to extract year from DateTimeOffset */
		static std::string extractYearFromDateTimeOffset( const datatypes::DateTimeOffset& dto ) { return "datetime:" + std::to_string( dto.year() ); }

		/* Type-specific suppression helpers */
		template <typename T>
		static void suppressUnused( const T& ) { static_cast<void>( 0 ); }
	};

	//=====================================================================
	// Parameterized test data
	//=====================================================================

	class ISO19848VersionTest : public ::testing::TestWithParam<transport::ISO19848Version>
	{
	};

	class DataChannelTypeNamesParseTest : public ::testing::TestWithParam<std::tuple<std::string, bool>>
	{
	};

	class FormatDataTypesParseTest : public ::testing::TestWithParam<std::tuple<std::string, bool>>
	{
	};

	class FormatDataTypeValidationTest : public ::testing::TestWithParam<std::tuple<std::string, std::string, bool>>
	{
	};

	//=====================================================================
	// Test_Instance
	//=====================================================================

	TEST_F( ISO19848Tests, Test_Instance )
	{
		auto& iso = transport::ISO19848::instance();
		static_cast<void>( iso );
		SUCCEED();
	}

	//=====================================================================
	// Test_EmbeddedResource
	//=====================================================================

	TEST_F( ISO19848Tests, Test_EmbeddedResource )
	{
		/* Test that we can find and read ISO19848 embedded resources */
		auto& iso = transport::ISO19848::instance();

		/* Verify we can load data channel type names (uses embedded resources) */
		auto dataChannelTypeNames = iso.dataChannelTypeNames( transport::ISO19848Version::v2024 );
		ASSERT_NE( dataChannelTypeNames.begin(), dataChannelTypeNames.end() );

		/* Verify we can load format data types (uses embedded resources) */
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );
		ASSERT_NE( formatDataTypes.begin(), formatDataTypes.end() );
	}

	//=====================================================================
	// Test_DataChannelTypeNames_Load
	//=====================================================================

	TEST_P( ISO19848VersionTest, Test_DataChannelTypeNames_Load )
	{
		auto version = GetParam();
		auto& iso = transport::ISO19848::instance();
		auto dataChannelTypeNames = iso.dataChannelTypeNames( version );

		ASSERT_NE( dataChannelTypeNames.begin(), dataChannelTypeNames.end() );
	}

	INSTANTIATE_TEST_SUITE_P( ISO19848Tests, ISO19848VersionTest, ::testing::Values( transport::ISO19848Version::v2018, transport::ISO19848Version::v2024 ) );

	//=====================================================================
	// Test_DataChannelTypeNames_Parse_Self
	//=====================================================================

	TEST_F( ISO19848Tests, Test_DataChannelTypeNames_Parse_Self )
	{
		auto& iso = transport::ISO19848::instance();
		auto dataChannelTypeNames = iso.dataChannelTypeNames( transport::ISO19848Version::v2024 );

		for ( const auto& typeName : dataChannelTypeNames )
		{
			auto result = dataChannelTypeNames.parse( typeName.type() );
			ASSERT_TRUE( result.isOk() );
			ASSERT_EQ( typeName.type(), result.ok().typeName().type() );
		}
	}

	//=====================================================================
	// Test_DataChannelTypeNames_Parse
	//=====================================================================

	TEST_P( DataChannelTypeNamesParseTest, Test_DataChannelTypeNames_Parse )
	{
		auto [value, expectedResult] = GetParam();
		auto& iso = transport::ISO19848::instance();
		auto dataChannelTypeNames = iso.dataChannelTypeNames( transport::ISO19848Version::v2024 );
		auto result = dataChannelTypeNames.parse( value );

		if ( expectedResult )
		{
			ASSERT_TRUE( result.isOk() );
			ASSERT_EQ( value, result.ok().typeName().type() );
		}
		else
		{
			ASSERT_TRUE( result.isInvalid() );
		}
	}

	INSTANTIATE_TEST_SUITE_P( ISO19848Tests, DataChannelTypeNamesParseTest,
		::testing::Values( std::make_tuple( "Inst", true ), std::make_tuple( "Average", true ), std::make_tuple( "Max", true ), std::make_tuple( "Min", true ),
			std::make_tuple( "Median", true ), std::make_tuple( "Mode", true ), std::make_tuple( "StandardDeviation", true ),
			std::make_tuple( "Calculated", true ), std::make_tuple( "SetPoint", true ), std::make_tuple( "Command", true ), std::make_tuple( "Alert", true ),
			std::make_tuple( "Status", true ), std::make_tuple( "ManualInput", true ), std::make_tuple( "manualInput", false ), std::make_tuple( "asd", false ),
			std::make_tuple( "some-random", false ), std::make_tuple( "InputManual", false ) ) );

	//=====================================================================
	// Test_FormatDataTypes_Load
	//=====================================================================

	TEST_P( ISO19848VersionTest, Test_FormatDataTypes_Load )
	{
		auto version = GetParam();
		auto& iso = transport::ISO19848::instance();
		auto types = iso.formatDataTypes( version );

		ASSERT_NE( types.begin(), types.end() );
	}

	//=====================================================================
	// Test_FormatDataType_Parse_Self
	//=====================================================================

	TEST_F( ISO19848Tests, Test_FormatDataType_Parse_Self )
	{
		auto& iso = transport::ISO19848::instance();
		auto types = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		for ( const auto& typeName : types )
		{
			auto result = types.parse( typeName.type() );
			ASSERT_TRUE( result.isOk() );
			ASSERT_EQ( typeName.type(), result.ok().typeName().type() );
		}
	}

	//=====================================================================
	// Test_FormatDataType_Parse
	//=====================================================================

	TEST_P( FormatDataTypesParseTest, Test_FormatDataType_Parse )
	{
		auto [value, expectedResult] = GetParam();
		auto& iso = transport::ISO19848::instance();
		auto types = iso.formatDataTypes( transport::ISO19848Version::v2024 );
		auto result = types.parse( value );

		if ( expectedResult )
		{
			ASSERT_TRUE( result.isOk() );
			ASSERT_EQ( value, result.ok().typeName().type() );
		}
		else
		{
			ASSERT_TRUE( result.isInvalid() );
		}
	}

	INSTANTIATE_TEST_SUITE_P( ISO19848Tests, FormatDataTypesParseTest,
		::testing::Values( std::make_tuple( "Decimal", true ), std::make_tuple( "Integer", true ), std::make_tuple( "Boolean", true ),
			std::make_tuple( "String", true ), std::make_tuple( "DateTime", true ), std::make_tuple( "decimal", false ), std::make_tuple( "string", false ),
			std::make_tuple( "asd", false ), std::make_tuple( "some-random", false ), std::make_tuple( "TimeDate", false ) ) );

	//=====================================================================
	// Test_FormatDataType_Parse_Valid
	//=====================================================================

	TEST_P( FormatDataTypeValidationTest, Test_FormatDataType_Parse_Valid )
	{
		auto [type, value, expectedResult] = GetParam();
		auto& iso = transport::ISO19848::instance();
		auto types = iso.formatDataTypes( transport::ISO19848Version::v2024 );
		auto result = types.parse( type );

		ASSERT_TRUE( result.isOk() );
		ASSERT_EQ( type, result.ok().typeName().type() );

		transport::Value outValue;
		auto validateResult = result.ok().typeName().validate( value, outValue );

		if ( expectedResult )
		{
			ASSERT_TRUE( validateResult.isOk() );
		}
		else
		{
			ASSERT_TRUE( validateResult.isInvalid() );
		}
	}

	INSTANTIATE_TEST_SUITE_P( ISO19848Tests, FormatDataTypeValidationTest,
		::testing::Values( std::make_tuple( "Decimal", "0.1", true ), std::make_tuple( "DateTime", "1994-11-20T10:25:33Z", true ),
			std::make_tuple( "DateTime", "1994-11-20T10", false ) ) );

	//=====================================================================
	// Test_FormatDataType_Switch_Match_Methods
	//=====================================================================

	TEST_F( ISO19848Tests, Test_FormatDataType_Switch_Match_Methods )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		/* Find the Integer format type for testing */
		auto parseResult = formatDataTypes.parse( "Integer" );
		ASSERT_TRUE( parseResult.isOk() );
		auto integerType = parseResult.ok().typeName();

		/* Test switchOn method with valid integer */
		bool switchCalled = false;
		int switchValue = 0;
		ASSERT_NO_THROW( {
			integerType.switchOn(
				"42",
				[]( const datatypes::Decimal128& d ) {
					static_cast<void>( d );
					ADD_FAILURE() << "Expected integer, got decimal";
				},
				[&]( int i ) {
					switchCalled = true;
					switchValue = i;
				},
				[]( bool b ) {
					static_cast<void>( b );
					ADD_FAILURE() << "Expected integer, got boolean";
				},
				[]( std::string_view s ) {
					static_cast<void>( s );
					ADD_FAILURE() << "Expected integer, got string";
				},
				[]( const datatypes::DateTimeOffset& tp ) {
					static_cast<void>( tp );
					ADD_FAILURE() << "Expected integer, got datetime";
				} );
		} );
		ASSERT_TRUE( switchCalled );
		ASSERT_EQ( switchValue, 42 );

		/* Test match method with valid integer */
		std::string matchResult;
		ASSERT_NO_THROW( {
			matchResult = integerType.matchOn<std::string>(
				"123",
				[]( const datatypes::Decimal128& d ) -> std::string {
					static_cast<void>( d );
					ADD_FAILURE() << "Expected integer, got decimal";
					return "";
				},
				[]( int i ) -> std::string { return "Matched integer: " + std::to_string( i ); },
				[]( bool b ) -> std::string {
					static_cast<void>( b );
					ADD_FAILURE() << "Expected integer, got boolean";
					return "";
				},
				[]( std::string_view s ) -> std::string {
					static_cast<void>( s );
					ADD_FAILURE() << "Expected integer, got string";
					return "";
				},
				[]( const datatypes::DateTimeOffset& tp ) -> std::string {
					static_cast<void>( tp );
					ADD_FAILURE() << "Expected integer, got datetime";
					return "";
				} );
		} );
		ASSERT_EQ( matchResult, "Matched integer: 123" );

		/* Test that invalid values throw exceptions */
		ASSERT_THROW(
			{
				integerType.switchOn(
					"not_a_number", []( const datatypes::Decimal128& d ) { static_cast<void>( d ); }, []( int i ) { static_cast<void>( i ); },
					[]( bool b ) { static_cast<void>( b ); }, []( std::string_view s ) { static_cast<void>( s ); },
					[]( const datatypes::DateTimeOffset& tp ) { static_cast<void>( tp ); } );
			},
			ValidationException );

		ASSERT_THROW(
			{
				integerType.matchOn<std::string>(
					"not_a_number",
					[]( const datatypes::Decimal128& d ) -> std::string {
						static_cast<void>( d );
						return "";
					},
					[]( int i ) -> std::string {
						static_cast<void>( i );
						return "";
					},
					[]( bool b ) -> std::string {
						static_cast<void>( b );
						return "";
					},
					[]( std::string_view s ) -> std::string {
						static_cast<void>( s );
						return "";
					},
					[]( const datatypes::DateTimeOffset& tp ) -> std::string {
						static_cast<void>( tp );
						return "";
					} );
			},
			ValidationException );
	}

	//=====================================================================
	// Test_SwitchOn_All_Actions
	//=====================================================================

	TEST_F( ISO19848Tests, Test_SwitchOn_Decimal_Action )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		auto parseResult = formatDataTypes.parse( "Decimal" );
		ASSERT_TRUE( parseResult.isOk() );
		auto decimalType = parseResult.ok().typeName();

		bool decimalCalled = false;
		datatypes::Decimal128 decimalValue;
		ASSERT_NO_THROW( {
			decimalType.switchOn(
				"123.456",
				[&]( datatypes::Decimal128 d ) {
					decimalCalled = true;
					decimalValue = d;
				},
				[]( int i ) {
					static_cast<void>( i );
					ADD_FAILURE() << "Expected decimal, got integer";
				},
				[]( bool b ) {
					static_cast<void>( b );
					ADD_FAILURE() << "Expected decimal, got boolean";
				},
				[]( std::string_view s ) {
					static_cast<void>( s );
					ADD_FAILURE() << "Expected decimal, got string";
				},
				[]( const datatypes::DateTimeOffset& tp ) {
					static_cast<void>( tp );
					ADD_FAILURE() << "Expected decimal, got datetime";
				} );
		} );
		ASSERT_TRUE( decimalCalled );

		/* Test with reasonable tolerance for IEEE 754 precision limits */
		datatypes::Decimal128 expected( "123.456" );
		datatypes::Decimal128 tolerance( "0.000001" );
		datatypes::Decimal128 difference = ( decimalValue - expected ).abs();
		ASSERT_TRUE( difference < tolerance );
	}

	TEST_F( ISO19848Tests, Test_SwitchOn_Integer_Action )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		auto parseResult = formatDataTypes.parse( "Integer" );
		ASSERT_TRUE( parseResult.isOk() );
		auto integerType = parseResult.ok().typeName();

		bool integerCalled = false;
		int integerValue = 0;
		ASSERT_NO_THROW( {
			integerType.switchOn(
				"42",
				[]( datatypes::Decimal128 d ) {
					static_cast<void>( d );
					ADD_FAILURE() << "Expected integer, got decimal";
				},
				[&]( int i ) {
					integerCalled = true;
					integerValue = i;
				},
				[]( bool b ) {
					static_cast<void>( b );
					ADD_FAILURE() << "Expected integer, got boolean";
				},
				[]( std::string_view s ) {
					static_cast<void>( s );
					ADD_FAILURE() << "Expected integer, got string";
				},
				[]( const datatypes::DateTimeOffset& tp ) {
					static_cast<void>( tp );
					ADD_FAILURE() << "Expected integer, got datetime";
				} );
		} );
		ASSERT_TRUE( integerCalled );
		ASSERT_EQ( integerValue, 42 );
	}

	TEST_F( ISO19848Tests, Test_SwitchOn_Boolean_Action )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		auto parseResult = formatDataTypes.parse( "Boolean" );
		ASSERT_TRUE( parseResult.isOk() );
		auto booleanType = parseResult.ok().typeName();

		bool booleanCalled = false;
		bool booleanValue = false;
		ASSERT_NO_THROW( {
			booleanType.switchOn(
				"true",
				[]( datatypes::Decimal128 d ) {
					static_cast<void>( d );
					ADD_FAILURE() << "Expected boolean, got decimal";
				},
				[]( int i ) {
					static_cast<void>( i );
					ADD_FAILURE() << "Expected boolean, got integer";
				},
				[&]( bool b ) {
					booleanCalled = true;
					booleanValue = b;
				},
				[]( std::string_view s ) {
					static_cast<void>( s );
					ADD_FAILURE() << "Expected boolean, got string";
				},
				[]( const datatypes::DateTimeOffset& tp ) {
					static_cast<void>( tp );
					ADD_FAILURE() << "Expected boolean, got datetime";
				} );
		} );
		ASSERT_TRUE( booleanCalled );
		ASSERT_TRUE( booleanValue );
	}

	TEST_F( ISO19848Tests, Test_SwitchOn_String_Action )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		auto parseResult = formatDataTypes.parse( "String" );
		ASSERT_TRUE( parseResult.isOk() );
		auto stringType = parseResult.ok().typeName();

		bool stringCalled = false;
		std::string stringValue;
		ASSERT_NO_THROW( {
			stringType.switchOn(
				"Hello World",
				[]( datatypes::Decimal128 d ) {
					static_cast<void>( d );
					ADD_FAILURE() << "Expected string, got decimal";
				},
				[]( int i ) {
					static_cast<void>( i );
					ADD_FAILURE() << "Expected string, got integer";
				},
				[]( bool b ) {
					static_cast<void>( b );
					ADD_FAILURE() << "Expected string, got boolean";
				},
				[&]( std::string_view s ) {
					stringCalled = true;
					stringValue = std::string( s );
				},
				[]( const datatypes::DateTimeOffset& tp ) {
					static_cast<void>( tp );
					ADD_FAILURE() << "Expected string, got datetime";
				} );
		} );
		ASSERT_TRUE( stringCalled );
		ASSERT_EQ( stringValue, "Hello World" );
	}

	TEST_F( ISO19848Tests, Test_SwitchOn_DateTime_Action )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		auto parseResult = formatDataTypes.parse( "DateTime" );
		ASSERT_TRUE( parseResult.isOk() );
		auto dateTimeType = parseResult.ok().typeName();

		bool dateTimeCalled = false;
		datatypes::DateTimeOffset dateTimeValue;
		ASSERT_NO_THROW( {
			dateTimeType.switchOn(
				"1994-11-20T10:25:33Z",
				[]( datatypes::Decimal128 d ) {
					static_cast<void>( d );
					ADD_FAILURE() << "Expected datetime, got decimal";
				},
				[]( int i ) {
					static_cast<void>( i );
					ADD_FAILURE() << "Expected datetime, got integer";
				},
				[]( bool b ) {
					static_cast<void>( b );
					ADD_FAILURE() << "Expected datetime, got boolean";
				},
				[]( std::string_view s ) {
					static_cast<void>( s );
					ADD_FAILURE() << "Expected datetime, got string";
				},
				[&]( const datatypes::DateTimeOffset& dto ) {
					dateTimeCalled = true;
					dateTimeValue = dto;
				} );
		} );
		ASSERT_TRUE( dateTimeCalled );
	}

	TEST_F( ISO19848Tests, Test_SwitchOn_Invalid_Values_Throw_Exception )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		/* Test Decimal with invalid value */
		auto decimalResult = formatDataTypes.parse( "Decimal" );
		ASSERT_TRUE( decimalResult.isOk() );
		auto decimalType = decimalResult.ok().typeName();
		ASSERT_THROW(
			{
				decimalType.switchOn(
					"not_a_decimal", []( datatypes::Decimal128 d ) { static_cast<void>( d ); }, []( int i ) { static_cast<void>( i ); },
					[]( bool b ) { static_cast<void>( b ); }, []( std::string_view s ) { static_cast<void>( s ); },
					[]( const datatypes::DateTimeOffset& tp ) { static_cast<void>( tp ); } );
			},
			ValidationException );

		/* Test Integer with invalid value */
		auto integerResult = formatDataTypes.parse( "Integer" );
		ASSERT_TRUE( integerResult.isOk() );
		auto integerType = integerResult.ok().typeName();
		ASSERT_THROW(
			{
				integerType.switchOn(
					"not_an_integer", []( datatypes::Decimal128 d ) { static_cast<void>( d ); }, []( int i ) { static_cast<void>( i ); },
					[]( bool b ) { static_cast<void>( b ); }, []( std::string_view s ) { static_cast<void>( s ); },
					[]( const datatypes::DateTimeOffset& tp ) { static_cast<void>( tp ); } );
			},
			ValidationException );

		/* Test Boolean with invalid value */
		auto booleanResult = formatDataTypes.parse( "Boolean" );
		ASSERT_TRUE( booleanResult.isOk() );
		auto booleanType = booleanResult.ok().typeName();
		ASSERT_THROW(
			{
				booleanType.switchOn(
					"not_a_boolean", []( datatypes::Decimal128 d ) { static_cast<void>( d ); }, []( int i ) { static_cast<void>( i ); },
					[]( bool b ) { static_cast<void>( b ); }, []( std::string_view s ) { static_cast<void>( s ); },
					[]( const datatypes::DateTimeOffset& tp ) { static_cast<void>( tp ); } );
			},
			ValidationException );

		/* Test DateTime with invalid value */
		auto dateTimeResult = formatDataTypes.parse( "DateTime" );
		ASSERT_TRUE( dateTimeResult.isOk() );
		auto dateTimeType = dateTimeResult.ok().typeName();
		ASSERT_THROW(
			{
				dateTimeType.switchOn(
					"not_a_datetime", []( datatypes::Decimal128 d ) { static_cast<void>( d ); }, []( int i ) { static_cast<void>( i ); },
					[]( bool b ) { static_cast<void>( b ); }, []( std::string_view s ) { static_cast<void>( s ); },
					[]( const datatypes::DateTimeOffset& tp ) { static_cast<void>( tp ); } );
			},
			ValidationException );
	}

	TEST_F( ISO19848Tests, Test_SwitchOn_Edge_Cases )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		/* Test negative decimal */
		auto decimalResult = formatDataTypes.parse( "Decimal" );
		ASSERT_TRUE( decimalResult.isOk() );
		auto decimalType = decimalResult.ok().typeName();

		bool negativeCalled = false;
		datatypes::Decimal128 negativeValue;
		ASSERT_NO_THROW( {
			decimalType.switchOn(
				"-456.789",
				[&]( datatypes::Decimal128 d ) {
					negativeCalled = true;
					negativeValue = d;
				},
				[]( int i ) {
					static_cast<void>( i );
					ADD_FAILURE() << "Expected decimal, got integer";
				},
				[]( bool b ) {
					static_cast<void>( b );
					ADD_FAILURE() << "Expected decimal, got boolean";
				},
				[]( std::string_view s ) {
					static_cast<void>( s );
					ADD_FAILURE() << "Expected decimal, got string";
				},
				[]( const datatypes::DateTimeOffset& tp ) {
					static_cast<void>( tp );
					ADD_FAILURE() << "Expected decimal, got datetime";
				} );
		} );
		ASSERT_TRUE( negativeCalled );

		/* Test with reasonable tolerance for IEEE 754 precision limits */
		datatypes::Decimal128 expected( "-456.789" );
		datatypes::Decimal128 tolerance( "0.000001" );
		datatypes::Decimal128 difference = ( negativeValue - expected ).abs();
		ASSERT_TRUE( difference < tolerance );

		/* Test negative integer */
		auto integerResult = formatDataTypes.parse( "Integer" );
		ASSERT_TRUE( integerResult.isOk() );
		auto integerType = integerResult.ok().typeName();

		bool negativeIntCalled = false;
		int negativeIntValue = 0;
		ASSERT_NO_THROW( {
			integerType.switchOn(
				"-123",
				[]( datatypes::Decimal128 d ) {
					static_cast<void>( d );
					ADD_FAILURE() << "Expected integer, got decimal";
				},
				[&]( int i ) {
					negativeIntCalled = true;
					negativeIntValue = i;
				},
				[]( bool b ) {
					static_cast<void>( b );
					ADD_FAILURE() << "Expected integer, got boolean";
				},
				[]( std::string_view s ) {
					static_cast<void>( s );
					ADD_FAILURE() << "Expected integer, got string";
				},
				[]( const datatypes::DateTimeOffset& tp ) {
					static_cast<void>( tp );
					ADD_FAILURE() << "Expected integer, got datetime";
				} );
		} );
		ASSERT_TRUE( negativeIntCalled );
		ASSERT_EQ( negativeIntValue, -123 );

		/* Test false boolean */
		auto booleanResult = formatDataTypes.parse( "Boolean" );
		ASSERT_TRUE( booleanResult.isOk() );
		auto booleanType = booleanResult.ok().typeName();

		bool falseCalled = false;
		bool falseValue = true;
		ASSERT_NO_THROW( {
			booleanType.switchOn(
				"false",
				[]( datatypes::Decimal128 d ) {
					static_cast<void>( d );
					ADD_FAILURE() << "Expected boolean, got decimal";
				},
				[]( int i ) {
					static_cast<void>( i );
					ADD_FAILURE() << "Expected boolean, got integer";
				},
				[&]( bool b ) {
					falseCalled = true;
					falseValue = b;
				},
				[]( std::string_view s ) {
					static_cast<void>( s );
					ADD_FAILURE() << "Expected boolean, got string";
				},
				[]( const datatypes::DateTimeOffset& tp ) {
					static_cast<void>( tp );
					ADD_FAILURE() << "Expected boolean, got datetime";
				} );
		} );
		ASSERT_TRUE( falseCalled );
		ASSERT_FALSE( falseValue );

		/* Test empty string */
		auto stringResult = formatDataTypes.parse( "String" );
		ASSERT_TRUE( stringResult.isOk() );
		auto stringType = stringResult.ok().typeName();

		bool emptyCalled = false;
		std::string emptyValue = "not_empty";
		ASSERT_NO_THROW( {
			stringType.switchOn(
				"",
				[]( datatypes::Decimal128 d ) {
					static_cast<void>( d );
					ADD_FAILURE() << "Expected string, got decimal";
				},
				[]( int i ) {
					static_cast<void>( i );
					ADD_FAILURE() << "Expected string, got integer";
				},
				[]( bool b ) {
					static_cast<void>( b );
					ADD_FAILURE() << "Expected string, got boolean";
				},
				[&]( std::string_view s ) {
					emptyCalled = true;
					emptyValue = std::string( s );
				},
				[]( const datatypes::DateTimeOffset& tp ) {
					static_cast<void>( tp );
					ADD_FAILURE() << "Expected string, got datetime";
				} );
		} );
		ASSERT_TRUE( emptyCalled );
		ASSERT_TRUE( emptyValue.empty() );
	}

	//=====================================================================
	// Test_Match_All_Functions
	//=====================================================================

	TEST_F( ISO19848Tests, Test_Match_Decimal_Function )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		auto parseResult = formatDataTypes.parse( "Decimal" );
		ASSERT_TRUE( parseResult.isOk() );
		auto decimalType = parseResult.ok().typeName();

		/* Test match with decimal value - returns string representation */
		std::string result;
		ASSERT_NO_THROW( {
			result = decimalType.matchOn<std::string>(
				"123.456", []( const datatypes::Decimal128& d ) -> std::string { return "decimal:" + d.toString(); },
				[]( int ) -> std::string {
					unexpectedInteger();
					return "";
				},
				[]( bool ) -> std::string {
					unexpectedBoolean();
					return "";
				},
				[]( std::string_view ) -> std::string {
					unexpectedString();
					return "";
				},
				[]( const datatypes::DateTimeOffset& ) -> std::string {
					unexpectedDateTime();
					return "";
				} );
		} );

		/* Test with reasonable tolerance for the result comparison */
		std::string expected = "decimal:" + datatypes::Decimal128( "123.456" ).toString();
		ASSERT_EQ( result, expected );

		/* Test match with decimal value - returns double */
		double doubleResult = 0.0;
		ASSERT_NO_THROW( {
			doubleResult = decimalType.matchOn<double>(
				"987.654", []( const datatypes::Decimal128& d ) -> double { return d.toDouble(); },
				[]( int ) -> double {
					unexpectedInteger();
					return 0.0;
				},
				[]( bool ) -> double {
					unexpectedBoolean();
					return 0.0;
				},
				[]( std::string_view ) -> double {
					unexpectedString();
					return 0.0;
				},
				[]( const datatypes::DateTimeOffset& ) -> double {
					unexpectedDateTime();
					return 0.0;
				} );
		} );
		ASSERT_NEAR( doubleResult, 987.654, 0.000001 );
	}

	TEST_F( ISO19848Tests, Test_Match_Integer_Function )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		auto parseResult = formatDataTypes.parse( "Integer" );
		ASSERT_TRUE( parseResult.isOk() );
		auto integerType = parseResult.ok().typeName();

		/* Test match with integer value - returns string representation */
		std::string result;
		ASSERT_NO_THROW( {
			result = integerType.matchOn<std::string>(
				"42",
				[]( const datatypes::Decimal128& ) -> std::string {
					unexpectedDecimal();
					return "";
				},
				[]( int i ) -> std::string { return "integer:" + std::to_string( i ); },
				[]( bool ) -> std::string {
					unexpectedBoolean();
					return "";
				},
				[]( std::string_view ) -> std::string {
					unexpectedString();
					return "";
				},
				[]( const datatypes::DateTimeOffset& ) -> std::string {
					unexpectedDateTime();
					return "";
				} );
		} );
		ASSERT_EQ( result, "integer:42" );

		/* Test match with integer value - returns squared value */
		int squaredResult = 0;
		ASSERT_NO_THROW( {
			squaredResult = integerType.matchOn<int>(
				"7",
				[]( const datatypes::Decimal128& ) -> int {
					unexpectedDecimal();
					return 0;
				},
				[]( int i ) -> int { return i * i; },
				[]( bool ) -> int {
					unexpectedBoolean();
					return 0;
				},
				[]( std::string_view ) -> int {
					unexpectedString();
					return 0;
				},
				[]( const datatypes::DateTimeOffset& ) -> int {
					unexpectedDateTime();
					return 0;
				} );
		} );
		ASSERT_EQ( squaredResult, 49 );
	}

	TEST_F( ISO19848Tests, Test_Match_Boolean_Function )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		auto parseResult = formatDataTypes.parse( "Boolean" );
		ASSERT_TRUE( parseResult.isOk() );
		auto booleanType = parseResult.ok().typeName();

		/* Test match with boolean true - returns string representation */
		std::string result;
		ASSERT_NO_THROW( {
			result = booleanType.matchOn<std::string>(
				"true",
				[]( const datatypes::Decimal128& ) -> std::string {
					unexpectedDecimal();
					return "";
				},
				[]( int ) -> std::string {
					unexpectedInteger();
					return "";
				},
				[]( bool b ) -> std::string { return b ? "boolean:true" : "boolean:false"; },
				[]( std::string_view ) -> std::string {
					unexpectedString();
					return "";
				},
				[]( const datatypes::DateTimeOffset& ) -> std::string {
					unexpectedDateTime();
					return "";
				} );
		} );
		ASSERT_EQ( result, "boolean:true" );

		/* Test match with boolean false - returns inverted value */
		bool invertedResult = true;
		ASSERT_NO_THROW( {
			invertedResult = booleanType.matchOn<bool>(
				"false",
				[]( const datatypes::Decimal128& ) -> bool {
					unexpectedDecimal();
					return false;
				},
				[]( int ) -> bool {
					unexpectedInteger();
					return false;
				},
				[]( bool b ) -> bool { return !b; },
				[]( std::string_view ) -> bool {
					unexpectedString();
					return false;
				},
				[]( const datatypes::DateTimeOffset& ) -> bool {
					unexpectedDateTime();
					return false;
				} );
		} );
		ASSERT_TRUE( invertedResult );
	}

	TEST_F( ISO19848Tests, Test_Match_String_Function )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		auto parseResult = formatDataTypes.parse( "String" );
		ASSERT_TRUE( parseResult.isOk() );
		auto stringType = parseResult.ok().typeName();

		/* Test match with string value - returns length */
		size_t lengthResult = 0;
		ASSERT_NO_THROW( {
			lengthResult = stringType.matchOn<size_t>(
				"Hello World",
				[]( const datatypes::Decimal128& ) -> size_t {
					unexpectedDecimal();
					return 0;
				},
				[]( int ) -> size_t {
					unexpectedInteger();
					return 0;
				},
				[]( bool ) -> size_t {
					unexpectedBoolean();
					return 0;
				},
				[]( std::string_view s ) -> size_t { return s.length(); },
				[]( const datatypes::DateTimeOffset& ) -> size_t {
					unexpectedDateTime();
					return 0;
				} );
		} );
		ASSERT_EQ( lengthResult, 11 );

		/* Test match with string value - returns uppercase */
		std::string uppercaseResult;
		ASSERT_NO_THROW( {
			uppercaseResult = stringType.matchOn<std::string>(
				"hello",
				[]( const datatypes::Decimal128& ) -> std::string {
					unexpectedDecimal();
					return "";
				},
				[]( int ) -> std::string {
					unexpectedInteger();
					return "";
				},
				[]( bool ) -> std::string {
					unexpectedBoolean();
					return "";
				},
				[]( std::string_view s ) -> std::string {
					std::string result( s );
					std::transform( result.begin(), result.end(), result.begin(), []( unsigned char c ) { return static_cast<char>( std::toupper( c ) ); } );
					return "string:" + result;
				},
				[]( const datatypes::DateTimeOffset& ) -> std::string {
					unexpectedDateTime();
					return "";
				} );
		} );
		ASSERT_EQ( uppercaseResult, "string:HELLO" );
	}

	TEST_F( ISO19848Tests, Test_Match_DateTime_Function )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		auto parseResult = formatDataTypes.parse( "DateTime" );
		ASSERT_TRUE( parseResult.isOk() );
		auto dateTimeType = parseResult.ok().typeName();

		/* Test match with datetime value - returns year */
		std::string result;
		ASSERT_NO_THROW( {
			result = dateTimeType.matchOn<std::string>(
				"1994-11-20T10:25:33Z",
				[]( const datatypes::Decimal128& ) -> std::string {
					unexpectedDecimal();
					return "";
				},
				[]( int ) -> std::string {
					unexpectedInteger();
					return "";
				},
				[]( bool ) -> std::string {
					unexpectedBoolean();
					return "";
				},
				[]( std::string_view ) -> std::string {
					unexpectedString();
					return "";
				},
				[]( const datatypes::DateTimeOffset& dto ) -> std::string { return extractYearFromDateTimeOffset( dto ); } );
		} );
		ASSERT_EQ( result, "datetime:1994" );

		/* Test match with datetime value - returns time_point duration */
		bool durationResult = false;
		ASSERT_NO_THROW( {
			durationResult = dateTimeType.matchOn<bool>(
				"1994-11-20T10:25:33Z",
				[]( const datatypes::Decimal128& ) -> bool {
					unexpectedDecimal();
					return false;
				},
				[]( int ) -> bool {
					unexpectedInteger();
					return false;
				},
				[]( bool ) -> bool {
					unexpectedBoolean();
					return false;
				},
				[]( std::string_view ) -> bool {
					unexpectedString();
					return false;
				},
				[]( const datatypes::DateTimeOffset& dto ) -> bool {
					/* Just verify we got a valid DateTimeOffset with the expected year */
					return dto.year() == 1994;
				} );
		} );
		ASSERT_TRUE( durationResult );
	}

	TEST_F( ISO19848Tests, Test_Match_Invalid_Values_Throw_Exception )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		/* Test Decimal with invalid value */
		auto decimalResult = formatDataTypes.parse( "Decimal" );
		ASSERT_TRUE( decimalResult.isOk() );
		auto decimalType = decimalResult.ok().typeName();
		ASSERT_THROW(
			{
				decimalType.matchOn<std::string>(
					"not_a_decimal", []( const datatypes::Decimal128& ) -> std::string { return ""; }, []( int ) -> std::string { return ""; },
					[]( bool ) -> std::string { return ""; }, []( std::string_view ) -> std::string { return ""; },
					[]( const datatypes::DateTimeOffset& ) -> std::string { return ""; } );
			},
			ValidationException );

		/* Test Integer with invalid value */
		auto integerResult = formatDataTypes.parse( "Integer" );
		ASSERT_TRUE( integerResult.isOk() );
		auto integerType = integerResult.ok().typeName();
		ASSERT_THROW(
			{
				integerType.matchOn<int>(
					"not_an_integer", []( const datatypes::Decimal128& ) -> int { return 0; }, []( int ) -> int { return 0; }, []( bool ) -> int { return 0; },
					[]( std::string_view ) -> int { return 0; }, []( const datatypes::DateTimeOffset& ) -> int { return 0; } );
			},
			ValidationException );

		/* Test Boolean with invalid value */
		auto booleanResult = formatDataTypes.parse( "Boolean" );
		ASSERT_TRUE( booleanResult.isOk() );
		auto booleanType = booleanResult.ok().typeName();
		ASSERT_THROW(
			{
				booleanType.matchOn<bool>(
					"not_a_boolean", []( const datatypes::Decimal128& ) -> bool { return false; }, []( int ) -> bool { return false; }, []( bool ) -> bool { return false; },
					[]( std::string_view ) -> bool { return false; }, []( const datatypes::DateTimeOffset& ) -> bool { return false; } );
			},
			ValidationException );

		/* Test DateTime with invalid value */
		auto dateTimeResult = formatDataTypes.parse( "DateTime" );
		ASSERT_TRUE( dateTimeResult.isOk() );
		auto dateTimeType = dateTimeResult.ok().typeName();
		ASSERT_THROW(
			{
				dateTimeType.matchOn<std::string>(
					"not_a_datetime", []( const datatypes::Decimal128& ) -> std::string { return ""; }, []( int ) -> std::string { return ""; },
					[]( bool ) -> std::string { return ""; }, []( std::string_view ) -> std::string { return ""; },
					[]( const datatypes::DateTimeOffset& ) -> std::string { return ""; } );
			},
			ValidationException );
	}

	TEST_F( ISO19848Tests, Test_Match_Edge_Cases_And_Return_Types )
	{
		auto& iso = transport::ISO19848::instance();
		auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

		/* Test negative decimal with custom struct return type */
		struct DecimalInfo
		{
			datatypes::Decimal128 value;
			bool isNegative;
			std::string toString() const { return isNegative ? "negative:" + value.toString()
															 : "positive:" + value.toString(); }
		};

		auto decimalResult = formatDataTypes.parse( "Decimal" );
		ASSERT_TRUE( decimalResult.isOk() );
		auto decimalType = decimalResult.ok().typeName();

		DecimalInfo info;
		ASSERT_NO_THROW( {
			info = decimalType.matchOn<DecimalInfo>(
				"-456.789", []( const datatypes::Decimal128& d ) -> DecimalInfo { return { d, d < datatypes::Decimal128{ 0 } }; },
				[]( int ) -> DecimalInfo {
					unexpectedInteger();
					return {};
				},
				[]( bool ) -> DecimalInfo {
					unexpectedBoolean();
					return {};
				},
				[]( std::string_view ) -> DecimalInfo {
					unexpectedString();
					return {};
				},
				[]( const datatypes::DateTimeOffset& ) -> DecimalInfo {
					unexpectedDateTime();
					return {};
				} );
		} );

		/* Test with reasonable tolerance for IEEE 754 precision limits */
		datatypes::Decimal128 expected( "-456.789" );
		datatypes::Decimal128 tolerance( "0.000001" );
		datatypes::Decimal128 difference = ( info.value - expected ).abs();
		ASSERT_TRUE( difference < tolerance );
		ASSERT_TRUE( info.isNegative );
		ASSERT_EQ( info.toString(), "negative:" + expected.toString() );

		/* Test integer with complex calculation return */
		auto integerResult = formatDataTypes.parse( "Integer" );
		ASSERT_TRUE( integerResult.isOk() );
		auto integerType = integerResult.ok().typeName();

		std::vector<int> fibResult;
		ASSERT_NO_THROW( {
			fibResult = integerType.matchOn<std::vector<int>>(
				"5",
				[]( const datatypes::Decimal128& ) -> std::vector<int> {
					unexpectedDecimal();
					return {};
				},
				[]( int n ) -> std::vector<int> {
					/* Generate fibonacci sequence up to n */
					std::vector<int> fib;
					if ( n <= 0 )
					{
						return fib;
					}

					fib.push_back( 0 );
					if ( n > 1 )
					{
						fib.push_back( 1 );
					}

					for ( int i = 2; i < n; ++i )
					{
						fib.push_back( fib[static_cast<size_t>( i - 1 )] + fib[static_cast<size_t>( i - 2 )] );
					}
					return fib;
				},
				[]( bool ) -> std::vector<int> {
					unexpectedBoolean();
					return {};
				},
				[]( std::string_view ) -> std::vector<int> {
					unexpectedString();
					return {};
				},
				[]( const datatypes::DateTimeOffset& ) -> std::vector<int> {
					unexpectedDateTime();
					return {};
				} );
		} );
		ASSERT_EQ( fibResult.size(), 5 );
		ASSERT_EQ( fibResult[0], 0 );
		ASSERT_EQ( fibResult[1], 1 );
		ASSERT_EQ( fibResult[2], 1 );
		ASSERT_EQ( fibResult[3], 2 );
		ASSERT_EQ( fibResult[4], 3 );

		/* Test empty string with optional return type */
		auto stringResult = formatDataTypes.parse( "String" );
		ASSERT_TRUE( stringResult.isOk() );
		auto stringType = stringResult.ok().typeName();

		std::optional<std::string> optionalResult;
		ASSERT_NO_THROW( {
			optionalResult = stringType.matchOn<std::optional<std::string>>(
				"",
				[]( const datatypes::Decimal128& ) -> std::optional<std::string> {
					unexpectedDecimal();
					return std::nullopt;
				},
				[]( int ) -> std::optional<std::string> {
					unexpectedInteger();
					return std::nullopt;
				},
				[]( bool ) -> std::optional<std::string> {
					unexpectedBoolean();
					return std::nullopt;
				},
				[]( std::string_view s ) -> std::optional<std::string> { return s.empty() ? std::nullopt
																						  : std::make_optional( std::string( s ) ); },
				[]( const datatypes::DateTimeOffset& ) -> std::optional<std::string> {
					unexpectedDateTime();
					return std::nullopt;
				} );
		} );
		ASSERT_FALSE( optionalResult.has_value() );
	}

	//=====================================================================
	// Test_FormatDataType_Value_Types_API
	//=====================================================================

	TEST_F( ISO19848Tests, Test_FormatDataType_Value_Types_API )
	{
		/* Test all Value types for complete API coverage */

		/* Test String Value */
		{
			transport::Value::String stringVal{ std::string_view{ "test" } };
			ASSERT_EQ( stringVal.value(), "test" );

			transport::Value value{ stringVal };
			ASSERT_TRUE( value.isString() );
			ASSERT_FALSE( value.isDecimal() );
			ASSERT_FALSE( value.isInteger() );
			ASSERT_FALSE( value.isBoolean() );
			ASSERT_FALSE( value.isDateTime() );
			ASSERT_FALSE( value.isChar() );
			ASSERT_FALSE( value.isUnsignedInteger() );
			ASSERT_FALSE( value.isLong() );
			ASSERT_FALSE( value.isDouble() );
			ASSERT_EQ( value.string().value(), "test" );
		}

		/* Test Decimal Value */
		{
			transport::Value::Decimal decimalVal{ 3.14 };
			ASSERT_EQ( decimalVal.value(), datatypes::Decimal128( 3.14 ) );

			transport::Value value{ decimalVal };
			ASSERT_FALSE( value.isString() );
			ASSERT_TRUE( value.isDecimal() );
			ASSERT_FALSE( value.isInteger() );
			ASSERT_FALSE( value.isBoolean() );
			ASSERT_FALSE( value.isDateTime() );
			ASSERT_FALSE( value.isChar() );
			ASSERT_FALSE( value.isUnsignedInteger() );
			ASSERT_FALSE( value.isLong() );
			ASSERT_FALSE( value.isDouble() );
			ASSERT_EQ( value.decimal().value(), datatypes::Decimal128( 3.14 ) );
		}

		/* Test Integer Value */
		{
			transport::Value::Integer intVal{ 42 };
			ASSERT_EQ( intVal.value(), 42 );

			transport::Value value{ intVal };
			ASSERT_FALSE( value.isString() );
			ASSERT_FALSE( value.isDecimal() );
			ASSERT_TRUE( value.isInteger() );
			ASSERT_FALSE( value.isBoolean() );
			ASSERT_FALSE( value.isDateTime() );
			ASSERT_FALSE( value.isChar() );
			ASSERT_FALSE( value.isUnsignedInteger() );
			ASSERT_FALSE( value.isLong() );
			ASSERT_FALSE( value.isDouble() );
			ASSERT_EQ( value.integer().value(), 42 );
		}

		/* Test Boolean Value */
		{
			transport::Value::Boolean boolVal{ true };
			ASSERT_EQ( boolVal.value(), true );

			transport::Value value{ boolVal };
			ASSERT_FALSE( value.isString() );
			ASSERT_FALSE( value.isDecimal() );
			ASSERT_FALSE( value.isInteger() );
			ASSERT_TRUE( value.isBoolean() );
			ASSERT_FALSE( value.isDateTime() );
			ASSERT_FALSE( value.isChar() );
			ASSERT_FALSE( value.isUnsignedInteger() );
			ASSERT_FALSE( value.isLong() );
			ASSERT_FALSE( value.isDouble() );
			ASSERT_EQ( value.boolean().value(), true );
		}

		/* Test DateTime Value */
		{
			auto dateTimeOffset = datatypes::DateTimeOffset::now();
			transport::Value::DateTime dateTimeVal{ dateTimeOffset };
			ASSERT_EQ( dateTimeVal.value(), dateTimeOffset );

			transport::Value value{ dateTimeVal };
			ASSERT_FALSE( value.isString() );
			ASSERT_FALSE( value.isDecimal() );
			ASSERT_FALSE( value.isInteger() );
			ASSERT_FALSE( value.isBoolean() );
			ASSERT_TRUE( value.isDateTime() );
			ASSERT_FALSE( value.isChar() );
			ASSERT_FALSE( value.isUnsignedInteger() );
			ASSERT_FALSE( value.isLong() );
			ASSERT_FALSE( value.isDouble() );
			ASSERT_EQ( value.dateTime().value(), dateTimeOffset );
		}

		/* Test Char Value */
		{
			transport::Value::Char charVal{ 'X' };
			ASSERT_EQ( charVal.value(), 'X' );

			transport::Value value{ charVal };
			ASSERT_FALSE( value.isString() );
			ASSERT_FALSE( value.isDecimal() );
			ASSERT_FALSE( value.isInteger() );
			ASSERT_FALSE( value.isBoolean() );
			ASSERT_FALSE( value.isDateTime() );
			ASSERT_TRUE( value.isChar() );
			ASSERT_FALSE( value.isUnsignedInteger() );
			ASSERT_FALSE( value.isLong() );
			ASSERT_FALSE( value.isDouble() );
			ASSERT_EQ( value.charValue().value(), 'X' );
		}

		/* Test UnsignedInteger Value */
		{
			transport::Value::UnsignedInteger uintVal{ 42u };
			ASSERT_EQ( uintVal.value(), 42u );

			transport::Value value{ uintVal };
			ASSERT_FALSE( value.isString() );
			ASSERT_FALSE( value.isDecimal() );
			ASSERT_FALSE( value.isInteger() );
			ASSERT_FALSE( value.isBoolean() );
			ASSERT_FALSE( value.isDateTime() );
			ASSERT_FALSE( value.isChar() );
			ASSERT_TRUE( value.isUnsignedInteger() );
			ASSERT_FALSE( value.isLong() );
			ASSERT_FALSE( value.isDouble() );
			ASSERT_EQ( value.unsignedInteger().value(), 42u );
		}

		/* Test Long Value */
		{
			transport::Value::Long longVal{ 9223372036854775807LL };
			ASSERT_EQ( longVal.value(), 9223372036854775807LL );

			transport::Value value{ longVal };
			ASSERT_FALSE( value.isString() );
			ASSERT_FALSE( value.isDecimal() );
			ASSERT_FALSE( value.isInteger() );
			ASSERT_FALSE( value.isBoolean() );
			ASSERT_FALSE( value.isDateTime() );
			ASSERT_FALSE( value.isChar() );
			ASSERT_FALSE( value.isUnsignedInteger() );
			ASSERT_TRUE( value.isLong() );
			ASSERT_FALSE( value.isDouble() );
			ASSERT_EQ( value.longValue().value(), 9223372036854775807LL );
		}

		/* Test Double Value */
		{
			transport::Value::Double doubleVal{ 2.71828 };
			ASSERT_EQ( doubleVal.value(), 2.71828 );

			transport::Value value{ doubleVal };
			ASSERT_FALSE( value.isString() );
			ASSERT_FALSE( value.isDecimal() );
			ASSERT_FALSE( value.isInteger() );
			ASSERT_FALSE( value.isBoolean() );
			ASSERT_FALSE( value.isDateTime() );
			ASSERT_FALSE( value.isChar() );
			ASSERT_FALSE( value.isUnsignedInteger() );
			ASSERT_FALSE( value.isLong() );
			ASSERT_TRUE( value.isDouble() );
			ASSERT_EQ( value.doubleValue().value(), 2.71828 );
		}
	}
}
