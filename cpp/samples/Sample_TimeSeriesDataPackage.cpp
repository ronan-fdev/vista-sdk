/**
 * @file Sample_TimeSeriesDataPackage.cpp
 * @brief TimeSeriesDataPackage Creation - Vista SDK C++
 *
 * This example demonstrates the creation and handling of TimeSeriesDataPackage:
 * - Creating ballast water management event data
 * - Building TimeSeriesDataPackage with header and metadata
 * - Working with tabular data structures
 * - Managing ship identification and time ranges
 * - Formatting data for shore-side transmission
 *
 * For more examples, see the other files in this directory.
 */

#include <iostream>

#include "dnv/vista/sdk/transport/ShipId.h"
#include "dnv/vista/sdk/transport/TimeSeriesData/TimeSeriesData.h"
#include "dnv/vista/sdk/transport/TimeSeriesData/DataChannelId.h"
#include "dnv/vista/sdk/ImoNumber.h"

using namespace dnv::vista::sdk;

//=====================================================================
// BallastEvent structure
//=====================================================================

/**
 * @brief Represents a ballast water management event with associated sensor data
 */
struct BallastEvent
{
	std::string mode;				 ///< Ballast operation mode
	std::string location;			 ///< Location identifier
	nfx::time::DateTimeOffset start; ///< Event start time
	nfx::time::DateTimeOffset stop;	 ///< Event stop time
	int volume;						 ///< Water volume (m³)
	int uvTreatment;				 ///< UV treatment level
	int salinity;					 ///< Salinity measurement (PSU)
	struct Position
	{
		double lat; ///< Latitude (degrees)
		double lng; ///< Longitude (degrees)
	} position;		///< Geographic coordinates
};

//=====================================================================
// Package creation
//=====================================================================

transport::timeseries::TimeSeriesDataPackage createPackage( const std::vector<BallastEvent>& sourceData )
{
	if ( sourceData.empty() )
	{
		throw std::invalid_argument{ "Source data cannot be empty" };
	}

	// Current timestamp for package creation
	const auto now = nfx::time::DateTimeOffset::now();

	// Define data channel identifiers
	const std::vector<std::string> dataChannelIds{
		"Mode",
		"Location",
		"Start",
		"Stop",
		"Volume",
		"UVTreatment",
		"Salinity",
		"Latitude",
		"Longitude" };

	// Convert ballast events to tabular data structure
	std::vector<transport::timeseries::TabularDataSet> dataSets;
	dataSets.reserve( sourceData.size() );

	for ( const auto& event : sourceData )
	{
		// Convert event data to string values
		std::vector<std::string> values;
		values.reserve( dataChannelIds.size() );

		values.emplace_back( event.mode );
		values.emplace_back( event.location );
		values.emplace_back( event.start.toString() );
		values.emplace_back( event.stop.toString() );
		values.emplace_back( std::to_string( event.volume ) );
		values.emplace_back( std::to_string( event.uvTreatment ) );
		values.emplace_back( std::to_string( event.salinity ) );
		values.emplace_back( std::to_string( event.position.lat ) );
		values.emplace_back( std::to_string( event.position.lng ) );

		// Use correct TabularDataSet constructor (timestamp, values, quality)
		dataSets.emplace_back(
			event.start,		 // timestamp
			std::move( values ), // values as std::vector<std::string>
			std::nullopt		 // quality (optional)
		);
	}

	// Calculate time span (from earliest start to latest stop)
	const auto timeFrom = std::min_element( sourceData.begin(), sourceData.end(),
		[]( const auto& a, const auto& b ) { return a.start < b.start; } )
							  ->start;

	const auto timeTo = std::max_element( sourceData.begin(), sourceData.end(),
		[]( const auto& a, const auto& b ) { return a.stop < b.stop; } )
							->stop;

	// Create TabularData
	std::vector<transport::DataChannelId> dataChannelIdObjects;
	dataChannelIdObjects.reserve( dataChannelIds.size() );
	for ( const auto& id : dataChannelIds )
	{
		dataChannelIdObjects.emplace_back( transport::DataChannelId{ id } );
	}

	auto tabularData = transport::timeseries::TabularData{
		std::move( dataChannelIdObjects ), // dataChannelIds as DataChannelId vector
		std::move( dataSets )			   // dataSet vector
	};

	// Create TimeSeriesData with tabular data and empty event data
	auto timeSeriesData = transport::timeseries::TimeSeriesData{
		std::nullopt,																 // dataConfiguration
		std::vector<transport::timeseries::TabularData>{ std::move( tabularData ) }, // tabularData
		std::nullopt,																 // eventData
		std::nullopt																 // customDataKinds
	};

	// Create proper IMO number and timeRange
	auto imoNumber = ImoNumber{ 1234567 };								   // Create ImoNumber object
	auto shipId = transport::ShipId{ imoNumber };						   // Create ShipId from ImoNumber
	auto timeRange = transport::timeseries::TimeRange{ timeFrom, timeTo }; // Create TimeRange

	// Create package header
	auto header = transport::timeseries::Header{
		shipId,														  // shipId
		std::optional<transport::timeseries::TimeRange>{ timeRange }, // timeRange
		std::optional<nfx::time::DateTimeOffset>{ now },			  // dateCreated
		std::optional<nfx::time::DateTimeOffset>{ now },			  // dateModified
		std::optional<std::string>{ "DNV" },						  // author
		std::nullopt,												  // systemConfiguration
		std::nullopt												  // customHeaders
	};

	// Create Package first, then TimeSeriesDataPackage
	auto package = transport::timeseries::Package{
		std::optional<transport::timeseries::Header>{ std::move( header ) },			  // header
		std::vector<transport::timeseries::TimeSeriesData>{ std::move( timeSeriesData ) } // timeSeriesData
	};

	// Create complete TimeSeriesDataPackage from Package
	return transport::timeseries::TimeSeriesDataPackage{ std::move( package ) };
}

