/**
 * @file HashMap.h
 * @brief High-performance map with Robin Hood hashing and string optimization
 */

#pragma once

#include "dnv/vista/sdk/Config.h"
#include "StringUtils.h"

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <cassert>

namespace dnv::vista::sdk
{
	//=====================================================================
	// HashMap class
	//=====================================================================

	/**
	 * @brief High-performance hash table with Robin Hood hashing
	 * @details Cache-friendly dictionary implementation optimized for string keys
	 *          with heterogeneous lookup support and aggressive inlining
	 */
	template <typename TKey, typename TValue>
	class HashMap
	{
	private:
		//----------------------------------------------
		// Robin Hood Hashing bucket structure
		//----------------------------------------------
		struct Bucket
		{
			TKey key{};
			TValue value{};
			std::uint32_t hash = 0;
			std::uint32_t distance = 0;
			bool occupied = false;
		};

		static constexpr size_t INITIAL_CAPACITY = 32;
		static constexpr size_t MAX_LOAD_FACTOR_PERCENT = 75;
		static constexpr std::uint32_t EMPTY_HASH = 0;

		std::vector<Bucket> m_buckets;
		size_t m_size = 0;
		size_t m_capacity = INITIAL_CAPACITY;
		size_t m_mask = INITIAL_CAPACITY - 1;

		[[no_unique_address]] std::conditional_t<
			std::is_same_v<TKey, std::string> || std::is_same_v<TKey, std::string_view>,
			StringViewHash,
			std::hash<TKey>> m_hasher;

	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		VISTA_SDK_CPP_FORCE_INLINE HashMap()
		{
			m_buckets.resize( INITIAL_CAPACITY );
		}

		VISTA_SDK_CPP_FORCE_INLINE explicit HashMap( size_t initialCapacity )
		{
			size_t capacity = 1;
			while ( capacity < initialCapacity )
			{
				capacity <<= 1;
			}
			m_capacity = capacity;
			m_mask = capacity - 1;
			m_buckets.resize( capacity );
		}

		//----------------------------------------------
		// Core operations
		//----------------------------------------------

		/**
		 * @brief Fast lookup with optional heterogeneous key types
		 * @param key The key to search for
		 * @param outValue Reference to pointer that will point to the value if found
		 * @return true if key exists, false otherwise
		 */
		template <typename KeyType = TKey>
		VISTA_SDK_CPP_FORCE_INLINE bool tryGetValue( const KeyType& key, TValue*& outValue ) noexcept
		{
			const std::uint32_t hash = static_cast<std::uint32_t>( m_hasher( key ) );
			if ( hash == EMPTY_HASH )
				return false;

			size_t pos = hash & m_mask;
			std::uint32_t distance = 0;

			while ( distance <= m_buckets[pos].distance && m_buckets[pos].occupied )
			{
				if ( m_buckets[pos].hash == hash && keysEqual( m_buckets[pos].key, key ) )
				{
					outValue = &m_buckets[pos].value;
					return true;
				}
				pos = ( pos + 1 ) & m_mask;
				++distance;
			}

			return false;
		}

		/**
		 * @brief Insert or update a key-value pair
		 * @param key The key to insert or update
		 * @param value The value to associate with the key
		 */
		VISTA_SDK_CPP_FORCE_INLINE void insertOrAssign( const TKey& key, TValue&& value )
		{
			insertOrAssignInternal( key, std::forward<TValue>( value ) );
		}

		VISTA_SDK_CPP_FORCE_INLINE void insertOrAssign( const TKey& key, const TValue& value )
		{
			insertOrAssignInternal( key, value );
		}

		/**
		 * @brief Container properties
		 */
		[[nodiscard]] size_t size() const noexcept { return m_size; }
		[[nodiscard]] size_t capacity() const noexcept { return m_capacity; }
		[[nodiscard]] bool empty() const noexcept { return m_size == 0; }

	private:
		//----------------------------------------------
		// Internal implementation
		//----------------------------------------------

		template <typename ValueType>
		VISTA_SDK_CPP_FORCE_INLINE void insertOrAssignInternal( const TKey& key, ValueType&& value )
		{
			if ( shouldResize() )
			{
				resize();
			}

			const std::uint32_t hash = static_cast<std::uint32_t>( m_hasher( key ) );
			if ( hash == EMPTY_HASH )
				return;

			Bucket newBucket{ key, std::forward<ValueType>( value ), hash, 0, true };
			size_t pos = hash & m_mask;

			while ( true )
			{
				if ( !m_buckets[pos].occupied )
				{
					m_buckets[pos] = std::move( newBucket );
					++m_size;
					return;
				}

				if ( m_buckets[pos].hash == hash && keysEqual( m_buckets[pos].key, key ) )
				{
					m_buckets[pos].value = std::forward<ValueType>( value );
					return;
				}

				if ( newBucket.distance > m_buckets[pos].distance )
				{
					std::swap( newBucket, m_buckets[pos] );
				}

				pos = ( pos + 1 ) & m_mask;
				++newBucket.distance;
			}
		}

		VISTA_SDK_CPP_FORCE_INLINE bool shouldResize() const noexcept
		{
			return ( m_size * 100 ) >= ( m_capacity * MAX_LOAD_FACTOR_PERCENT );
		}

		void resize()
		{
			const size_t oldCapacity = m_capacity;
			m_capacity <<= 1;
			m_mask = m_capacity - 1;

			std::vector<Bucket> oldBuckets = std::move( m_buckets );
			m_buckets.clear();
			m_buckets.resize( m_capacity );
			m_size = 0;

			for ( size_t i = 0; i < oldCapacity; ++i )
			{
				if ( oldBuckets[i].occupied )
				{
					insertOrAssignInternal( std::move( oldBuckets[i].key ), std::move( oldBuckets[i].value ) );
				}
			}
		}

		template <typename KeyType1, typename KeyType2>
		VISTA_SDK_CPP_FORCE_INLINE bool keysEqual( const KeyType1& k1, const KeyType2& k2 ) const noexcept
		{
			if constexpr ( std::is_same_v<KeyType1, std::string> && std::is_same_v<KeyType2, std::string_view> )
			{
				return StringViewEqual{}( k1, k2 );
			}
			else if constexpr ( std::is_same_v<KeyType1, std::string_view> && std::is_same_v<KeyType2, std::string> )
			{
				return StringViewEqual{}( k1, k2 );
			}
			else
			{
				return k1 == k2;
			}
		}
	};
}
