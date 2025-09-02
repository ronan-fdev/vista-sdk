/**
 * @file BM_HashMap.cpp
 * @brief Comprehensive benchmarks comparing StringMap, StringSet, HashMap vs STL containers
 */

#include <random>
#include <unordered_set>

#include <nfx/containers/HashMap.h>
#include <nfx/containers/StringMap.h>
#include <nfx/containers/StringSet.h>

#include <benchmark/benchmark.h>

namespace dnv::vista::sdk::benchmarks
{
	//=====================================================================
	// Test Data Generation
	//=====================================================================

	class BenchmarkData
	{
	public:
		static const std::vector<std::string>& getTestKeys()
		{
			static std::vector<std::string> keys = generateKeys();
			return keys;
		}

		static const std::vector<std::string_view>& getTestKeysViews()
		{
			static std::vector<std::string_view> views = generateViews();
			return views;
		}

		static const std::vector<const char*>& getTestKeysCStr()
		{
			static std::vector<const char*> cstrs = generateCStrings();
			return cstrs;
		}

	private:
		static std::vector<std::string> generateKeys()
		{
			std::vector<std::string> keys;
			keys.reserve( 10000 );

			for ( int i = 100; i < 1000; ++i )
			{
				keys.emplace_back( "621." + std::to_string( i ) );
				keys.emplace_back( "411." + std::to_string( i ) );
				keys.emplace_back( "511." + std::to_string( i ) );
			}

			for ( int major = 1; major <= 5; ++major )
			{
				for ( int minor = 0; minor <= 20; ++minor )
				{
					for ( int patch = 0; patch <= 10; ++patch )
					{
						keys.emplace_back( "v" + std::to_string( major ) + "." +
										   std::to_string( minor ) + "." +
										   std::to_string( patch ) );
					}
				}
			}

			std::mt19937 gen( 12345 );
			std::uniform_int_distribution<size_t> len_dist( 5, 30 );
			std::uniform_int_distribution<> char_dist( 97, 122 ); // a-z

			for ( int i = 0; i < 1000; ++i )
			{
				std::string key;
				auto len = len_dist( gen );
				key.reserve( len );
				for ( size_t j = 0; j < len; ++j )
				{
					key += static_cast<char>( char_dist( gen ) );
				}
				keys.emplace_back( std::move( key ) );
			}

			return keys;
		}

		static std::vector<std::string_view> generateViews()
		{
			const auto& keys = getTestKeys();
			std::vector<std::string_view> views;
			views.reserve( keys.size() );
			for ( const auto& key : keys )
			{
				views.emplace_back( key );
			}
			return views;
		}

		static std::vector<const char*> generateCStrings()
		{
			const auto& keys = getTestKeys();
			std::vector<const char*> cstrs;
			cstrs.reserve( keys.size() );
			for ( const auto& key : keys )
			{
				cstrs.emplace_back( key.c_str() );
			}
			return cstrs;
		}
	};

	//=====================================================================
	// Map Insertion Benchmarks
	//=====================================================================

