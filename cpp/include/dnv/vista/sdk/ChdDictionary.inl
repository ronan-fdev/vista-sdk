/**
 * @file ChdDictionary.inl
 * @brief Template implementation of CHD Dictionary class
 */

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// Internal helper components
		//=====================================================================

		//----------------------------------------------
		// Constants
		//----------------------------------------------

		/** @brief FNV offset basis constant for hash calculations. */
		static constexpr uint32_t FNV_OFFSET_BASIS{ 0x811C9DC5 };

		/** @brief FNV prime constant for hash calculations. */
		static constexpr uint32_t FNV_PRIME{ 0x01000193 };

		//----------------------------------------------
		// CPU feature detection
		//----------------------------------------------

		inline bool hasSSE42Support()
		{
			static const bool s_hasSSE42{ []() {
				bool hasSupport{ false };

#if defined( _MSC_VER )
				std::array<int, 4> cpuInfo{};
				::__cpuid( cpuInfo.data(), 1 );
				hasSupport = ( cpuInfo[2] & ( 1 << 20 ) ) != 0;
#elif defined( __GNUC__ )
				unsigned int eax{}, ebx{}, ecx{}, edx{};
				if ( ::__get_cpuid( 1, &eax, &ebx, &ecx, &edx ) )
				{
					hasSupport = ( ecx & ( 1 << 20 ) ) != 0;
				}
#else
				hasSupport = false;
#endif

				return hasSupport;
			}() };

			return s_hasSSE42;
		}

		//----------------------------------------------
		// Hashing class
		//----------------------------------------------

		//----------------------------
		// Public static methods
		//----------------------------

		inline constexpr uint32_t Hashing::Larson( uint32_t hash, uint8_t ch ) noexcept
		{
			return 37 * hash + ch;
		}

		inline constexpr uint32_t Hashing::fnv1a( uint32_t hash, uint8_t ch ) noexcept
		{
			return ( ch ^ hash ) * FNV_PRIME;
		}

		inline uint32_t Hashing::crc32( uint32_t hash, uint8_t ch ) noexcept
		{
			return _mm_crc32_u8( hash, ch );
		}

		inline constexpr uint32_t Hashing::seed( uint32_t seed, uint32_t hash, size_t size ) noexcept
		{
			/* Mixes the primary hash with the seed to find the final table slot */
			uint32_t x{ seed + hash };
			x ^= x >> 12;
			x ^= x << 25;
			x ^= x >> 27;

			return static_cast<uint32_t>( ( x * 0x2545F4914F6CDD1DUL ) & ( size - 1 ) );
		}
	}

	//=====================================================================
	// Constants
	//=====================================================================

	static constexpr uint32_t MAX_SEED_SEARCH_MULTIPLIER = 100;

	//=====================================================================
	// ChdDictionary class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	template <typename TValue>
	inline ChdDictionary<TValue>::ChdDictionary( std::vector<std::pair<std::string, TValue>>&& items )
		: m_table{},
		  m_seeds{}
	{
		if ( items.empty() )
		{
			return;
		}

		uint64_t size{ 1 };
		/* Ensure table size is a power of 2 and at least 2x item count for efficient modulo operations (using '&') */
		while ( size < items.size() )
		{
			size *= 2;
		}
		size *= 2;

		m_table.reserve( size );
		m_seeds.reserve( size );

		auto hashBuckets{ std::vector<std::vector<std::pair<unsigned, uint32_t>>>( size ) };
		for ( auto& bucket : hashBuckets )
		{
			bucket.reserve( 4 );
		}

		for ( size_t i{ 0 }; i < items.size(); ++i )
		{
			const auto& key{ items[i].first };
			uint32_t hashValue{ hash( key ) };
			auto bucketForItemIdx{ hashValue & ( size - 1 ) };
			hashBuckets[bucketForItemIdx].emplace_back( static_cast<unsigned int>( i + 1 ), hashValue );
		}

		std::sort( hashBuckets.begin(), hashBuckets.end(), []( const auto& a, const auto& b ) {
			return a.size() > b.size();
		} );

		/* Process buckets with the most items (highest collision potential) first */
		auto indices{ std::vector<unsigned int>( size, 0 ) };
		auto seeds{ std::vector<int>( size, 0 ) };

		size_t currentBucketIdx{ 0 };
		for ( ; currentBucketIdx < hashBuckets.size() && hashBuckets[currentBucketIdx].size() > 1; ++currentBucketIdx )
		{
			const auto& subKeys{ hashBuckets[currentBucketIdx] };
			auto entries{ std::unordered_map<size_t, unsigned>() };
			entries.reserve( subKeys.size() );
			uint32_t currentSeedValue{ 0 };

			while ( true )
			{
				++currentSeedValue;
				entries.clear();
				bool seedValid{ true };

				for ( const auto& k : subKeys )
				{
					/* Calculate final position using secondary hash with current seed */
					auto finalHash{ Hashing::seed( currentSeedValue, k.second, size ) };
					bool slotOccupied = indices[finalHash] != 0;
					bool entryClaimedThisTry = entries.count( finalHash ) != 0;

					if ( !slotOccupied && !entryClaimedThisTry )
					{
						entries[finalHash] = k.first;
					}
					else
					{
						seedValid = false;
						break;
					}
				}

				if ( seedValid )
				{
					break;
				}

				if ( currentSeedValue > size * MAX_SEED_SEARCH_MULTIPLIER )
				{
					throw std::runtime_error( fmt::format(
						"Bucket {}: Seed search exceeded threshold ({}), aborting construction!",
						currentBucketIdx, currentSeedValue ) );
				}
			}

			for ( const auto& [finalHash, itemIdx] : entries )
			{
				indices[finalHash] = itemIdx;
			}
			seeds[subKeys[0].second & ( size - 1 )] = static_cast<int>( currentSeedValue );
		}

		/* Resizes m_table to 'size' elements, initializing new slots with an empty string key and a value copied from the first input item. */
		m_table.resize( size, { std::string(), items[0].second } );

		std::vector<size_t> freeSlots;
		freeSlots.reserve( size );

		for ( size_t i{ 0 }; i < indices.size(); ++i )
		{
			if ( indices[i] != 0 )
			{
				auto itemIndex = indices[i] - 1;
				m_table[i] = std::move( items[itemIndex] );
			}
			else
			{
				freeSlots.push_back( i );
			}
		}

		size_t freeSlotsIndex{ 0 };
		for ( ; currentBucketIdx < hashBuckets.size() && !hashBuckets[currentBucketIdx].empty(); ++currentBucketIdx )
		{
			const auto& k{ hashBuckets[currentBucketIdx][0] };
			auto slotIndexInMTable{ freeSlots[freeSlotsIndex++] };
			auto itemIndex = k.first - 1;
			m_table[slotIndexInMTable] = std::move( items[itemIndex] );

			/* Use negative seed to directly encode the final table index for single-item buckets */
			seeds[k.second & ( size - 1 )] = -static_cast<int>( slotIndexInMTable + 1 );
		}

		m_seeds = std::move( seeds );
	}

	//----------------------------------------------
	// Lookup operators
	//----------------------------------------------

	template <typename TValue>
	inline TValue& ChdDictionary<TValue>::operator[]( std::string_view key )
	{
		if ( isEmpty() )
		{
			ThrowHelper::throwKeyNotFoundException( key );
		}

		const TValue* outValue = nullptr;
		if ( tryGetValue( key, outValue ) && outValue != nullptr )
		{
			return const_cast<TValue&>( *outValue );
		}

		ThrowHelper::throwKeyNotFoundException( key );
	}

	//----------------------------------------------
	// Lookup methods
	//----------------------------------------------

	template <typename TValue>
	inline const TValue& ChdDictionary<TValue>::at( std::string_view key ) const
	{
		if ( isEmpty() )
		{
			ThrowHelper::throwKeyNotFoundException( key );
		}

		const TValue* outValue = nullptr;
		if ( tryGetValue( key, outValue ) && outValue != nullptr )
		{
			return *outValue;
		}

		ThrowHelper::throwKeyNotFoundException( key );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	template <typename TValue>
	inline size_t ChdDictionary<TValue>::size() const noexcept
	{
		return m_table.size();
	}

	//----------------------------------------------
	// State inspection methods
	//----------------------------------------------

	template <typename TValue>
	inline bool ChdDictionary<TValue>::isEmpty() const noexcept
	{
		return m_table.empty();
	}

	//----------------------------------------------
	// Static query methods
	//----------------------------------------------

	template <typename TValue>
	inline bool ChdDictionary<TValue>::tryGetValue( std::string_view key, const TValue*& outValue ) const
	{
		outValue = nullptr;

		if ( key.empty() )
		{
			return false;
		}

		if ( m_table.empty() )
		{
			return false;
		}

		const uint32_t hashValue = hash( key );
		const size_t tableSize = m_table.size();
		const uint32_t index = hashValue & ( tableSize - 1 );
		const int seed = m_seeds[index];

		size_t finalIndex;
		if ( seed < 0 )
		{
			finalIndex = static_cast<size_t>( -seed - 1 );
		}
		else
		{
			const uint32_t finalHash = Hashing::seed(
				static_cast<uint32_t>( seed ),
				hashValue,
				tableSize );

			finalIndex = finalHash;
		}

		const auto& kvp = m_table[finalIndex];

		if ( key.size() == kvp.first.size() && std::memcmp( key.data(), kvp.first.data(), key.size() ) == 0 )
		{
			outValue = &kvp.second;

			return true;
		}

		return false;
	}

	//----------------------------------------------
	// Iteration
	//----------------------------------------------

	template <typename TValue>
	inline ChdDictionary<TValue>::Iterator ChdDictionary<TValue>::begin() const
	{
		for ( size_t i{ 0 }; i < m_table.size(); ++i )
		{
			if ( !m_table[i].first.empty() )
			{
				return Iterator{ &m_table, i };
			}
		}

		return end();
	}

	template <typename TValue>
	inline ChdDictionary<TValue>::Iterator ChdDictionary<TValue>::end() const
	{
		return Iterator{ &m_table, m_table.size() };
	}

	//----------------------------------------------
	// Enumeration
	//----------------------------------------------

	template <typename TValue>
	inline ChdDictionary<TValue>::Enumerator ChdDictionary<TValue>::enumerator() const
	{
		return Enumerator( &m_table );
	}

	//----------------------------------------------
	// Private helper methods
	//----------------------------------------------

	//---------------------------
	// Hashing
	//---------------------------

	template <typename TValue>
	inline uint32_t ChdDictionary<TValue>::hash( std::string_view key ) noexcept
	{
		uint32_t hashValue = FNV_OFFSET_BASIS;

		if ( key.empty() )
		{
			return hashValue;
		}

		static bool hasSSE42 = hasSSE42Support();

		if ( hasSSE42 )
		{
			const char* data = key.data();
			size_t length = key.length();

			while ( length >= 4 )
			{
				/* Process 4 low bytes at once */
				hashValue = Hashing::crc32( hashValue, static_cast<uint8_t>( data[0] ) );
				hashValue = Hashing::crc32( hashValue, static_cast<uint8_t>( data[1] ) );
				hashValue = Hashing::crc32( hashValue, static_cast<uint8_t>( data[2] ) );
				hashValue = Hashing::crc32( hashValue, static_cast<uint8_t>( data[3] ) );

				/* Process 4 high bytes (UTF-16 simulation - all zeros for ASCII) */
				hashValue = Hashing::crc32( hashValue, 0 );
				hashValue = Hashing::crc32( hashValue, 0 );
				hashValue = Hashing::crc32( hashValue, 0 );
				hashValue = Hashing::crc32( hashValue, 0 );

				data += 4;
				length -= 4;
			}

			for ( size_t i = 0; i < length; ++i )
			{
				hashValue = Hashing::crc32( hashValue, static_cast<uint8_t>( data[i] ) );
				/* UTF-16 high byte */
				hashValue = Hashing::crc32( hashValue, 0 );
			}
		}
		else
		{
			for ( char ch : key )
			{
				hashValue = ( static_cast<uint8_t>( ch ) ^ hashValue ) * FNV_PRIME;
				hashValue = ( 0 ^ hashValue ) * FNV_PRIME;
			}
		}

		return hashValue;
	}

	//----------------------------------------------
	// ChdDictionary::Iterator class
	//----------------------------------------------

	//---------------------------
	// Construction
	//---------------------------

	template <typename TValue>
	inline ChdDictionary<TValue>::Iterator::Iterator( const std::vector<std::pair<std::string, TValue>>* table, size_t index )
		: m_table{ table },
		  m_index{ index }
	{
	}

	//---------------------------
	// Operations
	//---------------------------

	template <typename TValue>
	inline const std::pair<std::string, TValue>& ChdDictionary<TValue>::Iterator::operator*() const
	{
		if ( m_index >= m_table->size() )
		{
			throw std::runtime_error( fmt::format(
				"Iterator: Dereference out of bounds (index: {}, table size: {})",
				m_index, m_table->size() ) );
		}
		return ( *m_table )[m_index];
	}

	template <typename TValue>
	inline const std::pair<std::string, TValue>* ChdDictionary<TValue>::Iterator::operator->() const
	{
		if ( m_index >= m_table->size() )
		{
			throw std::runtime_error( fmt::format(
				"Iterator: Arrow operator out of bounds (index: {}, table size: {})",
				m_index, m_table->size() ) );
		}
		return &( ( *m_table )[m_index] );
	}

	template <typename TValue>
	inline ChdDictionary<TValue>::Iterator& ChdDictionary<TValue>::Iterator::operator++()
	{
		if ( m_table == nullptr )
		{
			return *this;
		}

		while ( ++m_index < m_table->size() )
		{
			const auto& entry{ ( *m_table )[m_index] };
			if ( !entry.first.empty() )
			{
				return *this;
			}
		}

		m_index = m_table->size();

		return *this;
	}

	template <typename TValue>
	inline ChdDictionary<TValue>::Iterator ChdDictionary<TValue>::Iterator::operator++( int )
	{
		auto tmp{ Iterator{ *this } };
		++( *this );

		return tmp;
	}

	//---------------------------
	// Comparison
	//---------------------------

	template <typename TValue>
	inline bool ChdDictionary<TValue>::Iterator::operator==( const Iterator& other ) const noexcept
	{
		return m_table == other.m_table && m_index == other.m_index;
	}

	template <typename TValue>
	inline bool ChdDictionary<TValue>::Iterator::operator!=( const Iterator& other ) const noexcept
	{
		return !( *this == other );
	}

	//----------------------------------------------
	// ChdDictionary::Enumerator class
	//----------------------------------------------

	//----------------------------
	// Construction
	//----------------------------

	template <typename TValue>
	inline ChdDictionary<TValue>::Enumerator::Enumerator( const std::vector<std::pair<std::string, TValue>>* table )
		: m_table{ table },
		  m_index{ std::numeric_limits<size_t>::max() }
	{
	}

	//----------------------------
	// Enumeration
	//----------------------------

	template <typename TValue>
	inline bool ChdDictionary<TValue>::Enumerator::next() noexcept
	{
		do
		{
			++m_index;
		} while ( m_index < m_table->size() && ( *m_table )[m_index].first.empty() );

		return m_index < m_table->size();
	}

	template <typename TValue>
	inline const std::pair<std::string, TValue>& ChdDictionary<TValue>::Enumerator::current() const
	{
		if ( !m_table || m_index == SIZE_MAX || m_index >= m_table->size() )
		{
			ThrowHelper::throwInvalidOperationException();
		}

		return ( *m_table )[m_index];
	}

	template <typename TValue>
	inline void ChdDictionary<TValue>::Enumerator::reset() noexcept
	{
		m_index = std::numeric_limits<size_t>::max();
	}
}
