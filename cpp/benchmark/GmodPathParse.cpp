/**
 * @file GmodPathParse.cpp
 * @brief GMOD path parsing performance benchmarks comparing different parsing strategies
 */

#include "pch.h"

#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/VIS.h"

using namespace dnv::vista::sdk;

namespace dnv::vista::sdk::benchmarks
{
	static const Gmod* g_gmod = nullptr;
	static const Locations* g_locations = nullptr;
	static bool g_initialized = false;

	static void InitializeData()
	{
		if ( !g_initialized )
		{
			auto& vis = VIS::instance();
			g_gmod = &vis.gmod( VisVersion::v3_4a );
			g_locations = &vis.locations( VisVersion::v3_4a );
			g_initialized = true;
		}
	}

	static void TryParse( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		for ( auto _ : state )
		{
			std::optional<GmodPath> outPath;
			bool result = GmodPath::tryParse( "411.1/C101.72/I101", *g_gmod, *g_locations, outPath );

			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( outPath );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void TryParseFullPath( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		for ( auto _ : state )
		{
			std::optional<GmodPath> outPath;
			bool result = GmodPath::tryParseFullPath(
				"VE/400a/410/411/411i/411.1/CS1/C101/C101.7/C101.72/I101",
				*g_gmod, *g_locations, outPath );

			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( outPath );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void TryParseIndividualized( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		for ( auto _ : state )
		{
			std::optional<GmodPath> outPath;
			bool result = GmodPath::tryParse( "612.21-1/C701.13/S93", *g_gmod, *g_locations, outPath );

			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( outPath );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void TryParseFullPathIndividualized( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		for ( auto _ : state )
		{
			std::optional<GmodPath> outPath;
			bool result = GmodPath::tryParseFullPath(
				"VE/600a/610/612/612.2/612.2i/612.21-1/CS10/C701/C701.1/C701.13/S93",
				*g_gmod, *g_locations, outPath );

			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( outPath );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}
}

BENCHMARK( dnv::vista::sdk::benchmarks::TryParse )
	->MinTime( 10.0 )
	->Unit( benchmark::kMicrosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::TryParseFullPath )
	->MinTime( 10.0 )
	->Unit( benchmark::kMicrosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::TryParseIndividualized )
	->MinTime( 10.0 )
	->Unit( benchmark::kMicrosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::TryParseFullPathIndividualized )
	->MinTime( 10.0 )
	->Unit( benchmark::kMicrosecond );

BENCHMARK_MAIN();
