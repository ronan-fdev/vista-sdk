/**
 * @file PathConversionCache.h
 * @brief High-performance cache for GMOD path conversions between VIS versions
 * @details Thread-safe cache implementation with statistics tracking and automatic size management
 */

#pragma once

#include "dnv/vista/sdk/Config.h"
#include "HashMap.h"
#include "MemoryCache.h"
#include "dnv/vista/sdk//GmodPath.h"
#include "dnv/vista/sdk//VISVersion.h"

#include <string>
#include <functional>

namespace dnv::vista::sdk
{
	//=====================================================================
	// Path conversion cache key
	//=====================================================================

	/**
	 * @brief Composite key for path conversion caching
	 * @details Combines source version, target version, and path string
	 */
	struct PathConversionKey
	{
		VisVersion sourceVersion;
		VisVersion targetVersion;
		std::string pathString;

		VISTA_SDK_CPP_FORCE_INLINE PathConversionKey( VisVersion src, VisVersion tgt, const std::string& path )
			: sourceVersion( src ), targetVersion( tgt ), pathString( path ) {}

		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE bool operator==( const PathConversionKey& other ) const noexcept
		{
			return sourceVersion == other.sourceVersion &&
				   targetVersion == other.targetVersion &&
				   pathString == other.pathString;
		}
	};

	/**
	 * @brief Hash functor for PathConversionKey
	 */
	struct PathConversionKeyHash
	{
		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE std::size_t operator()( const PathConversionKey& key ) const noexcept
		{
			std::size_t h1 = std::hash<int>{}( static_cast<int>( key.sourceVersion ) );
			std::size_t h2 = std::hash<int>{}( static_cast<int>( key.targetVersion ) );
			std::size_t h3 = StringViewHash{}( key.pathString );

			return h1 ^ ( h2 << 1 ) ^ ( h3 << 2 );
		}
	};

	//=====================================================================
	// Path conversion cache
	//=====================================================================

	/**
	 * @brief High-performance cache for path conversion results
	 * @details Thread-safe implementation with LRU-style eviction and performance metrics
	 */
	class PathConversionCache
	{
	private:
		using PathCache = std::unordered_map<PathConversionKey, std::optional<GmodPath>, PathConversionKeyHash>;

		mutable std::mutex m_mutex;
		PathCache m_pathCache;

		mutable std::atomic<std::size_t> m_hits{ 0 };
		mutable std::atomic<std::size_t> m_misses{ 0 };
		static constexpr std::size_t MAX_CACHE_SIZE = 10000;

	public:
		PathConversionCache( const PathConversionCache& ) = delete;
		PathConversionCache& operator=( const PathConversionCache& ) = delete;

		PathConversionCache( PathConversionCache&& ) = delete;
		PathConversionCache& operator=( PathConversionCache&& ) = delete;

		PathConversionCache() = default;
		~PathConversionCache() = default;

		/**
		 * @brief Try to get cached path conversion result
		 * @param sourceVersion Source VIS version
		 * @param pathString Original path string representation
		 * @param targetVersion Target VIS version
		 * @param result Output parameter for cached result
		 * @return True if cache hit, false if cache miss
		 */
		VISTA_SDK_CPP_FORCE_INLINE bool tryGetCachedConversion(
			VisVersion sourceVersion,
			const std::string& pathString,
			VisVersion targetVersion,
			std::optional<GmodPath>& result ) const
		{
			PathConversionKey key{ sourceVersion, targetVersion, pathString };

			std::lock_guard<std::mutex> lock( m_mutex );
			auto it = m_pathCache.find( key );

			if ( it != m_pathCache.end() )
			{
				result = it->second;
				++m_hits;
				return true;
			}

			++m_misses;
			return false;
		}

		/**
		 * @brief Cache a path conversion result
		 * @param sourceVersion Source VIS version
		 * @param pathString Original path string representation
		 * @param targetVersion Target VIS version
		 * @param result Conversion result to cache
		 */
		VISTA_SDK_CPP_FORCE_INLINE void cacheConversion(
			VisVersion sourceVersion,
			const std::string& pathString,
			VisVersion targetVersion,
			const std::optional<GmodPath>& result )
		{
			PathConversionKey key{ sourceVersion, targetVersion, pathString };

			std::lock_guard<std::mutex> lock( m_mutex );

			if ( m_pathCache.size() >= MAX_CACHE_SIZE )
			{
				m_pathCache.erase( m_pathCache.begin() );
			}

			m_pathCache[key] = result;
		}

		/**
		 * @brief Get cache performance statistics
		 */
		struct CacheStats
		{
			std::size_t hits;
			std::size_t misses;
			double hitRatio;
			std::size_t cacheSize;
		};

		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE CacheStats getStats() const
		{
			std::lock_guard<std::mutex> lock( m_mutex );
			std::size_t hits = m_hits.load();
			std::size_t misses = m_misses.load();
			std::size_t total = hits + misses;

			return CacheStats{
				hits,
				misses,
				total > 0 ? static_cast<double>( hits ) / static_cast<double>( total ) : 0.0,
				m_pathCache.size() };
		}

		/**
		 * @brief Clear all cached conversions
		 */
		VISTA_SDK_CPP_FORCE_INLINE void clear()
		{
			std::lock_guard<std::mutex> lock( m_mutex );
			m_pathCache.clear();
			m_hits.store( 0 );
			m_misses.store( 0 );
		}

		/**
		 * @brief Get global cache instance
		 * @details Thread-safe singleton pattern for global cache access
		 */
		static PathConversionCache& instance()
		{
			static PathConversionCache cache;
			return cache;
		}
	};
}