//=====================================================================
// Application entry point
//=====================================================================

/**
 * @brief Demonstrates TimeSeriesDataPackage creation with sample ballast events
 */
int main()
{
	try
	{
		std::cout << "=== VISTA SDK C++ TimeSeriesDataPackage Sample ===\n\n";

		// Create sample ballast events (matching realistic maritime data)
		std::vector<BallastEvent> sampleEvents{
			{
				"Intake",													// mode
				"Port Tank #1",												// location
				nfx::time::DateTimeOffset::parse( "2025-08-28T08:00:00Z" ), // start
				nfx::time::DateTimeOffset::parse( "2025-08-28T10:30:00Z" ), // stop
				1500,														// volume (m³)
				95,															// uvTreatment (%)
				35,															// salinity (PSU)
				{ 59.3293, 18.0686 }										// position (Stockholm)
			},
			{
				"Discharge",												// mode
				"Starboard Tank #2",										// location
				nfx::time::DateTimeOffset::parse( "2025-08-28T12:15:00Z" ), // start
				nfx::time::DateTimeOffset::parse( "2025-08-28T14:45:00Z" ), // stop
				1200,														// volume (m³)
				98,															// uvTreatment (%)
				32,															// salinity (PSU)
				{ 55.6761, 12.5683 }										// position (Copenhagen)
			},
			{
				"Exchange",													// mode
				"Port Tank #3",												// location
				nfx::time::DateTimeOffset::parse( "2025-08-28T16:00:00Z" ), // start
				nfx::time::DateTimeOffset::parse( "2025-08-28T18:20:00Z" ), // stop
				2000,														// volume (m³)
				97,															// uvTreatment (%)
				38,															// salinity (PSU)
				{ 57.7089, 11.9746 }										// position (Gothenburg)
			} };

		std::cout << "Creating TimeSeriesDataPackage from " << sampleEvents.size()
				  << " ballast events...\n";

		// Create the package
		auto package = createPackage( sampleEvents );

		// Display package information
		std::cout << "\n=== Package Information ===\n";

		// Get package internal structure for display
		const auto& internalPackage = package.package();

		if ( internalPackage.header().has_value() )
		{
			const auto& header = internalPackage.header().value();
			std::cout << "Ship ID: " << header.shipId().toString() << "\n";

			if ( header.author().has_value() )
			{
				std::cout << "Author: " << header.author().value() << "\n";
			}

			if ( header.dateCreated().has_value() )
			{
				std::cout << "Created: " << header.dateCreated().value().toString() << "\n";
			}

			if ( header.timeRange().has_value() )
			{
				const auto& timeRange = header.timeRange().value();
				std::cout << "Time Range: " << timeRange.start().toString()
						  << " to " << timeRange.end().toString() << "\n";
			}
		}

		const auto& timeSeriesData = internalPackage.timeSeriesData()[0];
		if ( timeSeriesData.tabularData().has_value() )
		{
			const auto& tabular = timeSeriesData.tabularData().value()[0];
			std::cout << "Data Channels: " << tabular.dataChannelIds().size() << "\n";
			std::cout << "Data Sets: " << tabular.dataSets().size() << "\n";

			std::cout << "\n=== Data Channel IDs ===\n";
			for ( size_t i = 0; i < tabular.dataChannelIds().size(); ++i )
			{
				std::cout << "[" << i << "] " << tabular.dataChannelIds()[i].toString() << "\n";
			}
		}

		std::cout << "\n=== Sample Data Sets ===\n";
		if ( timeSeriesData.tabularData().has_value() )
		{
			const auto& dataSets = timeSeriesData.tabularData().value()[0].dataSets();
			for ( size_t i = 0; i < dataSets.size(); ++i )
			{
				const auto& dataSet = dataSets[i];
				std::cout << "Dataset " << i << " (" << dataSet.timeStamp().toString() << "):\n";

				for ( size_t j = 0; j < dataSet.value().size(); ++j )
				{
					std::cout << "  " << dataSet.value()[j] << "\n";
				}
				std::cout << "\n";
			}
		}

		std::cout << "TimeSeriesDataPackage created successfully!\n";
		std::cout << "Package ready for shore-side transmission and analysis.\n\n";

		return 0;
	}
	catch ( const std::exception& ex )
	{
		std::cerr << "Error: " << ex.what() << "\n";
		return 1;
	}
}
