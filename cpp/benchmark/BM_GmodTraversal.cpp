/**
 * @file BM_GmodTraversal.cpp
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

	static void initializeData()
	{
		if ( !g_initialized )
		{
			auto& vis = VIS::instance();
			g_gmod = &vis.gmod( VisVersion::v3_4a );
			g_initialized = true;
		}
	}

	static void BM_fullTraversal( benchmark::State& state )
	{
		initializeData();

		struct BenchmarkState
		{
			int nodeCount = 0;
		};

		for ( auto _ : state )
		{
			BenchmarkState benchState;

			TraverseHandlerWithState<BenchmarkState> handler =
				[]( BenchmarkState& state, const std::vector<GmodNode>&, const GmodNode& ) -> TraversalHandlerResult {
				++state.nodeCount;
				return TraversalHandlerResult::Continue;
			};

			bool result = dnv::vista::sdk::GmodTraversal::traverse( benchState, *g_gmod, handler );

			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( benchState.nodeCount );
		}
	}

	static void BM_fullTraversal_Simple( benchmark::State& state )
	{
		initializeData();

		for ( auto _ : state )
		{
			TraverseHandler handler = []( const std::vector<GmodNode>&, const GmodNode& ) -> TraversalHandlerResult {
				return TraversalHandlerResult::Continue;
			};

			bool result = dnv::vista::sdk::GmodTraversal::traverse( *g_gmod, handler );

			benchmark::DoNotOptimize( result );
		}
	}

	static void BM_fullTraversal_WithValidation( benchmark::State& state )
	{
		initializeData();

		struct BenchmarkState
		{
			int nodeCount = 0;
		};

		for ( auto _ : state )
		{
			BenchmarkState benchState;

			TraverseHandlerWithState<BenchmarkState> handler =
				[]( BenchmarkState& state, const std::vector<GmodNode>& parents, const GmodNode& node ) -> TraversalHandlerResult {
				(void)parents;
				(void)node;
				++state.nodeCount;
				return TraversalHandlerResult::Continue;
			};

			bool result = dnv::vista::sdk::GmodTraversal::traverse( benchState, *g_gmod, handler );

			if ( !result )
			{
				throw std::runtime_error( "Traversal failed to complete" );
			}

			if ( benchState.nodeCount < 1000 )
			{
				throw std::runtime_error( "Unexpectedly low node count: " + std::to_string( benchState.nodeCount ) );
			}

			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( benchState.nodeCount );

			state.counters["NodesPerSec"] = benchmark::Counter( benchState.nodeCount, benchmark::Counter::kIsRate );
			state.counters["TotalNodes"] = benchState.nodeCount;
		}
	}

	static void BM_fullTraversal_MemoryTracking( benchmark::State& state )
	{
		initializeData();

		for ( auto _ : state )
		{
			struct BenchmarkState
			{
				int nodeCount = 0;
				size_t maxParentDepth = 0;
			};

			BenchmarkState benchState;

			TraverseHandlerWithState<BenchmarkState> handler =
				[]( BenchmarkState& state, const std::vector<GmodNode>& parents, const GmodNode& ) -> TraversalHandlerResult {
				++state.nodeCount;
				state.maxParentDepth = std::max( state.maxParentDepth, parents.size() );
				return TraversalHandlerResult::Continue;
			};

			bool result = dnv::vista::sdk::GmodTraversal::traverse( benchState, *g_gmod, handler );

			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( benchState.nodeCount );
			benchmark::DoNotOptimize( benchState.maxParentDepth );

			state.counters["MaxDepth"] = benchState.maxParentDepth;
			state.counters["NodesPerSec"] = benchmark::Counter( benchState.nodeCount, benchmark::Counter::kIsRate );
		}
	}

	BENCHMARK( BM_fullTraversal )
		->MinTime( 10.0 )
		->Unit( benchmark::kMillisecond );

	BENCHMARK( BM_fullTraversal_Simple )
		->MinTime( 10.0 )
		->Unit( benchmark::kMillisecond );

	BENCHMARK( BM_fullTraversal_WithValidation )
		->MinTime( 10.0 )
		->Unit( benchmark::kMillisecond );

	BENCHMARK( BM_fullTraversal_MemoryTracking )
		->MinTime( 10.0 )
		->Unit( benchmark::kMillisecond );
}

BENCHMARK_MAIN();
