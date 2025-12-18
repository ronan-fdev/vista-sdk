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
 * @file ParsingErrors.inl
 * @brief Inline method implementations for ParsingErrors
 */

namespace dnv::vista::sdk
{
	inline bool ParsingErrors::operator==( const ParsingErrors& other ) const noexcept
	{
		return m_errors == other.m_errors;
	}

	inline bool ParsingErrors::operator!=( const ParsingErrors& other ) const noexcept
	{
		return !( *this == other );
	}

	inline size_t ParsingErrors::count() const noexcept
	{
		return m_errors.size();
	}

	inline bool ParsingErrors::hasErrors() const noexcept
	{
		return !m_errors.empty();
	}

	inline ParsingErrors::Iterator ParsingErrors::begin() const noexcept
	{
		return m_errors.begin();
	}

	inline ParsingErrors::Iterator ParsingErrors::end() const noexcept
	{
		return m_errors.end();
	}
} // namespace dnv::vista::sdk
