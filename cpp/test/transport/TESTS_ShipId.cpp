/**
 * @file TESTS_ShipId.cpp
 * @brief Comprehensive tests for ShipId class
 */

#include <gtest/gtest.h>

#include "dnv/vista/sdk/transport/ShipId.h"
#include "dnv/vista/sdk/ImoNumber.h"

namespace dnv::vista::sdk::tests
{
	//=====================================================================
	// ShipId Construction Tests
	//=====================================================================

	TEST( ShipId, ConstructFromImoNumber )
	{
		auto imo = ImoNumber::parse( "IMO1234567" );
		transport::ShipId shipId{ imo };

		EXPECT_TRUE( shipId.isImoNumber() );
		EXPECT_FALSE( shipId.isOtherId() );

		auto retrievedImo = shipId.imoNumber();
		ASSERT_TRUE( retrievedImo.has_value() );
		EXPECT_EQ( retrievedImo.value(), imo );

		auto otherId = shipId.otherId();
		EXPECT_FALSE( otherId.has_value() );
	}

	TEST( ShipId, ConstructFromStringView )
	{
		std::string_view testId = "ABC123";
		transport::ShipId shipId{ testId };

		EXPECT_FALSE( shipId.isImoNumber() );
		EXPECT_TRUE( shipId.isOtherId() );

		auto retrievedOtherId = shipId.otherId();
		ASSERT_TRUE( retrievedOtherId.has_value() );
		EXPECT_EQ( retrievedOtherId.value(), testId );

		auto imo = shipId.imoNumber();
		EXPECT_FALSE( imo.has_value() );
	}

	TEST( ShipId, ConstructFromEmptyStringThrows )
	{
		EXPECT_THROW( transport::ShipId{ "" }, std::invalid_argument );
		EXPECT_THROW( transport::ShipId{ std::string_view{} }, std::invalid_argument );
	}

	//=====================================================================
	// ShipId Equality Tests
	//=====================================================================

	TEST( ShipId, EqualityImoNumbers )
	{
		auto imo1 = ImoNumber::parse( "IMO1234567" );
		auto imo2 = ImoNumber::parse( "IMO1234567" );

		transport::ShipId shipId1{ imo1 };
		transport::ShipId shipId2{ imo2 };

		EXPECT_EQ( shipId1, shipId2 );
		EXPECT_FALSE( shipId1 != shipId2 );
	}

	TEST( ShipId, EqualityOtherIds )
	{
		transport::ShipId shipId1{ "ABC123" };
		transport::ShipId shipId2{ "ABC123" };

		EXPECT_EQ( shipId1, shipId2 );
		EXPECT_FALSE( shipId1 != shipId2 );
	}

	TEST( ShipId, InequalityDifferentTypes )
	{
		auto imo = ImoNumber::parse( "IMO1234567" );
		transport::ShipId imoShipId{ imo };
		transport::ShipId otherShipId{ "ABC123" };

		EXPECT_NE( imoShipId, otherShipId );
		EXPECT_TRUE( imoShipId != otherShipId );
	}

	TEST( ShipId, InequalityDifferentValues )
	{
		transport::ShipId shipId1{ "ABC123" };
		transport::ShipId shipId2{ "XYZ789" };

		EXPECT_NE( shipId1, shipId2 );
		EXPECT_TRUE( shipId1 != shipId2 );
	}

	//=====================================================================
	// ShipId ToString Tests
	//=====================================================================

	TEST( ShipId, ToStringImoNumber )
	{
		auto imo = ImoNumber::parse( "IMO1234567" );
		transport::ShipId shipId{ imo };

		std::string result = shipId.toString();
		EXPECT_EQ( result, "IMO1234567" );
	}

	TEST( ShipId, ToStringOtherId )
	{
		transport::ShipId shipId{ "ABC123" };

		std::string result = shipId.toString();
		EXPECT_EQ( result, "ABC123" );
	}

	//=====================================================================
	// ShipId Parse Tests
	//=====================================================================

	TEST( ShipId, ParseValidImoNumber )
	{
		auto shipId = transport::ShipId::parse( "IMO1234567" );

		EXPECT_TRUE( shipId.isImoNumber() );
		EXPECT_FALSE( shipId.isOtherId() );

		auto imo = shipId.imoNumber();
		ASSERT_TRUE( imo.has_value() );
		EXPECT_EQ( imo.value().toString(), "IMO1234567" );
	}

	TEST( ShipId, ParseCaseInsensitiveImo )
	{
		auto shipId1 = transport::ShipId::parse( "IMO1234567" );
		auto shipId2 = transport::ShipId::parse( "imo1234567" );
		auto shipId3 = transport::ShipId::parse( "Imo1234567" );

		EXPECT_TRUE( shipId1.isImoNumber() );
		EXPECT_TRUE( shipId2.isImoNumber() );
		EXPECT_TRUE( shipId3.isImoNumber() );
	}

