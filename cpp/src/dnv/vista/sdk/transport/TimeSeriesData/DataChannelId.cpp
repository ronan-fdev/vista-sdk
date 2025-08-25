/**
 * @file DataChannelId.cpp
 * @brief Implementation of DataChannelId class
 */

#include "dnv/vista/sdk/internal/StringBuilderPool.h"
#include "dnv/vista/sdk/transport/TimeSeriesData/DataChannelId.h"
#include "dnv/vista/sdk/LocalIdBuilder.h"

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
			case Tag::LocalId:
			{
				return m_localId->toString();
			}
			case Tag::ShortId:
			{
				return *m_shortId;
			}
			default:
			{
				auto lease = internal::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Invalid DataChannelId state: corrupted tag " );
				builder.append( std::to_string( static_cast<int>( m_tag ) ) );

				throw std::runtime_error( lease.toString() );
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
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "DataChannelId::parse: value cannot be empty" );

			throw std::invalid_argument( lease.toString() );
		}

		/* Try to parse as LocalId first */
		std::optional<LocalId> localId;
		if ( LocalId::tryParse( value, localId ) )
		{
			return DataChannelId{ *localId };
		}

		/* If not a valid LocalId, treat as short identifier */
		return DataChannelId{ value };
	}
}
