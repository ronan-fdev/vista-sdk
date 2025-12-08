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
 * @file Codebooks.inl
 * @brief Inline implementations for Codebooks class
 */

#include <stdexcept>

#include "dnv/vista/sdk/VisVersions.h"

namespace dnv::vista::sdk
{
	inline const Codebook& Codebooks::operator[]( CodebookName name ) const
	{
		// Convert enum to 0-based index
		auto index = static_cast<size_t>( name ) - 1;

		if ( index >= m_codebooks.size() || !m_codebooks[index].has_value() )
		{
			throw std::out_of_range{ "Invalid codebook name" };
		}

		return *m_codebooks[index];
	}

	inline VisVersion Codebooks::version() const noexcept
	{
		return m_version;
	}
} // namespace dnv::vista::sdk
