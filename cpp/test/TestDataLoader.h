/**
 * @file TestDataLoader.h
 * @brief Helper utilities for loading test data from JSON files.
 */

#pragma once

#include <nlohmann/json.hpp>

namespace dnv::vista::sdk::test
{
	const nlohmann::json& loadTestData( const char* testDataPath );
}
