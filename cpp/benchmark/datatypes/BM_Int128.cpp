/**
 * @file BM_Int128.cpp
 * @brief Performance benchmarks for Int128 128-bit signed integer operations
 * @details Comprehensive benchmarks covering construction, arithmetic, and comparisons
 */

#include "dnv/vista/sdk/datatypes/Int128.h"

namespace dnv::vista::sdk::benchmarks
{
	//----------------------------------------------
	// Benchmark Data Generation
	//----------------------------------------------

	/**
	 * @brief Generate random test data for benchmarks
	 */
	class BenchmarkData
	{
	public:
		static constexpr size_t DATASET_SIZE = 10000;

		/* Fixed seed for reproducibility */
		BenchmarkData() : gen{ 42 }
		{
			values64.reserve( DATASET_SIZE );
			valuesInt128.reserve( DATASET_SIZE );

			std::uniform_int_distribution<std::uint64_t> dist64;
			std::uniform_int_distribution<std::uint32_t> dist32;

			for ( size_t i = 0; i < DATASET_SIZE; ++i )
			{
				std::uint64_t low = dist64( gen );
				/* Use 32-bit for high to avoid overflow issues */
				std::uint64_t high = dist32( gen );

				values64.push_back( low );
				valuesInt128.emplace_back( low, high );
			}
		}

		std::vector<std::uint64_t> values64;
		std::vector<datatypes::Int128> valuesInt128;

	private:
		std::mt19937_64 gen;
	};

	static BenchmarkData g_benchData;

	//----------------------------------------------
	// Construction Benchmarks
	//----------------------------------------------

	static void BM_Int128_ConstructionDefault( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			datatypes::Int128 value;
			benchmark::DoNotOptimize( value );
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_ConstructionDefault );

	static void BM_Int128_ConstructionFromUint64( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			datatypes::Int128 value( g_benchData.values64[index % BenchmarkData::DATASET_SIZE] );
			benchmark::DoNotOptimize( value );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_ConstructionFromUint64 );

	static void BM_Int128_ConstructionFromInt64( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			datatypes::Int128 value( static_cast<std::int64_t>( g_benchData.values64[index % BenchmarkData::DATASET_SIZE] ) );
			benchmark::DoNotOptimize( value );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_ConstructionFromInt64 );

	static void BM_Int128_ConstructionFromLowHigh( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			std::uint64_t low = g_benchData.values64[index % BenchmarkData::DATASET_SIZE];
			std::uint64_t high = g_benchData.values64[( index + 1 ) % BenchmarkData::DATASET_SIZE] & 0xFFFFFFFF;
			datatypes::Int128 value( low, high );
			benchmark::DoNotOptimize( value );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_ConstructionFromLowHigh );

	//----------------------------------------------
	// Arithmetic Operation Benchmarks
	//----------------------------------------------

	static void BM_Int128_Addition( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			const auto& b = g_benchData.valuesInt128[( index + 1 ) % BenchmarkData::DATASET_SIZE];
			datatypes::Int128 result = a + b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_Addition );

	static void BM_Int128_Subtraction( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			const auto& b = g_benchData.valuesInt128[( index + 1 ) % BenchmarkData::DATASET_SIZE];
			datatypes::Int128 result = a - b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_Subtraction );

	static void BM_Int128_Multiplication( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			const auto& b = g_benchData.valuesInt128[( index + 1 ) % BenchmarkData::DATASET_SIZE];
			datatypes::Int128 result = a * b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_Multiplication );

	static void BM_Int128_Division( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			auto b = g_benchData.valuesInt128[( index + 1 ) % BenchmarkData::DATASET_SIZE];

			/* Ensure divisor is not zero */
			if ( b.isZero() )
			{
				b = datatypes::Int128{ 1 };
			}

			datatypes::Int128 result = a / b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_Division );

	static void BM_Int128_Modulo( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			auto b = g_benchData.valuesInt128[( index + 1 ) % BenchmarkData::DATASET_SIZE];

			/* Ensure divisor is not zero */
			if ( b.isZero() )
			{
				b = datatypes::Int128{ 1 };
			}

			datatypes::Int128 result = a % b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_Modulo );

	static void BM_Int128_UnaryMinus( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			datatypes::Int128 result = -a;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_UnaryMinus );

	//----------------------------------------------
	// Comparison Operation Benchmarks
	//----------------------------------------------

	static void BM_Int128_Equality( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			const auto& b = g_benchData.valuesInt128[( index + 1 ) % BenchmarkData::DATASET_SIZE];
			bool result = a == b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_Equality );

	static void BM_Int128_LessThan( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			const auto& b = g_benchData.valuesInt128[( index + 1 ) % BenchmarkData::DATASET_SIZE];
			bool result = a < b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_LessThan );

	static void BM_Int128_GreaterThan( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			const auto& b = g_benchData.valuesInt128[( index + 1 ) % BenchmarkData::DATASET_SIZE];
			bool result = a > b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_GreaterThan );

	//----------------------------------------------
	// State Checking Benchmarks
	//----------------------------------------------

	static void BM_Int128_IsZero( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			bool result = a.isZero();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_IsZero );

	static void BM_Int128_IsNegative( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			bool result = a.isNegative();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_IsNegative );

	static void BM_Int128_AbsoluteValue( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			datatypes::Int128 result = a.abs();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_AbsoluteValue );

	//----------------------------------------------
	// Access Operation Benchmarks
	//----------------------------------------------

	static void BM_Int128_ToLow( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			std::uint64_t result = a.toLow();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_ToLow );

