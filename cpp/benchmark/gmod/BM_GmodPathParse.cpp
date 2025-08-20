/**
 * @file GmodPathParse.cpp
 * @brief GMOD path parsing performance benchmarks comparing different parsing strategies
 */

#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk::benchmarks
{
	static const Gmod* g_gmod = nullptr;
	static const Locations* g_locations = nullptr;
	static bool g_initialized = false;

	static void initializeData()
	{
		if ( !g_initialized )
		{
			auto& vis = VIS::instance();
			g_gmod = &vis.gmod( VisVersion::v3_4a );
			g_locations = &vis.locations( VisVersion::v3_4a );
			g_initialized = true;
		}
	}

	static void BM_tryParse( benchmark::State& state )
	{
		initializeData();

		for ( auto _ : state )
		{
			std::optional<GmodPath> outPath;
			bool result = GmodPath::tryParse( "411.1/C101.72/I101", *g_gmod, *g_locations, outPath );

			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( outPath );
		}
	}

	static void BM_tryParseFullPath( benchmark::State& state )
	{
		initializeData();

		for ( auto _ : state )
		{
			std::optional<GmodPath> outPath;
			bool result = GmodPath::tryParseFullPath( "VE/400a/410/411/411i/411.1/CS1/C101/C101.7/C101.72/I101", *g_gmod, *g_locations, outPath );

			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( outPath );
		}
	}

	static void BM_tryParseIndividualized( benchmark::State& state )
	{
		initializeData();

		for ( auto _ : state )
		{
			std::optional<GmodPath> outPath;
			bool result = GmodPath::tryParse( "612.21-1/C701.13/S93", *g_gmod, *g_locations, outPath );

			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( outPath );
		}
	}

	static void BM_tryParseFullPathIndividualized( benchmark::State& state )
	{
		initializeData();

		for ( auto _ : state )
		{
			std::optional<GmodPath> outPath;
			bool result = GmodPath::tryParseFullPath(
				"VE/600a/610/612/612.2/612.2i/612.21-1/CS10/C701/C701.1/C701.13/S93",
				*g_gmod,
				*g_locations,
				outPath );

			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( outPath );
		}
	}

	BENCHMARK( BM_tryParse )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
	BENCHMARK( BM_tryParseFullPath )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
	BENCHMARK( BM_tryParseIndividualized )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
	BENCHMARK( BM_tryParseFullPathIndividualized )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
}

BENCHMARK_MAIN();
