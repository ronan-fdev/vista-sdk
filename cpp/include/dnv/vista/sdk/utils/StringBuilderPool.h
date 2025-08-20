/**
 * @file StringBuilderPool.h
 * @brief String pooling implementation
 * @details ObjectPool pattern for string resource management
 */

#pragma once

#include "dnv/vista/sdk/config/Platform.h"

namespace dnv::vista::sdk::utils
{
	namespace detail
	{
		class ObjectPool final
		{
		public:
			explicit ObjectPool( size_t initialCapacity = 128, size_t maximumRetainedCapacity = 1024, size_t maxPoolSize = 16 )
				: m_initialCapacity{ initialCapacity },
				  m_maximumRetainedCapacity{ maximumRetainedCapacity },
				  m_maxPoolSize{ maxPoolSize }
			{
			}

			ObjectPool( const ObjectPool& ) = delete;
			ObjectPool& operator=( const ObjectPool& ) = delete;
			ObjectPool( ObjectPool&& ) = delete;
			ObjectPool& operator=( ObjectPool&& ) = delete;

			~ObjectPool()
			{
				std::lock_guard<std::mutex> lock( m_mutex );
				while ( !m_pool.empty() )
				{
					delete m_pool.back();
					m_pool.pop_back();
				}
			}

			fmt::memory_buffer* get()
			{
				thread_local fmt::memory_buffer* t_cachedBuffer = nullptr;

				if ( t_cachedBuffer )
				{
					auto* buffer = t_cachedBuffer;
					t_cachedBuffer = nullptr;
					buffer->clear();
					return buffer;
				}

				fmt::memory_buffer* buffer = nullptr;

				{
					std::lock_guard<std::mutex> lock( m_mutex );
					if ( !m_pool.empty() )
					{
						buffer = m_pool.back();
						m_pool.pop_back();
					}
				}

				if ( !buffer )
				{
					buffer = new fmt::memory_buffer();
					buffer->reserve( m_initialCapacity );
				}
				else
				{
					buffer->clear();
				}

				return buffer;
			}

			void returnToPool( fmt::memory_buffer* buffer )
			{
				if ( !buffer )
				{
					return;
				}

				if ( buffer->capacity() > m_maximumRetainedCapacity )
				{
					delete buffer;

					return;
				}

				thread_local fmt::memory_buffer* t_cachedBuffer = nullptr;

				if ( !t_cachedBuffer )
				{
					t_cachedBuffer = buffer;
					{
						return;
					}
				}

				std::lock_guard<std::mutex> lock( m_mutex );
				if ( m_pool.size() < m_maxPoolSize )
				{
					m_pool.push_back( buffer );
				}
				else
				{
					delete buffer;
				}
			}

		private:
			std::vector<fmt::memory_buffer*> m_pool;
			std::mutex m_mutex;
			const size_t m_initialCapacity;
			const size_t m_maximumRetainedCapacity;
			const size_t m_maxPoolSize;
		};

		inline ObjectPool& instance() noexcept
		{
			static ObjectPool pool( 128, 1024, 16 );

			return pool;
		}
	}

	struct StringBuilderWrapper final
	{
		explicit StringBuilderWrapper( fmt::memory_buffer& buffer )
			: m_buffer{ buffer }
		{
		}

		StringBuilderWrapper( const StringBuilderWrapper& ) = default;

		StringBuilderWrapper( StringBuilderWrapper&& ) = delete;

		StringBuilderWrapper& operator=( const StringBuilderWrapper& ) = delete;

		StringBuilderWrapper& operator=( StringBuilderWrapper&& ) = delete;

		VISTA_SDK_CPP_FORCE_INLINE void append( std::string_view str )
		{
			m_buffer.append( str.data(), str.data() + str.size() );
		}

		VISTA_SDK_CPP_FORCE_INLINE void append( const std::string& str )
		{
			m_buffer.append( str.data(), str.data() + str.size() );
		}

