/**
 * @file LocalIdQuery.cpp
 * @brief Implementation of LocalIdQuery and LocalIdQueryBuilder classes
 */

#include "dnv/vista/sdk/LocalIdQuery.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// LocalIdQuery class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	LocalIdQuery::LocalIdQuery( const LocalIdQueryBuilder& builder )
		: m_primaryItem{ builder.m_primaryItem },
		  m_secondaryItem{ builder.m_secondaryItem },
		  m_tags{ builder.m_tags }
	{
	}

	//----------------------------------------------
	// Query execution
	//----------------------------------------------

	bool LocalIdQuery::match( const LocalId& localId ) const
	{
		// Get reference to the LocalId to use for matching (either original or converted)
		const LocalId* targetLocalId = &localId;

		// Store converted LocalId if needed
		std::optional<LocalId> convertedLocalId;

		// Convert to latest VIS version if needed
		if ( localId.visVersion() < VIS::instance().latestVisVersion() )
		{
			convertedLocalId = VIS::instance().convertLocalId( localId, VIS::instance().latestVisVersion() );
			if ( !convertedLocalId.has_value() )
			{
				throw std::runtime_error{ "Failed to convert LocalId to latest VIS version" };
			}
			targetLocalId = &convertedLocalId.value();
		}

		// Check primary item
		if ( m_primaryItem.has_value() )
		{
			// Check if primary item exists and extract the value
			if ( targetLocalId->primaryItem().has_value() )
			{
				const auto& primaryItem = targetLocalId->primaryItem().value();
				if ( !m_primaryItem->match( &primaryItem ) )
				{
					return false;
				}
			}
			else
			{
				// Query expects primary item but LocalId doesn't have one
				return false;
			}
		}

		// Check secondary item
		if ( m_secondaryItem.has_value() )
		{
			if ( targetLocalId->secondaryItem().has_value() )
			{
				const auto& secondaryItem = targetLocalId->secondaryItem().value();
				if ( !m_secondaryItem->match( &secondaryItem ) )
				{
					return false;
				}
			}
			else
			{
				// Query expects secondary item but LocalId doesn't have one
				return false;
			}
		}

		// Check metadata tags
		if ( m_tags.has_value() )
		{
			if ( !m_tags->match( targetLocalId ) )
			{
				return false;
			}
		}

		return true;
	}

	bool LocalIdQuery::match( const std::string& localIdStr ) const
	{
		try
		{
			auto localId = LocalId::parse( localIdStr );

			return match( localId );
		}
		catch ( const std::exception& )
		{
			return false;
		}
	}

	//=====================================================================
	// LocalIdQueryBuilder class
	//=====================================================================

	//----------------------------------------------
	// Factory methods
	//----------------------------------------------

	LocalIdQueryBuilder LocalIdQueryBuilder::from( const std::string& localIdStr )
	{
		return from( LocalId::parse( localIdStr ) );
	}

	LocalIdQueryBuilder LocalIdQueryBuilder::from( const LocalId& localId )
	{
		LocalIdQueryBuilder builder = empty();

		// Configure primary item - check if it exists first
		if ( localId.primaryItem().has_value() )
		{
			const auto& primaryItem = localId.primaryItem().value();
			builder = builder.withPrimaryItem( GmodPathQuery::fromPath( primaryItem ) );
		}

		// Configure secondary item if present
		if ( localId.secondaryItem().has_value() )
		{
			const auto& secondaryItem = localId.secondaryItem().value();
			builder = builder.withSecondaryItem( GmodPathQuery::fromPath( secondaryItem ) );
		}

		// Configure metadata tags
		builder = builder.withTags( MetadataTagsQuery::from( localId ) );

		return builder;
	}

	//----------------------------------------------
	// Build method
	//----------------------------------------------

	LocalIdQuery LocalIdQueryBuilder::build() const
	{
		return LocalIdQuery{ *this };
	}

	//----------------------------------------------
	// Primary item configuration
	//----------------------------------------------

	LocalIdQueryBuilder LocalIdQueryBuilder::withPrimaryItem( ConfigureFrom from, QueryConfiguration configure ) const
	{
		switch ( from )
		{
			case ConfigureFrom::Nodes:
			{
				auto query = configure( GmodPathQuery::empty() );
				return withPrimaryItem( query );
			}
			case ConfigureFrom::Path:
			{
				if ( !m_primaryItem.has_value() )
				{
					throw std::invalid_argument{ "Primary item is null - cannot configure without existing primary item" };
				}
				auto query = configure( m_primaryItem.value() );
				return withPrimaryItem( query );
			}
			default:
				throw std::invalid_argument{ "Invalid From enum value" };
		}
	}

	LocalIdQueryBuilder LocalIdQueryBuilder::withPrimaryItem( ConfigureFrom from, const GmodPath& primaryItem, QueryConfiguration configure ) const
	{
		switch ( from )
		{
			case ConfigureFrom::Path:
			{
				auto baseQuery = GmodPathQuery::fromPath( primaryItem );
				auto query = configure( baseQuery );
				return withPrimaryItem( query );
			}
			case ConfigureFrom::Nodes:
				throw std::invalid_argument{ "From::Nodes does not support GmodPath parameter - use the single parameter overload" };
			default:
				throw std::invalid_argument{ "Invalid From enum value" };
		}
	}

	LocalIdQueryBuilder LocalIdQueryBuilder::withPrimaryItem( const GmodPath& primaryItem ) const
	{
		return withPrimaryItem( GmodPathQuery::fromPath( primaryItem ) );
	}

	LocalIdQueryBuilder LocalIdQueryBuilder::withPrimaryItem( const GmodPathQuery& primaryItem ) const
	{
		LocalIdQueryBuilder result = *this;
		result.m_primaryItem = primaryItem;
		return result;
	}

	//----------------------------------------------
	// Secondary item configuration
	//----------------------------------------------

	LocalIdQueryBuilder LocalIdQueryBuilder::withSecondaryItem( ConfigureFrom from, QueryConfiguration configure ) const
	{
		switch ( from )
		{
			case ConfigureFrom::Nodes:
			{
				auto query = configure( GmodPathQuery::empty() );
				return withSecondaryItem( query );
			}
			case ConfigureFrom::Path:
			{
				if ( !m_secondaryItem.has_value() )
				{
					throw std::invalid_argument{ "Secondary item is null - cannot configure without existing secondary item" };
				}
				auto query = configure( m_secondaryItem.value() );
				return withSecondaryItem( query );
			}
			default:
				throw std::invalid_argument( "Invalid From enum value" );
		}
	}

	LocalIdQueryBuilder LocalIdQueryBuilder::withSecondaryItem( ConfigureFrom from, const GmodPath& secondaryItem, QueryConfiguration configure ) const
	{
		switch ( from )
		{
			case ConfigureFrom::Path:
			{
				auto baseQuery = GmodPathQuery::fromPath( secondaryItem );
				auto query = configure( baseQuery );
				return withSecondaryItem( query );
			}
			case ConfigureFrom::Nodes:
				throw std::invalid_argument{ "From::Nodes does not support GmodPath parameter - use the single parameter overload" };
			default:
				throw std::invalid_argument{ "Invalid From enum value" };
		}
	}

	LocalIdQueryBuilder LocalIdQueryBuilder::withSecondaryItem( const GmodPath& secondaryItem ) const
	{
		return withSecondaryItem( GmodPathQuery::fromPath( secondaryItem ) );
	}

	LocalIdQueryBuilder LocalIdQueryBuilder::withSecondaryItem( const GmodPathQuery& secondaryItem ) const
	{
		LocalIdQueryBuilder result = *this;
		result.m_secondaryItem = secondaryItem;
		return result;
	}

	//----------------------------------------------
	// Metadata tags configuration
	//----------------------------------------------

	LocalIdQueryBuilder LocalIdQueryBuilder::withTags( TagsConfiguration configure ) const
	{
		MetadataTagsQuery baseQuery = m_tags.value_or( MetadataTagsQuery::empty() );
		auto query = configure( baseQuery );
		return withTags( query );
	}

	LocalIdQueryBuilder LocalIdQueryBuilder::withTags( const MetadataTagsQuery& tags ) const
	{
		LocalIdQueryBuilder result = *this;
		result.m_tags = tags;
		return result;
	}

	//----------------------------------------------
	// Internal matching methods
	//----------------------------------------------

	bool LocalIdQueryBuilder::match( const std::string& localIdStr ) const
	{
		return match( LocalId::parse( localIdStr ) );
	}

	bool LocalIdQueryBuilder::match( const LocalId& localId ) const
	{
		return build().match( localId );
	}
}
