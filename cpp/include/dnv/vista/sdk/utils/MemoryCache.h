/**
 * @file MemoryCache.h
 * @brief Thread-safe memory cache with configurable size limits and expiration policies
 */

#pragma once

#include "dnv/vista/sdk/Config.h"

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>

namespace dnv::vista::sdk
{
	/**
	 * @brief Configuration options for MemoryCache behavior
	 *
	 * Controls cache size limits, expiration policies, and cleanup behavior.
	 * All time-based parameters use std::chrono::milliseconds for precision.
	 *
	 * @param sizeLimit Maximum number of entries allowed in cache (0 = unlimited)
	 * @param expirationScanFrequency How often to scan for expired entries (0 = manual cleanup only)
	 * @param defaultSlidingExpiration Default time after last access before entries expire (default: 1 hour)
	 */
	struct MemoryCacheOptions
	{
		std::size_t sizeLimit = 0;
		std::chrono::milliseconds expirationScanFrequency{ 0 };
		std::chrono::milliseconds defaultSlidingExpiration{ std::chrono::hours( 1 ) };
	};

	/**
	 * @brief Cache entry metadata
	 */
	struct CacheEntry
	{
		std::chrono::steady_clock::time_point lastAccessed;
		std::chrono::milliseconds slidingExpiration;
		std::size_t size = 1;

		VISTA_SDK_CPP_FORCE_INLINE CacheEntry( std::chrono::milliseconds expiration = std::chrono::hours( 1 ) )
			: lastAccessed( std::chrono::steady_clock::now() ), slidingExpiration( expiration )
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
			: m_options( options )
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
					return it->second.value;
				}
				else
				{
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

				return std::ref( it->second.value );
			}

			if ( it != m_cache.end() )
			{
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

			return m_cache.erase( key ) > 0;
		}

		/**
		 * @brief Clear all cache entries
		 */
		VISTA_SDK_CPP_FORCE_INLINE void clear()
		{
			std::lock_guard<std::mutex> lock( m_mutex );
			m_cache.clear();
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
				: value( std::move( val ) ), metadata( std::move( meta ) )
			{
			}
		};

		mutable std::mutex m_mutex;
		std::unordered_map<TKey, CachedItem> m_cache;
		MemoryCacheOptions m_options;

		VISTA_SDK_CPP_FORCE_INLINE void evictLeastRecentlyUsed()
		{
			if ( m_cache.empty() )
			{
				return;
			}

			auto oldest_it = m_cache.begin();
			auto oldest_time = oldest_it->second.metadata.lastAccessed;

			for ( auto it = m_cache.begin(); it != m_cache.end(); ++it )
			{
				if ( it->second.metadata.lastAccessed < oldest_time )
				{
					oldest_time = it->second.metadata.lastAccessed;
					oldest_it = it;
				}
			}

			m_cache.erase( oldest_it );
		}
	};
}
