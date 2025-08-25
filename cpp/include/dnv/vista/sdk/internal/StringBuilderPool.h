/**
 * @file StringBuilderPool.h
 * @brief String pooling implementation for high-performance string building
 * @details Thread-safe shared pool with RAII lease management and statistics
 */

/*
 * TODO: Consider making StringBuilder extensible (Templates/CRTP/ADL ?)Abacus Shipmanagement
 */

#pragma once

#include "dnv/vista/sdk/config/Platform.h"

namespace dnv::vista::sdk::internal
{
	//=====================================================================
	// DynamicStringBuffer class
	//=====================================================================

	class DynamicStringBuffer
	{
		friend class DynamicStringBufferPool;

	private:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		DynamicStringBuffer();

		/**
		 * @brief Constructor with specified initial capacity
		 * @param initialCapacity Initial buffer capacity in bytes
		 * @details Pre-allocates buffer to avoid reallocations during initial growth
		 */
		explicit DynamicStringBuffer( size_t initialCapacity );

	public:
		/** @brief Copy constructor */
		DynamicStringBuffer( const DynamicStringBuffer& other );

		/** @brief Move constructor */
		DynamicStringBuffer( DynamicStringBuffer&& other ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DynamicStringBuffer();

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		DynamicStringBuffer& operator=( const DynamicStringBuffer& other );

		/** @brief Move assignment operator */
		DynamicStringBuffer& operator=( DynamicStringBuffer&& other ) noexcept;

		//----------------------------------------------
		// Capacity and size information
		//----------------------------------------------

		/**
		 * @brief Get current buffer size in bytes
		 * @return Number of bytes currently stored in buffer
		 * @details Returns actual content size, not allocated capacity
		 */
		[[nodiscard]] size_t size() const noexcept;

		/**
		 * @brief Get current buffer capacity in bytes
		 * @return Number of bytes allocated for buffer storage
		 * @details Capacity may be larger than size to avoid frequent reallocations
		 */
		[[nodiscard]] size_t capacity() const noexcept;

		/**
		 * @brief Check if buffer is empty
		 * @return true if buffer contains no data, false otherwise
		 */
		[[nodiscard]] bool isEmpty() const noexcept;

		//----------------------------------------------
		// Buffer management
		//----------------------------------------------

		/**
		 * @brief Clear buffer content without deallocating memory
		 * @details Sets size to 0 but preserves allocated capacity for reuse
		 */
		void clear() noexcept;

		/**
		 * @brief Reserve minimum capacity for buffer
		 * @param newCapacity Minimum desired capacity in bytes
		 * @details May allocate more than requested for efficiency
		 * @throws std::bad_alloc if memory allocation fails
		 */
		void reserve( size_t newCapacity );

		/**
		 * @brief Resize buffer to specified size
		 * @param newSize New buffer size in bytes
		 * @details May truncate content or extend with undefined bytes
		 * @throws std::bad_alloc if memory allocation fails
		 */
		void resize( size_t newSize );

		//----------------------------------------------
		// Data access
		//----------------------------------------------

		/**
		 * @brief Get mutable pointer to buffer data
		 * @return Pointer to first byte of buffer data
		 * @details Provides direct memory access for high-performance operations
		 */
		[[nodiscard]] char* data() noexcept;

		/**
		 * @brief Get immutable pointer to buffer data
		 * @return Const pointer to first byte of buffer data
		 * @details Safe read-only access to buffer contents
		 */
		[[nodiscard]] const char* data() const noexcept;

		/**
		 * @brief Access buffer element by index (mutable)
		 * @param index Zero-based index of element to access
		 * @return Reference to element at specified index
		 * @details No bounds checking - undefined behavior if index >= size()
		 */
		char& operator[]( size_t index );

		/**
		 * @brief Access buffer element by index (immutable)
		 * @param index Zero-based index of element to access
		 * @return Const reference to element at specified index
		 * @details No bounds checking - undefined behavior if index >= size()
		 */
		const char& operator[]( size_t index ) const;

		//----------------------------------------------
		// Content manipulation
		//----------------------------------------------

		/**
		 * @brief Append string_view content to buffer
		 * @param str String view to append
		 * @details Efficient append without copying string data
		 * @throws std::bad_alloc if buffer expansion fails
		 */
		void append( std::string_view str );

		/**
		 * @brief Append std::string content to buffer
		 * @param str String to append
		 * @details Convenience overload for std::string
		 * @throws std::bad_alloc if buffer expansion fails
		 */
		void append( const std::string& str );

		/**
		 * @brief Append null-terminated C string to buffer
		 * @param str Null-terminated string to append
		 * @details Safe handling of nullptr (no-op)
		 * @throws std::bad_alloc if buffer expansion fails
		 */
		void append( const char* str );

		/**
		 * @brief Append single character to buffer
		 * @param c Character to append
		 * @details Efficient single-character append
		 * @throws std::bad_alloc if buffer expansion fails
		 */
		void push_back( char c );

		//----------------------------------------------
		// String conversion
		//----------------------------------------------

		/**
		 * @brief Convert buffer content to std::string
		 * @return String copy of buffer content
		 * @details Creates new string object - consider toStringView() for read-only access
		 */
		[[nodiscard]] std::string toString() const;

		/**
		 * @brief Get string_view of buffer content
		 * @return String view referencing buffer data
		 * @details Zero-copy access - view becomes invalid if buffer is modified
		 */
		[[nodiscard]] std::string_view toStringView() const noexcept;

		//----------------------------------------------
		// Iterator interface
		//----------------------------------------------

		using value_type = char;
		using iterator = char*;
		using const_iterator = const char*;

		/**
		 * @brief Get mutable iterator to beginning of buffer
		 * @return Iterator pointing to first element
		 * @details Enables range-based for loops and STL algorithms
		 */
		[[nodiscard]] iterator begin() noexcept;

		/**
		 * @brief Get immutable iterator to beginning of buffer
		 * @return Const iterator pointing to first element
		 * @details Safe read-only iteration over buffer contents
		 */
		[[nodiscard]] const_iterator begin() const noexcept;

		/**
		 * @brief Get mutable iterator to end of buffer
		 * @return Iterator pointing one past last element
		 * @details Standard STL end iterator semantics
		 */
		[[nodiscard]] iterator end() noexcept;

		/**
		 * @brief Get immutable iterator to end of buffer
		 * @return Const iterator pointing one past last element
		 * @details Standard STL end iterator semantics
		 */
		[[nodiscard]] const_iterator end() const noexcept;

	private:
		//----------------------------------------------
		// Pimpl
		//----------------------------------------------

		void* m_impl;
	};

