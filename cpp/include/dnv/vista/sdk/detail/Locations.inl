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
 * @file Locations.inl
 * @brief Inline implementations for Location classes
 * @details Provides inline implementations for simple accessor methods.
 */

namespace dnv::vista::sdk
{
	inline Location::Location( std::string_view value ) noexcept
		: m_value{ value }
	{
	}

	inline bool Location::operator==( const Location& other ) const noexcept
	{
		return m_value == other.m_value;
	}

	inline bool Location::operator!=( const Location& other ) const noexcept
	{
		return m_value != other.m_value;
	}

	inline const std::string& Location::value() const noexcept
	{
		return m_value;
	}

	inline RelativeLocation::RelativeLocation(
		char code,
		std::string_view name,
		const Location& location,
		std::optional<std::string> definition ) noexcept
		: m_code{ code },
		  m_name{ name },
		  m_location{ location },
		  m_definition{ std::move( definition ) }
	{
	}

	inline bool RelativeLocation::operator==( const RelativeLocation& other ) const noexcept
	{
		return m_code == other.m_code;
	}

	inline bool RelativeLocation::operator!=( const RelativeLocation& other ) const noexcept
	{
		return !( m_code == other.m_code );
	}

	inline char RelativeLocation::code() const noexcept
	{
		return m_code;
	}

	inline const std::string& RelativeLocation::name() const noexcept
	{
		return m_name;
	}

	inline const std::optional<std::string>& RelativeLocation::definition() const noexcept
	{
		return m_definition;
	}

	inline const Location& RelativeLocation::location() const noexcept
	{
		return m_location;
	}

	inline VisVersion Locations::version() const noexcept
	{
		return m_visVersion;
	}

	inline const std::vector<RelativeLocation>& Locations::relativeLocations() const noexcept
	{
		return m_relativeLocations;
	}

	inline const std::unordered_map<LocationGroup, std::vector<RelativeLocation>>& Locations::groups() const noexcept
	{
		return m_groups;
	}
} // namespace dnv::vista::sdk
