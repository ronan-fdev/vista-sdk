/**
 * @file StringBuilderPool.inl
 * @brief Inline method implementations for StringBuilderPool
 * @details High-performance inline implementations for string builder pooling infrastructure
 */

#pragma once

namespace dnv::vista::sdk::internal
{
	//=====================================================================
	// StringBuilder class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE StringBuilder::StringBuilder( DynamicStringBuffer& buffer )
		: m_buffer{ buffer }
	{
	}

	//----------------------------------------------
	// Array access operators
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE char& StringBuilder::operator[]( size_t index )
	{
		return m_buffer[index];
	}

	VISTA_SDK_CPP_FORCE_INLINE const char& StringBuilder::operator[]( size_t index ) const
	{
		return m_buffer[index];
	}

	//----------------------------------------------
	// String append operations
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE void StringBuilder::append( std::string_view str )
	{
		m_buffer.append( str );
	}

	VISTA_SDK_CPP_FORCE_INLINE void StringBuilder::append( const std::string& str )
	{
		m_buffer.append( str );
	}

	VISTA_SDK_CPP_FORCE_INLINE void StringBuilder::append( const char* str )
	{
		if ( str )
		{
			m_buffer.append( str );
		}
	}

	VISTA_SDK_CPP_FORCE_INLINE void StringBuilder::push_back( char c )
	{
		m_buffer.push_back( c );
	}

	//----------------------------------------------
	// Stream operators
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE StringBuilder& StringBuilder::operator<<( std::string_view str )
	{
		append( str );
		return *this;
	}

	VISTA_SDK_CPP_FORCE_INLINE StringBuilder& StringBuilder::operator<<( const std::string& str )
	{
		append( str );
		return *this;
	}

	VISTA_SDK_CPP_FORCE_INLINE StringBuilder& StringBuilder::operator<<( const char* str )
	{
		append( str );
		return *this;
	}

	VISTA_SDK_CPP_FORCE_INLINE StringBuilder& StringBuilder::operator<<( char c )
	{
		push_back( c );
		return *this;
	}

	//----------------------------------------------
	// Size and capacity management
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE size_t StringBuilder::length() const noexcept
	{
		return m_buffer.size();
	}

	VISTA_SDK_CPP_FORCE_INLINE void StringBuilder::resize( size_t newSize )
	{
		m_buffer.resize( newSize );
	}

	//----------------------------------------------
	// Iterator interface
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE StringBuilder::iterator StringBuilder::begin()
	{
		return m_buffer.data();
	}

	VISTA_SDK_CPP_FORCE_INLINE StringBuilder::const_iterator StringBuilder::begin() const
	{
		return m_buffer.data();
	}

	VISTA_SDK_CPP_FORCE_INLINE StringBuilder::iterator StringBuilder::end()
	{
		return m_buffer.data() + m_buffer.size();
	}

	VISTA_SDK_CPP_FORCE_INLINE StringBuilder::const_iterator StringBuilder::end() const
	{
		return m_buffer.data() + m_buffer.size();
	}

	//----------------------------------------------
	// StringBuilder::Enumerator class
	//----------------------------------------------

	//----------------------------
	// Construction
	//----------------------------

	VISTA_SDK_CPP_FORCE_INLINE StringBuilder::Enumerator::Enumerator( const StringBuilder& builder )
		: m_data{ builder.begin() },
		  m_end{ builder.end() },
		  m_current{ builder.begin() - 1 }
	{
	}

	//----------------------------
	// Enumerator operations
	//----------------------------

	VISTA_SDK_CPP_FORCE_INLINE bool StringBuilder::Enumerator::next()
	{
		if ( m_current + 1 < m_end )
		{
			++m_current;

			return true;
		}

		return false;
	}

	VISTA_SDK_CPP_FORCE_INLINE char StringBuilder::Enumerator::current() const
	{
		return *m_current;
	}

	VISTA_SDK_CPP_FORCE_INLINE void StringBuilder::Enumerator::reset()
	{
		m_current = m_data - 1;
	}

	//=====================================================================
	// StringBuilderLease class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE StringBuilderLease::StringBuilderLease( DynamicStringBuffer* buffer )
		: m_buffer{ buffer },
		  m_valid{ true }
	{
	}

	VISTA_SDK_CPP_FORCE_INLINE StringBuilderLease::StringBuilderLease( StringBuilderLease&& other ) noexcept
		: m_buffer{ std::exchange( other.m_buffer, nullptr ) },
		  m_valid{ std::exchange( other.m_valid, false ) }
	{
	}

	//----------------------------------------------
	// Assignment operators
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE StringBuilderLease& StringBuilderLease::operator=( StringBuilderLease&& other ) noexcept
	{
		if ( this != &other )
		{
			dispose();
			m_buffer = std::exchange( other.m_buffer, nullptr );
			m_valid = std::exchange( other.m_valid, false );
		}

		return *this;
	}

	//----------------------------------------------
	// Public interface implementations
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE StringBuilder StringBuilderLease::builder()
	{
		if ( !m_valid )
		{
			throwInvalidOperation();
		}

		return StringBuilder{ *m_buffer };
	}

	VISTA_SDK_CPP_FORCE_INLINE DynamicStringBuffer& StringBuilderLease::buffer()
	{
		if ( !m_valid )
		{
			throwInvalidOperation();
		}

		return *m_buffer;
	}

	VISTA_SDK_CPP_FORCE_INLINE std::string StringBuilderLease::toString() const
	{
		if ( !m_valid )
		{
			throwInvalidOperation();
		}

		return m_buffer->toString();
	}
}
