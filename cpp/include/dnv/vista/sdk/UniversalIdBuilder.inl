/**
 * @file UniversalIdBuilder.inl
 * @brief Inline implementations for performance-critical UniversalIdBuilder operations
 */

#pragma once

#include "config/Platform.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// UniversalIdBuilder class
	//=====================================================================

	//----------------------------------------------
	// Operators
	//----------------------------------------------

	inline bool UniversalIdBuilder::operator==( const UniversalIdBuilder& other ) const
	{
		return equals( other );
	}

	inline bool UniversalIdBuilder::operator!=( const UniversalIdBuilder& other ) const
	{
		return !equals( other );
	}

	inline bool UniversalIdBuilder::equals( const UniversalIdBuilder& other ) const
	{
		return m_imoNumber == other.m_imoNumber &&
			   m_localIdBuilder == other.m_localIdBuilder;
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::optional<ImoNumber>& UniversalIdBuilder::imoNumber() const noexcept
	{
		return m_imoNumber;
	}

	inline const std::optional<LocalIdBuilder>& UniversalIdBuilder::localId() const noexcept
	{
		return m_localIdBuilder;
	}

	//----------------------------------------------
	// State inspection methods
	//----------------------------------------------

	inline bool UniversalIdBuilder::isValid() const noexcept
	{
		return m_imoNumber.has_value() &&
			   m_localIdBuilder.has_value() &&
			   m_localIdBuilder->isValid();
	}
}
