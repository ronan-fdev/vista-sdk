/**
 * @file GmodLoad.cpp
 * @brief GMOD loading and construction performance analysis
 */

#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodDto.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk::benchmarks
{
	static void BM_gmodLoad( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto dto = VIS::loadGmodDto( VisVersion::v3_7a );

			if ( !dto.has_value() )
			{
				state.SkipWithError( "Failed to load GMOD DTO" );
				return;
			}

			auto gmod = std::make_unique<Gmod>( VisVersion::v3_7a, *dto );

			benchmark::DoNotOptimize( dto );
			benchmark::DoNotOptimize( gmod );
		}
	}

	BENCHMARK( BM_gmodLoad )->MinTime( 10.0 )->Unit( benchmark::kMillisecond );
}

BENCHMARK_MAIN();
