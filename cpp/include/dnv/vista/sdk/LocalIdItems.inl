/**
 * @file LocalIdItems.inl
 * @brief Inline implementations for performance-critical LocalIdItems operations
 */

#pragma once

#include "constants/LocalIdConstants.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// LocalIdItems class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline LocalIdItems::LocalIdItems( GmodPath&& primaryItem, std::optional<GmodPath>&& secondaryItem )
		: m_primaryItem{ std::move( primaryItem ) },
		  m_secondaryItem{ std::move( secondaryItem ) }
	{
	}

	inline LocalIdItems::LocalIdItems( LocalIdItems&& other, GmodPath&& newPrimaryItem )
		: m_primaryItem{ std::move( newPrimaryItem ) },
		  m_secondaryItem{ std::move( other.m_secondaryItem ) }
	{
	}

	inline LocalIdItems::LocalIdItems( LocalIdItems&& other, std::optional<GmodPath>&& newSecondaryItem )
		: m_primaryItem{ std::move( other.m_primaryItem ) },
		  m_secondaryItem{ std::move( newSecondaryItem ) }
	{
	}

	//----------------------------------------------
	// Comparison operators
	//----------------------------------------------

	inline bool LocalIdItems::operator==( const LocalIdItems& other ) const noexcept
	{
		return m_primaryItem == other.m_primaryItem &&
			   m_secondaryItem == other.m_secondaryItem;
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
		return !m_primaryItem.has_value() &&
			   !m_secondaryItem.has_value();
	}

	//=====================================================================
	//=====================================================================
	// Method implementations
	//=====================================================================

	inline void LocalIdItems::append( utils::StringBuilderWrapper& builder, bool verboseMode ) const
	{
		if ( !m_primaryItem && !m_secondaryItem )
		{
			return;
		}

		if ( m_primaryItem )
		{
			m_primaryItem->toString( builder );
			builder.push_back( '/' );
		}

		if ( m_secondaryItem )
		{
			builder.append( constants::localId::PREFIX_SEC );
			builder.push_back( '/' );
			m_secondaryItem->toString( builder );
			builder.push_back( '/' );
		}

		if ( verboseMode )
		{
			if ( m_primaryItem )
			{
				for ( const auto& [depth, name] : m_primaryItem->commonNames() )
				{
					builder.push_back( '~' );
					const GmodNode& nodeRef = ( *m_primaryItem )[depth];
					if ( nodeRef.location().has_value() )
					{
						const auto locationStr = nodeRef.location()->toString();
						appendCommonName( builder, name, std::optional<std::string>( locationStr ) );
					}
					else
					{
						appendCommonName( builder, name, std::nullopt );
					}
					builder.push_back( '/' );
				}
			}

			if ( m_secondaryItem )
			{
				bool isFirstSecondary = true;
				for ( const auto& [depth, name] : m_secondaryItem->commonNames() )
				{
					if ( isFirstSecondary )
					{
						builder.append( constants::localId::PREFIX_FOR );
						isFirstSecondary = false;
					}
					else
					{
						builder.append( constants::localId::PREFIX_TILDE );
					}

					const GmodNode& nodeRef = ( *m_secondaryItem )[depth];
					if ( nodeRef.location().has_value() )
					{
						const auto locationStr = nodeRef.location()->toString();
						appendCommonName( builder, name, std::optional<std::string>( locationStr ) );
					}
					else
					{
						appendCommonName( builder, name, std::nullopt );
					}
					builder.push_back( '/' );
				}
			}
		}
	}

	inline void LocalIdItems::appendCommonName(
		utils::StringBuilderWrapper& builder,
		std::string_view commonName,
		const std::optional<std::string>& location )
	{
		char prev = '\0';
		for ( const char ch : commonName )
		{
			if ( ch == '/' || ( prev == ' ' && ch == ' ' ) )
			{
				continue;
			}

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

			builder.push_back( current );
			prev = current;
		}

		if ( location.has_value() && !location->empty() )
		{
			builder.push_back( '.' );
			builder.append( *location );
		}
	}
}
