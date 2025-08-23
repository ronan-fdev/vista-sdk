/**
 * @file DataChannelId.cpp
 * @brief Implementation of DataChannelId class
 */

#include "dnv/vista/sdk/LocalIdBuilder.h"

#include "dnv/vista/sdk/transport/TimeSeriesData/DataChannelId.h"

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// DataChannelId class implementation
	//=====================================================================

	//----------------------------------------------
	// String conversion
	//----------------------------------------------

	std::string DataChannelId::toString() const
	{
		switch ( m_tag )
		{
			case 1:
			{
				return m_localId->toString();
			}
			case 2:
			{
				return *m_shortId;
			}
			default:
			{
				throw std::runtime_error( "Invalid DataChannelId state: corrupted tag" );
			}
		}
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	DataChannelId DataChannelId::parse( std::string_view value )
	{
		if ( value.empty() )
		{
			throw std::invalid_argument( "DataChannelId::parse: value cannot be empty" );
		}

		/* Try to parse as LocalId first */
		std::optional<LocalId> localId;
		if ( LocalId::tryParse( value, localId ) )
		{
			return DataChannelId{ *localId };
		}

		/* If not a valid LocalId, treat as short identifier */
		return DataChannelId{ std::string{ value } };
	}
}
