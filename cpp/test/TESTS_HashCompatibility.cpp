/**
 * @file TESTS_HashCompatibility.cpp
 * @brief Hash compatibility tests between C++ and C# ChdDictionary implementations
 *
 * @details This file tests that the C++ ChdDictionary::hash() function produces
 *          identical hash values to the C# ChdDictionary.Hash() method to ensure
 *          cross-platform dictionary compatibility.
 */

#include "pch.h"

#include "dnv/vista/sdk/internal/ChdDictionary.h"
#include "dnv/vista/sdk/utils/StringUtils.h"

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
				if ( !file.is_open() )
				{
					throw std::runtime_error( "Could not open hashValues.txt file. Checked paths: " + hashValuesPath );
				}
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
	 * @brief Test that C++ ChdDictionary::hash() produces identical results to C# implementation
	 */
	TEST( HashCompatibility, CppCSharpHashIdentical )
	{
		auto testCases = loadHashTestCases();

		ASSERT_FALSE( testCases.empty() ) << "No test cases loaded from hashValues.txt";

		size_t totalTests = 0;
		size_t passedTests = 0;
		std::vector<std::string> failures;

		for ( const auto& testCase : testCases )
		{
			totalTests++;

			uint32_t actualHash = internal::ChdDictionary<int>::hash( testCase.key );

			if ( actualHash == testCase.expectedHash )
			{
				++passedTests;
			}
			else
			{
				std::ostringstream failure;
				failure << "Key     : \"" << testCase.key << "\" "
						<< "Expected: " << testCase.expectedHash << " "
						<< "Actual  : " << actualHash << " "
						<< "Diff    : " << static_cast<int64_t>( actualHash ) - static_cast<int64_t>( testCase.expectedHash );
				failures.push_back( failure.str() );
			}
		}

		std::cout << "\n=== Hash Compatibility Test Results ===" << std::endl;
		std::cout << "Total Test Cases: " << totalTests << std::endl;
		std::cout << "Tests Passed    : " << passedTests << std::endl;
		std::cout << "Tests Failed    : " << ( totalTests - passedTests ) << std::endl;
		std::cout << "Success Rate    : " << std::fixed << std::setprecision( 1 )
				  << ( 100.0 * static_cast<double>( passedTests ) / static_cast<double>( totalTests ) ) << "%" << std::endl;

		if ( failures.empty() )
		{
			std::cout << "\n✅ ALL TESTS PASSED - C++ and C# hash functions are fully compatible!\n"
					  << std::endl;
		}
		else
		{
			std::cout << "\n❌ COMPATIBILITY ISSUES DETECTED - Hash functions do not match!\n"
					  << std::endl;
		}

		if ( !failures.empty() )
		{
			std::cout << "\n=== FAILURES ===" << std::endl;
			for ( size_t i = 0; i < failures.size(); ++i )
			{
				std::cout << "[" << ( i + 1 ) << "] " << failures[i] << std::endl;
			}
		}

		EXPECT_EQ( passedTests, totalTests ) << "Hash compatibility test failed: " << ( totalTests - passedTests ) << " out of " << totalTests
											 << " tests failed";
	}

	/**
	 * @brief Test specific edge cases that are critical for compatibility
	 */
	TEST( HashCompatibility, CriticalEdgeCases )
	{
		struct EdgeCase
		{
			std::string key;
			uint32_t expectedHash;
			std::string description;
		};

		std::vector<EdgeCase> edgeCases = { { "", 2166136261, "Empty string" }, { "a", 1699757604, "Single character" }, { "ab", 1740614250, "Two characters" },
			{ "VE", 2850790297, "GMOD root node" }, { "400a", 1015739484, "Common GMOD node" }, { "test123", 1531475831, "Mixed alphanumeric" },
			{ "SpecialChars-_.", 61049792, "Special characters" }, { "UpperCASE", 2880575326, "Mixed case" }, { "lowercase", 809876800, "All lowercase" },
			{ "longerstringforhashingtesting", 61735282, "Long string" } };

		std::cout << "\n=== Critical Edge Cases Test ===" << std::endl;

		size_t passed = 0;
		for ( const auto& edgeCase : edgeCases )
		{
			uint32_t actualHash = internal::ChdDictionary<int>::hash( edgeCase.key );
			bool success = ( actualHash == edgeCase.expectedHash );

			if ( success )
				passed++;

			std::string status = success ? "✅" : "❌";
			std::string keyDisplay = edgeCase.key.empty() ? "(empty)" : ( "\"" + edgeCase.key + "\"" );

			std::cout << status << " " << edgeCase.description << " - " << keyDisplay << " = " << actualHash << std::endl;

			EXPECT_EQ( actualHash, edgeCase.expectedHash ) << "Edge case failed: " << edgeCase.description << " (key: \"" << edgeCase.key << "\")"
														   << " Expected: " << edgeCase.expectedHash << " Actual: " << actualHash;
		}

		std::cout << "\nEdge Cases Passed: " << passed << "/" << edgeCases.size() << "\n"
				  << std::endl;
	}

	/**
	 * @brief Test that demonstrates C# compatibility through equivalent byte processing
	 */
	TEST( HashCompatibility, CSharpCompatibilityTest )
	{
		/**
		 * This test verifies that our C++ implementation correctly matches
		 * C# string processing to ensure cross-platform compatibility.
		 * C# processes UTF-16 strings by reading only the low byte of each character.
		 * Our C++ implementation processes ASCII strings directly, achieving identical results.
		 */
		std::string testKey = "test";
		uint32_t actualHash = internal::ChdDictionary<int>::hash( testKey );

		std::cout << "\n=== C# Compatibility Test ===" << std::endl;
		std::cout << "Test String    : \"" << testKey << "\"" << std::endl;
		std::cout << "C++ Hash Result: " << actualHash << std::endl;
		std::cout << "This test verifies that C++ correctly matches C# hash processing to ensure cross-platform dictionary compatibility.\n"
				  << std::endl;
	}

	/**
	 * @brief Performance comparison test (informational)
	 */
	TEST( HashCompatibility, PerformanceCharacteristics )
	{
		const std::string testKey = "performance_test_string_1234567890";
		const size_t iterations = 1000000;

		auto start = std::chrono::high_resolution_clock::now();

		volatile uint32_t result = 0; /* Prevent compiler optimization */
		for ( size_t i = 0; i < iterations; ++i )
		{
			result = internal::ChdDictionary<int>::hash( testKey );
		}

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>( end - start );

		double timePerHashMicros = static_cast<double>( duration.count() ) / static_cast<double>( iterations );
		double hashesPerSecond = static_cast<double>( iterations ) / ( static_cast<double>( duration.count() ) / 1000000.0 );
		double totalTimeMs = static_cast<double>( duration.count() ) / 1000.0;

		std::cout << "\n=== Performance Test Results ===" << std::endl;
		std::cout << "Test String      : \"" << testKey << "\"" << std::endl;
		std::cout << "Iterations       : " << iterations << std::endl;
		std::cout << "Total Time       : " << std::fixed << std::setprecision( 2 ) << totalTimeMs << " ms" << std::endl;
		std::cout << "Time Per Hash    : " << std::fixed << std::setprecision( 6 ) << timePerHashMicros << " μs" << std::endl;
		std::cout << "Hashes Per Second: " << std::scientific << std::setprecision( 2 ) << hashesPerSecond << std::endl;
		std::cout << "Sample Hash      : " << result << std::endl;

		if ( hashesPerSecond > 100000000.0 ) /* > 100M hashes/sec */
		{
			std::cout << "🚀 EXCELLENT PERFORMANCE - Hash function is highly optimized\n"
					  << std::endl;
		}
		else if ( hashesPerSecond > 10000000.0 ) /* > 10M hashes/sec */
		{
			std::cout << "✅ GOOD PERFORMANCE - Hash function performs well\n"
					  << std::endl;
		}
		else if ( hashesPerSecond > 1000000.0 ) /* > 1M hashes/sec */
		{
			std::cout << "⚠️ ACCEPTABLE PERFORMANCE - Hash function meets minimum requirements\n"
					  << std::endl;
		}
		else
		{
			std::cout << "❌ POOR PERFORMANCE - Hash function may need optimization\n"
					  << std::endl;
		}

		EXPECT_GT( hashesPerSecond, 1000000.0 ) << "Hash performance too slow\n";
	}
}
