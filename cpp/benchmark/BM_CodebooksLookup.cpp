/*
 * MIT License
 *
 * Copyright (c) 2025 DNV
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file BM_CodebooksLookup.cpp
 * @brief Performance benchmark for Codebooks lookup operations
 * @details Compares std::unordered_map, std::map, and Codebooks class lookup performance.
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/VIS.h>

#include <map>
#include <unordered_map>

namespace dnv::vista::sdk::benchmark
{
	//=====================================================================
	// Codebooks lookup benchmark
	//=====================================================================

	//----------------------------------------------
	// Benchmark fixtures
	//----------------------------------------------

	/**
	 * @brief Fixture for codebooks lookup benchmarks
	 * @details Sets up different container types for performance comparison.
	 */
	class CodebooksLookupFixture : public ::benchmark::Fixture
	{
	public:
		void SetUp( const ::benchmark::State& ) override
		{
			const auto& vis = VIS::instance();
			m_codebooks = &vis.codebooks( VisVersion::v3_7a );

			m_unorderedMap.clear();
			m_unorderedMap.reserve( 11 );

			// Populate both maps with all available codebooks
			m_unorderedMap.emplace( CodebookName::Quantity, ( *m_codebooks )[CodebookName::Quantity] );
			m_unorderedMap.emplace( CodebookName::Content, ( *m_codebooks )[CodebookName::Content] );
			m_unorderedMap.emplace( CodebookName::Calculation, ( *m_codebooks )[CodebookName::Calculation] );
			m_unorderedMap.emplace( CodebookName::State, ( *m_codebooks )[CodebookName::State] );
			m_unorderedMap.emplace( CodebookName::Command, ( *m_codebooks )[CodebookName::Command] );
			m_unorderedMap.emplace( CodebookName::Type, ( *m_codebooks )[CodebookName::Type] );
			m_unorderedMap.emplace( CodebookName::FunctionalServices, ( *m_codebooks )[CodebookName::FunctionalServices] );
			m_unorderedMap.emplace( CodebookName::MaintenanceCategory, ( *m_codebooks )[CodebookName::MaintenanceCategory] );
			m_unorderedMap.emplace( CodebookName::ActivityType, ( *m_codebooks )[CodebookName::ActivityType] );
			m_unorderedMap.emplace( CodebookName::Position, ( *m_codebooks )[CodebookName::Position] );
			m_unorderedMap.emplace( CodebookName::Detail, ( *m_codebooks )[CodebookName::Detail] );

			m_map.emplace( CodebookName::Quantity, ( *m_codebooks )[CodebookName::Quantity] );
			m_map.emplace( CodebookName::Content, ( *m_codebooks )[CodebookName::Content] );
			m_map.emplace( CodebookName::Calculation, ( *m_codebooks )[CodebookName::Calculation] );
			m_map.emplace( CodebookName::State, ( *m_codebooks )[CodebookName::State] );
			m_map.emplace( CodebookName::Command, ( *m_codebooks )[CodebookName::Command] );
			m_map.emplace( CodebookName::Type, ( *m_codebooks )[CodebookName::Type] );
			m_map.emplace( CodebookName::FunctionalServices, ( *m_codebooks )[CodebookName::FunctionalServices] );
			m_map.emplace( CodebookName::MaintenanceCategory, ( *m_codebooks )[CodebookName::MaintenanceCategory] );
			m_map.emplace( CodebookName::ActivityType, ( *m_codebooks )[CodebookName::ActivityType] );
			m_map.emplace( CodebookName::Position, ( *m_codebooks )[CodebookName::Position] );
			m_map.emplace( CodebookName::Detail, ( *m_codebooks )[CodebookName::Detail] );
		}

	protected:
		const Codebooks* m_codebooks = nullptr;
		std::unordered_map<CodebookName, Codebook> m_unorderedMap;
		std::map<CodebookName, Codebook> m_map;
	};

	//----------------------------------------------
	// Benchmark implementations
	//----------------------------------------------

	BENCHMARK_F( CodebooksLookupFixture, BM_UnorderedMap )( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = m_unorderedMap.find( CodebookName::Quantity ) != m_unorderedMap.end() &&
						  m_unorderedMap.find( CodebookName::Type ) != m_unorderedMap.end() &&
						  m_unorderedMap.find( CodebookName::Detail ) != m_unorderedMap.end();

			::benchmark::DoNotOptimize( result );
			::benchmark::ClobberMemory();
		}
	}

	BENCHMARK_F( CodebooksLookupFixture, BM_Map )( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = m_map.find( CodebookName::Quantity ) != m_map.end() &&
						  m_map.find( CodebookName::Type ) != m_map.end() &&
						  m_map.find( CodebookName::Detail ) != m_map.end();

			::benchmark::DoNotOptimize( result );
			::benchmark::ClobberMemory();
		}
	}

	BENCHMARK_F( CodebooksLookupFixture, BM_Codebooks )( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			const auto& a = ( *m_codebooks )[CodebookName::Quantity];
			const auto& b = ( *m_codebooks )[CodebookName::Type];
			const auto& c = ( *m_codebooks )[CodebookName::Detail];

			bool result = !a.standardValues().empty() &&
						  !b.standardValues().empty() &&
						  !c.standardValues().empty();

			::benchmark::DoNotOptimize( result );
			::benchmark::ClobberMemory();
		}
	}
} // namespace dnv::vista::sdk::benchmark

//----------------------------------------------
// Benchmark main entry point
//----------------------------------------------

BENCHMARK_MAIN();
