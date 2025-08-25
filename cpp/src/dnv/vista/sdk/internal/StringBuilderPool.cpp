/**
 * @file StringBuilderPool.cpp
 * @brief Implementation file for StringBuilderPool methods
 */

#include "dnv/vista/sdk/internal/StringBuilderPool.h"
#include "dnv/vista/sdk/internal/DynamicStringBufferPool.h"

#include "impl/DynamicStringBuffer_impl.h"

namespace dnv::vista::sdk::internal
{
	//=====================================================================
	// DynamicStringBuffer class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	DynamicStringBuffer::DynamicStringBuffer()
		: m_impl{ new DynamicStringBuffer_impl{} }
	{
	}

	DynamicStringBuffer::DynamicStringBuffer( size_t initialCapacity )
		: m_impl{ new DynamicStringBuffer_impl{ initialCapacity } }

	{
	}

	DynamicStringBuffer::DynamicStringBuffer( const DynamicStringBuffer& other )
		: m_impl{ other.m_impl
					  ? new DynamicStringBuffer_impl{ *static_cast<DynamicStringBuffer_impl*>( other.m_impl ) }
					  : nullptr }
	{
	}

	DynamicStringBuffer::DynamicStringBuffer( DynamicStringBuffer&& other ) noexcept
		: m_impl{ std::move( other.m_impl ) }

	{
	}

	//----------------------------------------------
	// Destruction
	//----------------------------------------------

	DynamicStringBuffer::~DynamicStringBuffer()
	{
		if ( m_impl )
		{
			delete static_cast<DynamicStringBuffer_impl*>( m_impl );
			m_impl = nullptr;
		}
	}

	//----------------------------------------------
	// Assignment operators
	//----------------------------------------------

	DynamicStringBuffer& DynamicStringBuffer::operator=( const DynamicStringBuffer& other )
	{
		if ( this != &other )
		{
			if ( other.m_impl )
			{
				if ( m_impl )
				{
					*static_cast<DynamicStringBuffer_impl*>( m_impl ) = *static_cast<DynamicStringBuffer_impl*>( other.m_impl );
				}
				else
				{
					m_impl = new DynamicStringBuffer_impl{ *static_cast<DynamicStringBuffer_impl*>( other.m_impl ) };
				}
			}
			else
			{
				delete static_cast<DynamicStringBuffer_impl*>( m_impl );
				m_impl = nullptr;
			}
		}
		return *this;
	}

	DynamicStringBuffer& DynamicStringBuffer::operator=( DynamicStringBuffer&& other ) noexcept
	{
		if ( this != &other )
		{
			delete static_cast<DynamicStringBuffer_impl*>( m_impl );
			m_impl = other.m_impl;
			other.m_impl = nullptr;
		}

		return *this;
	}

	//----------------------------------------------
	// Capacity and size information
	//----------------------------------------------

	size_t DynamicStringBuffer::size() const noexcept
	{
		return m_impl
				   ? static_cast<DynamicStringBuffer_impl*>( m_impl )->size()
				   : 0;
	}

	size_t DynamicStringBuffer::capacity() const noexcept
	{
		return m_impl
				   ? static_cast<DynamicStringBuffer_impl*>( m_impl )->capacity()
				   : 0;
	}

	bool DynamicStringBuffer::isEmpty() const noexcept
	{
		return m_impl
				   ? static_cast<DynamicStringBuffer_impl*>( m_impl )->empty()
				   : true;
	}

	//----------------------------------------------
	// Buffer management
	//----------------------------------------------

	void DynamicStringBuffer::clear() noexcept
	{
		if ( m_impl )
		{
			static_cast<DynamicStringBuffer_impl*>( m_impl )->clear();
		}
	}

	void DynamicStringBuffer::reserve( size_t newCapacity )
	{
		if ( !m_impl )
		{
			m_impl = new DynamicStringBuffer_impl{ newCapacity };
		}
		else
		{
			static_cast<DynamicStringBuffer_impl*>( m_impl )->reserve( newCapacity );
		}
	}

	void DynamicStringBuffer::resize( size_t newSize )
	{
		if ( !m_impl )
		{
			m_impl = new DynamicStringBuffer_impl{ newSize };
		}
		static_cast<DynamicStringBuffer_impl*>( m_impl )->resize( newSize );
	}

	//----------------------------------------------
	// Data access
	//----------------------------------------------

	char* DynamicStringBuffer::data() noexcept
	{
		return m_impl
				   ? static_cast<DynamicStringBuffer_impl*>( m_impl )->data()
				   : nullptr;
	}

	const char* DynamicStringBuffer::data() const noexcept
	{
		return m_impl
				   ? static_cast<DynamicStringBuffer_impl*>( m_impl )->data()
				   : nullptr;
	}

