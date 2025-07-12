/**
 * @file BM_CodebooksLookup.cpp
 * @brief Comprehensive performance comparison of different data structures for Vista SDK codebook access
 *
 * BENCHMARKS INCLUDED:
 * - BM_CodebooksInstance: SDK access via direct owned instance
 * - BM_CodebooksReference: SDK access via reference wrapper (cached)
 * - BM_Array: Linear search through std::array (fastest for small datasets)
 * - BM_Vector: Linear search through std::vector
 * - BM_UnorderedMap: Hash table lookup via std::unordered_map
 * - BM_ChdDictionary: Perfect hash lookup via ChdDictionary
 * - BM_Map: Red-black tree lookup via std::map
 * - BM_CodebooksAPI: SDK access via codebook() method call
 * - BM_CodebooksVISCall: SDK access via VIS::instance() call (worst case)
 *
 * PURPOSE: Determine optimal data structure for codebook lookups with 3 elements
 */

#include "pch.h"

#include "dnv/vista/sdk/Codebook.h"
#include "dnv/vista/sdk/Codebooks.h"
#include "dnv/vista/sdk/CodebookName.h"
#include "dnv/vista/sdk/ChdDictionary.h"
#include "dnv/vista/sdk/VIS.h"

using namespace dnv::vista::sdk;

// Hash specialization for CodebookName to enable std::unordered_map
namespace std
{
	template <>
	struct hash<dnv::vista::sdk::CodebookName>
	{
		size_t operator()( dnv::vista::sdk::CodebookName key ) const noexcept
		{
			return std::hash<int>{}( static_cast<int>( key ) );
		}
	};
}

namespace dnv::vista::sdk::benchmarks
{
	//=====================================================================
	// CodebooksLookup - Multiple STL container tests
	//=====================================================================

	class CodebooksLookup
	{
	private:
		const Codebooks m_codebooksInstance;
		std::optional<std::reference_wrapper<const dnv::vista::sdk::Codebooks>> m_codebooksReference;
		std::array<std::pair<CodebookName, Codebook>, 3> m_array;
		std::vector<std::pair<CodebookName, Codebook>> m_vector;
		std::unordered_map<CodebookName, Codebook> m_unorderedMap;
		std::unique_ptr<dnv::vista::sdk::ChdDictionary<Codebook>> m_chdDictionary;
		std::map<CodebookName, Codebook> m_map;

		bool tryGetValue( const std::array<std::pair<CodebookName, Codebook>, 3>& arr, CodebookName key, const Codebook*& outValue ) const noexcept
		{
			for ( const auto& [k, v] : arr )
			{
				if ( k == key )
				{
					outValue = &v;

					return true;
				}
			}

			outValue = nullptr;

			return false;
		}

		bool tryGetValue( const std::vector<std::pair<CodebookName, Codebook>>& vec, CodebookName key, const Codebook*& outValue ) const noexcept
		{
			for ( const auto& [k, v] : vec )
			{
				if ( k == key )
				{
					outValue = &v;

					return true;
				}
			}

			outValue = nullptr;

			return false;
		}

		bool tryGetValue( const dnv::vista::sdk::ChdDictionary<Codebook>& dict, CodebookName key, const Codebook*& outValue ) const noexcept
		{
			static const char* const keyMappings[] = {
				nullptr,
				"Quantity",
				"Content",
				"Calculation",
				"State",
				"Command",
				"Type",
				"FunctionalServices",
				"MaintenanceCategory",
				"ActivityType",
				"Position",
				"Detail" };

			const int keyIndex = static_cast<int>( key );
			if ( keyIndex < 1 || keyIndex >= static_cast<int>( std::size( keyMappings ) ) || keyMappings[keyIndex] == nullptr )
			{
				outValue = nullptr;
				return false;
			}

			return dict.tryGetValue( std::string_view( keyMappings[keyIndex] ), outValue );
		}

		template <typename T>
		bool tryGetValue( const std::unordered_map<CodebookName, T>& map, CodebookName key, const T*& outValue ) const noexcept
		{
			auto it = map.find( key );
			if ( it != map.end() )
			{
				outValue = &it->second;

				return true;
			}

			outValue = nullptr;

			return false;
		}

		template <typename T>
		bool tryGetValue( const std::map<CodebookName, T>& map, CodebookName key, const T*& outValue ) const noexcept
		{
			auto it = map.find( key );
			if ( it != map.end() )
			{
				outValue = &it->second;

				return true;
			}

			outValue = nullptr;

			return false;
		}

	public:
		CodebooksLookup() = default;

