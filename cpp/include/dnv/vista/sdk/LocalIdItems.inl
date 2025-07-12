/**
 * @file LocalIdItems.inl
 * @brief Inline implementations for performance-critical LocalIdItems operations
 */

#pragma once

#include "VIS.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// LocalIdItems class
	//=====================================================================

	//----------------------------------------------
	// Comparison operators
	//----------------------------------------------

	inline bool LocalIdItems::operator==( const LocalIdItems& other ) const noexcept
	{
		return m_primaryItem == other.m_primaryItem && m_secondaryItem == other.m_secondaryItem;
	}

	inline bool LocalIdItems::operator!=( const LocalIdItems& other ) const noexcept
	{
		return !( *this == other );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::optional<GmodPath>& LocalIdItems::primaryItem() const noexcept
	{
		return m_primaryItem;
	}

	inline const std::optional<GmodPath>& LocalIdItems::secondaryItem() const noexcept
	{
		return m_secondaryItem;
	}

	inline bool LocalIdItems::isEmpty() const noexcept
	{
		return !m_primaryItem.has_value() && !m_secondaryItem.has_value();
	}

	//=====================================================================
	// Template method implementations
	//=====================================================================

	template <typename OutputIt>
	inline OutputIt LocalIdItems::append( OutputIt out, bool verboseMode ) const
	{
		if ( !m_primaryItem && !m_secondaryItem )
		{
			return out;
		}

		if ( m_primaryItem )
		{
			out = m_primaryItem->toString( out );
			*out++ = '/';
		}

		if ( m_secondaryItem )
		{
			out = fmt::format_to( out, "sec/" );
			out = m_secondaryItem->toString( out );
			*out++ = '/';
		}

		if ( verboseMode )
		{
			if ( m_primaryItem )
			{
				for ( const auto& [depth, name] : m_primaryItem->commonNames() )
				{
					*out++ = '~';
					const GmodNode& nodeRef = ( *m_primaryItem )[depth];
					auto locationStr = nodeRef.location().has_value() ? std::optional( nodeRef.location()->toString() ) : std::nullopt;
					out = appendCommonName( out, name, locationStr );
					*out++ = '/';
				}
			}

			if ( m_secondaryItem )
			{
				std::string_view prefix = "~for.";
				for ( const auto& [depth, name] : m_secondaryItem->commonNames() )
				{
					out = fmt::format_to( out, "{}", prefix );
					if ( prefix.size() > 1 )
						prefix = "~";
					const GmodNode& nodeRef = ( *m_secondaryItem )[depth];
					auto locationStr = nodeRef.location().has_value() ? std::optional( nodeRef.location()->toString() ) : std::nullopt;
					out = appendCommonName( out, name, locationStr );
					*out++ = '/';
				}
			}
		}
		return out;
	}

	template <typename OutputIt>
	inline OutputIt LocalIdItems::appendCommonName(
		OutputIt out,
		std::string_view commonName,
		const std::optional<std::string>& location )
	{
		char prev = '\0';
		for ( const char ch : commonName )
		{
			if ( ch == '/' || ( prev == ' ' && ch == ' ' ) )
				continue;

			char current;
			if ( ch == ' ' )
			{
				current = '.';
			}
			else if ( !VIS::isISOString( ch ) )
			{
				current = '.';
			}
			else
			{
				current = static_cast<char>( std::tolower( static_cast<unsigned char>( ch ) ) );
			}

			if ( current == '.' && prev == '.' )
				continue;

			*out++ = current;
			prev = current;
		}

		if ( location.has_value() && !location->empty() )
		{
			*out++ = '.';
			out = fmt::format_to( out, "{}", *location );
		}
		return out;
	}
}
