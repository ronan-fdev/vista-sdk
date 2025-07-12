/**
 * @file LocationsDto.inl
 * @brief Inline implementations for performance-critical LocationsDto operations
 */

#pragma once

namespace dnv::vista::sdk
{
	//=====================================================================
	// Relative Location data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline RelativeLocationsDto::RelativeLocationsDto( char code, std::string name, std::optional<std::string> definition ) noexcept
		: m_code{ code },
		  m_name{ std::move( name ) },
		  m_definition{ std::move( definition ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline char RelativeLocationsDto::code() const noexcept
	{
		return m_code;
	}

	inline std::string_view RelativeLocationsDto::name() const noexcept
	{
		return m_name;
	}

	inline const std::optional<std::string>& RelativeLocationsDto::definition() const noexcept
	{
		return m_definition;
	}

	//=====================================================================
	// Location data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline LocationsDto::LocationsDto( std::string visVersion, std::vector<RelativeLocationsDto> items ) noexcept
		: m_visVersion{ std::move( visVersion ) },
		  m_items{ std::move( items ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline std::string_view LocationsDto::visVersion() const noexcept
	{
		return m_visVersion;
	}

	inline const std::vector<RelativeLocationsDto>& LocationsDto::items() const noexcept
	{
		return m_items;
	}
}
