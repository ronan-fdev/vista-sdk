/**
 * @file GmodLookup.cpp
 * @brief GMOD node lookup comparison between CHD Dictionary and std::unordered_map
 */

#include "pch.h"

#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/VIS.h"

#include "dnv/vista/sdk/utils/StringUtils.h"
#include "dnv/vista/sdk/utils/HashMap.h"

using namespace dnv::vista::sdk;

namespace dnv::vista::sdk::benchmarks
{
	static StringMap<const GmodNode*> g_dict;
	static StringMap<const GmodNode*> g_frozenDict;
	static HashMap<std::string, const GmodNode*> g_dictionary;
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

			bool result =
				( it1 != g_dict.end() && ( node1 = it1->second ) ) &&
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

			bool result =
				( it1 != g_frozenDict.end() && ( node1 = it1->second ) ) &&
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
			const GmodNode* node1 = nullptr;
			const GmodNode* node2 = nullptr;
			const GmodNode* node3 = nullptr;
			const GmodNode* node4 = nullptr;

			const GmodNode** ptr1 = &node1;
			const GmodNode** ptr2 = &node2;
			const GmodNode** ptr3 = &node3;
			const GmodNode** ptr4 = &node4;

			bool result =
				g_dictionary.tryGetValue( "VE", ptr1 ) &&
				g_dictionary.tryGetValue( "400a", ptr2 ) &&
				g_dictionary.tryGetValue( "400", ptr3 ) &&
				g_dictionary.tryGetValue( "H346.11112", ptr4 );

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
			bool result =
				g_gmod->tryGetNode( "VE", node ) &&
				g_gmod->tryGetNode( "400a", node ) &&
				g_gmod->tryGetNode( "400", node ) &&
				g_gmod->tryGetNode( "H346.11112", node );

			benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK( BM_dict )
		->MinTime( 10.0 )
		->Unit( benchmark::kNanosecond );

	BENCHMARK( BM_frozenDict )
		->MinTime( 10.0 )
		->Unit( benchmark::kNanosecond );

	BENCHMARK( BM_dictionary )
		->MinTime( 10.0 )
		->Unit( benchmark::kNanosecond );

	BENCHMARK( BM_gmod )
		->MinTime( 10.0 )
		->Unit( benchmark::kNanosecond );
}

BENCHMARK_MAIN();
