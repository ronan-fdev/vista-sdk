/**
 * @file ParsingErrors.inl
 * @brief Inline implementations for performance-critical ParsingErrors operations
 */

#pragma once

#include "config/Platform.h"
#include "constants/AlgorithmConstants.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// ParsingErrors class
	//=====================================================================

	//----------------------------------------------
	// Operators
	//----------------------------------------------

	inline bool ParsingErrors::operator==( const ParsingErrors& other ) const noexcept
	{
		if ( this == &other )
		{
			return true;
		}

		if ( m_errors.size() != other.m_errors.size() )
		{
			return false;
		}

		return m_errors == other.m_errors;
	}

	inline bool ParsingErrors::operator!=( const ParsingErrors& other ) const noexcept
	{
		return !( *this == other );
	}

	inline bool ParsingErrors::equals( const ParsingErrors& other ) const noexcept
	{
		return *this == other;
	}

	//----------------------------------------------
	// Public static members
	//----------------------------------------------

	inline const ParsingErrors& ParsingErrors::empty()
	{
		static const ParsingErrors instance{};

		return instance;
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline size_t ParsingErrors::count() const noexcept
	{
		return m_errors.size();
	}

	//----------------------------------------------
	// State inspection methods
	//----------------------------------------------

	inline bool ParsingErrors::hasErrors() const noexcept
	{
		return !m_errors.empty();
	}

	inline bool ParsingErrors::hasErrorType( std::string_view type ) const noexcept
	{
		if ( m_errors.empty() )
		{
			return false;
		}

		for ( const auto& error : m_errors )
		{
			if ( error.type == type )
			{
				return true;
			}
		}
		return false;
	}

	//----------------------------------------------
	// ParsingErrors::Enumerator class
	//----------------------------------------------

	//----------------------------
	// Enumeration methods
	//----------------------------

	inline bool ParsingErrors::Enumerator::next() noexcept
	{
		if ( m_index < m_data->size() )
		{
			++m_index;

			return true;
		}

		return false;
	}

	inline void ParsingErrors::Enumerator::reset() noexcept
	{
		m_index = 0;
	}

	//----------------------------
	// Operators
	//----------------------------

	inline bool ParsingErrors::ErrorEntry::operator==( const ErrorEntry& other ) const noexcept
	{
		return type == other.type && message == other.message;
	}

	inline bool ParsingErrors::ErrorEntry::operator!=( const ErrorEntry& other ) const noexcept
	{
		return !( *this == other );
	}
}
