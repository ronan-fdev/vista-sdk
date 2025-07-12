/**
 * @file VIS.inl
 * @brief Inline implementations for performance-critical VIS operations
 */

#pragma once

#include "EmbeddedResource.h"
#include "GmodDto.h"
#include "CodebooksDto.h"
#include "LocationsDto.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// VIS singleton
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline VIS::VIS()
	{
		m_gmodDtoCache.reserve( 10 );
		m_codebooksDtoCache.reserve( 10 );
		m_locationsDtoCache.reserve( 10 );
		m_gmodsCache.reserve( 10 );
		m_codebooksCache.reserve( 10 );
		m_locationsCache.reserve( 10 );
	}

	//----------------------------------------------
	// Singleton access
	//----------------------------------------------

	inline VIS& VIS::instance()
	{
		static VIS instance;

		return instance;
	}

	//----------------------------------------------
	// DTO Loading
	//----------------------------------------------

	inline std::optional<GmodDto> VIS::loadGmodDto( VisVersion visVersion )
	{
		return EmbeddedResource::gmod( VisVersionExtensions::toVersionString( visVersion ) );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	//-----------------------------
	// VisVersion
	//-----------------------------

	inline VisVersion VIS::latestVisVersion() const noexcept
	{
		return VisVersion::LATEST;
	}

	inline const std::vector<VisVersion>& VIS::visVersions() const noexcept
	{
		return VisVersionExtensions::allVersions();
	}

	//-----------------------------
	// Cached objects
	//-----------------------------

	inline const Gmod& VIS::gmod( VisVersion visVersion )
	{
		if ( !VisVersionExtensions::isValid( visVersion ) )
		{
			throw std::invalid_argument( fmt::format( "Invalid VIS version: {}", static_cast<int>( visVersion ) ) );
		}

		auto it = m_gmodsCache.find( visVersion );
		if ( it != m_gmodsCache.end() )
		{
			return it->second;
		}

		const auto& dto = gmodDto( visVersion );
		auto [emplace_it, inserted] = m_gmodsCache.try_emplace( visVersion, visVersion, dto );

		return emplace_it->second;
	}

	inline const Codebooks& VIS::codebooks( VisVersion visVersion )
	{
		if ( !VisVersionExtensions::isValid( visVersion ) )
		{
			throw std::invalid_argument( fmt::format( "Invalid VIS version: {}", static_cast<int>( visVersion ) ) );
		}

		auto it = m_codebooksCache.find( visVersion );
		if ( it != m_codebooksCache.end() )
		{
			return it->second;
		}

		const auto& dto = codebooksDto( visVersion );
		auto [emplace_it, inserted] = m_codebooksCache.try_emplace( visVersion, visVersion, dto );

		return emplace_it->second;
	}

	inline const Locations& VIS::locations( VisVersion visVersion )
	{
		if ( !VisVersionExtensions::isValid( visVersion ) )
		{
			throw std::invalid_argument( fmt::format( "Invalid VIS version: {}", static_cast<int>( visVersion ) ) );
		}

		auto it = m_locationsCache.find( visVersion );
		if ( it != m_locationsCache.end() )
		{
			return it->second;
		}

		const auto& dto = locationsDto( visVersion );
		auto [emplace_it, inserted] = m_locationsCache.try_emplace( visVersion, visVersion, dto );

		return emplace_it->second;
	}

	//----------------------------------------------
	// Cached DTO
	//----------------------------------------------

	inline const GmodDto& VIS::gmodDto( VisVersion visVersion )
	{
		if ( !VisVersionExtensions::isValid( visVersion ) )
		{
			throw std::invalid_argument( fmt::format( "Invalid VIS version: {}", static_cast<int>( visVersion ) ) );
		}

		auto [it, inserted] = m_gmodDtoCache.try_emplace( visVersion );
		if ( inserted )
		{
			auto dto = loadGmodDto( visVersion );
			if ( !dto )
			{
				m_gmodDtoCache.erase( it );
				throw std::runtime_error( fmt::format( "Failed to load GMOD DTO for version: {}", VisVersionExtensions::toVersionString( visVersion ) ) );
			}
			it->second = std::move( *dto );
		}

		return it->second;
	}

	inline const CodebooksDto& VIS::codebooksDto( VisVersion visVersion )
	{
		if ( !VisVersionExtensions::isValid( visVersion ) )
		{
			throw std::invalid_argument( fmt::format( "Invalid VIS version: {}", static_cast<int>( visVersion ) ) );
		}

		auto [it, inserted] = m_codebooksDtoCache.try_emplace( visVersion );
		if ( inserted )
		{
			auto dto = EmbeddedResource::codebooks( VisVersionExtensions::toVersionString( visVersion ) );
			if ( !dto )
			{
				m_codebooksDtoCache.erase( it );
				throw std::runtime_error( fmt::format( "Failed to load codebooks DTO for version: {}", VisVersionExtensions::toVersionString( visVersion ) ) );
			}
			it->second = std::move( *dto );
		}

		return it->second;
	}

	inline const LocationsDto& VIS::locationsDto( VisVersion visVersion )
	{
		if ( !VisVersionExtensions::isValid( visVersion ) )
		{
			throw std::invalid_argument( fmt::format( "Invalid VIS version: {}", static_cast<int>( visVersion ) ) );
		}

		auto [it, inserted] = m_locationsDtoCache.try_emplace( visVersion );
		if ( inserted )
		{
			auto dto = EmbeddedResource::locations( VisVersionExtensions::toVersionString( visVersion ) );
			if ( !dto )
			{
				m_locationsDtoCache.erase( it );
				throw std::runtime_error( fmt::format( "Failed to load locations DTO for version: {}", VisVersionExtensions::toVersionString( visVersion ) ) );
			}
			it->second = std::move( *dto );
		}

		return it->second;
	}

	//----------------------------------------------
	// ISO string validation
	//----------------------------------------------

	template <typename StringLike>
	inline bool VIS::matchISOLocalIdString( const StringLike& value ) noexcept
	{
		std::string_view sv = value;
		for ( char c : sv )
		{
			if ( c == '/' )
			{
				continue;
			}
			if ( !isISOString( c ) )
			{
				return false;
			}
		}

		return true;
	}

	template <typename StringLike>
	inline bool VIS::isISOString( const StringLike& value ) noexcept
	{
		std::string_view sv = value;
		for ( char c : sv )
		{
			if ( !isISOString( c ) )
			{
				return false;
			}
		}

		return true;
	}

	template <typename StringLike>
	inline bool VIS::isISOLocalIdString( const StringLike& value ) noexcept
	{
		return matchISOLocalIdString( value );
	}

	inline bool VIS::isISOString( char c ) noexcept
	{
		return matchAsciiDecimal( static_cast<int>( c ) );
	}

	inline constexpr bool VIS::matchAsciiDecimal( int code ) noexcept
	{
		return ( code >= 48 && code <= 57 )		/* Numbers:     0-9  */
			   || ( code >= 65 && code <= 90 )	/* Uppercase:   A-Z  */
			   || ( code >= 97 && code <= 122 ) /* Lowercase:   a-z  */
			   || ( code == 45					/* Hyphen:      -    */
					  || code == 46				/* Period:      .    */
					  || code == 95				/* Underscore:  _    */
					  || code == 126 );			/* Tilde:       ~    */
	}
}
