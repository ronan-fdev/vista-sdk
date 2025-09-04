/**
 * @file TESTS_HashCompatibility.cpp
 * @brief Hash compatibility tests between C++ and C# nfx::containers::ChdHashMap implementations
 *
 * @details This file tests that the C++ nfx::containers::ChdHashMap::hash() function produces
 *          identical hash values to the C# ChdHashMap.Hash() method to ensure
 *          cross-platform dictionary compatibility.
 */

#include <fstream>

#include <nfx/containers/ChdHashMap.h>

#include <nfx/string/Utils.h>
#include <gtest/gtest.h>

namespace dnv::vista::sdk::test
{
	namespace
	{
		constexpr const char* TEST_DATA_PATH = "testdata/hashValues.txt";

		/**
		 * @brief Structure to hold a test case with key and expected hash value
		 */
		struct HashTestCase
		{
			std::string key;
			uint32_t expectedHash;
		};

		/**
		 * @brief Loads hash test cases from the testdata/hashValues.txt file
		 * @return Vector of test cases with keys and expected hash values
		 */
		std::vector<HashTestCase> loadHashTestCases()
		{
			std::vector<HashTestCase> testCases;

			const std::string hashValuesPath = TEST_DATA_PATH;

			std::ifstream file( hashValuesPath );
			if ( !file.is_open() )
			{
				throw std::runtime_error( "Could not open hashValues.txt file. Checked paths: " + hashValuesPath );
			}

			std::string line;
			while ( std::getline( file, line ) )
			{
				if ( line.empty() || line[0] == '#' )
				{
					continue;
				}

				size_t pipePos = line.find( '|' );
				if ( pipePos == std::string::npos )
				{
					continue;
				}

				std::string key = line.substr( 0, pipePos );
				std::string hashStr = line.substr( pipePos + 1 );

				try
				{
					uint32_t expectedHash = static_cast<uint32_t>( std::stoull( hashStr ) );
					testCases.push_back( { key, expectedHash } );
				}
				catch ( const std::exception& )
				{
					continue;
				}
			}

			return testCases;
		}
	}

	/**
	 * @brief Test that C++ nfx::containers::ChdHashMap::hash() produces identical results to C# implementation
	 */
	TEST( HashCompatibility, CppCSharpHashIdentical )
	{
		auto testCases = loadHashTestCases();
		ASSERT_FALSE( testCases.empty() );

		for ( const auto& testCase : testCases )
		{
			uint32_t actualHash = nfx::containers::ChdHashMap<int>::hash( testCase.key );
			EXPECT_EQ( actualHash, testCase.expectedHash );
		}
	}
}
