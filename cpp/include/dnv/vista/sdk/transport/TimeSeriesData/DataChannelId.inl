/**
 * @file DataChannelId.inl
 * @brief Inline implementations for DataChannelId class
 */

#pragma once

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// DataChannelId class inline implementations
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline DataChannelId::DataChannelId( const LocalId& localId ) noexcept
		: m_tag{ 1 },
		  m_localId{ localId }
	{
	}

	inline DataChannelId::DataChannelId( std::string_view shortId )
		: m_tag{ 2 },
		  m_shortId{ shortId }
	{
		if ( m_shortId->empty() )
		{
			throw std::invalid_argument( "DataChannelId shortId cannot be empty" );
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
			case 1:
			{
				return m_localId == other.m_localId;
			}
			case 2:
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
		return m_tag == 1;
	}

	inline bool DataChannelId::isShortId() const noexcept
	{
		return m_tag == 2;
	}

	//----------------------------------------------
	// Value access
	//----------------------------------------------

	inline std::optional<LocalId> DataChannelId::localId() const noexcept
	{
		if ( m_tag == 1 )
		{
			return m_localId;
		}

		return std::nullopt;
	}

	inline std::optional<std::string_view> DataChannelId::shortId() const noexcept
	{
		if ( m_tag == 2 )
		{
			return std::string_view{ *m_shortId };
		}

		return std::nullopt;
	}

	//----------------------------------------------
	// Pattern matching
	//----------------------------------------------

	template <typename T>
	T inline DataChannelId::matchOn(
		std::function<T( const LocalId& )> onLocalId,
		std::function<T( std::string_view )> onShortId ) const
	{
		switch ( m_tag )
		{
			case 1:
			{
				return onLocalId( *m_localId );
			}
			case 2:
			{
				return onShortId( std::string_view{ *m_shortId } );
			}
			default:
			{
				throw std::runtime_error( "Invalid DataChannelId state: corrupted tag" );
			}
		}
	}

	inline void DataChannelId::switchOn(
		std::function<void( const LocalId& )> onLocalId,
		std::function<void( std::string_view )> onShortId ) const
	{
		switch ( m_tag )
		{
			case 1:
			{
				onLocalId( *m_localId );
				break;
			}
			case 2:
			{
				onShortId( std::string_view{ *m_shortId } );
				break;
			}
			default:
			{
				throw std::runtime_error( "Invalid DataChannelId state: corrupted tag" );
			}
		}
	}
}
