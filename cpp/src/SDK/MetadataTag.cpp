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
 * @file MetadataTag.cpp
 * @brief MetadataTag implementation
 * @details Implementation of MetadataTag constructor, operators, and string formatting
 */

#include "dnv/vista/sdk/MetadataTag.h"

namespace dnv::vista::sdk
{
	MetadataTag::MetadataTag( CodebookName name, std::string value, bool isCustom ) noexcept
		: m_name{ name },
		  m_custom{ isCustom },
		  m_value{ std::move( value ) }
	{
	}

	void MetadataTag::toString( StringBuilder& builder, char separator ) const
	{
		builder << CodebookNames::toPrefix( m_name )
				<< ( m_custom ? '~' : '-' )
				<< m_value
				<< separator;
	}
} // namespace dnv::vista::sdk