	static void BM_StdUnorderedMap_Insert_String( benchmark::State& state )
	{
		const auto& keys = BenchmarkData::getTestKeys();

		for ( auto _ : state )
		{
			std::unordered_map<std::string, int> map;
			map.reserve( keys.size() );

			for ( size_t i = 0; i < keys.size(); ++i )
			{
				map[keys[i]] = static_cast<int>( i & 0x7FFFFFFF );
			}

			benchmark::DoNotOptimize( map );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * keys.size() ) );
	}

	static void BM_StdUnorderedMap_Insert_StringView( benchmark::State& state )
	{
		const auto& views = BenchmarkData::getTestKeysViews();

		for ( auto _ : state )
		{
			std::unordered_map<std::string, int> map;
			map.reserve( views.size() );

			for ( size_t i = 0; i < views.size(); ++i )
			{
				map[std::string{ views[i] }] = static_cast<int>( i & 0x7FFFFFFF );
			}

			benchmark::DoNotOptimize( map );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * views.size() ) );
	}

	static void BM_StringMap_Insert_String( benchmark::State& state )
	{
		const auto& keys = BenchmarkData::getTestKeys();

		for ( auto _ : state )
		{
			nfx::containers::StringMap<int> map;
			map.reserve( keys.size() );

			for ( size_t i = 0; i < keys.size(); ++i )
			{
				map[keys[i]] = static_cast<int>( i & 0x7FFFFFFF );
			}

			benchmark::DoNotOptimize( map );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * keys.size() ) );
	}

	static void BM_StringMap_Insert_StringView( benchmark::State& state )
	{
		const auto& views = BenchmarkData::getTestKeysViews();

		for ( auto _ : state )
		{
			nfx::containers::StringMap<int> map;
			map.reserve( views.size() );

			for ( size_t i = 0; i < views.size(); ++i )
			{
				map[views[i]] = static_cast<int>( i & 0x7FFFFFFF );
			}

			benchmark::DoNotOptimize( map );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * views.size() ) );
	}

	static void BM_HashMap_Insert_String( benchmark::State& state )
	{
		const auto& keys = BenchmarkData::getTestKeys();

		for ( auto _ : state )
		{
			nfx::containers::HashMap<std::string, int> map;
			map.reserve( keys.size() );

			for ( size_t i = 0; i < keys.size(); ++i )
			{
				map.insertOrAssign( keys[i], static_cast<int>( i & 0x7FFFFFFF ) );
			}

			benchmark::DoNotOptimize( map );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * keys.size() ) );
	}

	//=====================================================================
	// Map Lookup Benchmarks
	//=====================================================================

	template <typename MapType>
	MapType createPopulatedMap()
	{
		const auto& keys = BenchmarkData::getTestKeys();
		MapType map;
		map.reserve( keys.size() );

		for ( size_t i = 0; i < keys.size(); ++i )
		{
			if constexpr ( std::is_same_v<MapType, nfx::containers::HashMap<std::string, int>> )
			{
				map.insertOrAssign( keys[i], static_cast<int>( i ) );
			}
			else
			{
				map[keys[i]] = static_cast<int>( i );
			}
		}

		return map;
	}

	static void BM_StdUnorderedMap_Lookup_String( benchmark::State& state )
	{
		auto map = createPopulatedMap<std::unordered_map<std::string, int>>();
		const auto& keys = BenchmarkData::getTestKeys();

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( const auto& key : keys )
			{
				auto it = map.find( key );
				if ( it != map.end() )
				{
					found_count += it->second;
				}
			}
			benchmark::DoNotOptimize( found_count );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * keys.size() ) );
	}

	static void BM_StdUnorderedMap_Lookup_StringView( benchmark::State& state )
	{
		auto map = createPopulatedMap<std::unordered_map<std::string, int>>();
		const auto& views = BenchmarkData::getTestKeysViews();

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( const auto& view : views )
			{
				auto it = map.find( std::string{ view } );
				if ( it != map.end() )
				{
					found_count += it->second;
				}
			}
			benchmark::DoNotOptimize( found_count );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * views.size() ) );
	}

	static void BM_StringMap_Lookup_String( benchmark::State& state )
	{
		auto map = createPopulatedMap<nfx::containers::StringMap<int>>();
		const auto& keys = BenchmarkData::getTestKeys();

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( const auto& key : keys )
			{
				auto it = map.find( key );
				if ( it != map.end() )
				{
					found_count += it->second;
				}
			}
			benchmark::DoNotOptimize( found_count );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * keys.size() ) );
	}

	static void BM_StringMap_Lookup_StringView( benchmark::State& state )
	{
		auto map = createPopulatedMap<nfx::containers::StringMap<int>>();
		const auto& views = BenchmarkData::getTestKeysViews();

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( const auto& view : views )
			{
				auto it = map.find( view );
				if ( it != map.end() )
				{
					found_count += it->second;
				}
			}
			benchmark::DoNotOptimize( found_count );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * views.size() ) );
	}

	static void BM_HashMap_Lookup_String( benchmark::State& state )
	{
		auto map = createPopulatedMap<nfx::containers::HashMap<std::string, int>>();
		const auto& keys = BenchmarkData::getTestKeys();

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( const auto& key : keys )
			{
				if ( const auto* value = map.tryGetValue( key ) )
				{
					found_count += *value;
				}
			}
			benchmark::DoNotOptimize( found_count );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * keys.size() ) );
	}

	static void BM_HashMap_Lookup_StringView( benchmark::State& state )
	{
		auto map = createPopulatedMap<nfx::containers::HashMap<std::string, int>>();
		const auto& views = BenchmarkData::getTestKeysViews();

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( const auto& view : views )
			{
				if ( const auto* value = map.tryGetValue( view ) )
				{
					found_count += *value;
				}
			}
			benchmark::DoNotOptimize( found_count );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * views.size() ) );
	}

	//=====================================================================
	// Set Insertion Benchmarks
	//=====================================================================

	static void BM_StdUnorderedSet_Insert_String( benchmark::State& state )
	{
		const auto& keys = BenchmarkData::getTestKeys();

		for ( auto _ : state )
		{
			std::unordered_set<std::string> set;
			set.reserve( keys.size() );

			for ( const auto& key : keys )
			{
				set.insert( key );
			}

			benchmark::DoNotOptimize( set );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * keys.size() ) );
	}

	static void BM_StdUnorderedSet_Insert_StringView( benchmark::State& state )
	{
		const auto& views = BenchmarkData::getTestKeysViews();

		for ( auto _ : state )
		{
			std::unordered_set<std::string> set;
			set.reserve( views.size() );

			for ( const auto& view : views )
			{
				set.insert( std::string{ view } );
			}

			benchmark::DoNotOptimize( set );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * views.size() ) );
	}

	static void BM_StringSet_Insert_String( benchmark::State& state )
	{
		const auto& keys = BenchmarkData::getTestKeys();

		for ( auto _ : state )
		{
			nfx::containers::StringSet set;
			set.reserve( keys.size() );

			for ( const auto& key : keys )
			{
				set.insert( key );
			}

			benchmark::DoNotOptimize( set );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * keys.size() ) );
	}

	static void BM_StringSet_Insert_StringView( benchmark::State& state )
	{
		const auto& views = BenchmarkData::getTestKeysViews();

		for ( auto _ : state )
		{
			nfx::containers::StringSet set;
			set.reserve( views.size() );

			for ( const auto& view : views )
			{
				set.emplace( view );
			}

			benchmark::DoNotOptimize( set );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * views.size() ) );
	}

	//=====================================================================
	// Set Lookup Benchmarks
	//=====================================================================

	template <typename SetType>
	SetType createPopulatedSet()
	{
		const auto& keys = BenchmarkData::getTestKeys();
		SetType set;
		set.reserve( keys.size() );

		for ( const auto& key : keys )
		{
			set.insert( key );
		}

		return set;
	}

	static void BM_StdUnorderedSet_Lookup_String( benchmark::State& state )
	{
		auto set = createPopulatedSet<std::unordered_set<std::string>>();
		const auto& keys = BenchmarkData::getTestKeys();

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( const auto& key : keys )
			{
				if ( set.contains( key ) )
				{
					++found_count;
				}
			}
			benchmark::DoNotOptimize( found_count );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * keys.size() ) );
	}

	static void BM_StdUnorderedSet_Lookup_StringView( benchmark::State& state )
	{
		auto set = createPopulatedSet<std::unordered_set<std::string>>();
		const auto& views = BenchmarkData::getTestKeysViews();

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( const auto& view : views )
			{
				if ( set.contains( std::string{ view } ) )
				{
					++found_count;
				}
			}
			benchmark::DoNotOptimize( found_count );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * views.size() ) );
	}

	static void BM_StringSet_Lookup_String( benchmark::State& state )
	{
		auto set = createPopulatedSet<nfx::containers::StringSet>();
		const auto& keys = BenchmarkData::getTestKeys();

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( const auto& key : keys )
			{
				if ( set.contains( key ) )
				{
					++found_count;
				}
			}
			benchmark::DoNotOptimize( found_count );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * keys.size() ) );
	}

	static void BM_StringSet_Lookup_StringView( benchmark::State& state )
	{
		auto set = createPopulatedSet<nfx::containers::StringSet>();
		const auto& views = BenchmarkData::getTestKeysViews();

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( const auto& view : views )
			{
				if ( set.contains( view ) )
				{
					++found_count;
				}
			}
			benchmark::DoNotOptimize( found_count );
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * views.size() ) );
	}

	//=====================================================================
	// Memory Usage Benchmarks
	//=====================================================================

	static void BM_Memory_StdUnorderedMap( benchmark::State& state )
	{
		const auto& keys = BenchmarkData::getTestKeys();

		for ( auto _ : state )
		{
			state.PauseTiming();

			std::unordered_map<std::string, int> map;
			map.reserve( keys.size() );

			state.ResumeTiming();

			for ( size_t i = 0; i < keys.size(); ++i )
			{
				map[keys[i]] = static_cast<int>( i );
			}

			state.PauseTiming();
			benchmark::DoNotOptimize( map );
			state.ResumeTiming();
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * keys.size() ) );
		state.SetBytesProcessed( static_cast<int64_t>( state.iterations() * keys.size() *
													   ( sizeof( std::string ) + sizeof( int ) + 32 ) ) );
	}

	static void BM_Memory_StringMap( benchmark::State& state )
	{
		const auto& keys = BenchmarkData::getTestKeys();

		for ( auto _ : state )
		{
			state.PauseTiming();

			nfx::containers::StringMap<int> map;
			map.reserve( keys.size() );

			state.ResumeTiming();

			for ( size_t i = 0; i < keys.size(); ++i )
			{
				map[keys[i]] = static_cast<int>( i );
			}

			state.PauseTiming();
			benchmark::DoNotOptimize( map );
			state.ResumeTiming();
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * keys.size() ) );
		state.SetBytesProcessed( static_cast<int64_t>( state.iterations() * keys.size() *
													   ( sizeof( std::string ) + sizeof( int ) + 32 ) ) );
	}

	static void BM_Memory_HashMap( benchmark::State& state )
	{
		const auto& keys = BenchmarkData::getTestKeys();

		for ( auto _ : state )
		{
			state.PauseTiming();

			nfx::containers::HashMap<std::string, int> map;
			map.reserve( keys.size() );

			state.ResumeTiming();

			for ( size_t i = 0; i < keys.size(); ++i )
			{
				map.insertOrAssign( keys[i], static_cast<int>( i ) );
			}

			state.PauseTiming();
			benchmark::DoNotOptimize( map );
			state.ResumeTiming();
		}

		state.SetItemsProcessed( static_cast<int64_t>( state.iterations() * keys.size() ) );
		state.SetBytesProcessed( static_cast<int64_t>( state.iterations() * keys.size() *
													   ( sizeof( std::string ) + sizeof( int ) + 16 ) ) );
	}

	//=====================================================================
	// Run benchmarks
	//=====================================================================

	BENCHMARK( BM_StdUnorderedMap_Insert_String );
	BENCHMARK( BM_StdUnorderedMap_Insert_StringView );
	BENCHMARK( BM_StringMap_Insert_String );
	BENCHMARK( BM_StringMap_Insert_StringView );
	BENCHMARK( BM_HashMap_Insert_String );

	BENCHMARK( BM_StdUnorderedMap_Lookup_String );
	BENCHMARK( BM_StdUnorderedMap_Lookup_StringView );
	BENCHMARK( BM_StringMap_Lookup_String );
	BENCHMARK( BM_StringMap_Lookup_StringView );
	BENCHMARK( BM_HashMap_Lookup_String );
	BENCHMARK( BM_HashMap_Lookup_StringView );

	BENCHMARK( BM_StdUnorderedSet_Insert_String );
	BENCHMARK( BM_StdUnorderedSet_Insert_StringView );
	BENCHMARK( BM_StringSet_Insert_String );
	BENCHMARK( BM_StringSet_Insert_StringView );

	BENCHMARK( BM_StdUnorderedSet_Lookup_String );
	BENCHMARK( BM_StdUnorderedSet_Lookup_StringView );
	BENCHMARK( BM_StringSet_Lookup_String );
	BENCHMARK( BM_StringSet_Lookup_StringView );

	BENCHMARK( BM_Memory_StdUnorderedMap );
	BENCHMARK( BM_Memory_StringMap );
	BENCHMARK( BM_Memory_HashMap );
}

BENCHMARK_MAIN();
