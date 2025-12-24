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
 * @file LocalIdParsingErrorBuilder.h
 * @brief Error builder for Location parsing validation
 * @details Accumulates validation errors during Location string parsing with specific error types
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace dnv::vista::sdk
{
	class ParsingErrors;

	namespace internal
	{
		/**
		 * @enum LocalIdParsingState
		 * @brief State codes for LocalId string parsing
		 * @details Indicates the parsing state or validation error encountered during LocalId parsing
		 */
		enum class LocalIdParsingState : std::uint8_t
		{
			NamingRule = 0,
			VisVersion,
			PrimaryItem,
			SecondaryItem,
			ItemDescription,
			MetaQuantity,
			MetaContent,
			MetaCalculation,
			MetaState,
			MetaCommand,
			MetaType,
			MetaPosition,
			MetaDetail,

			EmptyState = 100,
			Formatting = 101,
			Completeness = 102,

			NamingEntity = 200,
			IMONumber = 201
		};

		/**
		 * @class LocalIdParsingErrorBuilder
		 * @brief Builder for accumulating LocalId parsing errors
		 * @details Collects validation errors with specific error types during LocalId string parsing.
		 *          Use default constructor to instantiate, then addError() to accumulate errors,
		 *          and build() to produce final ParsingErrors object.
		 */
		class LocalIdParsingErrorBuilder final
		{
		public:
			/** @brief Default constructor */
			LocalIdParsingErrorBuilder() = default;

			/** @brief Copy constructor */
			LocalIdParsingErrorBuilder( const LocalIdParsingErrorBuilder& ) = default;

			/** @brief Move constructor */
			LocalIdParsingErrorBuilder( LocalIdParsingErrorBuilder&& ) noexcept = default;

			/** @brief Destructor */
			~LocalIdParsingErrorBuilder() = default;

			/** @brief Copy assignment operator */
			LocalIdParsingErrorBuilder& operator=( const LocalIdParsingErrorBuilder& ) = default;

			/** @brief Move assignment operator */
			LocalIdParsingErrorBuilder& operator=( LocalIdParsingErrorBuilder&& ) noexcept = default;

			/**
			 * @brief Get empty singleton instance
			 * @return Const reference to empty error builder
			 */
			[[nodiscard]] static const LocalIdParsingErrorBuilder& empty() noexcept;

			/**
			 * @brief Check if any errors have been accumulated
			 * @return True if errors exist, false otherwise
			 */
			[[nodiscard]] bool hasError() const noexcept;

			/**
			 * @brief Build the final ParsingErrors object
			 * @return ParsingErrors containing all accumulated errors
			 */
			[[nodiscard]] ParsingErrors build() const;

			/**
			 * @brief Add a validation error
			 * @param validationResult Type of validation error
			 * @param message Error message description
			 * @return Reference to this builder for chaining
			 */
			LocalIdParsingErrorBuilder& addError( LocalIdParsingState state, std::string_view message );

		private:
			std::vector<std::pair<LocalIdParsingState, std::string>> m_errors; ///< Accumulated validation errors
		};
	} // namespace internal
} // namespace dnv::vista::sdk
