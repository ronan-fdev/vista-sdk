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
 * @file BM_GmodVersioningConvertPath.cpp
 * @brief GMOD version conversion performance benchmark testing path conversion between VIS versions
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::benchmarks
{
	class GmodVersioningConvertPath
	{
	private:
		const Gmod* m_gmod = nullptr;
		std::optional<GmodPath> m_gmodPath;

	public:
		GmodVersioningConvertPath() = default;
		GmodVersioningConvertPath( const GmodVersioningConvertPath& ) = delete;
		GmodVersioningConvertPath& operator=( const GmodVersioningConvertPath& ) = delete;
		GmodVersioningConvertPath( GmodVersioningConvertPath&& ) = delete;
		GmodVersioningConvertPath& operator=( GmodVersioningConvertPath&& ) = delete;

		void Setup()
		{
			const auto& vis = VIS::instance();
			m_gmod = &vis.gmod( VisVersion::v3_4a );
			const auto& locations = vis.locations( VisVersion::v3_4a );

			ParsingErrors errors;
			auto path = GmodPath::fromString( "411.1/C101.72/I101", *m_gmod, locations, errors );
			if ( !path.has_value() )
			{
				throw std::runtime_error( "Failed to parse benchmark path" );
			}
			m_gmodPath = *path;
		}

		std::optional<GmodPath> ConvertPath()
		{
			return VIS::instance().convertPath( VisVersion::v3_4a, *m_gmodPath, VisVersion::v3_5a );
		}
	};

	static GmodVersioningConvertPath g_benchmarkInstance;

	class GmodVersioningConvertPathFixture : public benchmark::Fixture
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

	/** @brief GMOD path version conversion benchmark */
	BENCHMARK_F( GmodVersioningConvertPathFixture, ConvertPath )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto result = g_benchmarkInstance.ConvertPath();
			benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK_REGISTER_F( GmodVersioningConvertPathFixture, ConvertPath )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
} // namespace dnv::vista::sdk::benchmarks

BENCHMARK_MAIN();
