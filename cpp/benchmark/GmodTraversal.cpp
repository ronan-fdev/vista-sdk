/**
 * @file GmodTraversal.cpp
 * @brief GMOD tree traversal performance benchmark testing full tree iteration
 */

#include "pch.h"

#include "dnv/vista/sdk/GmodTraversal.h"
#include "dnv/vista/sdk/VIS.h"

using namespace dnv::vista::sdk;

namespace dnv::vista::sdk::benchmarks
{
	static const Gmod* g_gmod = nullptr;
	static bool g_initialized = false;

	static void InitializeData()
	{
		if ( !g_initialized )
		{
			auto& vis = VIS::instance();
			g_gmod = &vis.gmod( VisVersion::v3_4a );
			g_initialized = true;
		}
	}

	static void FullTraversal( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		for ( auto _ : state )
		{
			bool result = dnv::vista::sdk::GmodTraversal::traverse(
				*g_gmod,
				[]( const std::vector<const GmodNode*>&, const GmodNode& ) -> TraversalHandlerResult {
					return TraversalHandlerResult::Continue;
				} );

			benchmark::DoNotOptimize( result );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}
}

BENCHMARK( dnv::vista::sdk::benchmarks::FullTraversal )
	->MinTime( 10.0 )
	->Unit( benchmark::kMillisecond );

BENCHMARK_MAIN();
