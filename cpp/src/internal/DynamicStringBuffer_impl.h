/**
 * @file DynamicStringBuffer_impl.h
 * @brief Pimpl implementation for DynamicStringBuffer wrapper around fmt::memory_buffer
 * @details Hidden implementation details for the DynamicStringBuffer facade class
 */

#pragma once

#include <fmt/format.h>
#include <string>
#include <string_view>

namespace dnv::vista::sdk::internal
{
	/**
	 * @brief Pimpl implementation class for DynamicStringBuffer
	 * @details Encapsulates fmt::memory_buffer and provides controlled access to its functionality.
	 */
	class DynamicStringBuffer_impl final
	{
	public:
		//----------------------------------------------
		// Construction & Destruction
		//----------------------------------------------

		/** @brief Default constructor - creates empty buffer with default capacity */
		DynamicStringBuffer_impl();

		/** @brief Constructor with initial capacity */
		explicit DynamicStringBuffer_impl( size_t initialCapacity );

		/** @brief Destructor */
		~DynamicStringBuffer_impl() = default;

		/** @brief Copy constructor */
		DynamicStringBuffer_impl( const DynamicStringBuffer_impl& other );

		/** @brief Move constructor */
		DynamicStringBuffer_impl( DynamicStringBuffer_impl&& other ) noexcept;

		/** @brief Copy assignment */
		DynamicStringBuffer_impl& operator=( const DynamicStringBuffer_impl& other );

		/** @brief Move assignment */
		DynamicStringBuffer_impl& operator=( DynamicStringBuffer_impl&& other ) noexcept;

		//----------------------------------------------
		// Buffer access methods
		//----------------------------------------------

		/** @brief Gets reference to underlying fmt::memory_buffer */
		fmt::memory_buffer& buffer() noexcept { return m_buffer; }

		/** @brief Gets const reference to underlying fmt::memory_buffer */
		const fmt::memory_buffer& buffer() const noexcept { return m_buffer; }

		//----------------------------------------------
		// Capacity and size management
		//----------------------------------------------

		/** @brief Returns current buffer size in bytes */
		size_t size() const noexcept;

		/** @brief Returns current buffer capacity in bytes */
		size_t capacity() const noexcept;

		/** @brief Checks if buffer is empty */
		bool empty() const noexcept;

		/** @brief Clears buffer contents (size becomes 0) */
		void clear() noexcept;

		/** @brief Reserves capacity for at least the specified number of bytes */
		void reserve( size_t newCapacity );

		/** @brief Resizes buffer to specified size */
		void resize( size_t newSize );

		//----------------------------------------------
		// Data access
		//----------------------------------------------

		/** @brief Returns pointer to buffer data */
		char* data() noexcept;

		/** @brief Returns const pointer to buffer data */
		const char* data() const noexcept;

		/** @brief Array access operator */
		char& operator[]( size_t index );

		/** @brief Const array access operator */
		const char& operator[]( size_t index ) const;

		//----------------------------------------------
		// Content manipulation
		//----------------------------------------------

		/** @brief Appends string_view to buffer */
		void append( std::string_view str );

		/** @brief Appends std::string to buffer */
		void append( const std::string& str );

		/** @brief Appends C-string to buffer */
		void append( const char* str );

		/** @brief Appends single character to buffer */
		void push_back( char c );

		//----------------------------------------------
		// String conversion
		//----------------------------------------------

		/** @brief Converts buffer contents to std::string */
		std::string toString() const;

		/** @brief Converts buffer contents to std::string_view */
		std::string_view toStringView() const noexcept;

		//----------------------------------------------
		// Iterator interface
		//----------------------------------------------

		/** @brief Returns iterator to beginning */
		char* begin() noexcept;

		/** @brief Returns const iterator to beginning */
		const char* begin() const noexcept;

		/** @brief Returns iterator to end */
		char* end() noexcept;

		/** @brief Returns const iterator to end */
		const char* end() const noexcept;

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		/** @brief The underlying fmt::memory_buffer */
		fmt::memory_buffer m_buffer;
	};
}
