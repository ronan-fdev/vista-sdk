/*
 * MIT License
 *
 * Copyright (c) 2025 DNV
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "dnv/vista/sdk/LocalIdQuery.h"

#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
	LocalIdQuery::LocalIdQuery( const LocalIdQueryBuilder& builder )
		: m_primaryItem{},
		  m_secondaryItem{},
		  m_tags{},
		  m_requireSecondaryItem{}
	{
		if ( builder.m_primaryItem.has_value() )
			m_primaryItem = builder.m_primaryItem;
		if ( builder.m_secondaryItem.has_value() )
			m_secondaryItem = builder.m_secondaryItem;
		if ( builder.m_tags.has_value() )
			m_tags = builder.m_tags;
		if ( builder.m_requireSecondaryItem.has_value() )
			m_requireSecondaryItem = builder.m_requireSecondaryItem;
	}

	bool LocalIdQuery::match( const LocalId& localId ) const
	{
		const LocalId* targetLocalId = &localId;
		std::optional<LocalId> converted;
		const auto latest = VIS::instance().latest();

		if ( localId.version() < latest )
		{
			converted = VIS::instance().convertLocalId( localId, latest );
			if ( !converted.has_value() )
			{
				return false;
			}
			targetLocalId = &converted.value();
		}

		// Check primary item
		if ( m_primaryItem.has_value() )
		{
			if ( !m_primaryItem->match( targetLocalId->primaryItem() ) )
			{
				return false;
			}
		}

		// Check secondary item
		if ( m_secondaryItem.has_value() )
		{
			const auto& secondaryOpt = targetLocalId->secondaryItem();
			if ( !secondaryOpt.has_value() )
			{
				return false;
			}
			if ( !m_secondaryItem->match( secondaryOpt.value() ) )
			{
				return false;
			}
		}

		// Check secondary item requirement (presence/absence)
		if ( m_requireSecondaryItem.has_value() )
		{
			const bool hasSecondary = targetLocalId->secondaryItem().has_value();
			if ( m_requireSecondaryItem.value() != hasSecondary )
			{
				return false;
			}
		}

		// Check metadata tags
		if ( m_tags.has_value() )
		{
			if ( !m_tags->match( *targetLocalId ) )
			{
				return false;
			}
		}

		return true;
	}

	bool LocalIdQuery::match( std::string_view localIdStr ) const
	{
		auto localId = LocalId::fromString( localIdStr );
		if ( !localId.has_value() )
		{
			return false;
		}
		return match( localId.value() );
	}

	LocalIdQueryBuilder LocalIdQueryBuilder::create() noexcept
	{
		return LocalIdQueryBuilder();
	}

	LocalIdQueryBuilder LocalIdQueryBuilder::from( const LocalId& localId ) noexcept
	{
		LocalIdQueryBuilder builder;

		// Set primary item
		builder.m_primaryItem = GmodPathQueryBuilder::Path::from( localId.primaryItem() ).build();

		// Set secondary item
		if ( localId.secondaryItem().has_value() )
		{
			builder.m_secondaryItem = GmodPathQueryBuilder::Path::from( localId.secondaryItem().value() ).build();
			builder.m_requireSecondaryItem = true;
		}
		else
		{
			builder.m_requireSecondaryItem = false;
		}

		// Set metadata tags
		builder.m_tags = MetadataTagsQueryBuilder::from( localId ).build();

		return builder;
	}

	std::optional<LocalIdQueryBuilder> LocalIdQueryBuilder::from( std::string_view localIdStr ) noexcept
	{
		auto localId = LocalId::fromString( localIdStr );
		if ( !localId.has_value() )
		{
			return std::nullopt;
		}
		return from( localId.value() );
	}

	LocalIdQuery LocalIdQueryBuilder::build() const
	{
		return LocalIdQuery( *this );
	}

	LocalIdQueryBuilder& LocalIdQueryBuilder::withPrimaryItem( const GmodPath& primaryItem )
	{
		m_primaryItem = GmodPathQueryBuilder::Path::from( primaryItem ).build();
		return *this;
	}

	LocalIdQueryBuilder& LocalIdQueryBuilder::withPrimaryItem( const GmodPathQuery& primaryItem )
	{
		m_primaryItem = primaryItem;
		return *this;
	}

	LocalIdQueryBuilder& LocalIdQueryBuilder::withPrimaryItem(
		std::function<GmodPathQuery( GmodPathQueryBuilder::Nodes& )> configure )
	{
		auto nodes = GmodPathQueryBuilder::empty();
		m_primaryItem = configure( nodes );
		return *this;
	}

	LocalIdQueryBuilder& LocalIdQueryBuilder::withPrimaryItem( const GmodPath& primaryItem,
		std::function<GmodPathQuery( GmodPathQueryBuilder::Path& )> configure )
	{
		auto path = GmodPathQueryBuilder::Path::from( primaryItem );
		m_primaryItem = configure( path );
		return *this;
	}

	LocalIdQueryBuilder& LocalIdQueryBuilder::withSecondaryItem( const GmodPath& secondaryItem )
	{
		m_secondaryItem = GmodPathQueryBuilder::Path::from( secondaryItem ).build();
		m_requireSecondaryItem = true;
		return *this;
	}

	LocalIdQueryBuilder& LocalIdQueryBuilder::withSecondaryItem( const GmodPathQuery& secondaryItem )
	{
		m_secondaryItem = secondaryItem;
		m_requireSecondaryItem = true;
		return *this;
	}

	LocalIdQueryBuilder& LocalIdQueryBuilder::withSecondaryItem(
		std::function<GmodPathQuery( GmodPathQueryBuilder::Nodes& )> configure )
	{
		auto nodes = GmodPathQueryBuilder::empty();
		m_secondaryItem = configure( nodes );
		m_requireSecondaryItem = true;
		return *this;
	}

	LocalIdQueryBuilder& LocalIdQueryBuilder::withSecondaryItem( const GmodPath& secondaryItem,
		std::function<GmodPathQuery( GmodPathQueryBuilder::Path& )> configure )
	{
		auto path = GmodPathQueryBuilder::Path::from( secondaryItem );
		m_secondaryItem = configure( path );
		m_requireSecondaryItem = true;
		return *this;
	}

	LocalIdQueryBuilder& LocalIdQueryBuilder::withAnySecondaryItem()
	{
		m_secondaryItem = std::nullopt;
		m_requireSecondaryItem = std::nullopt;
		return *this;
	}

	LocalIdQueryBuilder& LocalIdQueryBuilder::withoutSecondaryItem()
	{
		m_secondaryItem = std::nullopt;
		m_requireSecondaryItem = false;
		return *this;
	}

	LocalIdQueryBuilder& LocalIdQueryBuilder::withTags( const MetadataTagsQuery& tags )
	{
		m_tags = tags;
		return *this;
	}

	LocalIdQueryBuilder& LocalIdQueryBuilder::withTags(
		std::function<MetadataTagsQuery( MetadataTagsQueryBuilder& )> configure )
	{
		auto builder = MetadataTagsQueryBuilder::empty();
		m_tags = configure( builder );
		return *this;
	}

	const GmodPath* LocalIdQueryBuilder::primaryItem() const noexcept
	{
		if ( !m_primaryItem.has_value() )
		{
			return nullptr;
		}

		const auto* builder = m_primaryItem->builder();
		if ( const auto* pathBuilder = dynamic_cast<const GmodPathQueryBuilder::Path*>( builder ) )
		{
			return &pathBuilder->path();
		}

		return nullptr;
	}

	const GmodPath* LocalIdQueryBuilder::secondaryItem() const noexcept
	{
		if ( !m_secondaryItem.has_value() )
		{
			return nullptr;
		}

		const auto* builder = m_secondaryItem->builder();
		if ( const auto* pathBuilder = dynamic_cast<const GmodPathQueryBuilder::Path*>( builder ) )
		{
			return &pathBuilder->path();
		}

		return nullptr;
	}

	LocalIdQueryBuilder& LocalIdQueryBuilder::withoutLocations()
	{
		// Remove locations from primary item if it's a Path query
		if ( m_primaryItem.has_value() )
		{
			const auto* builder = m_primaryItem->builder();
			if ( const auto* pathBuilder = dynamic_cast<const GmodPathQueryBuilder::Path*>( builder ) )
			{
				auto newPath = GmodPathQueryBuilder::Path::from( pathBuilder->path() );
				m_primaryItem = newPath.withoutLocations().build();
			}
		}

		// Remove locations from secondary item if it's a Path query
		if ( m_secondaryItem.has_value() )
		{
			const auto* builder = m_secondaryItem->builder();
			if ( const auto* pathBuilder = dynamic_cast<const GmodPathQueryBuilder::Path*>( builder ) )
			{
				auto newPath = GmodPathQueryBuilder::Path::from( pathBuilder->path() );
				m_secondaryItem = newPath.withoutLocations().build();
			}
		}

		return *this;
	}
} // namespace dnv::vista::sdk
