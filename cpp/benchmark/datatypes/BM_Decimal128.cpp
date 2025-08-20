/**
 * @file BM_Decimal128.cpp
 * @brief Benchmarks for high-precision Decimal128 arithmetic operations
 */

#include "dnv/vista/sdk/datatypes/Decimal128.h"

namespace dnv::vista::sdk::benchmarks
{
	//=====================================================================
	// Benchmark data generators
	//=====================================================================

	class DecimalBenchmarkFixture : public benchmark::Fixture
	{
	public:
		void SetUp( const benchmark::State& ) override
		{
			/* Generate test data sets */
			generateRandomDecimals();
			generateTypicalBusinessValues();
			generateStringData();
		}

	protected:
		std::vector<datatypes::Decimal128> m_randomDecimals;
		std::vector<datatypes::Decimal128> m_businessValues;
		std::vector<std::string> m_stringValues;
		std::vector<double> m_doubleValues;

	private:
		void generateRandomDecimals()
		{
			std::random_device rd;
			std::mt19937 gen( rd() );
			std::uniform_real_distribution<double> dist( -1000000.0, 1000000.0 );

			m_randomDecimals.reserve( 10000 );
			for ( int i = 0; i < 10000; ++i )
			{
				m_randomDecimals.emplace_back( datatypes::Decimal128( dist( gen ) ) );
			}
		}

		void generateTypicalBusinessValues()
		{
			/* Typical financial/business values */
			m_businessValues = {
				datatypes::Decimal128( 0.01 ),
				datatypes::Decimal128( 0.99 ),
				datatypes::Decimal128( 1.00 ),
				datatypes::Decimal128( 9.99 ),
				datatypes::Decimal128( 10.50 ),
				datatypes::Decimal128( 99.95 ),
				datatypes::Decimal128( 100.00 ),
				datatypes::Decimal128( 999.99 ),
				datatypes::Decimal128( 1000.00 ),
				datatypes::Decimal128( 9999.99 ),
				datatypes::Decimal128( 12345.67 ),
				datatypes::Decimal128( 100000.00 ),
				datatypes::Decimal128( -0.01 ),
				datatypes::Decimal128( -9.99 ),
				datatypes::Decimal128( -100.00 ),
				datatypes::Decimal128( -1000.00 ) };
		}

		void generateStringData()
		{
			m_stringValues = {
				"0", "1", "-1",
				"0.1", "-0.1",
				"123.456", "-123.456",
				"999999.999999", "-999999.999999",
				"1000000000.123456789",
				"0.000000001",
				"12345678901234567890.123456789",
				"79228162514264337593543950335",
				"-79228162514264337593543950335" };

			m_doubleValues = {
				0.0, 1.0, -1.0,
				0.1, -0.1,
				123.456, -123.456,
				999999.999999, -999999.999999,
				1000000000.123456789,
				0.000000001,
				1.23e15, -1.23e15,
				1.79e308, -1.79e308 };
		}
	};

	//=====================================================================
	// Construction benchmarks
	//=====================================================================

	BENCHMARK_F( DecimalBenchmarkFixture, Construction_FromInt32 )( benchmark::State& state )
	{
		std::int32_t value = 12345;
		for ( auto _ : state )
		{
			datatypes::Decimal128 d( value );
			benchmark::DoNotOptimize( d );
			value = ( value + 1 ) % 1000000;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Construction_FromInt64 )( benchmark::State& state )
	{
		std::int64_t value = 1234567890123LL;
		for ( auto _ : state )
		{
			datatypes::Decimal128 d( value );
			benchmark::DoNotOptimize( d );
			value = ( value + 1 ) % 10000000000000LL;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Construction_FromDouble )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			datatypes::Decimal128 d( m_doubleValues[index % m_doubleValues.size()] );
			benchmark::DoNotOptimize( d );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Construction_FromString )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			datatypes::Decimal128 d( m_stringValues[index % m_stringValues.size()] );
			benchmark::DoNotOptimize( d );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Construction_Copy )( benchmark::State& state )
	{
		const datatypes::Decimal128 source( "123.456789" );
		for ( auto _ : state )
		{
			datatypes::Decimal128 d( source );
			benchmark::DoNotOptimize( d );
		}
		state.SetItemsProcessed( state.iterations() );
	}

