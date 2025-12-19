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
 * @file LocationParsingErrorBuilder.cpp
 * @brief Implementation of LocationParsingErrorBuilder
 */

#include "LocationParsingErrorBuilder.h"

#include "dnv/vista/sdk/ParsingErrors.h"

namespace dnv::vista::sdk::internal
{
	const LocationParsingErrorBuilder& LocationParsingErrorBuilder::empty() noexcept
	{
		static const LocationParsingErrorBuilder instance{};
		return instance;
	}

	bool LocationParsingErrorBuilder::hasError() const noexcept
	{
		return !m_errors.empty();
	}

	ParsingErrors LocationParsingErrorBuilder::build() const
	{
		if ( m_errors.empty() )
		{
			return ParsingErrors::empty();
		}

		std::vector<ParsingErrors::ErrorEntry> errors;
		errors.reserve( m_errors.size() );

		for ( const auto& [result, message] : m_errors )
		{
			std::string resultStr;
			switch ( result )
			{
				case LocationValidationResult::Invalid:
					resultStr = "Invalid";
					break;
				case LocationValidationResult::InvalidCode:
					resultStr = "InvalidCode";
					break;
				case LocationValidationResult::InvalidOrder:
					resultStr = "InvalidOrder";
					break;
				case LocationValidationResult::NullOrWhiteSpace:
					resultStr = "NullOrWhiteSpace";
					break;
				case LocationValidationResult::Valid:
					resultStr = "Valid";
					break;
			}

			errors.emplace_back( std::move( resultStr ), message );
		}

		return ParsingErrors{ std::move( errors ) };
	}

	LocationParsingErrorBuilder& LocationParsingErrorBuilder::addError( LocationValidationResult validationResult,
		std::string_view message )
	{
		m_errors.emplace_back( validationResult, std::string{ message } );
		return *this;
	}
} // namespace dnv::vista::sdk::internal
