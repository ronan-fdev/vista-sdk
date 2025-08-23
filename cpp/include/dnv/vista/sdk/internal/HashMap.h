/**
 * @file HashMap.h
 * @brief Map with Robin Hood hashing and string optimization
 */

#pragma once

#include "dnv/vista/sdk/config/Platform.h"
#include "dnv/vista/sdk/utils/StringUtils.h"

namespace dnv::vista::sdk::internal
{
	//=====================================================================
	// HashMap class
	//=====================================================================

	/**
	 * @brief Hash table with Robin Hood hashing
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
			std::uint16_t distance = 0;
			bool occupied = false;
		};

		static constexpr size_t INITIAL_CAPACITY = 32;
		static constexpr size_t MAX_LOAD_FACTOR_PERCENT = 75;
		static constexpr std::uint32_t EMPTY_HASH = 0;

		std::vector<Bucket> m_buckets;
		size_t m_size = 0;
		size_t m_capacity = INITIAL_CAPACITY;
		size_t m_mask = INITIAL_CAPACITY - 1;

		VISTA_SDK_CPP_NO_UNIQUE_ADDRESS std::conditional_t<std::is_same_v<TKey, std::string> || std::is_same_v<TKey, std::string_view>,
			utils::StringViewHash, std::hash<TKey>>
			m_hasher;

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
		 * @brief Fast lookup with heterogeneous key types
		 * @param key The key to search for
		 * @return Pointer to the value if found, nullptr otherwise
		 */
		template <typename KeyType = TKey>
		VISTA_SDK_CPP_FORCE_INLINE const TValue* tryGetValue( const KeyType& key ) const noexcept
		{
			const std::uint32_t hash = static_cast<std::uint32_t>( m_hasher( key ) );
			if ( hash == EMPTY_HASH )
			{
				return nullptr;
			}

			size_t pos = hash & m_mask;

			for ( std::uint16_t distance = 0;; ++distance, pos = ( pos + 1 ) & m_mask )
			{
				const Bucket& bucket = m_buckets[pos];

				/* Check Robin Hood invariant and occupancy in single condition */
				if ( !bucket.occupied || distance > bucket.distance )
				{
					return nullptr;
				}

				/* Hot path: hash comparison first, then key equality */
				if ( bucket.hash == hash && keysEqual( bucket.key, key ) )
				{
					return &bucket.value;
				}
			}
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
		[[nodiscard]] size_t size() const noexcept
		{
			return m_size;
		}

		[[nodiscard]] size_t capacity() const noexcept
		{
			return m_capacity;
		}

		[[nodiscard]] bool isEmpty() const noexcept
		{
			return m_size == 0;
		}

		/**
		 * @brief Reserve capacity for at least the specified number of elements
		 * @param minCapacity Minimum capacity to reserve
		 */
		VISTA_SDK_CPP_FORCE_INLINE void reserve( size_t minCapacity )
		{
			if ( minCapacity > m_capacity )
			{
				size_t newCapacity = 1;
				while ( newCapacity < minCapacity )
				{
					newCapacity <<= 1;
				}

				if ( newCapacity > m_capacity )
				{
					std::vector<Bucket> oldBuckets = std::move( m_buckets );
					const size_t oldCapacity = m_capacity;

					m_capacity = newCapacity;
					m_mask = newCapacity - 1;
					m_buckets.clear();
					m_buckets.resize( newCapacity );
					m_size = 0;

					for ( size_t i = 0; i < oldCapacity; ++i )
					{
						if ( oldBuckets[i].occupied )
						{
							insertOrAssignInternal( std::move( oldBuckets[i].key ), std::move( oldBuckets[i].value ) );
						}
					}
				}
			}
		}

		/**
		 * @brief Remove a key-value pair from the map
		 * @param key The key to remove
		 * @return true if the key was found and removed, false otherwise
		 */
		template <typename KeyType = TKey>
		VISTA_SDK_CPP_FORCE_INLINE bool erase( const KeyType& key ) noexcept
		{
			const std::uint32_t hash = static_cast<std::uint32_t>( m_hasher( key ) );
			if ( hash == EMPTY_HASH )
			{
				return false;
			}

			size_t pos = hash & m_mask;
			std::uint16_t distance = 0;

			while ( distance <= m_buckets[pos].distance && m_buckets[pos].occupied )
			{
				if ( m_buckets[pos].hash == hash && keysEqual( m_buckets[pos].key, key ) )
				{
					eraseAtPosition( pos );
					--m_size;
					return true;
				}
				pos = ( pos + 1 ) & m_mask;
				++distance;
			}

			return false;
		}

	private:
		//----------------------------------------------
		// Internal implementation
		//----------------------------------------------

		/**
		 * @brief Internal insert or assign implementation with perfect forwarding
		 */
		template <typename ValueType>
		VISTA_SDK_CPP_FORCE_INLINE void insertOrAssignInternal( const TKey& key, ValueType&& value )
		{
			if ( shouldResize() )
			{
				resize();
			}

			const std::uint32_t hash = static_cast<std::uint32_t>( m_hasher( key ) );
			if ( hash == EMPTY_HASH )
			{
				return;
			}

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

		/**
		 * @brief Check if resize is needed based on load factor
		 */
		VISTA_SDK_CPP_FORCE_INLINE bool shouldResize() const noexcept
		{
			return ( m_size * 100 ) >= ( m_capacity * MAX_LOAD_FACTOR_PERCENT );
		}

		/**
		 * @brief Resize hash table to double capacity and rehash all elements
		 */
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

		/**
		 * @brief Erase element at specific position using backward shift deletion
		 */
		VISTA_SDK_CPP_FORCE_INLINE void eraseAtPosition( size_t pos ) noexcept
		{
			size_t nextPos = ( pos + 1 ) & m_mask;

			while ( m_buckets[nextPos].occupied && m_buckets[nextPos].distance > 0 )
			{
				m_buckets[pos] = std::move( m_buckets[nextPos] );
				--m_buckets[pos].distance;
				pos = nextPos;
				nextPos = ( nextPos + 1 ) & m_mask;
			}

			m_buckets[pos] = Bucket{};
		}

		/**
		 * @brief Compare keys with heterogeneous lookup support for string types
		 */
		template <typename KeyType1, typename KeyType2>
		VISTA_SDK_CPP_FORCE_INLINE bool keysEqual( const KeyType1& k1, const KeyType2& k2 ) const noexcept
		{
			if constexpr ( std::is_same_v<KeyType1, std::string> && std::is_same_v<KeyType2, std::string_view> )
			{
				return utils::StringViewEqual{}( k1, k2 );
			}
			else if constexpr ( std::is_same_v<KeyType1, std::string_view> && std::is_same_v<KeyType2, std::string> )
			{
				return utils::StringViewEqual{}( k1, k2 );
			}
			else
			{
				return k1 == k2;
			}
		}
	};
}