	static void BM_Int128_ToHigh( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			std::uint64_t result = a.toHigh();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_ToHigh );

	//----------------------------------------------
	// Comparative Benchmarks
	//----------------------------------------------

	static void BM_Int64_Addition_Baseline( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			std::int64_t a = static_cast<std::int64_t>( g_benchData.values64[index % BenchmarkData::DATASET_SIZE] );
			std::int64_t b = static_cast<std::int64_t>( g_benchData.values64[( index + 1 ) % BenchmarkData::DATASET_SIZE] );
			std::int64_t result = a + b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int64_Addition_Baseline );

	static void BM_Int64_Subtraction_Baseline( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			std::int64_t a = static_cast<std::int64_t>( g_benchData.values64[index % BenchmarkData::DATASET_SIZE] );
			std::int64_t b = static_cast<std::int64_t>( g_benchData.values64[( index + 1 ) % BenchmarkData::DATASET_SIZE] );
			std::int64_t result = a - b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int64_Subtraction_Baseline );

	static void BM_Int64_Multiplication_Baseline( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			std::int64_t a = static_cast<std::int64_t>( g_benchData.values64[index % BenchmarkData::DATASET_SIZE] );
			std::int64_t b = static_cast<std::int64_t>( g_benchData.values64[( index + 1 ) % BenchmarkData::DATASET_SIZE] );
			std::int64_t result = a * b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int64_Multiplication_Baseline );

	static void BM_Int64_Division_Baseline( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			std::int64_t a = static_cast<std::int64_t>( g_benchData.values64[index % BenchmarkData::DATASET_SIZE] );
			std::int64_t b = static_cast<std::int64_t>( g_benchData.values64[( index + 1 ) % BenchmarkData::DATASET_SIZE] );

			/* Ensure divisor is not zero */
			if ( b == 0 )
			{
				b = 1;
			}

			std::int64_t result = a / b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int64_Division_Baseline );

	static void BM_Int64_Modulo_Baseline( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			std::int64_t a = static_cast<std::int64_t>( g_benchData.values64[index % BenchmarkData::DATASET_SIZE] );
			std::int64_t b = static_cast<std::int64_t>( g_benchData.values64[( index + 1 ) % BenchmarkData::DATASET_SIZE] );

			/* Ensure divisor is not zero */
			if ( b == 0 )
			{
				b = 1;
			}

			std::int64_t result = a % b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int64_Modulo_Baseline );

	static void BM_Int64_UnaryMinus_Baseline( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			std::int64_t a = static_cast<std::int64_t>( g_benchData.values64[index % BenchmarkData::DATASET_SIZE] );
			std::int64_t result = -a;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int64_UnaryMinus_Baseline );

	static void BM_Int64_Equality_Baseline( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			std::int64_t a = static_cast<std::int64_t>( g_benchData.values64[index % BenchmarkData::DATASET_SIZE] );
			std::int64_t b = static_cast<std::int64_t>( g_benchData.values64[( index + 1 ) % BenchmarkData::DATASET_SIZE] );
			bool result = a == b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int64_Equality_Baseline );

	static void BM_Int64_LessThan_Baseline( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			std::int64_t a = static_cast<std::int64_t>( g_benchData.values64[index % BenchmarkData::DATASET_SIZE] );
			std::int64_t b = static_cast<std::int64_t>( g_benchData.values64[( index + 1 ) % BenchmarkData::DATASET_SIZE] );
			bool result = a < b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int64_LessThan_Baseline );

	static void BM_Uint64_Addition_Baseline( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			std::uint64_t a = g_benchData.values64[index % BenchmarkData::DATASET_SIZE];
			std::uint64_t b = g_benchData.values64[( index + 1 ) % BenchmarkData::DATASET_SIZE];
			std::uint64_t result = a + b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Uint64_Addition_Baseline );

	static void BM_Uint64_Multiplication_Baseline( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			std::uint64_t a = g_benchData.values64[index % BenchmarkData::DATASET_SIZE];
			std::uint64_t b = g_benchData.values64[( index + 1 ) % BenchmarkData::DATASET_SIZE];
			std::uint64_t result = a * b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Uint64_Multiplication_Baseline );

	//----------------------------------------------
	// Specialized Performance Tests
	//----------------------------------------------

	static void BM_Int128_ChainedArithmetic( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = g_benchData.valuesInt128[index % BenchmarkData::DATASET_SIZE];
			const auto& b = g_benchData.valuesInt128[( index + 1 ) % BenchmarkData::DATASET_SIZE];
			const auto& c = g_benchData.valuesInt128[( index + 2 ) % BenchmarkData::DATASET_SIZE];

			/* Test complex expression: (a + b) * c - a */
			datatypes::Int128 result{ ( a + b ) * c - a };
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_ChainedArithmetic );

	static void BM_Int128_FastPath64Bit( benchmark::State& state )
	{
		/* Test performance of operations that should use fast 64-bit paths */
		size_t index = 0;
		for ( auto _ : state )
		{
			datatypes::Int128 a( g_benchData.values64[index % BenchmarkData::DATASET_SIZE] );
			datatypes::Int128 b( g_benchData.values64[( index + 1 ) % BenchmarkData::DATASET_SIZE] );

			/* These should trigger fast path optimizations */
			datatypes::Int128 sum = a + b;
			datatypes::Int128 product = a * b;
			bool less = a < b;

			benchmark::DoNotOptimize( sum );
			benchmark::DoNotOptimize( product );
			benchmark::DoNotOptimize( less );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}
	BENCHMARK( BM_Int128_FastPath64Bit );
}

BENCHMARK_MAIN();
