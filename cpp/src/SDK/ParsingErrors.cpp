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
 * @file ParsingErrors.cpp
 * @brief Implementation of ParsingErrors container
 */

#include "dnv/vista/sdk/ParsingErrors.h"

#include "dnv/vista/sdk/StringBuilder.h"

#include <algorithm>

namespace dnv::vista::sdk
{
	//=====================================================================
	// ParsingErrors class
	//=====================================================================

	ParsingErrors::ParsingErrors( std::vector<ErrorEntry>&& errors ) noexcept
		: m_errors{ std::move( errors ) }
	{
	}

	const ParsingErrors& ParsingErrors::empty() noexcept
	{
		static const ParsingErrors instance{};
		return instance;
	}

	bool ParsingErrors::hasErrorType( std::string_view type ) const noexcept
	{
		return std::any_of( m_errors.begin(), m_errors.end(), [type]( const ErrorEntry& entry ) {
			return entry.type == type;
		} );
	}

	std::string ParsingErrors::toString() const
	{
		if ( m_errors.empty() )
		{
			return "Success";
		}

		StringBuilder builder{ 32 };
		builder << "Parsing errors:\n";

		for ( const auto& [type, message] : m_errors )
		{
			builder << '\t' << type << " - " << message << '\n';
		}

		return builder.toString();
	}
} // namespace dnv::vista::sdk
