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
 * @file LocationBuilder.inl
 * @brief Inline implementations for LocationBuilder class
 * @details Provides inline implementations for simple accessor methods.
 */

namespace dnv::vista::sdk
{
	inline VisVersion LocationBuilder::version() const noexcept
	{
		return m_visVersion;
	}

	inline std::optional<int> LocationBuilder::number() const noexcept
	{
		return m_number;
	}

	inline std::optional<char> LocationBuilder::side() const noexcept
	{
		return m_side;
	}

	inline std::optional<char> LocationBuilder::vertical() const noexcept
	{
		return m_vertical;
	}

	inline std::optional<char> LocationBuilder::transverse() const noexcept
	{
		return m_transverse;
	}

	inline std::optional<char> LocationBuilder::longitudinal() const noexcept
	{
		return m_longitudinal;
	}

	inline LocationBuilder LocationBuilder::withoutNumber() const
	{
		return withoutValue( LocationGroup::Number );
	}

	inline LocationBuilder LocationBuilder::withoutSide() const
	{
		return withoutValue( LocationGroup::Side );
	}

	inline LocationBuilder LocationBuilder::withoutVertical() const
	{
		return withoutValue( LocationGroup::Vertical );
	}

	inline LocationBuilder LocationBuilder::withoutTransverse() const
	{
		return withoutValue( LocationGroup::Transverse );
	}

	inline LocationBuilder LocationBuilder::withoutLongitudinal() const
	{
		return withoutValue( LocationGroup::Longitudinal );
	}
} // namespace dnv::vista::sdk
