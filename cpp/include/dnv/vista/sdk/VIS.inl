/**
 * @file VIS.inl
 * @brief Inline implementations for performance-critical VIS operations
 */

#pragma once

#include "CodebooksDto.h"
#include "EmbeddedResource.h"
#include "GmodDto.h"
#include "LocationsDto.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// VIS singleton
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline VIS::VIS() {}

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
			throw std::invalid_argument(
				fmt::format(
					"Invalid VIS version: {}",
					static_cast<int>( visVersion ) ) );
		}

		return gmodsCache().getOrCreate( visVersion, [visVersion]() {
			const auto& dto = instance().gmodDto( visVersion );

			return Gmod{ visVersion, dto };
		} );
	}

	inline const Codebooks& VIS::codebooks( VisVersion visVersion )
	{
		if ( !VisVersionExtensions::isValid( visVersion ) )
		{
			throw std::invalid_argument(
				fmt::format(
					"Invalid VIS version: {}",
					static_cast<int>( visVersion ) ) );
		}

		return codebooksCache().getOrCreate( visVersion, [visVersion]() {
			const auto& dto = instance().codebooksDto( visVersion );

			return Codebooks{ visVersion, dto };
		} );
	}

	inline const Locations& VIS::locations( VisVersion visVersion )
	{
		if ( !VisVersionExtensions::isValid( visVersion ) )
		{
			throw std::invalid_argument(
				fmt::format(
					"Invalid VIS version: {}",
					static_cast<int>( visVersion ) ) );
		}

		return locationsCache().getOrCreate(
			visVersion, [visVersion]() {
				const auto& dto = instance().locationsDto( visVersion );
				return Locations{ visVersion, dto };
			} );
	}

	//----------------------------------------------
	// Cached DTO
	//----------------------------------------------

	inline const GmodDto& VIS::gmodDto( VisVersion visVersion )
	{
		if ( !VisVersionExtensions::isValid( visVersion ) )
		{
			throw std::invalid_argument(
				fmt::format(
					"Invalid VIS version: {}",
					static_cast<int>( visVersion ) ) );
		}

		return gmodDtoCache().getOrCreate( visVersion, [visVersion]() {
			auto dto = loadGmodDto( visVersion );
			if ( !dto )
			{
				throw std::runtime_error(
					fmt::format(
						"Failed to load GMOD DTO for version: {}",
						VisVersionExtensions::toVersionString( visVersion ) ) );
			}

			return std::move( *dto );
		} );
	}

	inline const CodebooksDto& VIS::codebooksDto( VisVersion visVersion )
	{
		if ( !VisVersionExtensions::isValid( visVersion ) )
		{
			throw std::invalid_argument(
				fmt::format(
					"Invalid VIS version: {}",
					static_cast<int>( visVersion ) ) );
		}

		return codebooksDtoCache().getOrCreate( visVersion, [visVersion]() {
			auto dto = EmbeddedResource::codebooks( VisVersionExtensions::toVersionString( visVersion ) );
			if ( !dto )
			{
				throw std::runtime_error(
					fmt::format(
						"Failed to load codebooks DTO for version: {}",
						VisVersionExtensions::toVersionString( visVersion ) ) );
			}

			return std::move( *dto );
		} );
	}

	inline const LocationsDto& VIS::locationsDto( VisVersion visVersion )
	{
		if ( !VisVersionExtensions::isValid( visVersion ) )
		{
			throw std::invalid_argument(
				fmt::format(
					"Invalid VIS version: {}",
					static_cast<int>( visVersion ) ) );
		}

		return locationsDtoCache().getOrCreate( visVersion, [visVersion]() {
			auto dto = EmbeddedResource::locations( VisVersionExtensions::toVersionString( visVersion ) );
			if ( !dto )
			{
				throw std::runtime_error(
					fmt::format(
						"Failed to load locations DTO for version: {}",
						VisVersionExtensions::toVersionString( visVersion ) ) );
			}

			return std::move( *dto );
		} );
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
