/**
 * @file ShipId.cpp
 * @brief Implementation of ShipId class
 */

#include <nfx/string/Utils.h>
#include "dnv/vista/sdk/transport/ShipId.h"

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// ShipId class implementation
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	ShipId::ShipId( std::string_view otherId )
		: m_tag{ Tag::Other },
		  m_otherId{ otherId }
	{
		if ( m_otherId->empty() )
		{
			throw std::invalid_argument( "ShipId otherId cannot be empty" );
		}
	}

	//----------------------------------------------
	// String conversion
	//----------------------------------------------

	std::string ShipId::toString() const
	{
		switch ( m_tag )
		{
			case Tag::IMO:
			{
				/* In ISO-19848, IMO number as ShipID should include "IMO" prefix */
				return m_imoNumber->toString();
			}
			case Tag::Other:
			{
				return *m_otherId;
			}
			default:
			{
				throw std::runtime_error( "Invalid ShipId state: corrupted tag" );
			}
		}
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	ShipId ShipId::parse( std::string_view value )
	{
		if ( value.empty() )
		{
			throw std::invalid_argument( "ShipId::parse: value cannot be empty" );
		}

		/* In ISO-19848, IMO number as ShipID should be prefixed with "IMO" */
		if ( value.size() >= 3 && nfx::string::iequals( value.substr( 0, 3 ), "IMO" ) )
		{
			auto imoOpt = ImoNumber::tryParse( value );
			if ( imoOpt.has_value() )
			{
				return ShipId{ imoOpt.value() };
			}
		}

		/* If not a valid IMO number, treat as other identifier */
		return ShipId{ std::string{ value } };
	}
}
