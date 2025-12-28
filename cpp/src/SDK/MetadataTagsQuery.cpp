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

/**
 * @file MetadataTagsQuery.cpp
 * @brief Implementation of MetadataTagsQuery and MetadataTagsQueryBuilder
 */

#include "dnv/vista/sdk/MetadataTagsQuery.h"

#include "dnv/vista/sdk/LocalId.h"

namespace dnv::vista::sdk
{
	MetadataTagsQuery::MetadataTagsQuery( const MetadataTagsQueryBuilder* builder )
		: m_tags{ builder->m_tags },
		  m_matchExact{ builder->m_matchExact }
	{
	}

	bool MetadataTagsQuery::match( const LocalId* localId ) const
	{
		if ( localId == nullptr )
		{
			return false;
		}

		std::unordered_map<CodebookName, MetadataTag> metadataTags;
		for ( const auto& tag : localId->metadataTags() )
		{
			metadataTags.emplace( tag.name(), tag );
		}

		if ( !m_tags.empty() )
		{
			if ( m_matchExact )
			{
				if ( m_tags.size() != metadataTags.size() )
				{
					return false;
				}

				for ( const auto& [name, tag] : m_tags )
				{
					auto it = metadataTags.find( name );
					if ( it == metadataTags.end() || it->second != tag )
					{
						return false;
					}
				}
				return true;
			}
			else
			{
				// Subset match: must have AT LEAST these tags (can have more)
				for ( const auto& [name, tag] : m_tags )
				{
					auto it = metadataTags.find( name );
					if ( it == metadataTags.end() )
					{
						return false;
					}
					if ( it->second != tag )
					{
						return false;
					}
				}
				return true;
			}
		}
		else
		{
			// No tags specified:
			// - If matchExact (allowOthers=false): match only LocalIds with no tags
			// - If !matchExact (allowOthers=true): match any LocalId
			return !m_matchExact;
		}
	}

	bool MetadataTagsQuery::match( const LocalId& localId ) const
	{
		return match( &localId );
	}

	MetadataTagsQueryBuilder MetadataTagsQueryBuilder::empty()
	{
		return MetadataTagsQueryBuilder{};
	}

	MetadataTagsQueryBuilder MetadataTagsQueryBuilder::from( const LocalId& localId, bool allowOtherTags )
	{
		MetadataTagsQueryBuilder builder;
		for ( const auto& tag : localId.metadataTags() )
		{
			builder.withTag( tag );
		}
		builder.withAllowOtherTags( allowOtherTags );
		return builder;
	}

	MetadataTagsQueryBuilder& MetadataTagsQueryBuilder::withTag( CodebookName name, const std::string& value )
	{
		return withTag( MetadataTag{ name, value } );
	}

	MetadataTagsQueryBuilder& MetadataTagsQueryBuilder::withTag( const MetadataTag& tag )
	{
		m_tags.insert_or_assign( tag.name(), tag );
		return *this;
	}

	MetadataTagsQueryBuilder& MetadataTagsQueryBuilder::withAllowOtherTags( bool allowOthers )
	{
		m_matchExact = !allowOthers;
		return *this;
	}

	MetadataTagsQuery MetadataTagsQueryBuilder::build() const
	{
		return MetadataTagsQuery{ this };
	}
} // namespace dnv::vista::sdk
