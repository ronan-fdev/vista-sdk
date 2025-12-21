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
 * @file BM_GmodPathParse.cpp
 * @brief Performance benchmark for GmodPath parsing operations
 * @details Benchmarks short path and full path parsing with and without location individualization.
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::benchmarks
{
	class GmodPathParse
	{
	private:
		const Gmod* m_gmod;
		const Locations* m_locations;

	public:
		GmodPathParse() = default;
		GmodPathParse( const GmodPathParse& ) = delete;
		GmodPathParse& operator=( const GmodPathParse& ) = delete;
		GmodPathParse( GmodPathParse&& ) = delete;
		GmodPathParse& operator=( GmodPathParse&& ) = delete;

		void Setup()
		{
			const auto& vis = VIS::instance();
			m_gmod = &vis.gmod( VisVersion::v3_4a );
			m_locations = &vis.locations( VisVersion::v3_4a );
		}

		// No location benchmarks
		bool tryParse()
		{
			auto result = GmodPath::fromString( "411.1/C101.72/I101", *m_gmod, *m_locations );
			return result.has_value();
		}

		bool tryParseFullPath()
		{
			auto result = GmodPath::fromFullPathString( "VE/400a/410/411/411i/411.1/CS1/C101/C101.7/C101.72/I101", *m_gmod, *m_locations );
			return result.has_value();
		}

		// With location benchmarks
		bool tryParseIndividualized()
		{
			auto result = GmodPath::fromString( "612.21-1/C701.13/S93", *m_gmod, *m_locations );
			return result.has_value();
		}

		bool tryParseFullPathIndividualized()
		{
			auto result = GmodPath::fromFullPathString( "VE/600a/610/612/612.2/612.2i/612.21-1/CS10/C701/C701.1/C701.13/S93", *m_gmod, *m_locations );
			return result.has_value();
		}
	};

	//=====================================================================
	// Benchmark fixture for proper setup/teardown
	//=====================================================================

	static GmodPathParse g_benchmarkInstance;

	class GmodPathParseFixture : public benchmark::Fixture
	{
	public:
		void SetUp( [[maybe_unused]] const benchmark::State& state ) override
		{
			g_benchmarkInstance.Setup();
		}

		void TearDown( [[maybe_unused]] const benchmark::State& state ) override
		{
		}
	};

	//=====================================================================
	// Benchmark implementations
	//=====================================================================

	/** @brief TryParse benchmark - No location */
	BENCHMARK_F( GmodPathParseFixture, TryParse )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.tryParse();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief TryParseFullPath benchmark - No location */
	BENCHMARK_F( GmodPathParseFixture, TryParseFullPath )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.tryParseFullPath();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief TryParseIndividualized benchmark - With location */
	BENCHMARK_F( GmodPathParseFixture, TryParseIndividualized )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.tryParseIndividualized();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief TryParseFullPathIndividualized benchmark - With location */
	BENCHMARK_F( GmodPathParseFixture, TryParseFullPathIndividualized )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.tryParseFullPathIndividualized();
			benchmark::DoNotOptimize( result );
		}
	}

	//=====================================================================
	// Benchmark registrations
	//=====================================================================

	BENCHMARK_REGISTER_F( GmodPathParseFixture, TryParse )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
	BENCHMARK_REGISTER_F( GmodPathParseFixture, TryParseFullPath )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
	BENCHMARK_REGISTER_F( GmodPathParseFixture, TryParseIndividualized )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
	BENCHMARK_REGISTER_F( GmodPathParseFixture, TryParseFullPathIndividualized )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
} // namespace dnv::vista::sdk::benchmarks

BENCHMARK_MAIN();
