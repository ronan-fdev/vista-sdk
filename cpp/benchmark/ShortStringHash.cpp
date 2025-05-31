/**
 * @file ShortStringHash.cpp
 * @brief Short string hash function performance comparison between different algorithms
 */

#include "pch.h"

#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/ChdDictionary.h"

using namespace dnv::vista::sdk;

namespace dnv::vista::sdk::benchmarks
{
	static bool g_initialized = false;

	static void InitializeData()
	{
		if ( !g_initialized )
		{
			g_initialized = true;
		}
	}

	template <typename THasher>
	static uint32_t Hash( std::string_view inputStr )
	{
		const char* data = inputStr.data();
		size_t length = inputStr.length();

		uint32_t hash = 0x811C9DC5;

		for ( size_t i = 0; i < length; ++i )
		{
			hash = THasher::hash( hash, static_cast<uint8_t>( data[i] ) );
		}

		return hash;
	}

	struct LarssonHasher
	{
		static uint32_t hash( uint32_t hash, uint8_t ch )
		{
			return internal::Hashing::larssonHash( hash, ch );
		}
	};

	struct Crc32IntrinsicHasher
	{
		static uint32_t hash( uint32_t hash, uint8_t ch )
		{
			return internal::Hashing::crc32( hash, ch );
		}
	};

	struct FnvHasher
	{
		static uint32_t hash( uint32_t hash, uint8_t ch )
		{
			return internal::Hashing::fnv1a( hash, ch );
		}
	};

	static uint32_t GetHashCodeOrdinal( std::string_view str )
	{
		const char* data = str.data();
		size_t length = str.length();

		constexpr uint32_t Hash1Start = ( 5381 << 16 ) + 5381;
		constexpr uint32_t Factor = 1566083941;

		uint32_t hash1, hash2;

		switch ( length )
		{
			case 0:
				return Hash1Start + ( Hash1Start * Factor );

			case 1:
				hash2 = ( ( Hash1Start << 5 ) + Hash1Start ) ^ static_cast<uint8_t>( data[0] );
				return Hash1Start + ( hash2 * Factor );

			case 2:
				hash2 = ( ( Hash1Start << 5 ) + Hash1Start ) ^ static_cast<uint8_t>( data[0] );
				hash2 = ( ( hash2 << 5 ) + hash2 ) ^ static_cast<uint8_t>( data[1] );
				return Hash1Start + ( hash2 * Factor );

			case 3:
				hash2 = ( ( Hash1Start << 5 ) + Hash1Start ) ^ static_cast<uint8_t>( data[0] );
				hash2 = ( ( hash2 << 5 ) + hash2 ) ^ static_cast<uint8_t>( data[1] );
				hash2 = ( ( hash2 << 5 ) + hash2 ) ^ static_cast<uint8_t>( data[2] );
				return Hash1Start + ( hash2 * Factor );

			default:
				hash1 = Hash1Start;
				hash2 = hash1;

				size_t i = 0;
				while ( i + 3 < length )
				{
					uint32_t chunk1 = *reinterpret_cast<const uint32_t*>( data + i );
					uint32_t chunk2 = *reinterpret_cast<const uint32_t*>( data + i + 2 );
					hash1 = ( ( hash1 << 5 ) + hash1 ) ^ chunk1;
					hash2 = ( ( hash2 << 5 ) + hash2 ) ^ chunk2;
					i += 4;
				}

				while ( i < length )
				{
					hash2 = ( ( hash2 << 5 ) + hash2 ) ^ static_cast<uint8_t>( data[i] );
					++i;
				}

				return hash1 + ( hash2 * Factor );
		}
	}

	static void Bcl_400( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		const std::string input = "400";
		for ( auto _ : state )
		{
			auto result = std::hash<std::string>{}( input );
			benchmark::DoNotOptimize( result );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void Bcl_H346_11112( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		const std::string input = "H346.11112";
		for ( auto _ : state )
		{
			auto result = std::hash<std::string>{}( input );
			benchmark::DoNotOptimize( result );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void BclOrd_400( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		const std::string input = "400";
		for ( auto _ : state )
		{
			auto result = GetHashCodeOrdinal( input );
			benchmark::DoNotOptimize( result );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void BclOrd_H346_11112( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		const std::string input = "H346.11112";
		for ( auto _ : state )
		{
			auto result = GetHashCodeOrdinal( input );
			benchmark::DoNotOptimize( result );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void Larsson_400( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		const std::string input = "400";
		for ( auto _ : state )
		{
			auto result = Hash<LarssonHasher>( input );
			benchmark::DoNotOptimize( result );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void Larsson_H346_11112( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		const std::string input = "H346.11112";
		for ( auto _ : state )
		{
			auto result = Hash<LarssonHasher>( input );
			benchmark::DoNotOptimize( result );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void Crc32Intrinsic_400( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		const std::string input = "400";
		for ( auto _ : state )
		{
			auto result = Hash<Crc32IntrinsicHasher>( input );
			benchmark::DoNotOptimize( result );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void Crc32Intrinsic_H346_11112( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		const std::string input = "H346.11112";
		for ( auto _ : state )
		{
			auto result = Hash<Crc32IntrinsicHasher>( input );
			benchmark::DoNotOptimize( result );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void Fnv_400( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		const std::string input = "400";
		for ( auto _ : state )
		{
			auto result = Hash<FnvHasher>( input );
			benchmark::DoNotOptimize( result );
		}

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_end;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_end, sizeof( pmc_end ) );
		auto memoryDelta = static_cast<double>( pmc_end.WorkingSetSize - initialMemory );
		state.counters["MemoryDeltaKB"] = benchmark::Counter( memoryDelta / 1024.0 );
#endif
	}

	static void Fnv_H346_11112( benchmark::State& state )
	{
		InitializeData();

#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc_start;
		GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc_start, sizeof( pmc_start ) );
		size_t initialMemory = pmc_start.WorkingSetSize;
#endif

		const std::string input = "H346.11112";
		for ( auto _ : state )
		{
			auto result = Hash<FnvHasher>( input );
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

BENCHMARK( dnv::vista::sdk::benchmarks::Bcl_400 )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::Bcl_H346_11112 )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::BclOrd_400 )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::BclOrd_H346_11112 )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::Larsson_400 )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::Larsson_H346_11112 )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::Crc32Intrinsic_400 )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::Crc32Intrinsic_H346_11112 )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::Fnv_400 )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( dnv::vista::sdk::benchmarks::Fnv_H346_11112 )
	->MinTime( 10.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK_MAIN();
