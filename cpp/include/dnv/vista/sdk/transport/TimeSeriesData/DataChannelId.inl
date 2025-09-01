/**
 * @file DataChannelId.inl
 * @brief Inline implementations for DataChannelId class
 */

#include <nfx/string/StringBuilderPool.h>

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// DataChannelId class inline implementations
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline DataChannelId::DataChannelId( const LocalId& localId ) noexcept
		: m_tag{ Tag::LocalId },
		  m_localId{ localId }
	{
	}

	inline DataChannelId::DataChannelId( std::string_view shortId )
		: m_tag{ Tag::ShortId },
		  m_shortId{ shortId }
	{
		if ( m_shortId->empty() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "DataChannelId shortId cannot be empty" );

			throw std::invalid_argument( lease.toString() );
		}
	}

	//----------------------------------------------
	// Equality operators
	//----------------------------------------------

	inline bool DataChannelId::operator==( const DataChannelId& other ) const noexcept
	{
		if ( m_tag != other.m_tag )
		{
			return false;
		}

		switch ( m_tag )
		{
			case Tag::LocalId:
			{
				return m_localId == other.m_localId;
			}
			case Tag::ShortId:
			{
				return m_shortId == other.m_shortId;
			}
			default:
			{
				return false;
			}
		}
	}

	inline bool DataChannelId::operator!=( const DataChannelId& other ) const noexcept
	{
		return !( *this == other );
	}

	//----------------------------------------------
	// Type checking
	//----------------------------------------------

	inline bool DataChannelId::isLocalId() const noexcept
	{
		return m_tag == Tag::LocalId;
	}

	inline bool DataChannelId::isShortId() const noexcept
	{
		return m_tag == Tag::ShortId;
	}

	//----------------------------------------------
	// Value access
	//----------------------------------------------

	inline std::optional<LocalId> DataChannelId::localId() const noexcept
	{
		if ( m_tag == Tag::LocalId )
		{
			return m_localId;
		}

		return std::nullopt;
	}

	inline std::optional<std::string_view> DataChannelId::shortId() const noexcept
	{
		if ( m_tag == Tag::ShortId )
		{
			return std::string_view{ *m_shortId };
		}

		return std::nullopt;
	}

	//----------------------------------------------
	// Pattern matching
	//----------------------------------------------

	template <typename T, typename LocalIdFunc, typename ShortIdFunc>
	T inline DataChannelId::matchOn( LocalIdFunc&& onLocalId, ShortIdFunc&& onShortId ) const
	{
		switch ( m_tag )
		{
			case Tag::LocalId:
			{
				return std::forward<LocalIdFunc>( onLocalId )( *m_localId );
			}
			case Tag::ShortId:
			{
				return std::forward<ShortIdFunc>( onShortId )( std::string_view{ *m_shortId } );
			}
			default:
			{
				throw std::logic_error{ "Should never happen" };
			}
		}
	}

	template <typename LocalIdFunc, typename ShortIdFunc>
	inline void DataChannelId::switchOn( LocalIdFunc&& onLocalId, ShortIdFunc&& onShortId ) const
	{
		switch ( m_tag )
		{
			case Tag::LocalId:
			{
				std::forward<LocalIdFunc>( onLocalId )( *m_localId );
				break;
			}
			case Tag::ShortId:
			{
				std::forward<ShortIdFunc>( onShortId )( std::string_view{ *m_shortId } );
				break;
			}

			default:
			{
				throw std::logic_error{ "Should never happen" };
			}
		}
	}
}
