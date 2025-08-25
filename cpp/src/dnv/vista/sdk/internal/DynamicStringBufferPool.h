/**
 * @file DynamicStringBufferPool.h
 * @brief Thread-safe shared pool for memory buffer management
 * @details Provides optimized memory buffer pooling with thread-local caching for high-performance string operations.
 *          Uses a two-tier architecture: thread-local cache for immediate reuse and shared pool for cross-thread sharing.
 */

#pragma once

namespace dnv::vista::sdk::internal
{
	class DynamicStringBuffer;

	//=====================================================================
	// DynamicStringBufferPool class
	//=====================================================================

	/**
	 * @brief Thread-safe shared pool for memory buffer management with configurable performance parameters
	 * @details Implements a two-tier pooling strategy:
	 *          1. Thread-local cache: Each thread keeps one buffer for immediate reuse (fastest)
	 *          2. Shared pool: Cross-thread buffer sharing with mutex protection (slower but still fast)
	 *          3. New allocation: Only when both caches are exhausted (slowest)
	 *
	 * @note Thread-local buffers are automatically cleaned up when threads exit via ThreadLocalCleanup RAII pattern
	 */
	class DynamicStringBufferPool final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs DynamicStringBufferPool with configurable performance parameters
		 * @param initialCapacity Initial buffer capacity for new allocations
		 * @param maximumRetainedCapacity Maximum buffer size retained in pool before deletion
		 * @param maxPoolSize Maximum number of buffers stored in the shared pool
		 */
		explicit DynamicStringBufferPool(
			size_t initialCapacity = 256,
			size_t maximumRetainedCapacity = 2048,
			size_t maxPoolSize = 24 )
			: m_initialCapacity{ initialCapacity },
			  m_maximumRetainedCapacity{ maximumRetainedCapacity },
			  m_maxPoolSize{ maxPoolSize }
		{
			/*
			 * TODO:
			 *	-Make DynamicStringBufferPool class configurable
			 *	-Consider make it auto-adaptative:
			 * 		size_t initialCapacity = 256,
			 *		size_t maximumRetainedCapacity = 2048,
			 *		size_t maxPoolSize = std::max(8u, std::thread::hardware_concurrency() * 2)
			 */
		}

		DynamicStringBufferPool( const DynamicStringBufferPool& ) = delete;
		DynamicStringBufferPool( DynamicStringBufferPool&& ) = delete;
		DynamicStringBufferPool& operator=( const DynamicStringBufferPool& ) = delete;
		DynamicStringBufferPool& operator=( DynamicStringBufferPool&& ) = delete;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DynamicStringBufferPool();

		//----------------------------------------------
		// Pool management methods
		//----------------------------------------------

		/**
		 * @brief Retrieves buffer from pool or creates new one
		 * @return Pointer to memory buffer ready for use
		 * @details Retrieval priority: 1) Thread-local cache, 2) Shared pool, 3) New allocation
		 */
		DynamicStringBuffer* get();

		/**
		 * @brief Returns buffer to pool for reuse
		 * @param buffer Buffer to return (must not be null, but method handles null gracefully)
		 * @details Return priority: 1) Thread-local cache (if empty), 2) Shared pool (if not full), 3) Delete
		 */
		void returnToPool( DynamicStringBuffer* buffer );

		//----------------------------------------------
		// Statistics
		//----------------------------------------------

		//----------------------------
		// PoolStatistics struct
		//----------------------------

		/** @brief Statistics structure containing pool performance metrics */
		struct PoolStatistics
		{
			PoolStatistics() = default;
			PoolStatistics( const PoolStatistics& ) = delete;
			PoolStatistics( PoolStatistics&& ) noexcept = delete;
			PoolStatistics& operator=( const PoolStatistics& ) = delete;
			PoolStatistics& operator=( PoolStatistics&& ) noexcept = delete;

			/** @brief Calculates the pool hit rate as a percentage (0.0 to 1.0) */
			double hitRate() const noexcept;

			/** @brief Resets all statistics counters to zero */
			void reset() noexcept;

			std::atomic<uint64_t> threadLocalHits{ 0 };
			std::atomic<uint64_t> dynamicStringBufferPoolHits{ 0 };
			std::atomic<uint64_t> newAllocations{ 0 };
			std::atomic<uint64_t> totalRequests{ 0 };
		};

		//----------------------------
		// Statistics methods
		//----------------------------

		/** @brief Gets current pool statistics */
		const PoolStatistics& stats() const noexcept;

		/**
		 * @brief Clears all buffers from the pool and returns the count of cleared buffers
		 * @return Number of buffers that were cleared from current thread's cache and shared pool
		 * @note This method can throw if memory deallocation fails
		 * @warning Only clears current thread's cached buffer - other threads' caches remain untouched
		 */
		size_t clear();

		/**
		 * @brief Gets current number of buffers stored in the pool
		 * @return Total count including current thread's cached buffer and shared pool buffers
		 */
		size_t size() const noexcept;

		/** @brief Resets pool statistics to zero */
		void resetStats() noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Vector containing available pooled buffers for cross-thread sharing */
		std::vector<DynamicStringBuffer*> m_pool;

		/** @brief Mutex protecting shared pool access during concurrent operations */
		mutable std::mutex m_mutex;

		/** @brief Initial capacity for newly allocated buffers (optimization for common use cases) */
		const size_t m_initialCapacity;

		/** @brief Maximum buffer capacity retained in pool before deletion (prevents memory bloat) */
		const size_t m_maximumRetainedCapacity;

		/** @brief Maximum number of buffers stored in shared pool (prevents unbounded growth) */
		const size_t m_maxPoolSize;

		/** @brief Pool performance statistics with atomic counters for thread safety */
		mutable PoolStatistics m_stats;
	};

	//----------------------------------------------
	// Singleton instance access
	//----------------------------------------------

	/*
	 * TODO: Consider replacing the singleton pattern with a registry or factory.
	 */

	/**
	 * @brief Gets the singleton DynamicStringBufferPool instance
	 * @return Reference to the global shared pool instance
	 * @details Uses static local variable for thread-safe initialization and automatic cleanup
	 */
	inline static DynamicStringBufferPool& dynamicStringBufferPool() noexcept
	{
		/*
		 * TODO: Make DynamicStringBufferPool class configurable
		 */

		static DynamicStringBufferPool pool{ 256, 2048, 24 };

		return pool;
	}
}
