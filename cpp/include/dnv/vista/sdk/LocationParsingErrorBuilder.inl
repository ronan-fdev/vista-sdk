/**
 * @file LocationParsingErrorBuilder.inl
 * @brief Inline implementations for performance-critical LocationParsingErrorBuilder operations
 */

#pragma once

namespace dnv::vista::sdk::internal
{
	//=====================================================================
	// LocationParsingErrorBuilder class
	//=====================================================================

	//----------------------------------------------
	// State inspection methods
	//----------------------------------------------

	inline bool LocationParsingErrorBuilder::hasError() const noexcept
	{
		return !m_errors.empty();
	}

	//----------------------------------------------
	// Static factory method
	//----------------------------------------------

	inline LocationParsingErrorBuilder LocationParsingErrorBuilder::create()
	{
		return LocationParsingErrorBuilder();
	}
}
