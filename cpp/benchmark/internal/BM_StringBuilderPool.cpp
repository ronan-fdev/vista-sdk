/**
 * @file BM_StringBuilderPool.cpp
 * @brief StringBuilderPool performance analysis
 */

#include "dnv/vista/sdk/internal/StringBuilderPool.h"

namespace dnv::vista::sdk::benchmarks
{
	//=====================================================================
	// Test data generation
	//=====================================================================

	static std::string generateTestString( size_t length )
	{
		static constexpr std::string_view chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
		std::string result;
		result.reserve( length );

		for ( size_t i = 0; i < length; ++i )
		{
			result += chars[i % chars.size()];
		}
		return result;
	}

	//=====================================================================
	// StringBuilder benchmarks
	//=====================================================================

	//----------------------------------------------
	// String construction with builder
	//----------------------------------------------

	static void BM_StringBuilder_Construction( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();
			benchmark::DoNotOptimize( builder );
		}
	}

	static void BM_StringBuilder_AppendString( benchmark::State& state )
	{
		const auto testString = generateTestString( static_cast<size_t>( state.range( 0 ) ) );

		for ( auto _ : state )
		{
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();

			builder.append( testString );
			benchmark::DoNotOptimize( builder );
		}

		state.SetBytesProcessed( static_cast<int64_t>( state.iterations() * testString.size() ) );
	}

	static void BM_StringBuilder_AppendMultipleStrings( benchmark::State& state )
	{
		const size_t stringCount = static_cast<size_t>( state.range( 0 ) );
		const auto testString = generateTestString( 100 );

		for ( auto _ : state )
		{
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();

			for ( size_t i = 0; i < stringCount; ++i )
			{
				builder.append( testString );
			}
			benchmark::DoNotOptimize( builder );
		}

		state.SetBytesProcessed( static_cast<int64_t>( state.iterations() * stringCount * testString.size() ) );
	}

	//----------------------------------------------
	// String construction without builder benchmarks
	//----------------------------------------------

	static void BM_StdString_AppendString( benchmark::State& state )
	{
		const auto testString = generateTestString( static_cast<size_t>( state.range( 0 ) ) );

		for ( auto _ : state )
		{
			std::string result;
			result += testString;
			benchmark::DoNotOptimize( result );
		}

		state.SetBytesProcessed( static_cast<int64_t>( state.iterations() * testString.size() ) );
	}

	static void BM_StdString_AppendMultipleStrings( benchmark::State& state )
	{
		const size_t stringCount = static_cast<size_t>( state.range( 0 ) );
		const auto testString = generateTestString( 100 );

		for ( auto _ : state )
		{
			std::string result;
			for ( size_t i = 0; i < stringCount; ++i )
			{
				result += testString;
			}
			benchmark::DoNotOptimize( result );
		}

		state.SetBytesProcessed( static_cast<int64_t>( state.iterations() * stringCount * testString.size() ) );
	}

	static void BM_StdString_AppendMultipleStrings_WithReserve( benchmark::State& state )
	{
		const size_t stringCount = static_cast<size_t>( state.range( 0 ) );
		const auto testString = generateTestString( 100 );

		for ( auto _ : state )
		{
			std::string result;
			result.reserve( stringCount * testString.size() );
			for ( size_t i = 0; i < stringCount; ++i )
			{
				result += testString;
			}
			benchmark::DoNotOptimize( result );
		}

		state.SetBytesProcessed( static_cast<int64_t>( state.iterations() * stringCount * testString.size() ) );
	}

	static void BM_FmtMemoryBuffer_AppendString( benchmark::State& state )
	{
		const auto testString = generateTestString( static_cast<size_t>( state.range( 0 ) ) );

		for ( auto _ : state )
		{
			fmt::memory_buffer buffer;
			fmt::format_to( std::back_inserter( buffer ), "{}", testString );
			auto result = fmt::to_string( buffer );
			benchmark::DoNotOptimize( result );
		}

		state.SetBytesProcessed( static_cast<int64_t>( state.iterations() * testString.size() ) );
	}

	static void BM_FmtMemoryBuffer_AppendMultipleStrings( benchmark::State& state )
	{
		const size_t stringCount = static_cast<size_t>( state.range( 0 ) );
		const auto testString = generateTestString( 100 );

		for ( auto _ : state )
		{
			fmt::memory_buffer buffer;
			for ( size_t i = 0; i < stringCount; ++i )
			{
				fmt::format_to( std::back_inserter( buffer ), "{}", testString );
			}
			auto result = fmt::to_string( buffer );
			benchmark::DoNotOptimize( result );
		}

		state.SetBytesProcessed( static_cast<int64_t>( state.iterations() * stringCount * testString.size() ) );
	}

	static void BM_PlainCharArray_AppendMultipleStrings( benchmark::State& state )
	{
		const size_t stringCount = static_cast<size_t>( state.range( 0 ) );
		const auto testString = generateTestString( 100 );
		const size_t totalSize = stringCount * testString.size();

		for ( auto _ : state )
		{
			auto buffer = std::make_unique<char[]>( totalSize + 1 );
			char* pos = buffer.get();

			for ( size_t i = 0; i < stringCount; ++i )
			{
				std::memcpy( pos, testString.data(), testString.size() );
				pos += testString.size();
			}
			*pos = '\0';

			benchmark::DoNotOptimize( buffer.get() );
		}

		state.SetBytesProcessed( static_cast<int64_t>( state.iterations() * stringCount * testString.size() ) );
	}

	//----------------------------------------------
	// Pool effectiveness and statistics benchmarks
	//----------------------------------------------

	static void BM_PoolEffectiveness( benchmark::State& state )
	{
		// Reset statistics for clean measurement
		internal::StringBuilderPool::resetStats();

		// Warm up the pool
		for ( int i = 0; i < 10000; ++i )
		{
			auto lease = internal::StringBuilderPool::lease();
		}

		// Reset stats after warmup to get clean measurement
		internal::StringBuilderPool::resetStats();

		for ( auto _ : state )
		{
			auto lease = internal::StringBuilderPool::lease();
		}

		// Get pool statistics using the public interface
		const auto stats = internal::StringBuilderPool::stats();
		uint64_t totalRequests = stats.totalRequests;
		uint64_t threadLocalHits = stats.threadLocalHits;
		uint64_t dynamicStringBufferPoolHits = stats.dynamicStringBufferPoolHits;
		uint64_t newAllocations = stats.newAllocations;

		// Calculate metrics
		double hitRate = stats.hitRate;
		double threadLocalRate = totalRequests > 0 ? static_cast<double>( threadLocalHits ) / static_cast<double>( totalRequests ) : 0.0;
		double dynamicStringBufferPoolRate = totalRequests > 0 ? static_cast<double>( dynamicStringBufferPoolHits ) / static_cast<double>( totalRequests ) : 0.0;
		double newAllocationRate = totalRequests > 0 ? static_cast<double>( newAllocations ) / static_cast<double>( totalRequests ) : 0.0;

		// Report detailed statistics
		state.counters["TotalRequests"] = static_cast<double>( totalRequests );
		state.counters["OverallHitRate"] = hitRate;
		state.counters["ThreadLocalHitRate"] = threadLocalRate;
		state.counters["DynamicStringBufferPoolHitRate"] = dynamicStringBufferPoolRate;
		state.counters["NewAllocationRate"] = newAllocationRate;
		state.counters["ThreadLocalHits"] = static_cast<double>( threadLocalHits );
		state.counters["DynamicStringBufferPoolHits"] = static_cast<double>( dynamicStringBufferPoolHits );
		state.counters["NewAllocations"] = static_cast<double>( newAllocations );
	}

	static void BM_PoolEffectiveness_MultiThreaded( benchmark::State& state )
	{
		if ( state.thread_index() == 0 )
		{
			internal::StringBuilderPool::resetStats();

			// Warm up the pool across multiple threads
			for ( int i = 0; i < 1000; ++i )
			{
				auto lease = internal::StringBuilderPool::lease();
			}
		}

		// Reset stats after warmup to get clean measurement (only on thread 0)
		if ( state.thread_index() == 0 )
		{
			internal::StringBuilderPool::resetStats();
		}

		for ( auto _ : state )
		{
			auto lease = internal::StringBuilderPool::lease();
			// Use the buffer to simulate real work
			auto builder = lease.builder();
			builder.append( "Test data for thread " );
			builder.append( std::to_string( state.thread_index() ) );
		}

		// Only report statistics from thread 0 to avoid duplication
		if ( state.thread_index() == 0 )
		{
			// Get pool statistics using the public interface
			const auto stats = internal::StringBuilderPool::stats();
			uint64_t totalRequests = stats.totalRequests;
			uint64_t threadLocalHits = stats.threadLocalHits;
			uint64_t dynamicStringBufferPoolHits = stats.dynamicStringBufferPoolHits;
			uint64_t newAllocations = stats.newAllocations;

			// Calculate metrics
			double hitRate = stats.hitRate;
			double threadLocalRate = totalRequests > 0 ? static_cast<double>( threadLocalHits ) / static_cast<double>( totalRequests ) : 0.0;
			double dynamicStringBufferPoolRate = totalRequests > 0 ? static_cast<double>( dynamicStringBufferPoolHits ) / static_cast<double>( totalRequests ) : 0.0;
			double newAllocationRate = totalRequests > 0 ? static_cast<double>( newAllocations ) / static_cast<double>( totalRequests ) : 0.0;

			// Report detailed statistics
			state.counters["TotalRequests"] = static_cast<double>( totalRequests );
			state.counters["OverallHitRate"] = hitRate;
			state.counters["ThreadLocalHitRate"] = threadLocalRate;
			state.counters["DynamicStringBufferPoolHitRate"] = dynamicStringBufferPoolRate;
			state.counters["NewAllocationRate"] = newAllocationRate;
			state.counters["ThreadLocalHits"] = static_cast<double>( threadLocalHits );
			state.counters["DynamicStringBufferPoolHits"] = static_cast<double>( dynamicStringBufferPoolHits );
			state.counters["NewAllocations"] = static_cast<double>( newAllocations );
		}
	}

	//=====================================================================
	// Run StringBuilder benchmarks
	//=====================================================================

	//----------------------------------------------
	// String construction with builder
	//----------------------------------------------

	BENCHMARK( BM_StringBuilder_Construction );
	BENCHMARK( BM_StringBuilder_AppendString )->Range( 8, 8192 )->Unit( benchmark::kMicrosecond );
	BENCHMARK( BM_StringBuilder_AppendMultipleStrings )->Range( 1, 1000 )->Unit( benchmark::kMicrosecond );

	//----------------------------------------------
	// String construction without builder benchmarks
	//----------------------------------------------

	BENCHMARK( BM_StdString_AppendString )->Range( 8, 8192 )->Unit( benchmark::kMicrosecond );
	BENCHMARK( BM_StdString_AppendMultipleStrings )->Range( 1, 1000 )->Unit( benchmark::kMicrosecond );
	BENCHMARK( BM_StdString_AppendMultipleStrings_WithReserve )->Range( 1, 1000 )->Unit( benchmark::kMicrosecond );
	BENCHMARK( BM_FmtMemoryBuffer_AppendString )->Range( 8, 8192 )->Unit( benchmark::kMicrosecond );
	BENCHMARK( BM_FmtMemoryBuffer_AppendMultipleStrings )->Range( 1, 1000 )->Unit( benchmark::kMicrosecond );
	BENCHMARK( BM_PlainCharArray_AppendMultipleStrings )->Range( 1, 1000 )->Unit( benchmark::kMicrosecond );

	//----------------------------------------------
	// Pool effectiveness and statistics benchmarks
	//----------------------------------------------

	BENCHMARK( BM_PoolEffectiveness )->Unit( benchmark::kNanosecond );
	BENCHMARK( BM_PoolEffectiveness_MultiThreaded )->ThreadRange( 1, 8 )->Unit( benchmark::kNanosecond );
}

BENCHMARK_MAIN();