	TEST( ShipId, ParseOtherId )
	{
		auto shipId = transport::ShipId::parse( "ABC123" );

		EXPECT_FALSE( shipId.isImoNumber() );
		EXPECT_TRUE( shipId.isOtherId() );

		auto otherId = shipId.otherId();
		ASSERT_TRUE( otherId.has_value() );
		EXPECT_EQ( otherId.value(), "ABC123" );
	}

	TEST( ShipId, ParseInvalidImoBecomesOtherId )
	{
		auto shipId = transport::ShipId::parse( "IMO123" );

		EXPECT_FALSE( shipId.isImoNumber() );
		EXPECT_TRUE( shipId.isOtherId() );

		auto otherId = shipId.otherId();
		ASSERT_TRUE( otherId.has_value() );
		EXPECT_EQ( otherId.value(), "IMO123" );
	}

	TEST( ShipId, ParseEmptyStringThrows )
	{
		EXPECT_THROW( transport::ShipId::parse( "" ), std::invalid_argument );
		EXPECT_THROW( transport::ShipId::parse( std::string_view{} ), std::invalid_argument );
	}

	//=====================================================================
	// ShipId Pattern Matching Tests
	//=====================================================================

	TEST( ShipId, MatchOnImoNumber )
	{
		auto imo = ImoNumber::parse( "IMO1234567" );
		transport::ShipId shipId{ imo };

		std::string result = shipId.matchOn<std::string>(
			[]( const ImoNumber& imoNum ) { return "IMO: " + imoNum.toString(); },
			[]( std::string_view otherId ) { return "Other: " + std::string{ otherId }; } );

		EXPECT_EQ( result, "IMO: IMO1234567" );
	}

	TEST( ShipId, MatchOnOtherId )
	{
		transport::ShipId shipId{ "ABC123" };

		std::string result = shipId.matchOn<std::string>(
			[]( const ImoNumber& imoNum ) { return "IMO: " + imoNum.toString(); },
			[]( std::string_view otherId ) { return "Other: " + std::string{ otherId }; } );

		EXPECT_EQ( result, "Other: ABC123" );
	}

	TEST( ShipId, SwitchOnImoNumber )
	{
		auto imo = ImoNumber::parse( "IMO1234567" );
		transport::ShipId shipId{ imo };

		std::string result;
		shipId.switchOn(
			[&result]( const ImoNumber& imoNum ) { result = "IMO: " + imoNum.toString(); },
			[&result]( std::string_view otherId ) { result = "Other: " + std::string{ otherId }; } );

		EXPECT_EQ( result, "IMO: IMO1234567" );
	}

	TEST( ShipId, SwitchOnOtherId )
	{
		transport::ShipId shipId{ "ABC123" };

		std::string result;
		shipId.switchOn(
			[&result]( const ImoNumber& imoNum ) { result = "IMO: " + imoNum.toString(); },
			[&result]( std::string_view otherId ) { result = "Other: " + std::string{ otherId }; } );

		EXPECT_EQ( result, "Other: ABC123" );
	}

	//=====================================================================
	// ShipId Copy/Move Tests
	//=====================================================================

	TEST( ShipId, CopyConstructor )
	{
		transport::ShipId original{ "ABC123" };
		transport::ShipId copy{ original };

		EXPECT_EQ( original, copy );
		EXPECT_TRUE( copy.isOtherId() );
	}

	TEST( ShipId, MoveConstructor )
	{
		transport::ShipId original{ "ABC123" };
		transport::ShipId moved{ std::move( original ) };

		EXPECT_TRUE( moved.isOtherId() );
		auto otherId = moved.otherId();
		ASSERT_TRUE( otherId.has_value() );
		EXPECT_EQ( otherId.value(), "ABC123" );
	}

	TEST( ShipId, CopyAssignment )
	{
		transport::ShipId original{ "ABC123" };
		auto imo = ImoNumber::parse( "IMO1234567" );
		transport::ShipId target{ imo };

		target = original;

		EXPECT_EQ( original, target );
		EXPECT_TRUE( target.isOtherId() );
	}

	TEST( ShipId, MoveAssignment )
	{
		transport::ShipId original{ "ABC123" };
		auto imo = ImoNumber::parse( "IMO1234567" );
		transport::ShipId target{ imo };

		target = std::move( original );

		EXPECT_TRUE( target.isOtherId() );
		auto otherId = target.otherId();
		ASSERT_TRUE( otherId.has_value() );
		EXPECT_EQ( otherId.value(), "ABC123" );
	}
}