	//=====================================================================
	// StringBuilder class
	//=====================================================================

	class StringBuilder final
	{
		friend class StringBuilderLease;

		//----------------------------------------------
		// Construction
		//----------------------------------------------
	private:
		/** @brief Constructs StringBuilder wrapper around memory buffer */
		VISTA_SDK_CPP_FORCE_INLINE explicit StringBuilder( DynamicStringBuffer& buffer );

	public:
		/** @brief Default constructor */
		StringBuilder() = delete;

		/** @brief Copy constructor */
		StringBuilder( const StringBuilder& ) = default;

		/** @brief Move constructor */
		StringBuilder( StringBuilder&& ) noexcept = delete;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~StringBuilder() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		StringBuilder& operator=( const StringBuilder& ) = delete;

		/** @brief Move assignment operator */
		StringBuilder& operator=( StringBuilder&& ) noexcept = delete;

		//----------------------------------------------
		// Array access operators
		//----------------------------------------------

		/**
		 * @brief Provides read-write access to character at specified index
		 * @param index Zero-based character index
		 * @return Reference to character at the specified position
		 */
		VISTA_SDK_CPP_FORCE_INLINE char& operator[]( size_t index );

		/**
		 * @brief Provides read-only access to character at specified index
		 * @param index Zero-based character index
		 * @return Const reference to character at the specified position
		 */
		VISTA_SDK_CPP_FORCE_INLINE const char& operator[]( size_t index ) const;

		//----------------------------------------------
		// String append operations
		//----------------------------------------------

		/**
		 * @brief Appends string_view contents to the buffer efficiently
		 * @param str String view to append
		 */
		VISTA_SDK_CPP_FORCE_INLINE void append( std::string_view str );

