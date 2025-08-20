/**
 * @file MemoryCache.h
 * @brief Thread-safe memory cache with configurable size limits and expiration policies
 */

#pragma once

#include "dnv/vista/sdk/config/Platform.h"

namespace dnv::vista::sdk::internal
{
	/**
	 * @brief Configuration options for MemoryCache behavior
	 *
	 * Controls cache size limits, expiration policies, and cleanup behavior.
	 * All time-based parameters use std::chrono::milliseconds for precision.
	 */
	struct MemoryCacheOptions
	{
		/** @var sizeLimit Maximum number of entries allowed in cache (0 = unlimited)*/
		std::size_t sizeLimit = 0;

		/** @var expirationScanFrequency How often to scan for expired entries (0 = manual cleanup only) */
		std::chrono::milliseconds expirationScanFrequency{ 0 };

		/** @var defaultSlidingExpiration Default time after last access before entries expire (default: 1 hour) */
		std::chrono::milliseconds defaultSlidingExpiration{ std::chrono::hours( 1 ) };
	};

	/** @brief Cache entry metadata with intrusive LRU list support */
	struct CacheEntry
	{
		std::chrono::steady_clock::time_point lastAccessed;
		std::chrono::milliseconds slidingExpiration;
		std::size_t size = 1;

		CacheEntry* lruPrev = nullptr;
		CacheEntry* lruNext = nullptr;

		const void* keyPtr = nullptr;

		VISTA_SDK_CPP_FORCE_INLINE CacheEntry( std::chrono::milliseconds expiration = std::chrono::hours( 1 ) )
			: lastAccessed{ std::chrono::steady_clock::now() },
			  slidingExpiration{ expiration }
		{
		}

		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE bool isExpired() const noexcept
		{
			auto now = std::chrono::steady_clock::now();
			return ( now - lastAccessed ) > slidingExpiration;
		}

		void VISTA_SDK_CPP_FORCE_INLINE updateAccess() noexcept
		{
			lastAccessed = std::chrono::steady_clock::now();
		}
	};

	/**
	 * @brief Thread-safe memory cache with size limits and expiration policies
	 * @tparam TKey Key type for cache entries
	 * @tparam TValue Value type for cached objects
	 */
	template <typename TKey, typename TValue>
	class MemoryCache final
	{
	public:
		using FactoryFunction = std::function<TValue()>;
		using ConfigFunction = std::function<void( CacheEntry& )>;

		MemoryCache( const MemoryCache& ) = delete;
		MemoryCache& operator=( const MemoryCache& ) = delete;

		MemoryCache( MemoryCache&& ) = delete;
		MemoryCache& operator=( MemoryCache&& ) = delete;

		/**
		 * @brief Construct memory cache with specified options
		 * @param options Configuration options for cache behavior
		 */
		VISTA_SDK_CPP_FORCE_INLINE explicit MemoryCache( const MemoryCacheOptions& options = {} )
			: m_options{ options },
			  m_lruHead{ nullptr },
			  m_lruTail{ nullptr }
		{
			if ( m_options.sizeLimit > 0 )
			{
				m_cache.reserve( m_options.sizeLimit );
			}
		}

		// Default destructor
		~MemoryCache() = default;

		/**
		 * @brief Get or create a cache entry using factory function
		 * @param key The cache key
		 * @param factory Function to create the value if not cached
		 * @param configure Optional function to configure cache entry
		 * @return Reference to the cached value
		 */
		VISTA_SDK_CPP_FORCE_INLINE TValue& getOrCreate( const TKey& key, FactoryFunction factory, ConfigFunction configure = nullptr )
		{
			std::lock_guard<std::mutex> lock( m_mutex );

			auto it = m_cache.find( key );
			if ( it != m_cache.end() )
			{
				if ( !it->second.metadata.isExpired() )
				{
					it->second.metadata.updateAccess();
					moveToLruHead( &it->second.metadata );

					return it->second.value;
				}
				else
				{
					removeFromLru( &it->second.metadata );

					m_cache.erase( it );
				}
			}

			TValue value = factory();
			CacheEntry metadata( m_options.defaultSlidingExpiration );

			if ( configure )
			{
				configure( metadata );
			}

			if ( m_options.sizeLimit > 0 && m_cache.size() >= m_options.sizeLimit )
			{
				evictLeastRecentlyUsed();
			}

			auto [insert_it, inserted] = m_cache.try_emplace( key, std::move( value ), std::move( metadata ) );
			insert_it->second.metadata.keyPtr = &insert_it->first;
			addToLruHead( &insert_it->second.metadata );

			return insert_it->second.value;
		}

