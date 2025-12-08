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
 * @file Codebook.inl
 * @brief Inline implementations for Codebook class
 * @details Provides inline implementations for simple accessor methods.
 */

namespace dnv::vista::sdk
{
	inline CodebookName Codebook::name() const noexcept
	{
		return m_name;
	}

	inline const std::unordered_set<std::string, StringHash, std::equal_to<>>& Codebook::standardValues() const noexcept
	{
		return m_standardValues;
	}

	inline const std::unordered_set<std::string, StringHash, std::equal_to<>>& Codebook::groups() const noexcept
	{
		return m_groups;
	}

	inline bool Codebook::hasGroup( std::string_view group ) const noexcept
	{
		return m_groups.find( group ) != m_groups.end();
	}

	inline bool Codebook::hasStandardValue( std::string_view value ) const noexcept
	{
		if ( m_name == CodebookName::Position && !value.empty() )
		{
			bool allDigits = true;
			for ( char c : value )
			{
				if ( c < '0' || c > '9' )
				{
					allDigits = false;
					break;
				}
			}
			if ( allDigits )
			{
				return true;
			}
		}

		return m_standardValues.find( value ) != m_standardValues.end();
	}
} // namespace dnv::vista::sdk