		/**
		 * @brief Appends std::string contents to the buffer
		 * @param str String to append
		 */
		VISTA_SDK_CPP_FORCE_INLINE void append( const std::string& str );

		/**
		 * @brief Appends null-terminated C-string to the buffer
		 * @param str Null-terminated C-string to append (null pointer handled gracefully)
		 */
		VISTA_SDK_CPP_FORCE_INLINE void append( const char* str );

		/**
		 * @brief Appends single character to the buffer
		 * @param c Character to append
		 */
		VISTA_SDK_CPP_FORCE_INLINE void push_back( char c );

		//----------------------------------------------
		// Stream operators
		//----------------------------------------------

		/**
		 * @brief Stream operator for string_view
		 * @param str String view to append
		 * @return Reference to this StringBuilder for chaining
		 */
		VISTA_SDK_CPP_FORCE_INLINE StringBuilder& operator<<( std::string_view str );

		/**
		 * @brief Stream operator for std::string
		 * @param str String to append
		 * @return Reference to this StringBuilder for chaining
		 */
		VISTA_SDK_CPP_FORCE_INLINE StringBuilder& operator<<( const std::string& str );

		/**
		 * @brief Stream operator for C-string
		 * @param str Null-terminated C-string to append
		 * @return Reference to this StringBuilder for chaining
		 */
		VISTA_SDK_CPP_FORCE_INLINE StringBuilder& operator<<( const char* str );

		/**
		 * @brief Stream operator for single character
		 * @param c Character to append
		 * @return Reference to this StringBuilder for chaining
		 */
		VISTA_SDK_CPP_FORCE_INLINE StringBuilder& operator<<( char c );

		//----------------------------------------------
		// Size and capacity management
		//----------------------------------------------

		/** @brief Returns current buffer size in characters */
		VISTA_SDK_CPP_FORCE_INLINE size_t length() const noexcept;

		/**
		 * @brief Resizes buffer to specified character count
		 * @param newSize New buffer size in characters
		 */
		VISTA_SDK_CPP_FORCE_INLINE void resize( size_t newSize );

		//----------------------------------------------
		// Iterator interface
		//----------------------------------------------

		using value_type = char;
		using iterator = char*;
		using const_iterator = const char*;

		/** @brief Returns mutable iterator to beginning of character sequence */
		VISTA_SDK_CPP_FORCE_INLINE iterator begin();

		/** @brief Returns const iterator to beginning of character sequence */
		VISTA_SDK_CPP_FORCE_INLINE const_iterator begin() const;

		/** @brief Returns mutable iterator to end of character sequence */
		VISTA_SDK_CPP_FORCE_INLINE iterator end();

		/** @brief Returns const iterator to end of character sequence */
		VISTA_SDK_CPP_FORCE_INLINE const_iterator end() const;

		//----------------------------------------------
		// StringBuilder::Enumerator class
		//----------------------------------------------

		class Enumerator
		{
		public:
			//----------------------------
			// Construction
			//----------------------------

			/**
			 * @brief Constructs an enumerator for iterating over the characters in the given StringBuilder buffer.
			 * @param builder The StringBuilder to enumerate
			 * @note Calling current() is only valid after next() has returned true and until it returns false.
			 */
			VISTA_SDK_CPP_FORCE_INLINE Enumerator( const StringBuilder& builder );

			//----------------------------
			// Destruction
			//----------------------------

			/** @brief Destructor */
			~Enumerator() = default;

			//----------------------------
			// Enumerator operations
			//----------------------------

			/** @brief Advances to the next character in the buffer. Returns true if successful. */
			VISTA_SDK_CPP_FORCE_INLINE bool next();

			/** @brief Returns the current character in the buffer. */
			VISTA_SDK_CPP_FORCE_INLINE char current() const;

			/** @brief Resets the enumerator to the initial position (before the first character). */
			VISTA_SDK_CPP_FORCE_INLINE void reset();

		private:
			//----------------------------
			// Private member variables
			//----------------------------

			/** @brief Pointer to start of character data */
			const char* m_data;

			/** @brief Pointer to end of character data */
			const char* m_end;

			/** @brief Pointer to current position in iteration */
			const char* m_current;
		};

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Reference to the underlying buffer */
		DynamicStringBuffer& m_buffer;
	};

