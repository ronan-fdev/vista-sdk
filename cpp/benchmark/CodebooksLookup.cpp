/**
 * @file CodebooksLookup.cpp
 * @brief Codebooks lookup performance comparison between hash tables and array access
 */

#include "pch.h"

#include "dnv/vista/sdk/VIS.h"

using namespace dnv::vista::sdk;

namespace dnv::vista::sdk::benchmarks
{
	static std::unordered_map<CodebookName, const Codebook*> g_dict;
	static const Codebooks* g_codebooks = nullptr;
	static bool g_initialized = false;

	static void InitializeData()
	{
		if ( !g_initialized )
		{
			auto& vis = VIS::instance();
			g_codebooks = &vis.codebooks( VisVersion::v3_7a );

			g_dict.clear();
			for ( auto it = g_codebooks->begin(); it != g_codebooks->end(); ++it )
			{
				auto [name, codebook] = *it;
				g_dict[name] = &codebook.get();
			}
			g_initialized = true;
		}
	}

	static void Dict( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		for ( auto _ : state )
		{
			bool result =
				( g_dict.find( CodebookName::Quantity ) != g_dict.end() ) &&
				( g_dict.find( CodebookName::Type ) != g_dict.end() ) &&
				( g_dict.find( CodebookName::Detail ) != g_dict.end() );

			benchmark::DoNotOptimize( result );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void FrozenDict( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		for ( auto _ : state )
		{
			bool result =
				( g_dict.find( CodebookName::Quantity ) != g_dict.end() ) &&
				( g_dict.find( CodebookName::Type ) != g_dict.end() ) &&
				( g_dict.find( CodebookName::Detail ) != g_dict.end() );

			benchmark::DoNotOptimize( result );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void Codebooks( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		for ( auto _ : state )
		{
			const auto& a = g_codebooks->codebook( CodebookName::Quantity );
			const auto& b = g_codebooks->codebook( CodebookName::Type );
			const auto& c = g_codebooks->codebook( CodebookName::Detail );

			bool result = true;
			benchmark::DoNotOptimize( a );
			benchmark::DoNotOptimize( b );
			benchmark::DoNotOptimize( c );
			benchmark::DoNotOptimize( result );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void CodebooksArray( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		for ( auto _ : state )
		{
			const auto& a = ( *g_codebooks )[CodebookName::Quantity];
			const auto& b = ( *g_codebooks )[CodebookName::Type];
			const auto& c = ( *g_codebooks )[CodebookName::Detail];

			bool result = true;
			benchmark::DoNotOptimize( a );
			benchmark::DoNotOptimize( b );
			benchmark::DoNotOptimize( c );
			benchmark::DoNotOptimize( result );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void SingleLookup_Dict( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		for ( auto _ : state )
		{
			auto it = g_dict.find( CodebookName::Quantity );
			bool result = ( it != g_dict.end() );
			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( it );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void SingleLookup_Codebooks( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		for ( auto _ : state )
		{
			const auto& codebook = g_codebooks->codebook( CodebookName::Quantity );
			benchmark::DoNotOptimize( codebook );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void SingleLookup_Array( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		for ( auto _ : state )
		{
			const auto& codebook = ( *g_codebooks )[CodebookName::Quantity];
			benchmark::DoNotOptimize( codebook );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}
}

BENCHMARK( dnv::vista::sdk::benchmarks::Dict )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::FrozenDict )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::Codebooks )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::CodebooksArray )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::SingleLookup_Dict )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::SingleLookup_Codebooks )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::SingleLookup_Array )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK_MAIN();
