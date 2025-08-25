/**
 * @file DynamicStringBufferPool.cpp
 * @brief Implementation of thread-safe shared memory buffer pool
 */

#include "DynamicStringBufferPool.h"

#include "dnv/vista/sdk/internal/StringBuilderPool.h"

namespace dnv::vista::sdk::internal
{
	//=====================================================================
	// Thread-local cache for DynamicStringBufferPool
	//=====================================================================

	/** @brief Thread-local cache for single buffer to optimize sequential allocations */
	thread_local DynamicStringBuffer* t_cachedBuffer = nullptr;

	/**
	 * @brief Thread-local RAII cleanup object to prevent memory leaks on thread exit
	 * @details This object exists purely for its destructor behavior, ensuring automatic cleanup
	 *          of cached buffers when threads terminate. Uses RAII pattern for deterministic cleanup.
	 */
	thread_local struct ThreadLocalCleanup
	{
		~ThreadLocalCleanup()
		{
			if ( t_cachedBuffer )
			{
				delete t_cachedBuffer;
				t_cachedBuffer = nullptr;
			}
		}
	} t_cleanup;

	//=====================================================================
	// DynamicStringBufferPool class
	//=====================================================================

	//----------------------------------------------
	// Destruction
	//----------------------------------------------

	DynamicStringBufferPool::~DynamicStringBufferPool()
	{
		std::lock_guard<std::mutex> lock( m_mutex );
		while ( !m_pool.empty() )
		{
			delete m_pool.back();
			m_pool.pop_back();
		}
	}

	//----------------------------------------------
	// Pool management methods
	//----------------------------------------------

	DynamicStringBuffer* DynamicStringBufferPool::get()
	{
		m_stats.totalRequests.fetch_add( 1, std::memory_order_relaxed );

		if ( t_cachedBuffer )
		{
			m_stats.threadLocalHits.fetch_add( 1, std::memory_order_relaxed );
			auto* buffer = t_cachedBuffer;
			t_cachedBuffer = nullptr;
			buffer->clear();

			return buffer;
		}

		DynamicStringBuffer* buffer = nullptr;

		{
			std::lock_guard<std::mutex> lock( m_mutex );
			if ( !m_pool.empty() )
			{
				m_stats.dynamicStringBufferPoolHits.fetch_add( 1, std::memory_order_relaxed );
				buffer = m_pool.back();
				m_pool.pop_back();
			}
		}

		if ( !buffer )
		{
			m_stats.newAllocations.fetch_add( 1, std::memory_order_relaxed );
			buffer = new DynamicStringBuffer();
			buffer->reserve( m_initialCapacity );
		}
		else
		{
			buffer->clear();
		}

		return buffer;
	}

	void DynamicStringBufferPool::returnToPool( DynamicStringBuffer* buffer )
	{
		if ( !buffer )
		{
			return;
		}

		if ( buffer->capacity() > m_maximumRetainedCapacity )
		{
			delete buffer;
			return;
		}

		if ( !t_cachedBuffer )
		{
			t_cachedBuffer = buffer;

			return;
		}

		std::lock_guard<std::mutex> lock( m_mutex );
		if ( m_pool.size() < m_maxPoolSize )
		{
			m_pool.push_back( buffer );
		}
		else
		{
			delete buffer;
		}
	}

	//----------------------------------------------
	// Statistics methods
	//----------------------------------------------

	//----------------------------
	// PoolStatistics struct
	//----------------------------

	double DynamicStringBufferPool::PoolStatistics::hitRate() const noexcept
	{
		uint64_t total = totalRequests.load();
		if ( total == 0 )
		{
			return 0.0;
		}
		uint64_t hits = threadLocalHits.load() + dynamicStringBufferPoolHits.load();

		return static_cast<double>( hits ) / static_cast<double>( total );
	}

	void DynamicStringBufferPool::PoolStatistics::reset() noexcept
	{
		threadLocalHits = 0;
		dynamicStringBufferPoolHits = 0;
		newAllocations = 0;
		totalRequests = 0;
	}

	//----------------------------
	// Statistics methods
	//----------------------------

	const DynamicStringBufferPool::PoolStatistics& DynamicStringBufferPool::stats() const noexcept
	{
		return m_stats;
	}

	size_t DynamicStringBufferPool::clear()
	{
		std::lock_guard<std::mutex> lock( m_mutex );

		size_t clearedCount = m_pool.size();

		// Clear the shared pool
		for ( auto* buffer : m_pool )
		{
			delete buffer;
		}
		m_pool.clear();

		// Clear this thread's cached buffer (each thread only touches its own)
		if ( t_cachedBuffer )
		{
			delete t_cachedBuffer;
			t_cachedBuffer = nullptr;
			clearedCount += 1;
		}

		return clearedCount;
	}

	size_t DynamicStringBufferPool::size() const noexcept
	{
		std::lock_guard<std::mutex> lock( m_mutex );

		auto count = m_pool.size();

		// Add this thread's cached buffer to the count
		if ( t_cachedBuffer )
		{
			count += 1;
		}

		return count;
	}

	void DynamicStringBufferPool::resetStats() noexcept
	{
		m_stats.reset();
	}
}
