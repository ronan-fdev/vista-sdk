/**
 * @file ChdDictionary.h
 * @brief Perfect hashing dictionary using the CHD algorithm.
 *
 * @details This file defines the ChdDictionary class, a core data structure in the VISTA SDK
 * that provides fast, memory-efficient lookups using the Compress, Hash, and Displace
 * (CHD) algorithm for perfect hashing. This implementation optimizes for read-heavy
 * operations with minimal memory overhead.
 *
 * @see https://en.wikipedia.org/wiki/Perfect_hash_function#CHD_algorithm
 */

#pragma once

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// Internal helper components
		//=====================================================================

		//----------------------------------------------
		// CPU feature detection
		//----------------------------------------------

		/**
		 * @brief Gets the cached SSE4.2 support status.
		 * @details Checks CPU capabilities for SSE4.2 CRC32 instructions, which provide
		 *          3-5x faster hashing compared to software fallback. Result is cached
		 *          via static initialization for zero runtime overhead.
		 * @return `true` if SSE4.2 is supported, `false` otherwise.
		 */
		[[nodiscard]] inline bool hasSSE42Support() noexcept;

		//----------------------------------------------
		// ThrowHelper class
		//----------------------------------------------

		class ThrowHelper final
		{
		public:
			//----------------------------
			// Construction
			//----------------------------

			/** @brief Default constructor. */
			ThrowHelper() = delete;

			/** @brief Copy constructor */
			ThrowHelper( const ThrowHelper& ) = delete;

			/** @brief Move constructor */
			ThrowHelper( ThrowHelper&& ) noexcept = delete;

			//----------------------------
			// Destruction
			//----------------------------

			/** @brief Destructor */
			~ThrowHelper() = delete;

			//----------------------------
			// Assignment operators
			//----------------------------

			/** @brief Copy assignment operator */
			ThrowHelper& operator=( const ThrowHelper& ) = delete;

			/** @brief Move assignment operator */
			ThrowHelper& operator=( ThrowHelper&& ) noexcept = delete;

			//----------------------------
			// Public static methods
			//----------------------------

			/**
			 * @brief Throws a key not found exception with the specified key.
			 * @param[in] key The key that was not found.
			 * @throws key_not_found_exception Always.
			 */
			[[noreturn]] inline static void throwKeyNotFoundException( std::string_view key );

			/**
			 * @brief Throws an invalid operation exception.
			 * @details This exception is thrown when an operation is not valid due to
			 *          the current state of the object, such as accessing Current on
			 *          an enumerator that hasn't been positioned properly.
			 * @throws invalid_operation_exception Always.
			 */
			[[noreturn]] inline static void throwInvalidOperationException();
		};

		//----------------------------------------------
		// Hashing class
		//----------------------------------------------

		/**
		 * @class Hashing
		 * @brief Provides hashing function utilities required for the CHD algorithm.
		 */
		class Hashing final
		{
		public:
			//----------------------------
			// Construction
			//----------------------------

			/** @brief Default constructor. */
			Hashing() = delete;

			/** @brief Copy constructor */
			Hashing( const Hashing& ) = delete;

			/** @brief Move constructor */
			Hashing( Hashing&& ) noexcept = delete;

			//----------------------------
			// Destruction
			//----------------------------

			/** @brief Destructor */
			~Hashing() = delete;

			//----------------------------
			// Assignment operators
			//----------------------------

			/** @brief Copy assignment operator */
			Hashing& operator=( const Hashing& ) = delete;

			/** @brief Move assignment operator */
			Hashing& operator=( Hashing&& ) noexcept = delete;

			//----------------------------
			// Public static methods
			//----------------------------

			/**
			 * @brief Larson multiplicative hash function: 37 * hash + ch
			 * @details Simple hash by Paul Larson, provided for benchmarking.
			 * @note Not used by CHD algorithm.
			 */
			[[nodiscard]] inline static constexpr uint32_t Larson( uint32_t hash, uint8_t ch ) noexcept;

			/**
			 * @brief Computes one step of the FNV-1a hash function.
			 * @param[in] hash The current hash value.
			 * @param[in] ch The character (byte) to incorporate into the hash.
			 * @return The updated hash value.
			 * @see https://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function
			 */
			[[nodiscard]] inline static constexpr uint32_t fnv1a( uint32_t hash, uint8_t ch ) noexcept;

			/**
			 * @brief Computes one step of the CRC32 hash function using SSE4.2 instructions.
			 * @param[in] hash The current hash value.
			 * @param[in] ch The character (byte) to incorporate into the hash.
			 * @return The updated hash value.
			 * @note Requires SSE4.2 support. Use hasSSE42Support() to check availability.
			 * @see https://en.wikipedia.org/wiki/Cyclic_redundancy_check
			 */
			[[nodiscard]] inline static uint32_t crc32( uint32_t hash, uint8_t ch ) noexcept;

			/**
			 * @brief Computes the final table index using the seed mixing function for the CHD algorithm.
			 * @param[in] seed The seed value associated with the hash bucket.
			 * @param[in] hash The 32-bit hash value of the key.
			 * @param[in] size The total size (capacity) of the dictionary's main table. Must be a power of 2.
			 * @return The final table index for the key (as size_t).
			 * @see https://en.wikipedia.org/wiki/Perfect_hash_function#CHD_algorithm
			 */
			[[nodiscard]] inline static constexpr uint32_t seed( uint32_t seed, uint32_t hash, size_t size ) noexcept;
		};
	}

	//======================================================================
	// Exception classes
	//======================================================================

	/**
	 * @brief Exception thrown when a requested key is not found in the dictionary.
	 * @details This exception is thrown by operator[] and at() methods when
	 *          the specified key does not exist in the dictionary.
	 */
	class key_not_found_exception : public std::runtime_error
	{
	public:
		/**
		 * @brief Constructs a key not found exception.
		 * @param[in] key The key that was not found.
		 */
		explicit key_not_found_exception( std::string_view key );
	};

	/**
	 * @brief Exception thrown when an operation is not valid due to the current state.
	 * @details This exception is thrown by enumerator operations when the enumerator
	 *          is not positioned on a valid element.
	 */
	class invalid_operation_exception : public std::runtime_error
	{
	public:
		/**
		 * @brief Constructs an invalid operation exception with default message.
		 */
		invalid_operation_exception();

		/**
		 * @brief Constructs an invalid operation exception with custom message.
		 * @param[in] message The exception message.
		 */
		explicit invalid_operation_exception( std::string_view message );
	};

	//=====================================================================
	// ChdDictionary class
	//=====================================================================

	/**
	 * @class ChdDictionary
	 * @brief A read-only dictionary using the Compress, Hash, and Displace (CHD)
	 * perfect hashing algorithm for guaranteed O(1) worst-case lookups after construction.
	 *
	 * @details Provides O(1) expected lookup time with minimal memory overhead for essentially read-only
	 * dictionaries. It uses a two-level perfect hashing scheme based on the CHD algorithm
	 * by Botelho, Pagh, and Ziviani, ensuring no collisions for the stored keys.
	 * This implementation is suitable for scenarios where a fixed set of key-value pairs
	 * needs to be queried frequently and efficiently. It includes optimizations like
	 * optional SSE4.2 hashing and thread-local caching.
	 *
	 * @tparam TValue The type of values stored in the dictionary.
	 *
	 * @warning **UTF-16 Compatibility Note:**
	 * This C++ implementation is designed to be **FULLY COMPATIBLE** with the C# version.
	 * Both implementations simulate UTF-16 string processing to ensure identical hash values:
	 *   - **C++ Processing:** Processes each ASCII character as 2 bytes (low byte + high byte 0)
	 *     to match UTF-16 encoding used by C# strings.
	 *   - **C# Processing:** Processes UTF-16 strings by reading low byte of each character
	 *     and skipping the high byte (which is 0 for ASCII characters).
	 * This ensures **binary compatibility** and **identical hash values** between C++ and C# versions.
	 * Dictionaries created by either implementation can be used interchangeably.
	 *
	 * @see https://en.wikipedia.org/wiki/Perfect_hash_function#CHD_algorithm
	 */
	template <typename TValue>
	class ChdDictionary final
	{
	public:
		//----------------------------------------------
		// Forward declarations
		//----------------------------------------------

		class Iterator;
		class Enumerator;

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs the dictionary from a vector of key-value pairs.
		 * @param[in] items A vector of key-value pairs. The keys must be unique.
		 * @throws std::invalid_argument if duplicate keys are found.
		 * @throws std::runtime_error if perfect hash construction fails.
		 */
		inline explicit ChdDictionary( std::vector<std::pair<std::string, TValue>>&& items );

		/** @brief Default constructor */
		ChdDictionary() = default;

		/** @brief Copy constructor */
		ChdDictionary( const ChdDictionary& other ) = default;

		/** @brief Move constructor */
		ChdDictionary( ChdDictionary&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~ChdDictionary() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		ChdDictionary& operator=( const ChdDictionary& other ) = default;

		/** @brief Move assignment operator */
		ChdDictionary& operator=( ChdDictionary&& other ) noexcept = default;

		//----------------------------------------------
		// Lookup operators
		//----------------------------------------------

		/**
		 * @brief Accesses the value associated with the specified key (non-const version).
		 * @details Provides read-write access to the value. Performs a lookup using the perfect hash function.
		 *          Allows modification of the retrieved value if TValue is mutable.
		 * @param[in] key The key whose associated value is to be retrieved.
		 * @return A reference to the value associated with `key`.
		 * @throws key_not_found_exception if the `key` is not found in the dictionary or if the dictionary is empty.
		 */
		[[nodiscard]] inline TValue& operator[]( std::string_view key );

		//----------------------------------------------
		// Lookup methods
		//----------------------------------------------

		/**
		 * @brief Accesses the value associated with the specified key (const version with bounds checking).
		 * @details Provides read-only access to the value. Performs a lookup using the perfect hash function.
		 * @param[in] key The key whose associated value is to be retrieved.
		 * @return A constant reference to the value associated with `key`.
		 * @throws key_not_found_exception if the `key` is not found in the dictionary or if the dictionary is empty.
		 */
		[[nodiscard]] inline const TValue& at( std::string_view key ) const;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Returns the number of elements in the dictionary.
		 * @return The number of key-value pairs stored in the dictionary.
		 */
		[[nodiscard]] inline size_t size() const noexcept;

		//----------------------------------------------
		// State inspection methods
		//----------------------------------------------

		/**
		 * @brief Checks if the dictionary is empty.
		 * @return `true` if the dictionary contains no elements, `false` otherwise.
		 */
		[[nodiscard]] inline bool isEmpty() const noexcept;

		//----------------------------------------------
		// Static query methods
		//----------------------------------------------

		/**
		 * @brief Attempts to retrieve the value associated with the specified key without throwing exceptions.
		 * @details Performs a lookup using the perfect hash function. If the key is found, the output
		 *          parameter `outValue` is updated to point to the associated value within the dictionary's
		 *          internal storage.
		 * @param[in] key The key whose associated value is to be retrieved.
		 * @param[out] outValue A reference to a pointer-to-const TValue. On success, this pointer will be
		 *                      set to the address of the found value. On failure, it will be set to `nullptr`.
		 * @return `true` if the `key` was found and `outValue` was updated, `false` otherwise.
		 */
		[[nodiscard]] inline bool tryGetValue( std::string_view key, const TValue*& outValue ) const noexcept;

		//----------------------------------------------
		// Iteration
		//----------------------------------------------

		/**
		 * @brief Gets an iterator pointing to the first element of the dictionary.
		 * @return An `Iterator` positioned at the beginning of the dictionary's data.
		 *         If the dictionary is empty, this will be equal to `end()`.
		 */
		[[nodiscard]] inline Iterator begin() const noexcept;

		/**
		 * @brief Gets an iterator pointing past the last element of the dictionary.
		 * @return An `Iterator` representing the position after the last element.
		 *         This iterator should not be dereferenced.
		 */
		[[nodiscard]] inline Iterator end() const noexcept;

		//----------------------------------------------
		// Enumeration
		//----------------------------------------------

		/**
		 * @brief Gets an enumerator for the dictionary key-value pairs.
		 * @return An enumerator for iterating through the key-value pairs.
		 * @details The enumerator starts positioned before the first element.
		 */
		[[nodiscard]] inline Enumerator enumerator() const noexcept;

		//----------------------------------------------
		// Public helper methods
		//----------------------------------------------

		//---------------------------
		// Hashing
		//---------------------------

		/**
		 * @brief Calculates hash value using hardware-accelerated CRC32 or software FNV-1a fallback.
		 * @details Two-path implementation:
		 *   - **SSE4.2 Path:** Uses _mm_crc32_u64 for 4-character chunks (3-5x faster)
		 *   - **Fallback Path:** Uses FNV-1a algorithm for compatibility
		 *   Both paths simulate UTF-16 processing for C# compatibility.
		 * @param[in] key ASCII string key to hash
		 * @return 32-bit hash value.
		 */
		[[nodiscard]] static inline uint32_t hash( std::string_view key ) noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief The primary storage table containing the key-value pairs. Order determined during construction. */
		std::vector<std::pair<std::string, TValue>> m_table;

		/** @brief The seed values used by the CHD perfect hash function to resolve hash collisions. Size matches `m_table`. */
		std::vector<int> m_seeds;

	public:
		//----------------------------------------------
		// ChdDictionary::Iterator class
		//----------------------------------------------

		/**
		 * @class Iterator
		 * @brief Enables iteration over dictionary key-value pairs.
		 *
		 * @details Provides sequential access to all stored elements with support for:
		 *          - Range-based for loops
		 *          - STL algorithms (std::find_if, std::for_each, etc.)
		 *          - Manual iteration with ++, *, and -> operators
		 *
		 * @code{.cpp}
		 * ChdDictionary<int> dict(items);
		 *
		 * // Range-based for loop (recommended)
		 * for (const auto& [key, value] : dict) {
		 *     std::cout << key << " = " << value << std::endl;
		 * }
		 *
		 * // STL algorithm usage
		 * auto found = std::find_if(dict.begin(), dict.end(),
		 *     [](const auto& pair) { return pair.second > 100; });
		 *
		 * // Manual iteration
		 * for (auto it = dict.begin(); it != dict.end(); ++it) {
		 *     std::cout << it->first << " = " << it->second << std::endl;
		 * }
		 * @endcode
		 */
		class Iterator final
		{
		public:
			//----------------------------
			// Construction
			//----------------------------

			/**
			 * @brief Constructs an iterator pointing to a specific element in the dictionary's table.
			 * @param[in] table Pointer to the dictionary's internal storage vector. Must not be null.
			 * @param[in] index The index within the table this iterator should point to.
			 * @note If index >= table->size(), the iterator represents an end iterator.
			 */
			explicit Iterator( const std::vector<std::pair<std::string, TValue>>* table, size_t index ) noexcept;

			/** @brief Default constructor */
			Iterator() = default;

			/** @brief Copy constructor */
			Iterator( const Iterator& ) = default;

			/** @brief Move constructor */
			Iterator( Iterator&& ) noexcept = default;

			//----------------------------
			// Destruction
			//----------------------------

			/** @brief Destructor */
			~Iterator() = default;

			//----------------------------
			// Assignment operators
			//----------------------------

			/** @brief Copy assignment operator */
			Iterator& operator=( const Iterator& ) = default;

			/** @brief Move assignment operator */
			Iterator& operator=( Iterator&& ) noexcept = default;

			//---------------------------
			// Operations
			//---------------------------

			/**
			 * @brief Dereferences the iterator to access the current key-value pair.
			 * @return A constant reference to the `std::pair<std::string, TValue>` at the current position.
			 */
			[[nodiscard]] inline const std::pair<std::string, TValue>& operator*() const;

			/**
			 * @brief Provides member access to the current key-value pair.
			 * @return A constant pointer to the `std::pair<std::string, TValue>` at the current position.
			 */
			[[nodiscard]] inline const std::pair<std::string, TValue>* operator->() const;

			/**
			 * @brief Advances the iterator to the next element (pre-increment).
			 * @return A reference to this iterator after advancing.
			 */
			inline Iterator& operator++() noexcept;

			/**
			 * @brief Advances the iterator to the next element (post-increment).
			 * @return A copy of the iterator *before* it was advanced.
			 */
			[[nodiscard]] inline Iterator operator++( int ) noexcept;

			//---------------------------
			// Comparison
			//---------------------------

			/**
			 * @brief Checks if this iterator is equal to another iterator.
			 * @param[in] other The iterator to compare against.
			 * @return `true` if both iterators point to the same element or are both end iterators for the same container, `false` otherwise.
			 */
			[[nodiscard]] inline bool operator==( const Iterator& other ) const noexcept;

			/**
			 * @brief Checks if this iterator is not equal to another iterator.
			 * @param[in] other The iterator to compare against.
			 * @return `true` if the iterators point to different elements, `false` otherwise.
			 */
			[[nodiscard]] inline bool operator!=( const Iterator& other ) const noexcept;

		private:
			//---------------------------
			// Private member variables
			//---------------------------

			/** @brief Pointer to the dictionary's internal data table. Null for default-constructed iterators. */
			const std::vector<std::pair<std::string, TValue>>* m_table = nullptr;

			/** @brief Current index within the `m_table`. */
			size_t m_index = 0;
		};

		//----------------------------------------------
		// ChdDictionary::Enumerator class
		//----------------------------------------------

		/**
		 * @class Enumerator
		 * @brief Provides explicit enumeration over dictionary key-value pairs.
		 *
		 * @details Maintains internal position state with these methods:
		 *          - `next()` - Advances to next element, returns true if successful
		 *          - `current()` - Gets the current element
		 *          - `reset()` - Returns to initial position
		 *
		 *          Must call `next()` before first `current()` access.
		 *
		 * @code{.cpp}
		 * auto enumerator = dict.enumerator();
		 *
		 * // Enumerate all elements
		 * while (enumerator.next()) {
		 *     const auto& [key, value] = enumerator.current();
		 *     std::cout << key << " = " << value << std::endl;
		 * }
		 *
		 * // Reset and enumerate with condition
		 * enumerator.reset();
		 * while (enumerator.next()) {
		 *     if (enumerator.current().second > 100) {
		 *         processHighValue(enumerator.current());
		 *     }
		 * }
		 * @endcode
		 */
		class Enumerator final
		{
		public:
			//----------------------------
			// Construction
			//----------------------------

			/**
			 * @brief Constructs an enumerator for the given dictionary table.
			 * @param table Pointer to the dictionary's internal storage vector.
			 */
			explicit Enumerator( const std::vector<std::pair<std::string, TValue>>* table ) noexcept;

			/** @brief Default constructor */
			Enumerator() = delete;

			/** @brief Copy constructor */
			Enumerator( const Enumerator& ) = default;

			/** @brief Move constructor */
			Enumerator( Enumerator&& ) noexcept = default;

			//----------------------------
			// Destruction
			//----------------------------

			/** @brief Destructor */
			~Enumerator() = default;

			//----------------------------
			// Assignment operators
			//----------------------------

			/** @brief Copy assignment operator */
			Enumerator& operator=( const Enumerator& ) = default;

			/** @brief Move assignment operator */
			Enumerator& operator=( Enumerator&& ) noexcept = default;

			//----------------------------
			// Enumeration
			//----------------------------

			/**
			 * @brief Advances the enumerator to the next element.
			 * @return True if the enumerator successfully moved to the next element;
			 *         false if the enumerator has passed the end of the collection.
			 */
			[[nodiscard]] inline bool next() noexcept;

			/**
			 * @brief Gets the current key-value pair from the enumerator.
			 * @details Returns the element that the enumerator is currently positioned on.
			 *          The enumerator must be positioned on a valid element by calling `next()`
			 *          and ensuring it returned `true`.
			 * @return A constant reference to the current `std::pair<std::string, TValue>`.
			 * @throws invalid_operation_exception if the enumerator is not positioned on a valid element.
			 */
			[[nodiscard]] inline const std::pair<std::string, TValue>& current() const;

			/** @brief Resets the enumerator to its initial position. */
			inline void reset() noexcept;

		private:
			//----------------------------
			// Private member variables
			//----------------------------

			/** @brief Pointer to the dictionary's internal data table. */
			const std::vector<std::pair<std::string, TValue>>* m_table = nullptr;

			/** @brief Current index within the table.*/
			size_t m_index;
		};
	};
}

#include "ChdDictionary.inl"
