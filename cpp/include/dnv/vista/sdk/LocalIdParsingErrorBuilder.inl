/**
 * @file LocalIdParsingErrorBuilder.inl
 * @brief Inline implementations for performance-critical LocalIdParsingErrorBuilder operations
 */

#pragma once

namespace dnv::vista::sdk
{
	//=====================================================================
	// LocalIdParsingErrorBuilder class
	//=====================================================================

	//----------------------------------------------
	// State inspection methods
	//----------------------------------------------

	inline bool LocalIdParsingErrorBuilder::hasError() const noexcept
	{
		return !m_errors.empty();
	}

	//----------------------------------------------
	// Static factory method
	//----------------------------------------------

	inline LocalIdParsingErrorBuilder LocalIdParsingErrorBuilder::create()
	{
		return LocalIdParsingErrorBuilder{};
	}
}
