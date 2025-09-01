/**
 * @file ShipId.inl
 * @brief Inline implementations for ShipId class
 */

#include <stdexcept>

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// ShipId class inline implementations
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline ShipId::ShipId( ImoNumber imoNumber ) noexcept
		: m_tag{ Tag::IMO },
		  m_imoNumber{ std::move( imoNumber ) }
	{
	}

	//----------------------------------------------
	// Equality operators
	//----------------------------------------------

	inline bool ShipId::operator==( const ShipId& other ) const noexcept
	{
		if ( m_tag != other.m_tag )
		{
			return false;
		}

		switch ( m_tag )
		{
			case Tag::IMO:
			{
				return m_imoNumber == other.m_imoNumber;
			}
			case Tag::Other:
			{
				return m_otherId == other.m_otherId;
			}
			default:
			{
				return false;
			}
		}
	}

	inline bool ShipId::operator!=( const ShipId& other ) const noexcept
	{
		return !( *this == other );
	}

	//----------------------------------------------
	// Type checking
	//----------------------------------------------

	inline bool ShipId::isImoNumber() const noexcept
	{
		return m_tag == Tag::IMO;
	}

	inline bool ShipId::isOtherId() const noexcept
	{
		return m_tag == Tag::Other;
	}

	//----------------------------------------------
	// Value access
	//----------------------------------------------

	inline std::optional<ImoNumber> ShipId::imoNumber() const noexcept
	{
		if ( m_tag == Tag::IMO )
		{
			return m_imoNumber;
		}

		return std::nullopt;
	}

	inline std::optional<std::string_view> ShipId::otherId() const noexcept
	{
		if ( m_tag == Tag::Other )
		{
			return std::string_view{ *m_otherId };
		}

		return std::nullopt;
	}

	//----------------------------------------------
	// Pattern matching
	//----------------------------------------------

	template <typename T>
	inline T ShipId::matchOn(
		std::function<T( const ImoNumber& )> onImoNumber,
		std::function<T( std::string_view )> onOtherId ) const
	{
		switch ( m_tag )
		{
			case Tag::IMO:
			{
				return onImoNumber( *m_imoNumber );
			}
			case Tag::Other:
			{
				return onOtherId( std::string_view{ *m_otherId } );
			}
			default:
			{
				throw std::logic_error{ "Should never happen" };
			}
		}
	}

	inline void ShipId::switchOn(
		std::function<void( const ImoNumber& )> onImoNumber,
		std::function<void( std::string_view )> onOtherId ) const
	{
		switch ( m_tag )
		{
			case Tag::IMO:
			{
				onImoNumber( *m_imoNumber );
				break;
			}
			case Tag::Other:
			{
				onOtherId( std::string_view{ *m_otherId } );
				break;
			}
			default:
			{
				throw std::logic_error{ "Should never happen" };
			}
		}
	}
}