		void Setup()
		{
			/* Initialize VIS instance and get codebooks reference */
			auto& vis = VIS::instance();
			m_codebooksReference = std::cref( vis.codebooks( VisVersion::v3_7a ) );
			const auto& codebooks_ref = m_codebooksReference->get();

			/* Setup array (fixed-size, stack allocated) */
			m_array[0] = { CodebookName::Quantity, codebooks_ref[CodebookName::Quantity] };
			m_array[1] = { CodebookName::Type, codebooks_ref[CodebookName::Type] };
			m_array[2] = { CodebookName::Detail, codebooks_ref[CodebookName::Detail] };

			/* Setup vector (dynamic array, heap allocated) */
			m_vector.clear();
			m_vector.reserve( 3 );
			m_vector.emplace_back( CodebookName::Quantity, codebooks_ref[CodebookName::Quantity] );
			m_vector.emplace_back( CodebookName::Type, codebooks_ref[CodebookName::Type] );
			m_vector.emplace_back( CodebookName::Detail, codebooks_ref[CodebookName::Detail] );

			/* Setup unordered_map (hash table, heap allocated) */
			m_unorderedMap.clear();
			m_unorderedMap.reserve( 3 );
			m_unorderedMap[CodebookName::Quantity] = codebooks_ref[CodebookName::Quantity];
			m_unorderedMap[CodebookName::Type] = codebooks_ref[CodebookName::Type];
			m_unorderedMap[CodebookName::Detail] = codebooks_ref[CodebookName::Detail];

			/* Setup ChdDictionary (optimized read-only hash table) */
			std::vector<std::pair<std::string, Codebook>> chdItems;
			chdItems.reserve( 3 );
			chdItems.emplace_back( "Quantity", codebooks_ref[CodebookName::Quantity] );
			chdItems.emplace_back( "Type", codebooks_ref[CodebookName::Type] );
			chdItems.emplace_back( "Detail", codebooks_ref[CodebookName::Detail] );
			m_chdDictionary = std::make_unique<dnv::vista::sdk::ChdDictionary<Codebook>>( std::move( chdItems ) );

			/* Setup map (red-black tree) */
			m_map.clear();
			m_map[CodebookName::Quantity] = codebooks_ref[CodebookName::Quantity];
			m_map[CodebookName::Type] = codebooks_ref[CodebookName::Type];
			m_map[CodebookName::Detail] = codebooks_ref[CodebookName::Detail];
		}

		//----------------------------------------------
		// Benchmark methods for different containers
		//----------------------------------------------

		bool CodebooksInstance()
		{
			const Codebook* a = &m_codebooksInstance[CodebookName::Quantity];
			const Codebook* b = &m_codebooksInstance[CodebookName::Type];
			const Codebook* c = &m_codebooksInstance[CodebookName::Detail];

			return ( a != nullptr ) && ( b != nullptr ) && ( c != nullptr );
		}

		bool CodebooksReference()
		{
			const auto& codebooks_ref = m_codebooksReference->get();

			const Codebook* a = &codebooks_ref[CodebookName::Quantity];
			const Codebook* b = &codebooks_ref[CodebookName::Type];
			const Codebook* c = &codebooks_ref[CodebookName::Detail];

			return ( a != nullptr ) && ( b != nullptr ) && ( c != nullptr );
		}

		bool Array()
		{
			const Codebook* dummy1 = nullptr;
			const Codebook* dummy2 = nullptr;
			const Codebook* dummy3 = nullptr;

			return tryGetValue( m_array, CodebookName::Quantity, dummy1 ) &&
				   tryGetValue( m_array, CodebookName::Type, dummy2 ) &&
				   tryGetValue( m_array, CodebookName::Detail, dummy3 );
		}

		bool Vector()
		{
			const Codebook* dummy1 = nullptr;
			const Codebook* dummy2 = nullptr;
			const Codebook* dummy3 = nullptr;

			return tryGetValue( m_vector, CodebookName::Quantity, dummy1 ) &&
				   tryGetValue( m_vector, CodebookName::Type, dummy2 ) &&
				   tryGetValue( m_vector, CodebookName::Detail, dummy3 );
		}

		bool UnorderedMap()
		{
			const Codebook* dummy1 = nullptr;
			const Codebook* dummy2 = nullptr;
			const Codebook* dummy3 = nullptr;

			return tryGetValue( m_unorderedMap, CodebookName::Quantity, dummy1 ) &&
				   tryGetValue( m_unorderedMap, CodebookName::Type, dummy2 ) &&
				   tryGetValue( m_unorderedMap, CodebookName::Detail, dummy3 );
		}

