/**
 * @file ParsingErrors.cpp
 * @brief Implementation of ParsingErrors class
 */

#include "pch.h"

#include "dnv/vista/sdk/ParsingErrors.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// ParsingErrors class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	ParsingErrors::ParsingErrors( const std::vector<ErrorEntry>& errors )
		: m_errors{ errors }
	{
	}

	ParsingErrors::ParsingErrors( std::vector<ErrorEntry>&& errors ) noexcept
		: m_errors{ std::move( errors ) }
	{
	}

	ParsingErrors::ParsingErrors() : m_errors{}
	{
	}

	ParsingErrors::ParsingErrors( ParsingErrors&& errors ) noexcept
		: m_errors{ std::move( errors.m_errors ) }
	{
	}

	//----------------------------------------------
	// String conversion methods
	//----------------------------------------------

	std::string ParsingErrors::toString() const
	{
		if ( m_errors.empty() )
		{
			return "Success";
		}

		constexpr std::string_view header = "Parsing errors:\n";

		/* Pre-calculate exact capacity */
		size_t capacity = header.size();
		for ( const auto& error : m_errors )
		{
			capacity += 1 + error.type.size() + 3 + error.message.size() + 1;
			/*          ↑                       ↑                          ↑  */
			/*        '\t'                    " - "                      '\n' */
		}

		std::string result;
		result.reserve( capacity );
		result = header;

		for ( const auto& error : m_errors )
		{
			result += '\t';
			result += error.type;
			result += " - ";
			result += error.message;
			result += '\n';
		}

		return result;
	}

	//----------------------------------------------
	// Enumeration
	//----------------------------------------------

	ParsingErrors::Enumerator ParsingErrors::enumerator() const
	{
		return Enumerator( &m_errors );
	}

	//----------------------------------------------
	// ParsingErrors::Enumerator class
	//----------------------------------------------

	//----------------------------
	// Construction
	//----------------------------

	ParsingErrors::Enumerator::Enumerator( const std::vector<ErrorEntry>* data )
		: m_data{ data }, m_index{ 0 }
	{
	}

	//----------------------------
	// Enumeration methods
	//----------------------------

	const ParsingErrors::ErrorEntry& ParsingErrors::Enumerator::current() const
	{
		if ( m_index == 0 || m_index > m_data->size() )
		{
			throw std::out_of_range( "Enumerator not positioned on valid element" );
		}

		return ( *m_data )[m_index - 1];
	}

	//----------------------------------------------
	// ParsingErrors::ErrorEntry struct
	//----------------------------------------------

	ParsingErrors::ErrorEntry::ErrorEntry( std::string_view type, std::string_view message )
		: type{ type },
		  message{ message }
	{
	}

	ParsingErrors::ErrorEntry::ErrorEntry( std::string&& type, std::string&& message )
		: type{ std::move( type ) },
		  message{ std::move( message ) }
	{
	}
}
