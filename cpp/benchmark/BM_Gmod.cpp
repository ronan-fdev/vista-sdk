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
 * @file BM_Gmod.cpp
 * @brief Performance benchmark for Gmod operations
 * @details Benchmarks Gmod loading (parse + construction) and cache access performance.
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/VIS.h>

#include <unordered_map>

namespace dnv::vista::sdk::benchmark
{
	//=====================================================================
	// Gmod benchmarks
	//=====================================================================

	static void BM_GmodCacheAccess( ::benchmark::State& state )
	{
		const auto& vis = VIS::instance();
		const auto& gmodwarmup = vis.gmod( VisVersion::v3_7a );
		(void)( gmodwarmup );

		for ( auto _ : state )
		{
			// This measures: Cache lookup and reference return
			const auto& gmod = vis.gmod( VisVersion::v3_7a );
			::benchmark::DoNotOptimize( &gmod );
		}
	}

	static void BM_GmodLookup_UnorderedMap( ::benchmark::State& state )
	{
		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( VisVersion::v3_7a );

		std::unordered_map<std::string, const GmodNode*> dict;
		for ( const auto& [code, node] : gmod )
		{
			dict[code] = &node;
		}

		for ( auto _ : state )
		{
			bool result = dict.find( "VE" ) != dict.end() && dict.find( "400a" ) != dict.end() &&
						  dict.find( "400" ) != dict.end() && dict.find( "H346.11112" ) != dict.end();

			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_GmodLookup_Gmod( ::benchmark::State& state )
	{
		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( VisVersion::v3_7a );

		for ( auto _ : state )
		{
			bool result = gmod.node( "VE" ).has_value() && gmod.node( "400a" ).has_value() &&
						  gmod.node( "400" ).has_value() && gmod.node( "H346.11112" ).has_value();

			::benchmark::DoNotOptimize( result );
		}
	}
} // namespace dnv::vista::sdk::benchmark

//=====================================================================
// Benchmarks registration
//=====================================================================

BENCHMARK( dnv::vista::sdk::benchmark::BM_GmodCacheAccess )->Unit( ::benchmark::kNanosecond );
BENCHMARK( dnv::vista::sdk::benchmark::BM_GmodLookup_UnorderedMap )->Unit( ::benchmark::kNanosecond );
BENCHMARK( dnv::vista::sdk::benchmark::BM_GmodLookup_Gmod )->Unit( ::benchmark::kNanosecond );

//----------------------------------------------
// Benchmark main entry point
//----------------------------------------------

BENCHMARK_MAIN();
