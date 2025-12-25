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
 * @file BM_GmodTraversal.cpp
 * @brief GMOD tree traversal performance benchmark testing full tree iteration
 */

#include <vector>

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/Gmod.h>
#include <dnv/vista/sdk/GmodNode.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::benchmarks
{
	class GmodTraversal
	{
	private:
		const Gmod* m_gmod;

	public:
		GmodTraversal() = default;
		GmodTraversal( const GmodTraversal& ) = delete;
		GmodTraversal& operator=( const GmodTraversal& ) = delete;
		GmodTraversal( GmodTraversal&& ) = delete;
		GmodTraversal& operator=( GmodTraversal&& ) = delete;

		void Setup()
		{
			const auto& vis = VIS::instance();
			m_gmod = &vis.gmod( VisVersion::v3_4a );
		}

		bool FullTraversal()
		{
			// Simple traversal handler that continues through all nodes
			return m_gmod->traverse( []( [[maybe_unused]] const std::vector<const GmodNode*>& parents, [[maybe_unused]] const GmodNode& node ) -> TraversalHandlerResult {
				return TraversalHandlerResult::Continue;
			} );
		}
	};

	static GmodTraversal g_benchmarkInstance;

	class GmodTraversalFixture : public benchmark::Fixture
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

	BENCHMARK_F( GmodTraversalFixture, FullTraversal )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.FullTraversal();
			benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK_REGISTER_F( GmodTraversalFixture, FullTraversal )->MinTime( 10.0 )->Unit( benchmark::kMillisecond );
} // namespace dnv::vista::sdk::benchmarks

BENCHMARK_MAIN();
