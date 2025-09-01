/**
 * @file GmodLookup.cpp
 * @brief GMOD node lookup comparison between CHD Dictionary and std::unordered_map
 */

#include <nfx/string/Utils.h>
#include <benchmark/benchmark.h>

#include "dnv/vista/sdk/internal/HashMap.h"
#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk::benchmarks
{
	static nfx::containers::StringMap<const GmodNode*> g_dict;
	static nfx::containers::StringMap<const GmodNode*> g_frozenDict;
	static internal::HashMap<std::string, const GmodNode*> g_dictionary;
	static const Gmod* g_gmod = nullptr;
	static bool g_initialized = false;

	static void initializeData()
	{
		if ( !g_initialized )
		{
			auto& vis = VIS::instance();
			g_gmod = &vis.gmod( VisVersion::v3_7a );

			g_dict.clear();
			g_frozenDict.clear();

			auto enumerator = g_gmod->enumerator();
			while ( enumerator.next() )
			{
				const auto& node = enumerator.current();
				g_dict[node.code()] = &node;
				g_frozenDict[node.code()] = &node;
				g_dictionary.insertOrAssign( std::string{ node.code() }, &node );
			}
			g_initialized = true;
		}
	}

	static void BM_dict( benchmark::State& state )
	{
		initializeData();

		for ( auto _ : state )
		{
			const GmodNode* node1 = nullptr;
			const GmodNode* node2 = nullptr;
			const GmodNode* node3 = nullptr;
			const GmodNode* node4 = nullptr;

			auto it1 = g_dict.find( "VE" );
			auto it2 = g_dict.find( "400a" );
			auto it3 = g_dict.find( "400" );
			auto it4 = g_dict.find( "H346.11112" );

			bool result = ( it1 != g_dict.end() && ( node1 = it1->second ) ) &&
						  ( it2 != g_dict.end() && ( node2 = it2->second ) ) &&
						  ( it3 != g_dict.end() && ( node3 = it3->second ) ) &&
						  ( it4 != g_dict.end() && ( node4 = it4->second ) );

			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( node1 );
			benchmark::DoNotOptimize( node2 );
			benchmark::DoNotOptimize( node3 );
			benchmark::DoNotOptimize( node4 );
		}
	}

	static void BM_frozenDict( benchmark::State& state )
	{
		initializeData();

		for ( auto _ : state )
		{
			const GmodNode* node1 = nullptr;
			const GmodNode* node2 = nullptr;
			const GmodNode* node3 = nullptr;
			const GmodNode* node4 = nullptr;

			auto it1 = g_frozenDict.find( "VE" );
			auto it2 = g_frozenDict.find( "400a" );
			auto it3 = g_frozenDict.find( "400" );
			auto it4 = g_frozenDict.find( "H346.11112" );

			bool result = ( it1 != g_frozenDict.end() && ( node1 = it1->second ) ) &&
						  ( it2 != g_frozenDict.end() && ( node2 = it2->second ) ) &&
						  ( it3 != g_frozenDict.end() && ( node3 = it3->second ) ) &&
						  ( it4 != g_frozenDict.end() && ( node4 = it4->second ) );

			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( node1 );
			benchmark::DoNotOptimize( node2 );
			benchmark::DoNotOptimize( node3 );
			benchmark::DoNotOptimize( node4 );
		}
	}

	static void BM_dictionary( benchmark::State& state )
	{
		initializeData();

		for ( auto _ : state )
		{
			auto ptr1 = g_dictionary.tryGetValue( "VE" );
			auto ptr2 = g_dictionary.tryGetValue( "400a" );
			auto ptr3 = g_dictionary.tryGetValue( "400" );
			auto ptr4 = g_dictionary.tryGetValue( "H346.11112" );

			const GmodNode* node1 = ptr1 ? *ptr1 : nullptr;
			const GmodNode* node2 = ptr2 ? *ptr2 : nullptr;
			const GmodNode* node3 = ptr3 ? *ptr3 : nullptr;
			const GmodNode* node4 = ptr4 ? *ptr4 : nullptr;

			bool result = node1 && node2 && node3 && node4;

			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( node1 );
			benchmark::DoNotOptimize( node2 );
			benchmark::DoNotOptimize( node3 );
			benchmark::DoNotOptimize( node4 );
		}
	}

	static void BM_gmod( benchmark::State& state )
	{
		initializeData();

		for ( auto _ : state )
		{
			const GmodNode* node;
			bool result = g_gmod->tryGetNode( "VE", node ) &&
						  g_gmod->tryGetNode( "400a", node ) &&
						  g_gmod->tryGetNode( "400", node ) &&
						  g_gmod->tryGetNode( "H346.11112", node );

			benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK( BM_dict )->MinTime( 10.0 )->Unit( benchmark::kNanosecond );
	BENCHMARK( BM_frozenDict )->MinTime( 10.0 )->Unit( benchmark::kNanosecond );
	BENCHMARK( BM_dictionary )->MinTime( 10.0 )->Unit( benchmark::kNanosecond );
	BENCHMARK( BM_gmod )->MinTime( 10.0 )->Unit( benchmark::kNanosecond );
}

BENCHMARK_MAIN();