	//=====================================================================
	// StringBuilderLease class
	//=====================================================================

	class StringBuilderLease final
	{
		friend class StringBuilderPool;

		//----------------------------------------------
		// Construction
		//----------------------------------------------
	private:
		/** @brief Constructs lease with pooled buffer ownership */
		VISTA_SDK_CPP_FORCE_INLINE explicit StringBuilderLease( DynamicStringBuffer* buffer );

	public:
		/** @brief Default constructor */
		StringBuilderLease() = delete;

		/** @brief Copy constructor */
		StringBuilderLease( const StringBuilderLease& ) = delete;

		/** @brief Move constructor */
		VISTA_SDK_CPP_FORCE_INLINE StringBuilderLease( StringBuilderLease&& other ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~StringBuilderLease();

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		StringBuilderLease& operator=( const StringBuilderLease& ) = delete;

		/** @brief Move assignment operator */
		VISTA_SDK_CPP_FORCE_INLINE StringBuilderLease& operator=( StringBuilderLease&& other ) noexcept;

		//----------------------------------------------
		// Public interface methods
		//----------------------------------------------

		/** @brief Creates StringBuilder wrapper for buffer manipulation */
		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE StringBuilder builder();

		/** @brief Provides direct access to underlying memory buffer */
		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE DynamicStringBuffer& buffer();

		/** @brief Converts buffer contents to std::string */
		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE std::string toString() const;

	private:
		//----------------------------------------------
		// Private implementation methods
		//----------------------------------------------

		/** @brief Returns buffer to pool and invalidates lease */
		void dispose();

		/** @brief Throws exception for invalid lease access */
		[[noreturn]] void throwInvalidOperation() const;

		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Pointer to the pooled memory buffer */
		DynamicStringBuffer* m_buffer;

		/** @brief Flag indicating if the lease is valid and buffer is accessible */
		bool m_valid;
	};

	//=====================================================================
	// StringBuilderPool class
	//=====================================================================

	class StringBuilderPool final
	{
	public:
		//----------------------------------------------
		// Pool statistics structure
		//----------------------------------------------

		/** @brief Pool performance statistics for external access */
		struct PoolStatistics
		{
			uint64_t threadLocalHits;
			uint64_t dynamicStringBufferPoolHits;
			uint64_t newAllocations;
			uint64_t totalRequests;
			double hitRate;
		};

	private:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Private constructor for singleton pattern */
		StringBuilderPool() = default;

	public:
		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/**
		 * @brief Creates a new StringBuilder lease with an optimally sourced memory buffer
		 * @return StringBuilderLease managing a pooled buffer with automatic cleanup
		 * @details This is the primary factory method for obtaining StringBuilder instances.
		 *          The buffer is sourced using a three-tier optimization strategy:
		 *          1. Thread-local cache (fastest, zero synchronization overhead)
		 *          2. Shared cross-thread pool (fast, mutex-protected access)
		 *          3. New allocation (fallback, pre-sized for optimal performance)
		 *
		 *          The returned lease automatically returns the buffer to the pool when
		 *          destroyed, ensuring optimal memory reuse and preventing leaks.
		 *
		 * @note This method is thread-safe and optimized for high-frequency usage patterns.
		 *       Buffers are automatically cleared before reuse and size-limited to prevent bloat.
		 *
		 * @example
		 * @code
		 * auto lease = StringBuilderPool::lease();
		 * auto builder = lease.builder();
		 * builder << "Hello, " << "World!";
		 * std::string result = lease.toString();
		 * // Buffer automatically returned to pool when lease destructs
		 * @endcode
		 */
		[[nodiscard]] static StringBuilderLease lease();

		//----------------------------
		// Statistics methods
		//----------------------------

		/** @brief Gets current pool statistics */
		static PoolStatistics stats() noexcept;

		/** @brief Resets pool statistics */
		static void resetStats() noexcept;

		//----------------------------
		// Lease management
		//----------------------------

		/** @brief Clears all buffers from the pool and returns the count of cleared buffers */
		static size_t clear();

		/** @brief Gets current number of buffers stored in the pool */
		static size_t size() noexcept;
	};
}

#include "StringBuilderPool.inl"