		VISTA_SDK_CPP_FORCE_INLINE void append( const char* str )
		{
			if ( str )
			{
				const size_t len = std::strlen( str );
				m_buffer.append( str, str + len );
			}
		}

		VISTA_SDK_CPP_FORCE_INLINE void push_back( char c )
		{
			m_buffer.push_back( c );
		}

		VISTA_SDK_CPP_FORCE_INLINE size_t size() const noexcept
		{
			return m_buffer.size();
		}

		VISTA_SDK_CPP_FORCE_INLINE size_t length() const noexcept
		{
			return m_buffer.size();
		}
		VISTA_SDK_CPP_FORCE_INLINE void resize( size_t new_size )
		{
			m_buffer.resize( new_size );
		}

		VISTA_SDK_CPP_FORCE_INLINE char& operator[]( size_t index )
		{
			return m_buffer[index];
		}

		VISTA_SDK_CPP_FORCE_INLINE const char& operator[]( size_t index ) const
		{
			return m_buffer[index];
		}

		using value_type = char;
		using iterator = char*;
		using const_iterator = const char*;

		VISTA_SDK_CPP_FORCE_INLINE iterator begin()
		{
			return m_buffer.data();
		}

		VISTA_SDK_CPP_FORCE_INLINE const_iterator begin() const
		{
			return m_buffer.data();
		}

		VISTA_SDK_CPP_FORCE_INLINE iterator end()
		{
			return m_buffer.data() + m_buffer.size();
		}

		VISTA_SDK_CPP_FORCE_INLINE const_iterator end() const
		{
			return m_buffer.data() + m_buffer.size();
		}

	private:
		fmt::memory_buffer& m_buffer;
	};

	struct StringBuilderLease final
	{
		StringBuilderLease() = delete;
		StringBuilderLease( const StringBuilderLease& ) = delete;
		StringBuilderLease& operator=( const StringBuilderLease& ) = delete;

		StringBuilderLease( StringBuilderLease&& other ) noexcept
			: m_buffer{ std::exchange( other.m_buffer, nullptr ) },
			  m_valid{ std::exchange( other.m_valid, false ) }
		{
		}

		StringBuilderLease& operator=( StringBuilderLease&& other ) noexcept
		{
			if ( this != &other )
			{
				dispose();
				m_buffer = std::exchange( other.m_buffer, nullptr );
				m_valid = std::exchange( other.m_valid, false );
			}

			return *this;
		}

		~StringBuilderLease() { dispose(); }

		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE StringBuilderWrapper builder()
		{
			if ( !m_valid )
			{
				throwInvalidOperation();
			}

			return StringBuilderWrapper{ *m_buffer };
		}

		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE fmt::memory_buffer& buffer()
		{
			if ( !m_valid )
			{
				throwInvalidOperation();
			}

			return *m_buffer;
		}

		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE std::string toString() const
		{
			if ( !m_valid )
			{
				throwInvalidOperation();
			}

			return fmt::to_string( *m_buffer );
		}

	private:
		friend class StringBuilderPool;

		explicit StringBuilderLease( fmt::memory_buffer* buffer )
			: m_buffer{ buffer }, m_valid{ true }
		{
		}

		void dispose()
		{
			if ( m_valid )
			{
				detail::instance().returnToPool( m_buffer );
				m_buffer = nullptr;
				m_valid = false;
			}
		}

		[[noreturn]] void throwInvalidOperation() const
		{
			throw std::runtime_error( "Tried to access StringBuilder after it was returned to pool" );
		}

		fmt::memory_buffer* m_buffer{ nullptr };
		bool m_valid = false;
	};

	class StringBuilderPool final
	{
	public:
		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE static StringBuilderLease instance()
		{
			return StringBuilderLease( detail::instance().get() );
		}

	private:
		StringBuilderPool() = default;
	};
}
