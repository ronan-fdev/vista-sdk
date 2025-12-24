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
 * @file LocalId.cpp
 * @brief Implementation of VIS LocalId class
 * @details Provides implementation for the immutable LocalId class.
 */

#include "dnv/vista/sdk/LocalId.h"

#include "dnv/vista/sdk/LocalIdBuilder.h"

#include <stdexcept>

namespace dnv::vista::sdk
{
	LocalId::LocalId( const LocalIdBuilder& builder )
		: m_builder{ builder }
	{
		if ( m_builder.isEmpty() )
		{
			throw std::invalid_argument{ "LocalId cannot be constructed from empty LocalIdBuilder" };
		}
		if ( !m_builder.isValid() )
		{
			throw std::invalid_argument{ "LocalId cannot be constructed from invalid LocalIdBuilder" };
		}
	}

	std::string LocalId::toString() const
	{
		return m_builder.toString();
	}

	std::optional<LocalId> LocalId::fromString( std::string_view localIdStr ) noexcept
	{
		return LocalIdBuilder::fromString( localIdStr );
	}

	std::optional<LocalId> LocalId::fromString( std::string_view localIdStr, ParsingErrors& errors ) noexcept
	{
		return LocalIdBuilder::fromString( localIdStr, errors );
	}
} // namespace dnv::vista::sdk
