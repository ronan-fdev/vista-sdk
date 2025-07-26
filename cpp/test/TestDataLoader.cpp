/**
 * @file TestDataLoader.cpp
 * @brief Helper utilities for loading test data from JSON files.
 */

#include "pch.h"
#include "TestDataLoader.h"
#include "dnv/vista/sdk/utils/HashMap.h"

namespace dnv::vista::sdk
{
	static HashMap<std::string, nlohmann::json> g_testDataCache;

	const nlohmann::json& loadTestData( const char* testDataPath )
	{
		std::string_view pathView{ testDataPath };

		nlohmann::json* cachedData = nullptr;
		if ( g_testDataCache.tryGetValue( pathView, cachedData ) )
		{
			return *cachedData;
		}

		std::ifstream jsonFile( testDataPath );
		if ( !jsonFile.is_open() )
		{
			throw std::runtime_error( std::string{ "Failed to open test data file: " } + testDataPath );
		}

		try
		{
			nlohmann::json data;
			jsonFile >> data;

			g_testDataCache.insertOrAssign( std::string{ pathView }, std::move( data ) );

			nlohmann::json* result = nullptr;
			if ( g_testDataCache.tryGetValue( pathView, result ) )
			{
				return *result;
			}

			throw std::runtime_error( "Failed to retrieve cached test data after insertion" );
		}
		catch ( const nlohmann::json::parse_error& ex )
		{
			throw std::runtime_error(
				std::string{ "JSON parse error in '" } + testDataPath +
				"'. Type: " + std::to_string( ex.id ) +
				", Byte: " + std::to_string( ex.byte ) +
				". Original what() likely too long." );
		}
	}
}
