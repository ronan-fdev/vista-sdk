/**
 * @file GmodVersioningConvertPath.cpp
 * @brief GMOD version conversion performance benchmark testing path conversion between VIS versions
 */

#include "pch.h"

#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/VIS.h"

using namespace dnv::vista::sdk;

namespace dnv::vista::sdk::benchmarks
{
	class GmodVersioningFixture : public benchmark::Fixture
	{
	public:
		void SetUp( const benchmark::State& state ) override
		{
			(void)state;

			m_vis = &VIS::instance();
			m_gmod = &m_vis->gmod( VisVersion::v3_4a );

			std::optional<GmodPath> parsedPath;
			if ( !m_gmod->tryParsePath( "411.1/C101.72/I101", parsedPath ) || !parsedPath.has_value() )
			{
				throw std::runtime_error( "Failed to parse test path" );
			}

			m_gmodPath = std::move( parsedPath.value() );
		}

		void TearDown( const benchmark::State& state ) override
		{
			(void)state;
		}

	protected:
		const Gmod* m_gmod = nullptr;
		GmodPath m_gmodPath;
		VIS* m_vis = nullptr;
	};

	BENCHMARK_F( GmodVersioningFixture, ConvertPath )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto result = m_vis->convertPath( VisVersion::v3_4a, m_gmodPath, VisVersion::v3_5a );
			benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK_REGISTER_F( GmodVersioningFixture, ConvertPath )
		->MinTime( 10.0 )
		->Unit( benchmark::kMicrosecond );
}

BENCHMARK_MAIN();
