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
 * @file UniversalId.cpp
 * @brief Implementation of VIS UniversalId
 * @details Provides implementation for UniversalId class representing globally unique sensor identifiers.
 */

#include "dnv/vista/sdk/UniversalId.h"
#include "dnv/vista/sdk/ParsingErrors.h"

#include <stdexcept>

namespace dnv::vista::sdk
{
	UniversalId::UniversalId( const UniversalIdBuilder& builder )
		: m_builder{ builder },
		  m_localId{ builder.localId()->build() }
	{
		if ( !m_builder.isValid() )
		{
			throw std::invalid_argument(
				"Cannot create UniversalId: builder is invalid (missing IMO number or LocalId)" );
		}
	}

	std::string UniversalId::toString() const { return m_builder.toString(); }

	std::optional<UniversalId> UniversalId::fromString( std::string_view universalIdStr ) noexcept
	{
		ParsingErrors errors;
		return fromString( universalIdStr, errors );
	}

	std::optional<UniversalId> UniversalId::fromString( std::string_view universalIdStr,
		ParsingErrors& errors ) noexcept
	{
		return UniversalIdBuilder::fromString( universalIdStr, errors );
	}
} // namespace dnv::vista::sdk
