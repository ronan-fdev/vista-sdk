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
 * @file StringHash.h
 * @brief Transparent hash functor for heterogeneous string lookups
 * @details Provides StringHash functor enabling zero-allocation lookups in unordered containers.
 *          Supports transparent hashing for std::string, std::string_view, and const char*.
 *          Used throughout vista-sdk for performance-optimized string-keyed collections.
 */

#pragma once

#include <string>
#include <string_view>

namespace dnv::vista::sdk
{
	/**
	 * @brief Transparent hash functor for string_view/string heterogeneous lookup
	 * @details Enables heterogeneous lookup in unordered_map without temporary string allocations.
	 *          Allows lookups with string_view, std::string, or const char* without conversion.
	 */
	struct StringHash
	{
		/** @brief Tag type to enable transparent hashing (C++20 heterogeneous lookup) */
		using is_transparent = void;
		/** @brief Hash implementation type */
		using hash_type = std::hash<std::string_view>;

		/**
		 * @brief Hash a string_view
		 * @param str String view to hash
		 * @return Hash value
		 */
		[[nodiscard]] std::size_t operator()( std::string_view str ) const noexcept
		{
			return hash_type{}( str );
		}

		/**
		 * @brief Hash a std::string
		 * @param str String to hash
		 * @return Hash value
		 */
		[[nodiscard]] std::size_t operator()( const std::string& str ) const noexcept
		{
			return hash_type{}( str );
		}

		/**
		 * @brief Hash a C-style string
		 * @param str C-string to hash
		 * @return Hash value
		 */
		[[nodiscard]] std::size_t operator()( const char* str ) const noexcept
		{
			return hash_type{}( str );
		}
	};
} // namespace dnv::vista::sdk
