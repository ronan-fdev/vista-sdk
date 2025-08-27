/**
 * @file MetadataTagsQuery.inl
 * @brief Inline implementations for performance-critical MetadataTagsQuery operations
 */

#pragma once

#include "CodebookName.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// MetadataTagsQuery class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline MetadataTagsQuery::MetadataTagsQuery(
		std::unordered_map<CodebookName, MetadataTag> tags,
		bool matchExact ) noexcept
		: m_tags{ std::move( tags ) },
		  m_matchExact{ matchExact }
	{
	}

	//----------------------------------------------
	// Factory methods
	//----------------------------------------------

	inline MetadataTagsQuery MetadataTagsQuery::empty()
	{
		return MetadataTagsQuery{ {}, false };
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::unordered_map<CodebookName, MetadataTag>& MetadataTagsQuery::tags() const noexcept
	{
		return m_tags;
	}

	inline bool MetadataTagsQuery::matchExact() const noexcept
	{
		return m_matchExact;
	}

	inline bool MetadataTagsQuery::isEmpty() const noexcept
	{
		return m_tags.empty();
	}
}
