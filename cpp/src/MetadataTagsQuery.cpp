/**
 * @file MetadataTagsQuery.cpp
 * @brief Implementation of MetadataTagsQuery class
 */

#include "dnv/vista/sdk/MetadataTagsQuery.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// MetadataTagsQuery class
	//=====================================================================

	//----------------------------------------------
	// Factory methods
	//----------------------------------------------

	MetadataTagsQuery MetadataTagsQuery::from( const LocalId& localId, bool allowOtherTags )
	{
		std::unordered_map<CodebookName, MetadataTag> tags;

		for ( const auto& tag : localId.metadataTags() )
		{
			tags.insert_or_assign( tag.name(), tag );
		}

		return MetadataTagsQuery{ std::move( tags ), !allowOtherTags };
	}

	//----------------------------------------------
	// Fluent builder methods (Immutable)
	//----------------------------------------------

	MetadataTagsQuery MetadataTagsQuery::withTag( CodebookName name, const std::string& value ) const
	{
		return withTag( MetadataTag{ name, value } );
	}

	MetadataTagsQuery MetadataTagsQuery::withTag( const MetadataTag& tag ) const
	{
		auto newTags = m_tags;
		newTags.insert_or_assign( tag.name(), tag );
		return MetadataTagsQuery{ std::move( newTags ), m_matchExact };
	}

	MetadataTagsQuery MetadataTagsQuery::withTag( MetadataTag&& tag ) const
	{
		auto newTags = m_tags;
		newTags.insert_or_assign( tag.name(), std::move( tag ) );
		return MetadataTagsQuery{ std::move( newTags ), m_matchExact };
	}

	MetadataTagsQuery MetadataTagsQuery::withAllowOtherTags( bool allowOthers ) const
	{
		return MetadataTagsQuery{ m_tags, !allowOthers };
	}

	//----------------------------------------------
	// Query execution
	//----------------------------------------------

	bool MetadataTagsQuery::match( const LocalId* localId ) const
	{
		if ( localId == nullptr )
		{
			return false;
		}

		// Create a map of the LocalId's metadata tags for efficient lookup
		std::unordered_map<CodebookName, MetadataTag> localIdTags;
		for ( const auto& tag : localId->metadataTags() )
		{
			localIdTags.insert_or_assign( tag.name(), tag );
		}

		if ( !m_tags.empty() )
		{
			if ( m_matchExact )
			{
				// Exact match: same number of tags and all must match exactly
				if ( m_tags.size() != localIdTags.size() )
				{
					return false;
				}

				return std::all_of( m_tags.begin(), m_tags.end(),
					[&localIdTags]( const auto& queryTagPair ) {
						auto it = localIdTags.find( queryTagPair.first );
						return it != localIdTags.end() && queryTagPair.second == it->second;
					} );
			}
			else
			{
				// Partial match: all query tags must be present and match
				return std::all_of( m_tags.begin(), m_tags.end(),
					[&localIdTags]( const auto& queryTagPair ) {
						auto it = localIdTags.find( queryTagPair.first );
						return it != localIdTags.end() && queryTagPair.second == it->second;
					} );
			}
		}
		else
		{
			// No tags specified: return true only if not requiring exact match
			return !m_matchExact;
		}
	}
}
