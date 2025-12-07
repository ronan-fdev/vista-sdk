/*
 * MIT License
 *
 * Copyright (c) 2025 DNV
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file StringBuilder.h
 * @brief String builder for Vista SDK
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace dnv::vista::sdk
{
	//=====================================================================
	// StringBuilder class
	//=====================================================================

	/**
	 * @brief String builder with fluent interface
	 * @warning Not thread-safe - do not share instances across threads
	 */
	class StringBuilder final
	{
	public:
		/**
		 * @brief Default constructor
		 */
		StringBuilder();

		/**
		 * @brief Constructor with initial capacity hint
		 * @param initialCapacity Hint for initial buffer capacity
		 */
		explicit StringBuilder( size_t initialCapacity );

		/**
		 * @brief Destructor
		 */
		~StringBuilder();

		/**
		 * @brief Move constructor
		 * @param other StringBuilder to move from
		 */
		StringBuilder( StringBuilder&& other ) noexcept;

		/**
		 * @brief Move assignment operator
		 * @param other StringBuilder to move from
		 * @return Reference to this StringBuilder after move assignment
		 */
		StringBuilder& operator=( StringBuilder&& other ) noexcept;

		/** @brief Copy constructor - deleted */
		StringBuilder( const StringBuilder& ) = delete;

		/** @brief Copy assignment - deleted */
		StringBuilder& operator=( const StringBuilder& ) = delete;

		/**
		 * @brief Converts buffer to std::string
		 * @return String copy of buffer contents
		 */
		[[nodiscard]] std::string toString() const;

		/**
		 * @brief Get zero-copy view of buffer content
		 * @return String view referencing buffer data (invalidated if buffer is modified)
		 * @note More efficient than toString() when copy is not needed
		 */
		[[nodiscard]] std::string_view toStringView() const;

		/**
		 * @brief Get current buffer size in bytes
		 * @return Number of bytes currently stored in buffer
		 */
		[[nodiscard]] size_t size() const;

		/**
		 * @brief Check if buffer is empty
		 * @return true if buffer contains no data, false otherwise
		 */
		[[nodiscard]] bool isEmpty() const;

		/**
		 * @brief Clear buffer content without deallocating memory
		 * @details Resets size to 0 but preserves allocated capacity for reuse
		 */
		void clear();

		/**
		 * @brief Appends string_view content to the buffer
		 * @param str String view to append
		 * @return Reference to this StringBuilder for method chaining
		 */
		StringBuilder& append( std::string_view str );

		/**
		 * @brief Appends std::string content to the buffer
		 * @param str String to append
		 * @return Reference to this StringBuilder for method chaining
		 */
		StringBuilder& append( const std::string& str );

		/**
		 * @brief Appends null-terminated C string to the buffer
		 * @param str Null-terminated string to append
		 * @return Reference to this StringBuilder for method chaining
		 */
		StringBuilder& append( const char* str );

		/**
		 * @brief Appends single character to the buffer
		 * @param c Character to append
		 * @return Reference to this StringBuilder for method chaining
		 */
		StringBuilder& append( char c );

		/**
		 * @brief Stream operator for string_view
		 * @param str String view to append
		 * @return Reference to this StringBuilder for method chaining
		 */
		StringBuilder& operator<<( std::string_view str );

		/**
		 * @brief Stream operator for std::string
		 * @param str String to append
		 * @return Reference to this StringBuilder for method chaining
		 */
		StringBuilder& operator<<( const std::string& str );

		/**
		 * @brief Stream operator for C-string
		 * @param str Null-terminated C-string to append
		 * @return Reference to this StringBuilder for method chaining
		 */
		StringBuilder& operator<<( const char* str );

		/**
		 * @brief Stream operator for single character
		 * @param c Character to append
		 * @return Reference to this StringBuilder for method chaining
		 */
		StringBuilder& operator<<( char c );

		/**
		 * @brief Stream operator for 32-bit signed integer
		 * @param value Integer value to append
		 * @return Reference to this StringBuilder for chaining
		 */
		StringBuilder& operator<<( std::int32_t value );

		/**
		 * @brief Stream operator for 32-bit unsigned integer
		 * @param value Integer value to append
		 * @return Reference to this StringBuilder for chaining
		 */
		StringBuilder& operator<<( std::uint32_t value );

		/**
		 * @brief Stream operator for 64-bit signed integer
		 * @param value Integer value to append
		 * @return Reference to this StringBuilder for chaining
		 */
		StringBuilder& operator<<( std::int64_t value );

		/**
		 * @brief Stream operator for 64-bit unsigned integer
		 * @param value Integer value to append
		 * @return Reference to this StringBuilder for chaining
		 */
		StringBuilder& operator<<( std::uint64_t value );

		/**
		 * @brief Stream operator for single-precision floating-point
		 * @param value Floating-point value to append
		 * @return Reference to this StringBuilder for chaining
		 */
		StringBuilder& operator<<( float value );

		/**
		 * @brief Stream operator for double-precision floating-point
		 * @param value Floating-point value to append
		 * @return Reference to this StringBuilder for chaining
		 */
		StringBuilder& operator<<( double value );

	private:
		struct Impl;
		Impl* m_impl;
	};
} // namespace dnv::vista::sdk