	char& DynamicStringBuffer::operator[]( size_t index )
	{
		return ( *static_cast<DynamicStringBuffer_impl*>( m_impl ) )[index];
	}

	const char& DynamicStringBuffer::operator[]( size_t index ) const
	{
		return ( *static_cast<DynamicStringBuffer_impl*>( m_impl ) )[index];
	}

	//----------------------------------------------
	// Content manipulation
	//----------------------------------------------

	void DynamicStringBuffer::append( std::string_view str )
	{
		if ( !m_impl )
		{
			m_impl = new DynamicStringBuffer_impl{};
		}
		static_cast<DynamicStringBuffer_impl*>( m_impl )->append( str );
	}

	void DynamicStringBuffer::append( const std::string& str )
	{
		if ( !m_impl )
		{
			m_impl = new DynamicStringBuffer_impl{};
		}
		static_cast<DynamicStringBuffer_impl*>( m_impl )->append( str );
	}

	void DynamicStringBuffer::append( const char* str )
	{
		if ( !m_impl )
		{
			m_impl = new DynamicStringBuffer_impl{};
		}
		static_cast<DynamicStringBuffer_impl*>( m_impl )->append( str );
	}

	void DynamicStringBuffer::push_back( char c )
	{
		if ( !m_impl )
		{
			m_impl = new DynamicStringBuffer_impl{};
		}
		static_cast<DynamicStringBuffer_impl*>( m_impl )->push_back( c );
	}

	//----------------------------------------------
	// String conversion
	//----------------------------------------------

	std::string DynamicStringBuffer::toString() const
	{
		return m_impl
				   ? static_cast<DynamicStringBuffer_impl*>( m_impl )->toString()
				   : std::string{};
	}

	std::string_view DynamicStringBuffer::toStringView() const noexcept
	{
		return m_impl
				   ? static_cast<DynamicStringBuffer_impl*>( m_impl )->toStringView()
				   : std::string_view{};
	}

	//----------------------------------------------
	// Iterator interface
	//----------------------------------------------

	DynamicStringBuffer::iterator DynamicStringBuffer::begin() noexcept
	{
		return m_impl
				   ? static_cast<DynamicStringBuffer_impl*>( m_impl )->begin()
				   : nullptr;
	}

	DynamicStringBuffer::const_iterator DynamicStringBuffer::begin() const noexcept
	{
		return m_impl
				   ? static_cast<DynamicStringBuffer_impl*>( m_impl )->begin()
				   : nullptr;
	}

	DynamicStringBuffer::iterator DynamicStringBuffer::end() noexcept
	{
		return m_impl
				   ? static_cast<DynamicStringBuffer_impl*>( m_impl )->end()
				   : nullptr;
	}

	DynamicStringBuffer::const_iterator DynamicStringBuffer::end() const noexcept
	{
		return m_impl
				   ? static_cast<DynamicStringBuffer_impl*>( m_impl )->end()
				   : nullptr;
	}

	//=====================================================================
	// StringBuilderLease class
	//=====================================================================

	//----------------------------------------------
	// Destruction
	//----------------------------------------------

	StringBuilderLease::~StringBuilderLease()
	{
		dispose();
	}

	//----------------------------------------------
	// Private implementation methods
	//----------------------------------------------

	void StringBuilderLease::dispose()
	{
		if ( m_valid )
		{
			dynamicStringBufferPool().returnToPool( m_buffer );
			m_buffer = nullptr;
			m_valid = false;
		}
	}

	void StringBuilderLease::throwInvalidOperation() const
	{
		throw std::runtime_error( "Tried to access StringBuilder after it was returned to pool" );
	}

	//=====================================================================
	// StringBuilderPool class
	//=====================================================================

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	StringBuilderLease StringBuilderPool::lease()
	{
		return StringBuilderLease( dynamicStringBufferPool().get() );
	}

	//----------------------------
	// Statistics methods
	//----------------------------

	StringBuilderPool::PoolStatistics StringBuilderPool::stats() noexcept
	{
		const auto& internalStats = dynamicStringBufferPool().stats();
		return PoolStatistics{
			.threadLocalHits = internalStats.threadLocalHits.load(),
			.dynamicStringBufferPoolHits = internalStats.dynamicStringBufferPoolHits.load(),
			.newAllocations = internalStats.newAllocations.load(),
			.totalRequests = internalStats.totalRequests.load(),
			.hitRate = internalStats.hitRate() };
	}

	void StringBuilderPool::resetStats() noexcept
	{
		dynamicStringBufferPool().resetStats();
	}

	//----------------------------
	// Lease management
	//----------------------------

	size_t StringBuilderPool::clear()
	{
		dynamicStringBufferPool().resetStats();

		return dynamicStringBufferPool().clear();
	}

	size_t StringBuilderPool::size() noexcept
	{
		return dynamicStringBufferPool().size();
	}
}
