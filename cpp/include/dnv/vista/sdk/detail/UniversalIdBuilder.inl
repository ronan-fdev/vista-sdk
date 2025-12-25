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
 * @file UniversalIdBuilder.inl
 * @brief Inline implementations for UniversalIdBuilder class
 * @details Provides inline implementations for simple accessor methods.
 */

namespace dnv::vista::sdk
{
	inline bool UniversalIdBuilder::operator==( const UniversalIdBuilder& other ) const noexcept
	{
		return m_imoNumber == other.m_imoNumber &&
			   m_localIdBuilder == other.m_localIdBuilder;
	}

	inline bool UniversalIdBuilder::operator!=( const UniversalIdBuilder& other ) const noexcept
	{
		return !( *this == other );
	}

	inline const std::optional<ImoNumber>& UniversalIdBuilder::imoNumber() const noexcept
	{
		return m_imoNumber;
	}

	inline const std::optional<LocalIdBuilder>& UniversalIdBuilder::localId() const noexcept
	{
		return m_localIdBuilder;
	}

	inline bool UniversalIdBuilder::isValid() const noexcept
	{
		return m_imoNumber.has_value() &&
			   m_localIdBuilder.has_value() &&
			   m_localIdBuilder->isValid();
	}
} // namespace dnv::vista::sdk
