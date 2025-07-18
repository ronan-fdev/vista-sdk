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

	inline bool LocalIdParsingErrorBuilder::hasError() const
	{
		return !m_errors.empty();
	}
}
