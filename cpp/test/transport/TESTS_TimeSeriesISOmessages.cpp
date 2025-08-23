/**
 * @file TESTS_TimeSeriesISOmessages.cpp
 * @brief Comprehensive tests for TimeSeries infrastructure
 */

#include "dnv/vista/sdk/datatypes/DateTimeISO8601.h"
#include "dnv/vista/sdk/transport/TimeSeriesData/TimeSeriesData.h"
#include "dnv/vista/sdk/transport/TimeSeriesData/DataChannelId.h"
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

			transport::datachannel::Property property{ dataChannelType, format, range, unit, std::nullopt };
			property.setQualityCoding( "OPC_QUALITY" );
			property.setName( "M/E #1 Air Cooler CFW OUT Temp" );
			property.setRemarks( " Location: ECR, Manufacturer: AAA Company, Type: TYPE-AAA " );

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
	 * @brief Creates TestTimeSeriesDataPackage
	 */
	transport::timeseries::TimeSeriesDataPackage createTestTimeSeriesDataPackage()
	{
		auto testDataChannelListPackage = createValidFullyCustomDataChannelList();

		/* Create data channel IDs from test package */
		std::vector<transport::DataChannelId> allDataChannelIds;
		for ( const auto& dc : testDataChannelListPackage.dataChannelList().dataChannels() )
		{
			auto dataChannelId = transport::DataChannelId::parse(
				dc.dataChannelId().shortId().has_value() ? *dc.dataChannelId().shortId() : dc.dataChannelId().localId().toString() );
			allDataChannelIds.push_back( dataChannelId );
		}
		std::vector<transport::DataChannelId> firstDataChannelId;
		firstDataChannelId.push_back( allDataChannelIds[0] );

		/* Create timestamps */
		auto start = datatypes::DateTimeOffset::parse( "2016-01-01T12:00:00Z" );
		auto end = datatypes::DateTimeOffset::parse( "2016-01-03T12:00:00Z" );
		auto timestamp1 = datatypes::DateTimeOffset::parse( "2016-01-01T12:00:00Z" );
		auto timestamp2 = datatypes::DateTimeOffset::parse( "2016-01-02T12:00:00Z" );
		auto timestamp3 = datatypes::DateTimeOffset::parse( "2016-01-03T12:00:00Z" );
		auto eventTimestamp = datatypes::DateTimeOffset::parse( "2016-01-01T12:00:01Z" );
		auto createdModified = datatypes::DateTimeOffset::parse( "2016-01-03T12:00:00Z" );
		auto configTimestamp = datatypes::DateTimeOffset::parse( "2016-01-03T00:00:00Z" );

		/* Create first tabular data */
		std::vector<transport::timeseries::TabularDataSet> dataSets1 = {
			transport::timeseries::TabularDataSet(
				timestamp1,
				std::vector<std::string>{ "100.0", "200.0" },
				std::vector<std::string>{ "0", "0" } ),
			transport::timeseries::TabularDataSet(
				timestamp2,
				std::vector<std::string>{ "105.0", "210.0" },
				std::vector<std::string>{ "0", "0" } ) };

		transport::timeseries::TabularData tabularData1( allDataChannelIds, dataSets1 );

		/* Create second tabular data */
		std::vector<transport::timeseries::TabularDataSet> dataSets2 = {
			transport::timeseries::TabularDataSet(
				timestamp1,
				std::vector<std::string>{ "100.0" },
				std::nullopt ),
			transport::timeseries::TabularDataSet(
				timestamp2,
				std::vector<std::string>{ "100.1" },
				std::nullopt ),
			transport::timeseries::TabularDataSet(
				timestamp3,
				std::vector<std::string>{ "100.2" },
				std::nullopt ) };

		transport::timeseries::TabularData tabularData2( firstDataChannelId, dataSets2 );

		/* Create event data */
		std::vector<transport::timeseries::EventDataSet> eventDataSets = {
			transport::timeseries::EventDataSet(
				eventTimestamp,
				allDataChannelIds[1],
				"HIGH",
				std::string( "0" ) ),
			transport::timeseries::EventDataSet(
				eventTimestamp,
				allDataChannelIds[1],
				"LOW",
				std::string( "0" ) ),
			transport::timeseries::EventDataSet(
				eventTimestamp,
				allDataChannelIds[1],
				"AVERAGE",
				std::string( "0" ) ) };

		transport::timeseries::EventData eventData( eventDataSets );

		/* Create configuration reference */
		transport::timeseries::ConfigurationReference dataConfig(
			"DataChannelList.xml",
			datatypes::DateTimeOffset::parse( "2016-01-01T00:00:00Z" ) );

		/* Create system configuration references */
		std::vector<transport::timeseries::ConfigurationReference> systemConfigs = {
			transport::timeseries::ConfigurationReference(
				"SystemConfiguration.xml",
				configTimestamp ),
			transport::timeseries::ConfigurationReference(
				"SystemConfiguration.xml",
				configTimestamp ) };

		/* Create time range */
		transport::timeseries::TimeRange timeRange( start, end );

		/* Create first time series data */
		transport::timeseries::TimeSeriesData timeSeriesData1(
			dataConfig,
			std::vector<transport::timeseries::TabularData>{ tabularData1, tabularData2 },
			eventData,
			std::nullopt );

		/* Create second time series data */
		transport::timeseries::TimeSeriesData timeSeriesData2(
			dataConfig,
			std::vector<transport::timeseries::TabularData>{ tabularData1, tabularData2 },
			eventData,
			std::nullopt );

		/* Create header */
		transport::timeseries::Header header(
			transport::ShipId::parse( "IMO1234567" ),
			timeRange,
			createdModified,
			createdModified,
			std::string( "Shipboard data server" ),
			systemConfigs,
			std::nullopt );

		/* Create package */
		transport::timeseries::Package package(
			header,
			std::vector<transport::timeseries::TimeSeriesData>{ timeSeriesData1, timeSeriesData2 } );

		return transport::timeseries::TimeSeriesDataPackage( package );
	}

	//=====================================================================
	// Test methods
	//=====================================================================

	TEST( IsoMessageTests, Test_TimeSeriesData )
	{
		auto message = createTestTimeSeriesDataPackage();
		EXPECT_FALSE( message.package().timeSeriesData().empty() );
	}

	TEST( IsoMessageTests, Test_TimeSeriesData_Validation )
	{
		auto message = createTestTimeSeriesDataPackage();

		for ( const auto& tsData : message.package().timeSeriesData() )
		{
			EXPECT_TRUE( tsData.tabularData().has_value() );
			if ( tsData.tabularData().has_value() )
			{
				const auto& tabularDataList = *tsData.tabularData();
				EXPECT_FALSE( tabularDataList.empty() );

				for ( const auto& tabularData : tabularDataList )
				{
					EXPECT_FALSE( tabularData.dataChannelIds().empty() );
					EXPECT_FALSE( tabularData.dataSets().empty() );
				}
			}

			EXPECT_TRUE( tsData.eventData().has_value() );
			if ( tsData.eventData().has_value() )
			{
				const auto& eventData = *tsData.eventData();
				EXPECT_FALSE( eventData.dataSet().empty() );
			}
		}
	}

	//=====================================================================
	// JSON Serialization Tests
	//=====================================================================

	/**
	 * @brief Test TimeSeriesData JSON serialization
	 * @details Tests JSON round-trip serialization
	 */
	TEST( IsoMessageTests, TimeSeriesDataJson )
	{
		/* TODO */
	}

	//=====================================================================
	// Advanced TimeSeries Tests
	//=====================================================================

	/**
	 * @brief Test TimeRange validation and construction
	 * @details Tests TimeRange class validation rules similar to DataChannel Range tests
	 */
	TEST( TimeSeriesTests, Test_TimeRange_Validation )
	{
		auto start = datatypes::DateTimeOffset::parse( "2016-01-01T12:00:00Z" );
		auto end = datatypes::DateTimeOffset::parse( "2016-01-03T12:00:00Z" );

		/* Test valid range construction */
		transport::timeseries::TimeRange timeRange( start, end );
		EXPECT_EQ( timeRange.start().toString(), start.toString() );
		EXPECT_EQ( timeRange.end().toString(), end.toString() );

		/* Test setting range values */
		auto newStart = datatypes::DateTimeOffset::parse( "2016-01-02T12:00:00Z" );
		timeRange.setStart( newStart );
		EXPECT_EQ( timeRange.start().toString(), newStart.toString() );

		auto newEnd = datatypes::DateTimeOffset::parse( "2016-01-04T12:00:00Z" );
		timeRange.setEnd( newEnd );
		EXPECT_EQ( timeRange.end().toString(), newEnd.toString() );

		/* Test invalid range: start > end should throw */
		auto invalidEnd = datatypes::DateTimeOffset::parse( "2016-01-01T12:00:00Z" );
		EXPECT_THROW( timeRange.setEnd( invalidEnd ), std::invalid_argument );

		auto invalidStart = datatypes::DateTimeOffset::parse( "2016-01-05T12:00:00Z" );
		EXPECT_THROW( timeRange.setStart( invalidStart ), std::invalid_argument );
	}

	/**
	 * @brief Test TabularData validation functionality
	 * @details Tests TabularData validation similar to DataChannel Property validation
	 */
	TEST( TimeSeriesTests, Test_TabularData_Validation )
	{
		/* Create valid DataChannelIds */
		std::vector<transport::DataChannelId> dataChannelIds;
		dataChannelIds.push_back( transport::DataChannelId::parse( "0010" ) );
		dataChannelIds.push_back( transport::DataChannelId::parse( "0020" ) );

		/* Create valid TabularDataSets */
		auto timestamp = datatypes::DateTimeOffset::parse( "2016-01-01T12:00:00Z" );
		std::vector<transport::timeseries::TabularDataSet> dataSets = {
			transport::timeseries::TabularDataSet(
				timestamp,
				std::vector<std::string>{ "100.0", "200.0" },
				std::vector<std::string>{ "0", "0" } ) };

		transport::timeseries::TabularData tabularData( dataChannelIds, dataSets );

		/* Test validation */
		auto result = tabularData.validate();
		EXPECT_TRUE( result.isOk() );

		/* Test static validation method */
		auto staticResult = transport::timeseries::TabularData::Validate( tabularData );
		EXPECT_TRUE( staticResult.isOk() );
	}

	/**
	 * @brief Test EventData collection operations
	 * @details Tests EventData add, clear operations similar to DataChannelList tests
	 */
	TEST( TimeSeriesTests, Test_EventData_Operations )
	{
		transport::timeseries::EventData eventData;
		EXPECT_EQ( eventData.dataSet().size(), 0U );
		EXPECT_TRUE( eventData.dataSet().empty() );

		/* Add event data sets */
		auto timestamp = datatypes::DateTimeOffset::parse( "2016-01-01T12:00:01Z" );
		auto dataChannelId = transport::DataChannelId::parse( "0010" );

		transport::timeseries::EventDataSet eventDataSet1(
			timestamp,
			dataChannelId,
			"HIGH",
			std::string( "0" ) );

		eventData.add( eventDataSet1 );
		EXPECT_EQ( eventData.dataSet().size(), 1U );
		EXPECT_FALSE( eventData.dataSet().empty() );

		transport::timeseries::EventDataSet eventDataSet2(
			timestamp,
			dataChannelId,
			"LOW",
			std::string( "0" ) );

		eventData.add( eventDataSet2 );
		EXPECT_EQ( eventData.dataSet().size(), 2U );

		/* Test clear */
		eventData.clear();
		EXPECT_EQ( eventData.dataSet().size(), 0U );
		EXPECT_TRUE( eventData.dataSet().empty() );
	}

	//=====================================================================
	// Validation Tests
	//=====================================================================

	/**
	 * @brief Test ConfigurationReference validation
	 * @details Tests ConfigurationReference construction and property access
	 */
	TEST( ValidationTests, Test_ConfigurationReference_Validation )
	{
		auto timestamp = datatypes::DateTimeOffset::parse( "2016-01-01T00:00:00Z" );

		transport::timeseries::ConfigurationReference configRef( "TestConfig.xml", timestamp );

		EXPECT_EQ( configRef.id(), "TestConfig.xml" );
		EXPECT_EQ( configRef.timeStamp().toString(), timestamp.toString() );

		/* Test setters */
		configRef.setId( "NewConfig.xml" );
		EXPECT_EQ( configRef.id(), "NewConfig.xml" );

		auto newTimestamp = datatypes::DateTimeOffset::parse( "2016-01-02T00:00:00Z" );
		configRef.setTimeStamp( newTimestamp );
		EXPECT_EQ( configRef.timeStamp().toString(), newTimestamp.toString() );
	}

	/**
	 * @brief Test TabularDataSet validation
	 * @details Tests TabularDataSet construction and property validation
	 */
	TEST( ValidationTests, Test_TabularDataSet_Validation )
	{
		auto timestamp = datatypes::DateTimeOffset::parse( "2016-01-01T12:00:00Z" );
		std::vector<std::string> values = { "100.0", "200.0" };
		std::vector<std::string> quality = { "0", "0" };

		transport::timeseries::TabularDataSet dataSet( timestamp, values, quality );

		EXPECT_EQ( dataSet.timeStamp().toString(), timestamp.toString() );
		EXPECT_EQ( dataSet.value().size(), 2U );
		EXPECT_EQ( dataSet.value()[0], "100.0" );
		EXPECT_EQ( dataSet.value()[1], "200.0" );
		EXPECT_TRUE( dataSet.quality().has_value() );
		EXPECT_EQ( dataSet.quality()->size(), 2U );
		EXPECT_EQ( ( *dataSet.quality() )[0], "0" );
		EXPECT_EQ( ( *dataSet.quality() )[1], "0" );

		/* Test without quality */
		transport::timeseries::TabularDataSet dataSetNoQuality( timestamp, values );
		EXPECT_FALSE( dataSetNoQuality.quality().has_value() );
	}

	/**
	 * @brief Test EventDataSet validation
	 * @details Tests EventDataSet construction and property validation
	 */
	TEST( ValidationTests, Test_EventDataSet_Validation )
	{
		auto timestamp = datatypes::DateTimeOffset::parse( "2016-01-01T12:00:01Z" );
		auto dataChannelId = transport::DataChannelId::parse( "0010" );

		transport::timeseries::EventDataSet eventDataSet(
			timestamp,
			dataChannelId,
			"HIGH",
			std::string( "0" ) );

		EXPECT_EQ( eventDataSet.timeStamp().toString(), timestamp.toString() );
		EXPECT_EQ( eventDataSet.dataChannelId().toString(), dataChannelId.toString() );
		EXPECT_EQ( eventDataSet.value(), "HIGH" );
		EXPECT_TRUE( eventDataSet.quality().has_value() );
		EXPECT_EQ( *eventDataSet.quality(), "0" );

		/* Test without quality */
		transport::timeseries::EventDataSet eventDataSetNoQuality(
			timestamp,
			dataChannelId,
			"LOW" );
		EXPECT_FALSE( eventDataSetNoQuality.quality().has_value() );
	}

	//=====================================================================
	// Error Handling Tests
	//=====================================================================

	/**
	 * @brief Test TimeRange error scenarios
	 * @details Tests TimeRange error conditions similar to DataChannel Range error tests
	 */
	TEST( ErrorHandlingTests, Test_TimeRange_Error_Handling )
	{
		auto start = datatypes::DateTimeOffset::parse( "2016-01-03T12:00:00Z" );
		auto end = datatypes::DateTimeOffset::parse( "2016-01-01T12:00:00Z" );

		/* Test invalid range construction: start > end should throw */
		EXPECT_THROW( transport::timeseries::TimeRange( start, end ), std::invalid_argument );

		/* Test valid construction first */
		auto validStart = datatypes::DateTimeOffset::parse( "2016-01-01T12:00:00Z" );
		auto validEnd = datatypes::DateTimeOffset::parse( "2016-01-03T12:00:00Z" );
		transport::timeseries::TimeRange timeRange( validStart, validEnd );

		/* Test invalid setters */
		auto invalidStart = datatypes::DateTimeOffset::parse( "2016-01-04T12:00:00Z" );
		EXPECT_THROW( timeRange.setStart( invalidStart ), std::invalid_argument );

		auto invalidEnd = datatypes::DateTimeOffset::parse( "2015-12-31T12:00:00Z" );
		EXPECT_THROW( timeRange.setEnd( invalidEnd ), std::invalid_argument );
	}

	/**
	 * @brief Test TabularData mismatched dimensions
	 * @details Tests TabularData with mismatched channel IDs and data columns
	 */
	TEST( ErrorHandlingTests, Test_TabularData_Dimension_Mismatch )
	{
		/* Create 2 DataChannelIds but data with 3 columns - should fail validation */
		std::vector<transport::DataChannelId> dataChannelIds;
		dataChannelIds.push_back( transport::DataChannelId::parse( "0010" ) );
		dataChannelIds.push_back( transport::DataChannelId::parse( "0020" ) );

		/* 3 values but only 2 channels */
		auto timestamp = datatypes::DateTimeOffset::parse( "2016-01-01T12:00:00Z" );
		std::vector<transport::timeseries::TabularDataSet> dataSets = {
			transport::timeseries::TabularDataSet(
				timestamp,
				std::vector<std::string>{ "100.0", "200.0", "300.0" },
				std::vector<std::string>{ "0", "0", "0" } ) };

		transport::timeseries::TabularData tabularData( dataChannelIds, dataSets );

		/* Validation should fail due to dimension mismatch */
		auto result = tabularData.validate();
		EXPECT_FALSE( result.isOk() );
	}

	/**
	 * @brief Test Package with invalid TimeSeriesData
	 * @details Tests Package error handling with malformed data
	 */
	TEST( ErrorHandlingTests, Test_Package_Error_Handling )
	{
		/* Test Package with empty TimeSeriesData vector */
		std::vector<transport::timeseries::TimeSeriesData> emptyTimeSeriesData;

		/* Create minimal header */
		transport::timeseries::Header header(
			transport::ShipId::parse( "IMO1234567" ),
			std::nullopt, /* no time range */
			std::nullopt, /* no date created */
			std::nullopt, /* no date modified */
			std::nullopt, /* no author */
			std::nullopt, /* no system configuration */
			std::nullopt  /* no custom headers */
		);

		/* This should be allowed but worth testing */
		transport::timeseries::Package package( header, emptyTimeSeriesData );
		EXPECT_TRUE( package.timeSeriesData().empty() );
		EXPECT_TRUE( package.header().has_value() );
	}
}