		/**
		 * @brief Try to get a cached value without creating it
		 * @param key The cache key
		 * @return Optional containing the value if found and not expired
		 */
		VISTA_SDK_CPP_FORCE_INLINE std::optional<std::reference_wrapper<TValue>> tryGet( const TKey& key )
		{
			std::lock_guard<std::mutex> lock( m_mutex );

			auto it = m_cache.find( key );
			if ( it != m_cache.end() && !it->second.metadata.isExpired() )
			{
				it->second.metadata.updateAccess();
				moveToLruHead( &it->second.metadata );

				return std::ref( it->second.value );
			}

			if ( it != m_cache.end() )
			{
				removeFromLru( &it->second.metadata );

				m_cache.erase( it );
			}

			return std::nullopt;
		}

		/**
		 * @brief Remove an entry from the cache
		 * @param key The cache key to remove
		 * @return True if entry was removed, false if not found
		 */
		VISTA_SDK_CPP_FORCE_INLINE bool remove( const TKey& key )
		{
			std::lock_guard<std::mutex> lock( m_mutex );

			auto it = m_cache.find( key );
			if ( it != m_cache.end() )
			{
				removeFromLru( &it->second.metadata );
				m_cache.erase( it );
				return true;
			}

			return false;
		}

		/**
		 * @brief Clear all cache entries
		 */
		VISTA_SDK_CPP_FORCE_INLINE void clear()
		{
			std::lock_guard<std::mutex> lock( m_mutex );
			m_cache.clear();
			m_lruHead = nullptr;
			m_lruTail = nullptr;
		}

		/**
		 * @brief Get current cache size
		 * @return Number of entries in cache
		 */
		VISTA_SDK_CPP_FORCE_INLINE std::size_t size() const
		{
			std::lock_guard<std::mutex> lock( m_mutex );

			return m_cache.size();
		}

		/**
		 * @brief Check if cache is empty
		 * @return True if cache contains no entries
		 */
		VISTA_SDK_CPP_FORCE_INLINE bool empty() const
		{
			std::lock_guard<std::mutex> lock( m_mutex );

			return m_cache.empty();
		}

		/**
		 * @brief Manually trigger cleanup of expired entries
		 */
		VISTA_SDK_CPP_FORCE_INLINE void cleanupExpired()
		{
			std::lock_guard<std::mutex> lock( m_mutex );

			auto it = m_cache.begin();
			while ( it != m_cache.end() )
			{
				if ( it->second.metadata.isExpired() )
				{
					removeFromLru( &it->second.metadata );
					it = m_cache.erase( it );
				}
				else
				{
					++it;
				}
			}
		}

	private:
		struct CachedItem
		{
			TValue value;
			CacheEntry metadata;

			CachedItem( TValue val, CacheEntry meta )
				: value{ std::move( val ) },
				  metadata{ std::move( meta ) }
			{
			}
		};

		mutable std::mutex m_mutex;
		std::unordered_map<TKey, CachedItem> m_cache;
		MemoryCacheOptions m_options;

		CacheEntry* m_lruHead;
		CacheEntry* m_lruTail;

		/**
		 * @brief Add entry to head of LRU list (most recently used)
		 * @param entry Entry to add to LRU list head
		 */
		VISTA_SDK_CPP_FORCE_INLINE void addToLruHead( CacheEntry* entry ) noexcept
		{
			entry->lruNext = m_lruHead;
			entry->lruPrev = nullptr;

			if ( m_lruHead != nullptr )
			{
				m_lruHead->lruPrev = entry;
			}
			else
			{
				m_lruTail = entry;
			}

			m_lruHead = entry;
		}

		/**
		 * @brief Remove entry from LRU list
		 * @param entry Entry to remove from LRU list
		 */
		VISTA_SDK_CPP_FORCE_INLINE void removeFromLru( CacheEntry* entry ) noexcept
		{
			if ( entry->lruPrev != nullptr )
			{
				entry->lruPrev->lruNext = entry->lruNext;
			}
			else
			{
				m_lruHead = entry->lruNext;
			}

			if ( entry->lruNext != nullptr )
			{
				entry->lruNext->lruPrev = entry->lruPrev;
			}
			else
			{
				m_lruTail = entry->lruPrev;
			}

			entry->lruNext = nullptr;
			entry->lruPrev = nullptr;
		}

		/**
		 * @brief Move entry to head of LRU list (mark as most recently used)
		 * @param entry Entry to move to LRU list head
		 */
		VISTA_SDK_CPP_FORCE_INLINE void moveToLruHead( CacheEntry* entry ) noexcept
		{
			if ( entry == m_lruHead )
			{
				return;
			}

			removeFromLru( entry );
			addToLruHead( entry );
		}

		/**
		 * @brief Evict least recently used entry in O(1) time
		 */
		VISTA_SDK_CPP_FORCE_INLINE void evictLeastRecentlyUsed()
		{
			if ( m_lruTail == nullptr )
			{
				return;
			}

			const TKey* keyPtr = static_cast<const TKey*>( m_lruTail->keyPtr );
			if ( keyPtr != nullptr )
			{
				removeFromLru( m_lruTail );
				m_cache.erase( *keyPtr );
			}
		}
	};
}
