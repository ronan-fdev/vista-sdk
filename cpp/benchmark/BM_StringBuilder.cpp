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
 * @file BM_StringBuilder.cpp
 * @brief Performance benchmark for StringBuilder wrapper overhead
 * @details Compares dnv::vista::sdk::StringBuilder wrapper against direct nfx usage and std::string
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/StringBuilder.h>
#include <nfx/string/StringBuilder.h>

#include <sstream>
#include <string>

namespace dnv::vista::sdk::benchmark
{
	//=====================================================================
	// StringBuilder benchmark suite
	//=====================================================================

	//----------------------------------------------
	// Simple append operations
	//----------------------------------------------

	static void BM_StdString_ShortAppends( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			std::string result;
			result += "qty";
			result += "-";
			result += "temperature";
			result += "/";

			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_NFX_StringBuilder_ShortAppends( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto sb = nfx::string::StringBuilder();

			sb.append( "qty" );
			sb.append( "-" );
			sb.append( "temperature" );
			sb.append( "/" );

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_DNV_StringBuilder_ShortAppends( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto sb = StringBuilder();

			sb.append( "qty" );
			sb.append( "-" );
			sb.append( "temperature" );
			sb.append( "/" );

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	//----------------------------------------------
	// Stream operator usage
	//----------------------------------------------

	static void BM_StdStringStream_StreamOps( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			std::ostringstream oss;
			oss << "qty" << "-" << "temperature" << "/" << "cnt" << "-" << "oil" << "/" << "pos" << "-" << "1" << "/";

			auto result = oss.str();
			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_NFX_StringBuilder_StreamOps( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto sb = nfx::string::StringBuilder();

			sb << "qty" << "-" << "temperature" << "/" << "cnt" << "-" << "oil" << "/" << "pos" << "-" << "1" << "/";

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_DNV_StringBuilder_StreamOps( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto sb = StringBuilder();

			sb << "qty" << "-" << "temperature" << "/" << "cnt" << "-" << "oil" << "/" << "pos" << "-" << "1" << "/";

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	//----------------------------------------------
	// MetadataTag-like usage (realistic workload)
	//----------------------------------------------

	static void BM_StdString_MetadataPath( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			std::string result;
			result.reserve( 256 );

			result += "qty";
			result += "-";
			result += "temperature";
			result += "/";
			result += "cnt";
			result += "-";
			result += "oil";
			result += "/";
			result += "pos";
			result += "-";
			result += "1";
			result += "/";
			result += "state";
			result += "-";
			result += "running";
			result += "/";
			result += "detail";
			result += "-";
			result += "alarm";
			result += "/";
			result += "cmd";
			result += "-";
			result += "start";
			result += "/";

			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_NFX_StringBuilder_MetadataPath( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto sb = nfx::string::StringBuilder( 256 );

			// Simulate building a VIS Local ID metadata path
			sb << "qty" << "-" << "temperature" << "/";
			sb << "cnt" << "-" << "oil" << "/";
			sb << "pos" << "-" << "1" << "/";
			sb << "state" << "-" << "running" << "/";
			sb << "detail" << "-" << "alarm" << "/";
			sb << "cmd" << "-" << "start" << "/";

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_DNV_StringBuilder_MetadataPath( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto sb = StringBuilder( 256 );

			// Simulate building a VIS Local ID metadata path
			sb << "qty" << "-" << "temperature" << "/";
			sb << "cnt" << "-" << "oil" << "/";
			sb << "pos" << "-" << "1" << "/";
			sb << "state" << "-" << "running" << "/";
			sb << "detail" << "-" << "alarm" << "/";
			sb << "cmd" << "-" << "start" << "/";

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	//----------------------------------------------
	// Large string building
	//----------------------------------------------

	static void BM_StdString_LargeString( ::benchmark::State& state )
	{
		const int iterations = state.range( 0 );

		for ( auto _ : state )
		{
			std::string result;
			result.reserve( iterations * 20 );

			for ( int i = 0; i < iterations; ++i )
			{
				result += "item-";
				result += std::to_string( i );
				result += "/";
			}

			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_NFX_StringBuilder_LargeString( ::benchmark::State& state )
	{
		const int iterations = state.range( 0 );

		for ( auto _ : state )
		{
			auto sb = nfx::string::StringBuilder( iterations * 20 );

			for ( int i = 0; i < iterations; ++i )
			{
				sb << "item-" << i << "/";
			}

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_DNV_StringBuilder_LargeString( ::benchmark::State& state )
	{
		const int iterations = state.range( 0 );

		for ( auto _ : state )
		{
			auto sb = StringBuilder( iterations * 20 );

			for ( int i = 0; i < iterations; ++i )
			{
				sb << "item-" << i << "/";
			}

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	//----------------------------------------------
	// Multiple toString()
	//----------------------------------------------

	static void BM_NFX_StringBuilder_MultipleToString( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto sb = nfx::string::StringBuilder();

			sb << "qty" << "-" << "temperature";
			auto result1 = sb.toString();
			::benchmark::DoNotOptimize( result1 );

			sb << "/" << "cnt" << "-" << "oil";
			auto result2 = sb.toString();
			::benchmark::DoNotOptimize( result2 );

			sb << "/" << "pos" << "-" << "1";
			auto result3 = sb.toString();
			::benchmark::DoNotOptimize( result3 );
		}
	}

	static void BM_DNV_StringBuilder_MultipleToString( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto sb = StringBuilder();

			sb << "qty" << "-" << "temperature";
			auto result1 = sb.toString();
			::benchmark::DoNotOptimize( result1 );

			sb << "/" << "cnt" << "-" << "oil";
			auto result2 = sb.toString();
			::benchmark::DoNotOptimize( result2 );

			sb << "/" << "pos" << "-" << "1";
			auto result3 = sb.toString();
			::benchmark::DoNotOptimize( result3 );
		}
	}

	//----------------------------------------------
	// ToStringView vs ToString
	//----------------------------------------------

	static void BM_NFX_StringBuilder_ToString( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto sb = nfx::string::StringBuilder();
			sb << "qty" << "-" << "temperature" << "/" << "cnt" << "-" << "oil" << "/";

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_NFX_StringBuilder_ToStringView( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto sb = nfx::string::StringBuilder();
			sb << "qty" << "-" << "temperature" << "/" << "cnt" << "-" << "oil" << "/";

			auto view = sb.toStringView();
			::benchmark::DoNotOptimize( view );
		}
	}

	static void BM_DNV_StringBuilder_ToString( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto sb = StringBuilder();
			sb << "qty" << "-" << "temperature" << "/" << "cnt" << "-" << "oil" << "/";

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_DNV_StringBuilder_ToStringView( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto sb = StringBuilder();
			sb << "qty" << "-" << "temperature" << "/" << "cnt" << "-" << "oil" << "/";

			auto view = sb.toStringView();
			::benchmark::DoNotOptimize( view );
		}
	}

	//----------------------------------------------
	// Builder reuse with clear()
	//----------------------------------------------

	static void BM_NFX_StringBuilder_Recreate( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto sb = nfx::string::StringBuilder( 256 );
			sb << "qty" << "-" << "temperature" << "/";
			auto result1 = sb.toString();
			::benchmark::DoNotOptimize( result1 );
		}
	}

	static void BM_NFX_StringBuilder_ClearReuse( ::benchmark::State& state )
	{
		auto sb = nfx::string::StringBuilder( 256 );

		for ( auto _ : state )
		{
			sb.clear();
			sb << "qty" << "-" << "temperature" << "/";
			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_DNV_StringBuilder_Recreate( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto sb = StringBuilder( 256 );
			sb << "qty" << "-" << "temperature" << "/";
			auto result1 = sb.toString();
			::benchmark::DoNotOptimize( result1 );
		}
	}

	static void BM_DNV_StringBuilder_ClearReuse( ::benchmark::State& state )
	{
		auto sb = StringBuilder( 256 );

		for ( auto _ : state )
		{
			sb.clear();
			sb << "qty" << "-" << "temperature" << "/";
			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}
} // namespace dnv::vista::sdk::benchmark

//=====================================================================
// Benchmarks registration
//=====================================================================

//----------------------------------------------
// Simple append operations
//----------------------------------------------

BENCHMARK( dnv::vista::sdk::benchmark::BM_StdString_ShortAppends );
BENCHMARK( dnv::vista::sdk::benchmark::BM_NFX_StringBuilder_ShortAppends );
BENCHMARK( dnv::vista::sdk::benchmark::BM_DNV_StringBuilder_ShortAppends );

//----------------------------------------------
// Stream operator usage
//----------------------------------------------

BENCHMARK( dnv::vista::sdk::benchmark::BM_StdStringStream_StreamOps );
BENCHMARK( dnv::vista::sdk::benchmark::BM_NFX_StringBuilder_StreamOps );
BENCHMARK( dnv::vista::sdk::benchmark::BM_DNV_StringBuilder_StreamOps );

//----------------------------------------------
// MetadataTag-like usage (realistic workload)
//----------------------------------------------

BENCHMARK( dnv::vista::sdk::benchmark::BM_StdString_MetadataPath );
BENCHMARK( dnv::vista::sdk::benchmark::BM_NFX_StringBuilder_MetadataPath );
BENCHMARK( dnv::vista::sdk::benchmark::BM_DNV_StringBuilder_MetadataPath );

//----------------------------------------------
// Large string building
//----------------------------------------------

BENCHMARK( dnv::vista::sdk::benchmark::BM_StdString_LargeString )->Arg( 10 )->Arg( 100 )->Arg( 1000 );
BENCHMARK( dnv::vista::sdk::benchmark::BM_NFX_StringBuilder_LargeString )->Arg( 10 )->Arg( 100 )->Arg( 1000 );
BENCHMARK( dnv::vista::sdk::benchmark::BM_DNV_StringBuilder_LargeString )->Arg( 10 )->Arg( 100 )->Arg( 1000 );

//----------------------------------------------
// Multiple toString()
//----------------------------------------------

BENCHMARK( dnv::vista::sdk::benchmark::BM_NFX_StringBuilder_MultipleToString );
BENCHMARK( dnv::vista::sdk::benchmark::BM_DNV_StringBuilder_MultipleToString );

//----------------------------------------------
// ToStringView vs ToString
//----------------------------------------------

BENCHMARK( dnv::vista::sdk::benchmark::BM_NFX_StringBuilder_ToString );
BENCHMARK( dnv::vista::sdk::benchmark::BM_NFX_StringBuilder_ToStringView );
BENCHMARK( dnv::vista::sdk::benchmark::BM_DNV_StringBuilder_ToString );
BENCHMARK( dnv::vista::sdk::benchmark::BM_DNV_StringBuilder_ToStringView );

//----------------------------------------------
// Builder reuse with clear()
//----------------------------------------------

BENCHMARK( dnv::vista::sdk::benchmark::BM_NFX_StringBuilder_Recreate );
BENCHMARK( dnv::vista::sdk::benchmark::BM_NFX_StringBuilder_ClearReuse );
BENCHMARK( dnv::vista::sdk::benchmark::BM_DNV_StringBuilder_Recreate );
BENCHMARK( dnv::vista::sdk::benchmark::BM_DNV_StringBuilder_ClearReuse );

//----------------------------------------------
// Benchmark main entry point
//----------------------------------------------

BENCHMARK_MAIN();
