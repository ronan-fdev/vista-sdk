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
 * @file MetadataTag.inl
 * @brief MetadataTag inline implementations
 * @details Inline method implementations for MetadataTag accessors
 */

namespace dnv::vista::sdk
{
	inline bool MetadataTag::operator==( const MetadataTag& other ) const noexcept
	{
		return m_name == other.m_name && m_value == other.m_value;
	}

	inline bool MetadataTag::operator!=( const MetadataTag& other ) const noexcept
	{
		return !( *this == other );
	}

	inline CodebookName MetadataTag::name() const noexcept
	{
		return m_name;
	}

	inline const std::string& MetadataTag::value() const noexcept
	{
		return m_value;
	}

	inline char MetadataTag::prefix() const noexcept
	{
		return m_custom ? '~' : '-';
	}

	inline bool MetadataTag::isCustom() const noexcept
	{
		return m_custom;
	}
} // namespace dnv::vista::sdk