		bool ChdDictionary()
		{
			const Codebook* dummy1 = nullptr;
			const Codebook* dummy2 = nullptr;
			const Codebook* dummy3 = nullptr;

			return tryGetValue( *m_chdDictionary, CodebookName::Quantity, dummy1 ) &&
				   tryGetValue( *m_chdDictionary, CodebookName::Type, dummy2 ) &&
				   tryGetValue( *m_chdDictionary, CodebookName::Detail, dummy3 );
		}

		bool Map()
		{
			const Codebook* dummy1 = nullptr;
			const Codebook* dummy2 = nullptr;
			const Codebook* dummy3 = nullptr;

			return tryGetValue( m_map, CodebookName::Quantity, dummy1 ) &&
				   tryGetValue( m_map, CodebookName::Type, dummy2 ) &&
				   tryGetValue( m_map, CodebookName::Detail, dummy3 );
		}

		bool CodebooksAPI()
		{
			auto a = m_codebooksInstance.codebook( CodebookName::Quantity );
			auto b = m_codebooksInstance.codebook( CodebookName::Type );
			auto c = m_codebooksInstance.codebook( CodebookName::Detail );

			return ( !a.rawData().empty() ) && ( !b.rawData().empty() ) && ( !c.rawData().empty() );
		}

		bool CodebooksVISCall()
		{
			auto codebooks = VIS::instance().codebooks( VisVersion::v3_7a );

			const Codebook* a = &codebooks[CodebookName::Quantity];
			const Codebook* b = &codebooks[CodebookName::Type];
			const Codebook* c = &codebooks[CodebookName::Detail];

			return ( a != nullptr ) && ( b != nullptr ) && ( c != nullptr );
		}
	};

	//=====================================================================
	// Benchmark setup
	//=====================================================================

	static CodebooksLookup g_benchmarkInstance;

	static void BM_Setup( benchmark::State& state )
	{
		if ( state.thread_index() == 0 )
		{
			g_benchmarkInstance.Setup();
		}
	}

	//=====================================================================
	// Benchmark wrappers
	//=====================================================================

	/**  @brief Direct owned codebooks instance benchmark */
	static void BM_CodebooksInstance( benchmark::State& state )
	{
		BM_Setup( state );
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.CodebooksInstance();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief Direct SDK reference benchmark */
	static void BM_CodebooksReference( benchmark::State& state )
	{
		BM_Setup( state );
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.CodebooksReference();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief Fixed array benchmark( std::array with linear search ) */
	static void BM_Array( benchmark::State& state )
	{
		BM_Setup( state );
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.Array();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief Dynamic vector benchmark( std::vector with linear search ) */
	static void BM_Vector( benchmark::State& state )
	{
		BM_Setup( state );
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.Vector();
			benchmark::DoNotOptimize( result );
		}
	}

	/**  @brief Hash table benchmark( std::unordered_map ) */
	static void BM_UnorderedMap( benchmark::State& state )
	{
		BM_Setup( state );
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.UnorderedMap();
			benchmark::DoNotOptimize( result );
		}
	}

	/**  @brief Optimized read-only hash table benchmark( ChdDictionary ) */
	static void BM_ChdDictionary( benchmark::State& state )
	{
		BM_Setup( state );
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.ChdDictionary();
			benchmark::DoNotOptimize( result );
		}
	}

	/**  @brief Red - black tree benchmark( std::map ) */
	static void BM_Map( benchmark::State& state )
	{
		BM_Setup( state );
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.Map();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief SDK method call benchmark (codebook() method */
	static void BM_CodebooksAPI( benchmark::State& state )
	{
		BM_Setup( state );
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.CodebooksAPI();
			benchmark::DoNotOptimize( result );
		}
	}

	/**  @brief VIS singleton call benchmark */
	static void BM_CodebooksVISCall( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.CodebooksVISCall();
			benchmark::DoNotOptimize( result );
		}
	}

	//=====================================================================
	// Benchmark registrations
	//=====================================================================
	BENCHMARK( BM_CodebooksInstance )->MinTime( 10.0 );
	BENCHMARK( BM_CodebooksReference )->MinTime( 10.0 );
	BENCHMARK( BM_Array )->MinTime( 10.0 );
	BENCHMARK( BM_Vector )->MinTime( 10.0 );
	BENCHMARK( BM_UnorderedMap )->MinTime( 10.0 );
	BENCHMARK( BM_ChdDictionary )->MinTime( 10.0 );
	BENCHMARK( BM_Map )->MinTime( 10.0 );
	BENCHMARK( BM_CodebooksAPI )->MinTime( 10.0 );
	BENCHMARK( BM_CodebooksVISCall )->MinTime( 10.0 );
}

BENCHMARK_MAIN();
