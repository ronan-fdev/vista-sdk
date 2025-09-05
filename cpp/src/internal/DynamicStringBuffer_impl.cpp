/**
 * @file DynamicStringBuffer_impl.cpp
 * @brief Implementation of DynamicStringBuffer_impl Pimpl class
 * @details Provides concrete implementation for DynamicStringBuffer facade wrapping fmt::memory_buffer
 */

#include "DynamicStringBuffer_impl.h"
#include <cstring>

namespace dnv::vista::sdk::internal
{
	//----------------------------------------------
	// Construction & Destruction
	//----------------------------------------------

	DynamicStringBuffer_impl::DynamicStringBuffer_impl()
		: m_buffer{}
	{
		m_buffer.reserve( 256 ); // Default initial capacity
	}

	DynamicStringBuffer_impl::DynamicStringBuffer_impl( size_t initialCapacity )
		: m_buffer{}
	{
		m_buffer.reserve( initialCapacity );
	}

	DynamicStringBuffer_impl::DynamicStringBuffer_impl( const DynamicStringBuffer_impl& other )
		: m_buffer{}
	{
		m_buffer.reserve( other.m_buffer.capacity() );
		m_buffer.append( other.m_buffer.data(), other.m_buffer.data() + other.m_buffer.size() );
	}

	DynamicStringBuffer_impl::DynamicStringBuffer_impl( DynamicStringBuffer_impl&& other ) noexcept
		: m_buffer{ std::move( other.m_buffer ) }
	{
	}

	DynamicStringBuffer_impl& DynamicStringBuffer_impl::operator=( const DynamicStringBuffer_impl& other )
	{
		if ( this != &other )
		{
			m_buffer.clear();
			m_buffer.reserve( other.m_buffer.capacity() );
			m_buffer.append( other.m_buffer.data(), other.m_buffer.data() + other.m_buffer.size() );
		}
		return *this;
	}

	DynamicStringBuffer_impl& DynamicStringBuffer_impl::operator=( DynamicStringBuffer_impl&& other ) noexcept
	{
		if ( this != &other )
		{
			m_buffer = std::move( other.m_buffer );
		}
		return *this;
	}

	//----------------------------------------------
	// Capacity and size management
	//----------------------------------------------

	size_t DynamicStringBuffer_impl::size() const noexcept
	{
		return m_buffer.size();
	}

	size_t DynamicStringBuffer_impl::capacity() const noexcept
	{
		return m_buffer.capacity();
	}

	bool DynamicStringBuffer_impl::empty() const noexcept
	{
		return m_buffer.size() == 0;
	}

	void DynamicStringBuffer_impl::clear() noexcept
	{
		m_buffer.clear();
	}

	void DynamicStringBuffer_impl::reserve( size_t newCapacity )
	{
		m_buffer.reserve( newCapacity );
	}

	void DynamicStringBuffer_impl::resize( size_t newSize )
	{
		m_buffer.resize( newSize );
	}

	//----------------------------------------------
	// Data access
	//----------------------------------------------

	char* DynamicStringBuffer_impl::data() noexcept
	{
		return m_buffer.data();
	}

	const char* DynamicStringBuffer_impl::data() const noexcept
	{
		return m_buffer.data();
	}

	char& DynamicStringBuffer_impl::operator[]( size_t index )
	{
		return m_buffer[index];
	}

	const char& DynamicStringBuffer_impl::operator[]( size_t index ) const
	{
		return m_buffer[index];
	}

	//----------------------------------------------
	// Content manipulation
	//----------------------------------------------

	void DynamicStringBuffer_impl::append( std::string_view str )
	{
		m_buffer.append( str.data(), str.data() + str.size() );
	}

	void DynamicStringBuffer_impl::append( const std::string& str )
	{
		m_buffer.append( str.data(), str.data() + str.size() );
	}

	void DynamicStringBuffer_impl::append( const char* str )
	{
		if ( str )
		{
			const size_t len = std::strlen( str );
			m_buffer.append( str, str + len );
		}
	}

	void DynamicStringBuffer_impl::push_back( char c )
	{
		m_buffer.push_back( c );
	}

	//----------------------------------------------
	// String conversion
	//----------------------------------------------

	std::string DynamicStringBuffer_impl::toString() const
	{
		return fmt::to_string( m_buffer );
	}

	std::string_view DynamicStringBuffer_impl::toStringView() const noexcept
	{
		return std::string_view{ m_buffer.data(), m_buffer.size() };
	}

	//----------------------------------------------
	// Iterator interface
	//----------------------------------------------

	char* DynamicStringBuffer_impl::begin() noexcept
	{
		return m_buffer.data();
	}

	const char* DynamicStringBuffer_impl::begin() const noexcept
	{
		return m_buffer.data();
	}

	char* DynamicStringBuffer_impl::end() noexcept
	{
		return m_buffer.data() + m_buffer.size();
	}

	const char* DynamicStringBuffer_impl::end() const noexcept
	{
		return m_buffer.data() + m_buffer.size();
	}
}
