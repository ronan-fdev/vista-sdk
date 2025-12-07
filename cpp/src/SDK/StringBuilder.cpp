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
 * @file StringBuilder.cpp
 * @brief StringBuilder implementation
 * @details Wrapper implementation around nfx::string::StringBuilder providing
 *          DNV Vista SDK public API without exposing third-party dependencies
 */

#include "dnv/vista/sdk/StringBuilder.h"

#include <nfx/string/StringBuilder.h>

namespace dnv::vista::sdk
{
	//=====================================================================
	// StringBuilder::Impl - Pimpl implementation
	//=====================================================================

	struct StringBuilder::Impl
	{
		nfx::string::StringBuilder builder;

		Impl() = default;

		explicit Impl( size_t capacity ) : builder{ capacity } {}
	};

	//=====================================================================
	// StringBuilder class
	//=====================================================================

	StringBuilder::StringBuilder()
		: m_impl{ new Impl() }
	{
	}

	StringBuilder::StringBuilder( size_t initialCapacity )
		: m_impl{ new Impl( initialCapacity ) }
	{
	}

	StringBuilder::~StringBuilder()
	{
		delete m_impl;
	}

	StringBuilder::StringBuilder( StringBuilder&& other ) noexcept
		: m_impl{ other.m_impl }
	{
		other.m_impl = nullptr;
	}

	StringBuilder& StringBuilder::operator=( StringBuilder&& other ) noexcept
	{
		if ( this != &other )
		{
			delete m_impl;
			m_impl = other.m_impl;
			other.m_impl = nullptr;
		}
		return *this;
	}

	std::string StringBuilder::toString() const
	{
		return m_impl->builder.toString();
	}

	std::string_view StringBuilder::toStringView() const
	{
		return m_impl->builder.toStringView();
	}

	size_t StringBuilder::size() const
	{
		return m_impl->builder.size();
	}

	bool StringBuilder::isEmpty() const
	{
		return m_impl->builder.isEmpty();
	}

	void StringBuilder::clear()
	{
		m_impl->builder.clear();
	}

	StringBuilder& StringBuilder::append( std::string_view str )
	{
		m_impl->builder.append( str );
		return *this;
	}

	StringBuilder& StringBuilder::append( const std::string& str )
	{
		m_impl->builder.append( str );
		return *this;
	}

	StringBuilder& StringBuilder::append( const char* str )
	{
		m_impl->builder.append( str );
		return *this;
	}

	StringBuilder& StringBuilder::append( char c )
	{
		m_impl->builder.append( c );
		return *this;
	}

	StringBuilder& StringBuilder::operator<<( std::string_view str )
	{
		m_impl->builder.append( str );
		return *this;
	}

	StringBuilder& StringBuilder::operator<<( const std::string& str )
	{
		m_impl->builder.append( str );
		return *this;
	}

	StringBuilder& StringBuilder::operator<<( const char* str )
	{
		m_impl->builder.append( str );
		return *this;
	}

	StringBuilder& StringBuilder::operator<<( char c )
	{
		m_impl->builder.append( c );
		return *this;
	}

	StringBuilder& StringBuilder::operator<<( std::int32_t value )
	{
		m_impl->builder.append( value );
		return *this;
	}

	StringBuilder& StringBuilder::operator<<( std::uint32_t value )
	{
		m_impl->builder.append( value );
		return *this;
	}

	StringBuilder& StringBuilder::operator<<( std::int64_t value )
	{
		m_impl->builder.append( value );
		return *this;
	}

	StringBuilder& StringBuilder::operator<<( std::uint64_t value )
	{
		m_impl->builder.append( value );
		return *this;
	}

	StringBuilder& StringBuilder::operator<<( float value )
	{
		m_impl->builder.append( value );
		return *this;
	}

	StringBuilder& StringBuilder::operator<<( double value )
	{
		m_impl->builder.append( value );
		return *this;
	}
} // namespace dnv::vista::sdk
