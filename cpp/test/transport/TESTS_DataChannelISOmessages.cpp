/**
 * @file TESTS_DataChannelISOmessages.cpp
 * @brief Comprehensive tests for DataChannel infrastructure
 */

#include "dnv/vista/sdk/datatypes/DateTimeISO8601.h"
#include "dnv/vista/sdk/transport/DataChannel/DataChannel.h"
#include "dnv/vista/sdk/transport/ShipId.h"
#include "dnv/vista/sdk/LocalId.h"
#include "dnv/vista/sdk/LocalIdBuilder.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk::tests
{
	//=====================================================================
	// Helper functions for creating test data
	//=====================================================================

	/**
	 * @brief Creates ValidFullyCustomDataChannelList
	 */
	transport::datachannel::DataChannelListPackage createValidFullyCustomDataChannelList()
	{
		/* Create ConfigurationReference for DataChannelListId */
		auto timeStamp = datatypes::DateTimeOffset::parse( "2016-01-01T00:00:00Z" );
		transport::datachannel::ConfigurationReference dataChannelListId{
			"DataChannelList.xml",
			timeStamp,
			"1.0" };

		/* Create Header */
		transport::datachannel::Header header{
			transport::ShipId::parse( "IMO1234567" ),
			dataChannelListId,
			"Author1" };

		transport::datachannel::VersionInformation versionInfo;
		versionInfo.setNamingRule( "some_naming_rule" );
		versionInfo.setNamingSchemeVersion( "2.0" );
		versionInfo.setReferenceUrl( "http://somewhere.net" );
		header.setVersionInformation( versionInfo );

		header.setDateCreated( datatypes::DateTimeOffset::parse( "2015-12-01T00:00:00Z" ) );

		/* Add custom headers */
		internal::StringMap<transport::Value> customHeaders;
		customHeaders["nr:CustomHeaderElement"] = transport::Value::String{ "Vendor specific headers" };
		header.setCustomHeaders( customHeaders );

		/* Create DataChannelList */
		transport::datachannel::DataChannelList dataChannelList;

		/* First DataChannel - Temperature sensor */
		{
			std::optional<LocalIdBuilder> localIdBuilderOpt;
			bool parsed = LocalIdBuilder::tryParse(
				"/dnv-v2/vis-3-4a/411.1-1/C101.63/S206/meta/qty-temperature/cnt-cooling.air",
				localIdBuilderOpt );
			EXPECT_TRUE( parsed );
			auto localId = localIdBuilderOpt->build();

			transport::datachannel::DataChannelId dataChannelId{ localId, "0010" };

			transport::datachannel::NameObject nameObject;
			nameObject.setNamingRule( "Naming_Rule" );

			/* Add custom name objects */
			internal::StringMap<transport::Value> customNameObjects;
			customNameObjects["nr:CustomNameObject"] = transport::Value::String{ "Vendor specific NameObject" };
			nameObject.setCustomNameObjects( customNameObjects );

			dataChannelId.setNameObject( nameObject );

			transport::datachannel::DataChannelType dataChannelType{ "Inst" };
			dataChannelType.setUpdateCycle( 1.0 );

			transport::datachannel::Format format{ "Decimal" };
			transport::datachannel::Restriction restriction;
			restriction.setFractionDigits( 1 );
			restriction.setMaxInclusive( 200.0 );
			restriction.setMinInclusive( -150.0 );
			format.setRestriction( restriction );

			transport::datachannel::Range range{ 0.0, 150.0 };

			transport::datachannel::Unit unit{ "°C" };
			unit.setQuantityName( "Temperature" );

			/* Add custom elements to unit */
			internal::StringMap<transport::Value> customElements;
			customElements["nr:CustomUnitElement"] = transport::Value::String{ "Vendor specific unit element" };
			unit.setCustomElements( customElements );

			transport::datachannel::Property property{ dataChannelType, format, range, unit, std::nullopt };
			property.setQualityCoding( "OPC_QUALITY" );
			property.setName( "M/E #1 Air Cooler CFW OUT Temp" );
			property.setRemarks( " Location: ECR, Manufacturer: AAA Company, Type: TYPE-AAA " );

			/* Add custom properties */
			internal::StringMap<transport::Value> customProperties;
			customProperties["nr:CustomPropertyElement"] = transport::Value::String{ "Vendor specific property element" };
			property.setCustomProperties( customProperties );

			transport::datachannel::DataChannel dataChannel{ dataChannelId, property };
			dataChannelList.add( dataChannel );
		}

		/* Second DataChannel - Alert type */
		{
			std::optional<LocalIdBuilder> localIdBuilderOpt;
			bool parsed = LocalIdBuilder::tryParse(
				"/dnv-v2/vis-3-4a/511.15-1/E32/meta/qty-power",
				localIdBuilderOpt );
			EXPECT_TRUE( parsed );
			auto localId = localIdBuilderOpt->build();

			transport::datachannel::DataChannelId dataChannelId{ localId, "0020" };

			transport::datachannel::DataChannelType dataChannelType{ "Alert" };

			transport::datachannel::Format format{ "String" };
			transport::datachannel::Restriction restriction;
			restriction.setMaxLength( 100 );
			restriction.setMinLength( 0 );
			format.setRestriction( restriction );

			transport::datachannel::Property property{ dataChannelType, format, std::nullopt, std::nullopt, "Warning" };

			transport::datachannel::DataChannel dataChannel{ dataChannelId, property };
			dataChannelList.add( dataChannel );
		}

		transport::datachannel::Package package{ header, std::move( dataChannelList ) };
		return transport::datachannel::DataChannelListPackage{ std::move( package ) };
	}

	/**
	 * @brief Creates ValidDataChannelList
	 */
	transport::datachannel::DataChannelListPackage createValidDataChannelList()
	{
		/* Create ConfigurationReference for DataChannelListId */
		transport::datachannel::ConfigurationReference dataChannelListId{
			"some-id",
			datatypes::DateTimeOffset( "2016-01-01T00:00:00Z" ) };

		/* Create Header */
		transport::datachannel::Header header{
			transport::ShipId::parse( "IMO1234567" ),
			dataChannelListId,
			"some-author" };

		/* Create DataChannelList */
		transport::datachannel::DataChannelList dataChannelList;

		/* Single DataChannel */
		{
			std::optional<LocalIdBuilder> localIdBuilderOpt;
			bool parsed = LocalIdBuilder::tryParse(
				"/dnv-v2/vis-3-4a/511.15-1/E32/meta/qty-power",
				localIdBuilderOpt );
			EXPECT_TRUE( parsed );
			auto localId = localIdBuilderOpt->build();

			transport::datachannel::DataChannelId dataChannelId{ localId, "0010" };

			transport::datachannel::DataChannelType dataChannelType{ "Inst" };
			transport::datachannel::Format format{ "String" };

			transport::datachannel::Property property{ dataChannelType, format, std::nullopt, std::nullopt, std::nullopt };

			transport::datachannel::DataChannel dataChannel{ dataChannelId, property };
			dataChannelList.add( dataChannel );
		}

		transport::datachannel::Package package{ header, std::move( dataChannelList ) };
		return transport::datachannel::DataChannelListPackage{ std::move( package ) };
	}

	//=====================================================================
	// Test methods
	//=====================================================================

	TEST( IsoMessageTests, Test_DataChannelList )
	{
		auto message = createValidDataChannelList();
		EXPECT_FALSE( message.package().dataChannelList().isEmpty() );

		auto& first = message.package().dataChannelList().dataChannels().front();
		EXPECT_TRUE( first.dataChannelId().localId().builder().isValid() );

		EXPECT_NO_THROW( first.property() );
	}

	TEST( IsoMessageTests, Test_LocalId_Lookup )
	{
		auto message = createValidDataChannelList();
		const auto& dataChannel = message.package().dataChannelList()[0];
		const auto& localId = dataChannel.dataChannelId().localId();
		const auto& lookup = message.package().dataChannelList()[localId];
		const auto* lookup2 = message.package().dataChannelList().tryGetByLocalId( localId );
		EXPECT_NE( lookup2, nullptr );
		EXPECT_EQ( &dataChannel, &lookup );
		EXPECT_EQ( &dataChannel, lookup2 );
	}

	TEST( IsoMessageTests, Test_ShortId_Lookup )
	{
		auto message = createValidDataChannelList();
		const auto& dataChannel = message.package().dataChannelList()[0];
		const auto& shortId = dataChannel.dataChannelId().shortId();
		EXPECT_TRUE( shortId.has_value() );
		const auto& lookup = message.package().dataChannelList()[*shortId];
		const auto* lookup2 = message.package().dataChannelList().tryGetByShortId( *shortId );
		EXPECT_NE( lookup2, nullptr );
		EXPECT_EQ( &dataChannel, &lookup );
		EXPECT_EQ( &dataChannel, lookup2 );
	}

	TEST( IsoMessageTests, Test_DataChannelList_Enumerator )
	{
		auto message = createValidDataChannelList();
		const auto expectedLength = message.package().dataChannelList().dataChannels().size();
		const auto actualLength = message.package().dataChannelList().size();
		size_t counter = 0;
		EXPECT_EQ( expectedLength, actualLength );

		/* Test range-based for loop iteration */
		for ( const auto& dc : message.package().dataChannelList() )
		{
			EXPECT_NE( &dc, nullptr );
			counter++;
		}
		EXPECT_EQ( expectedLength, counter );

		/* Test iterator-based iteration */
		counter = 0;
		for ( auto it = message.package().dataChannelList().begin(); it != message.package().dataChannelList().end(); ++it )
		{
			EXPECT_NE( &( *it ), nullptr );
			counter++;
		}
		EXPECT_EQ( expectedLength, counter );
	}

	//=====================================================================
	// JSON Serialization Tests
	//=====================================================================

	/**
	 * @brief Test DataChannelList JSON serialization
	 * @details Tests JSON round-trip serialization
	 */
	TEST( IsoMessageTests, DataChannelListJson )
	{
		/* TODO */
	}

	//=====================================================================
	// Advanced DataChannel Tests
	//=====================================================================

	/**
	 * @brief Test fully populated DataChannel with all properties
	 * @details Tests complex DataChannel with all optional fields populated
	 */
	TEST( DataChannelTests, Test_FullyCustomDataChannel )
	{
		auto dataChannelList = createValidFullyCustomDataChannelList();

		EXPECT_EQ( dataChannelList.package().dataChannelList().size(), 2U );

		/* Test custom headers */
		const auto& header = dataChannelList.package().header();
		EXPECT_TRUE( header.customHeaders().has_value() );
		const auto& customHeaders = *header.customHeaders();
		EXPECT_TRUE( customHeaders.contains( "nr:CustomHeaderElement" ) );
		const auto& headerValue = customHeaders.at( "nr:CustomHeaderElement" );
		EXPECT_EQ( headerValue.type(), transport::Value::Type::String );
		EXPECT_EQ( headerValue.string().value(), "Vendor specific headers" );

		/* Test first DataChannel (temperature sensor) */
		const auto& tempChannel = dataChannelList.package().dataChannelList()[0];
		const auto& tempProperty = tempChannel.property();

		EXPECT_EQ( tempProperty.dataChannelType().type(), "Inst" );
		EXPECT_TRUE( tempProperty.dataChannelType().updateCycle().has_value() );
		EXPECT_EQ( *tempProperty.dataChannelType().updateCycle(), 1.0 );

		EXPECT_EQ( tempProperty.format().type(), "Decimal" );
		EXPECT_TRUE( tempProperty.format().restriction().has_value() );
		const auto& restriction = *tempProperty.format().restriction();
		EXPECT_TRUE( restriction.fractionDigits().has_value() );
		EXPECT_EQ( *restriction.fractionDigits(), 1U );
		EXPECT_TRUE( restriction.maxInclusive().has_value() );
		EXPECT_EQ( *restriction.maxInclusive(), 200.0 );
		EXPECT_TRUE( restriction.minInclusive().has_value() );
		EXPECT_EQ( *restriction.minInclusive(), -150.0 );

		EXPECT_TRUE( tempProperty.range().has_value() );
		const auto& range = *tempProperty.range();
		EXPECT_EQ( range.low(), 0.0 );
		EXPECT_EQ( range.high(), 150.0 );

		EXPECT_TRUE( tempProperty.unit().has_value() );
		const auto& unit = *tempProperty.unit();
		EXPECT_EQ( unit.unitSymbol(), "°C" );
		EXPECT_TRUE( unit.quantityName().has_value() );
		EXPECT_EQ( *unit.quantityName(), "Temperature" );

		EXPECT_TRUE( tempProperty.qualityCoding().has_value() );
		EXPECT_EQ( *tempProperty.qualityCoding(), "OPC_QUALITY" );

		EXPECT_TRUE( tempProperty.name().has_value() );
		EXPECT_EQ( *tempProperty.name(), "M/E #1 Air Cooler CFW OUT Temp" );

		EXPECT_TRUE( tempProperty.remarks().has_value() );
		EXPECT_EQ( *tempProperty.remarks(), " Location: ECR, Manufacturer: AAA Company, Type: TYPE-AAA " );

		/* Test second DataChannel (alert type) */
		const auto& alertChannel = dataChannelList.package().dataChannelList()[1];
		const auto& alertProperty = alertChannel.property();

		EXPECT_EQ( alertProperty.dataChannelType().type(), "Alert" );
		EXPECT_EQ( alertProperty.format().type(), "String" );
		EXPECT_TRUE( alertProperty.alertPriority().has_value() );
		EXPECT_EQ( *alertProperty.alertPriority(), "Warning" );

		EXPECT_TRUE( alertProperty.format().restriction().has_value() );
		const auto& stringRestriction = *alertProperty.format().restriction();
		EXPECT_TRUE( stringRestriction.maxLength().has_value() );
		EXPECT_EQ( *stringRestriction.maxLength(), 100U );
		EXPECT_TRUE( stringRestriction.minLength().has_value() );
		EXPECT_EQ( *stringRestriction.minLength(), 0U );
	}

	//=====================================================================
	// Validation Tests
	//=====================================================================

	/**
	 * @brief Test Property validation rules
	 * @details Tests business rule validation in Property class
	 */
	TEST( DataChannelTests, Test_Property_Validation )
	{
		/* Test valid decimal property */
		{
			transport::datachannel::DataChannelType dataChannelType{ "Inst" };
			transport::datachannel::Format format{ "Decimal" };
			transport::datachannel::Range range{ 0.0, 100.0 };
			transport::datachannel::Unit unit{ "°C" };

			transport::datachannel::Property property{ dataChannelType, format, range, unit, std::nullopt };
			auto result = property.validate();
			EXPECT_TRUE( result.isOk() );
		}

		/* Test alert property with priority */
		{
			transport::datachannel::DataChannelType dataChannelType{ "Alert" };
			transport::datachannel::Format format{ "String" };

			transport::datachannel::Property property{ dataChannelType, format, std::nullopt, std::nullopt, "Critical" };
			auto result = property.validate();
			EXPECT_TRUE( result.isOk() );
		}
	}

	/**
	 * @brief Test Restriction validation functionality
	 * @details Tests various restriction validation scenarios
	 */
	TEST( DataChannelTests, Test_Restriction_Validation )
	{
		transport::datachannel::Restriction restriction;
		transport::datachannel::Format format{ "String" };

		/* Test length validation */
		restriction.setLength( 5 );
		auto result = restriction.validateValue( "hello", format );
		EXPECT_TRUE( result.isOk() );

		result = restriction.validateValue( "hi", format );
		EXPECT_FALSE( result.isOk() );

		result = restriction.validateValue( "toolong", format );
		EXPECT_FALSE( result.isOk() );

		/* Test min/max length validation */
		restriction = transport::datachannel::Restriction{};
		restriction.setMinLength( 2 );
		restriction.setMaxLength( 10 );

		result = restriction.validateValue( "ok", format );
		EXPECT_TRUE( result.isOk() );

		result = restriction.validateValue( "x", format );
		EXPECT_FALSE( result.isOk() );

		result = restriction.validateValue( "thisstringistoolong", format );
		EXPECT_FALSE( result.isOk() );
	}

	/**
	 * @brief Test Range validation and construction
	 * @details Tests Range class validation rules
	 */
	TEST( RangeTest, RangeCreationAndValidation )
	{
		/* Test valid range construction */
		transport::datachannel::Range range{ 0.0, 100.0 };
		EXPECT_EQ( range.low(), 0.0 );
		EXPECT_EQ( range.high(), 100.0 );

		/* Test invalid range construction should throw */
		EXPECT_THROW( transport::datachannel::Range( 100.0, 0.0 ), std::invalid_argument );

		/* Test setting range values */
		range.setLow( 10.0 );
		EXPECT_EQ( range.low(), 10.0 );

		range.setHigh( 90.0 );
		EXPECT_EQ( range.high(), 90.0 );

		/* low > high */
		EXPECT_THROW( range.setLow( 200.0 ), std::invalid_argument );
		/*high < low*/
		EXPECT_THROW( range.setHigh( -10.0 ), std::invalid_argument );
	}

	/**
	 * @brief Test DataChannelList collection operations
	 * @details Tests add, remove, and lookup operations
	 */
	TEST( DataChannelTests, Test_DataChannelList_Operations )
	{
		transport::datachannel::DataChannelList dataChannelList;
		EXPECT_TRUE( dataChannelList.isEmpty() );
		EXPECT_EQ( dataChannelList.size(), 0U );

		/* Add single DataChannel */
		auto singleList = createValidDataChannelList();
		const auto& originalChannel = singleList.package().dataChannelList()[0];

		/* Create a copy for adding */
		transport::datachannel::DataChannel channelCopy = originalChannel;
		dataChannelList.add( channelCopy );

		EXPECT_FALSE( dataChannelList.isEmpty() );
		EXPECT_EQ( dataChannelList.size(), 1U );

		/* Test lookups work */
		const auto& localId = originalChannel.dataChannelId().localId();
		const auto* foundChannel = dataChannelList.tryGetByLocalId( localId );
		EXPECT_NE( foundChannel, nullptr );

		/* Test clear */
		dataChannelList.clear();
		EXPECT_TRUE( dataChannelList.isEmpty() );
		EXPECT_EQ( dataChannelList.size(), 0U );
	}

	//=====================================================================
	// Error Handling Tests
	//=====================================================================

	/**
	 * @brief Test error scenarios and exception handling
	 * @details Tests various error conditions and proper error reporting
	 */
	TEST( DataChannelTests, Test_Error_Handling )
	{
		/* Test invalid DataChannelType construction */
		EXPECT_THROW( transport::datachannel::DataChannelType{ "InvalidType" }, std::invalid_argument );

		/* Test invalid Format type */
		EXPECT_THROW( transport::datachannel::Format{ "InvalidFormat" }, std::invalid_argument );

		/* Test duplicate LocalId addition */
		transport::datachannel::DataChannelList dataChannelList;
		auto validList = createValidDataChannelList();
		const auto& originalChannel = validList.package().dataChannelList()[0];

		/* Add first channel */
		transport::datachannel::DataChannel firstChannel = originalChannel;
		dataChannelList.add( firstChannel );

		/* Try to add duplicate LocalId - should throw */
		transport::datachannel::DataChannel duplicateChannel = originalChannel;
		EXPECT_THROW( dataChannelList.add( duplicateChannel ), std::invalid_argument );
	}

	/**
	 * @brief Test string optimization in error messages
	 */
	TEST( RestrictionTest, StringLengthRestrictionValidation )
	{
		/* Test exact length validation with StringBuilderPool error messages */
		transport::datachannel::Restriction restriction;
		restriction.setLength( 5 );

		transport::datachannel::Format format{ "String" };

		/* Test that error messages are properly constructed using StringBuilderPool */
		auto result = restriction.validateValue( "toolongstring", format );
		EXPECT_FALSE( result.isOk() );

		const auto& errorResult = result.invalid();
		EXPECT_FALSE( errorResult.errors().empty() );
		EXPECT_TRUE( errorResult.errors()[0].find( "exactly 5 characters" ) != std::string::npos );
	}
}
