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
 * @file ParsingErrors.h
 * @brief Container for parsing validation errors
 * @details Holds a collection of typed error messages from parsing operations.
 *          Provides STL-compatible iteration and error querying.
 */

#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace dnv::vista::sdk
{
	/**
	 * @brief Container for parsing validation errors
	 * @details Immutable collection of typed error messages from parsing operations.
	 *          Supports iteration and provides convenience methods for error checking.
	 *          Comparable and hashable for use in containers.
	 */
	class ParsingErrors final
	{
	public:
		/**
		 * @brief Single error entry with type and message
		 */
		struct ErrorEntry
		{
			std::string type;	 ///< Error type identifier
			std::string message; ///< Human-readable error message

			/**
			 * @brief Equality comparison operator
			 * @param other ErrorEntry to compare with
			 * @return True if both type and message are equal
			 */
			bool operator==( const ErrorEntry& other ) const noexcept
			{
				return type == other.type && message == other.message;
			}
		};

		/** @brief Const iterator type for iterating over error entries */
		using Iterator = std::vector<ErrorEntry>::const_iterator;

		/**
		 * @brief Constructor from error vector
		 * @param errors Vector of error entries (will be moved)
		 */
		explicit ParsingErrors( std::vector<ErrorEntry>&& errors ) noexcept;

		/** @brief Default constructor */
		ParsingErrors() = default;

		/** @brief Copy constructor */
		ParsingErrors( const ParsingErrors& ) = default;

		/** @brief Move constructor */
		ParsingErrors( ParsingErrors&& ) noexcept = default;

		/** @brief Destructor */
		~ParsingErrors() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		ParsingErrors& operator=( const ParsingErrors& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		ParsingErrors& operator=( ParsingErrors&& ) noexcept = default;

		/**
		 * @brief Get empty singleton instance
		 * @return Const reference to empty ParsingErrors
		 */
		[[nodiscard]] static const ParsingErrors& empty() noexcept;

		/**
		 * @brief Equality comparison
		 * @param other ParsingErrors to compare with
		 * @return True if both contain same errors in same order
		 */
		[[nodiscard]] bool operator==( const ParsingErrors& other ) const noexcept;

		/**
		 * @brief Inequality comparison
		 * @param other ParsingErrors to compare with
		 * @return True if errors differ
		 */
		[[nodiscard]] bool operator!=( const ParsingErrors& other ) const noexcept;

		/**
		 * @brief Get number of errors
		 * @return Error count
		 */
		[[nodiscard]] size_t count() const noexcept;

		/**
		 * @brief Check if any errors exist
		 * @return True if errors present, false if empty
		 */
		[[nodiscard]] bool hasErrors() const noexcept;

		/**
		 * @brief Check if specific error type exists
		 * @param type Error type to search for
		 * @return True if error with given type exists
		 */
		[[nodiscard]] bool hasErrorType( std::string_view type ) const noexcept;

		/**
		 * @brief Convert to human-readable string
		 * @return Formatted error list or "Success" if empty
		 */
		[[nodiscard]] std::string toString() const;

		/**
		 * @brief Get iterator to first error
		 * @return Const iterator to beginning
		 */
		[[nodiscard]] Iterator begin() const noexcept;

		/**
		 * @brief Get iterator past last error
		 * @return Const iterator to end
		 */
		[[nodiscard]] Iterator end() const noexcept;

	private:
		std::vector<ErrorEntry> m_errors; ///< Collection of error entries
	};
} // namespace dnv::vista::sdk

#include "detail/ParsingErrors.inl"