	//=====================================================================
	// Arithmetic operation benchmarks
	//=====================================================================

	BENCHMARK_F( DecimalBenchmarkFixture, Arithmetic_Addition )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = m_randomDecimals[index % m_randomDecimals.size()];
			const auto& b = m_randomDecimals[( index + 1 ) % m_randomDecimals.size()];
			datatypes::Decimal128 result = a + b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Arithmetic_Subtraction )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = m_randomDecimals[index % m_randomDecimals.size()];
			const auto& b = m_randomDecimals[( index + 1 ) % m_randomDecimals.size()];
			datatypes::Decimal128 result = a - b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Arithmetic_Multiplication )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = m_businessValues[index % m_businessValues.size()];
			const auto& b = m_businessValues[( index + 1 ) % m_businessValues.size()];
			datatypes::Decimal128 result = a * b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Arithmetic_Division )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = m_businessValues[index % m_businessValues.size()];
			const auto& b = m_businessValues[( index + 1 ) % m_businessValues.size()];
			if ( !b.isZero() )
			{
				datatypes::Decimal128 result = a / b;
				benchmark::DoNotOptimize( result );
			}
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Arithmetic_InPlace_Addition )( benchmark::State& state )
	{
		auto accumulator = datatypes::Decimal128::zero();
		size_t index = 0;
		for ( auto _ : state )
		{
			accumulator += m_businessValues[index % m_businessValues.size()];
			benchmark::DoNotOptimize( accumulator );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	//=====================================================================
	// Comparison operation benchmarks
	//=====================================================================

	BENCHMARK_F( DecimalBenchmarkFixture, Comparison_Equality )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = m_randomDecimals[index % m_randomDecimals.size()];
			const auto& b = m_randomDecimals[( index + 1 ) % m_randomDecimals.size()];
			bool result = a == b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Comparison_LessThan )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& a = m_randomDecimals[index % m_randomDecimals.size()];
			const auto& b = m_randomDecimals[( index + 1 ) % m_randomDecimals.size()];
			bool result = a < b;
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	//=====================================================================
	// Mathematical function benchmarks
	//=====================================================================

	BENCHMARK_F( DecimalBenchmarkFixture, Math_Abs )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& value = m_randomDecimals[index % m_randomDecimals.size()];
			datatypes::Decimal128 result = value.abs();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Math_Round )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& value = m_randomDecimals[index % m_randomDecimals.size()];
			datatypes::Decimal128 result = value.round( 2 );
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Math_Truncate )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& value = m_randomDecimals[index % m_randomDecimals.size()];
			datatypes::Decimal128 result = value.truncate();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Math_Floor )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& value = m_randomDecimals[index % m_randomDecimals.size()];
			datatypes::Decimal128 result = value.floor();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Math_Ceiling )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& value = m_randomDecimals[index % m_randomDecimals.size()];
			datatypes::Decimal128 result = value.ceiling();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	//=====================================================================
	// Conversion benchmarks
	//=====================================================================

	BENCHMARK_F( DecimalBenchmarkFixture, Conversion_ToString )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& value = m_businessValues[index % m_businessValues.size()];
			std::string result = value.toString();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Conversion_ToDouble )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& value = m_businessValues[index % m_businessValues.size()];
			double result = value.toDouble();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Conversion_ToBits )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& value = m_businessValues[index % m_businessValues.size()];
			auto result = value.toBits();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	//=====================================================================
	// Parsing benchmarks
	//=====================================================================

	BENCHMARK_F( DecimalBenchmarkFixture, Parsing_TryParse_Valid )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& str = m_stringValues[index % m_stringValues.size()];
			datatypes::Decimal128 result;
			bool success = datatypes::Decimal128::tryParse( str, result );
			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( success );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Parsing_TryParse_Invalid )( benchmark::State& state )
	{
		const std::vector<std::string> invalidStrings = {
			"",
			"abc",
			"12.34.56",
			"1.2.3",
			"not_a_number",
			"1e10",
			"inf",
			"nan" };

		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& str = invalidStrings[index % invalidStrings.size()];
			datatypes::Decimal128 result;
			bool success = datatypes::Decimal128::tryParse( str, result );
			benchmark::DoNotOptimize( result );
			benchmark::DoNotOptimize( success );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	//=====================================================================
	// Property access benchmarks
	//=====================================================================

	BENCHMARK_F( DecimalBenchmarkFixture, Properties_IsZero )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& value = m_randomDecimals[index % m_randomDecimals.size()];
			bool result = value.isZero();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Properties_IsNegative )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& value = m_randomDecimals[index % m_randomDecimals.size()];
			bool result = value.isNegative();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Properties_Scale )( benchmark::State& state )
	{
		size_t index = 0;
		for ( auto _ : state )
		{
			const auto& value = m_randomDecimals[index % m_randomDecimals.size()];
			std::uint8_t result = value.scale();
			benchmark::DoNotOptimize( result );
			++index;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	//=====================================================================
	// Complex scenario benchmarks
	//=====================================================================

	BENCHMARK_F( DecimalBenchmarkFixture, Scenario_FinancialCalculation )( benchmark::State& state )
	{
		/* Simulate typical financial calculation: principal * (1 + rate)^periods */
		const datatypes::Decimal128 principal( "10000.00" );
		const datatypes::Decimal128 rate( "0.05" );
		const datatypes::Decimal128 one = datatypes::Decimal128::one();

		for ( auto _ : state )
		{
			datatypes::Decimal128 factor = one + rate;
			datatypes::Decimal128 result = principal;

			/* Compound for 12 periods (monthly for 1 year) */
			for ( int period = 0; period < 12; ++period )
			{
				result *= factor;
			}

			benchmark::DoNotOptimize( result );
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK_F( DecimalBenchmarkFixture, Scenario_SummationAccuracy )( benchmark::State& state )
	{
		/* Test summation accuracy with many small values */
		const datatypes::Decimal128 smallValue{ "0.01" };

		for ( auto _ : state )
		{
			auto sum = datatypes::Decimal128::zero();

			/* Sum 1000 small values */
			for ( int i = 0; i < 1000; ++i )
			{
				sum += smallValue;
			}

			benchmark::DoNotOptimize( sum );
		}
		state.SetItemsProcessed( state.iterations() );
	}

	//=====================================================================
	// Pure double benchmarks for comparison against Decimal128
	//=====================================================================

	static void BM_Double_Addition( benchmark::State& state )
	{
		double a = 123.456;
		double b = 789.012;

		for ( auto _ : state )
		{
			double result = a + b;
			benchmark::DoNotOptimize( result );
			a += 0.001;
			b += 0.001;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	static void BM_Double_Subtraction( benchmark::State& state )
	{
		double a = 789.012;
		double b = 123.456;

		for ( auto _ : state )
		{
			double result = a - b;
			benchmark::DoNotOptimize( result );
			a += 0.001;
			b += 0.001;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	static void BM_Double_Multiplication( benchmark::State& state )
	{
		double a = 123.456;
		double b = 2.5;

		for ( auto _ : state )
		{
			double result = a * b;
			benchmark::DoNotOptimize( result );
			a += 0.001;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	static void BM_Double_Division( benchmark::State& state )
	{
		double a = 789.012;
		double b = 2.5;

		for ( auto _ : state )
		{
			double result = a / b;
			benchmark::DoNotOptimize( result );
			a += 0.001;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	static void BM_Double_ToString( benchmark::State& state )
	{
		double value = 123.456789;

		for ( auto _ : state )
		{
			std::string result = std::to_string( value );
			benchmark::DoNotOptimize( result );
			value += 0.000001;
		}
		state.SetItemsProcessed( state.iterations() );
	}

	BENCHMARK( BM_Double_Addition );
	BENCHMARK( BM_Double_Subtraction );
	BENCHMARK( BM_Double_Multiplication );
	BENCHMARK( BM_Double_Division );
	BENCHMARK( BM_Double_ToString );
}

BENCHMARK_MAIN();
