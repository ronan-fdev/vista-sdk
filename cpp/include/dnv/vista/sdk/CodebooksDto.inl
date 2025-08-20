/**
 * @file CodebooksDto.inl
 * @brief Inline implementations for performance-critical CodebooksDto operations
 */

#pragma once

namespace dnv::vista::sdk
{
	//=====================================================================
	// Codebook Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline CodebookDto::CodebookDto( std::string name, ValuesMap values ) noexcept
		: m_name{ std::move( name ) },
		  m_values{ std::move( values ) }
	{
	}

	//----------------------------------------------
	// Accessor
	//----------------------------------------------

	inline std::string_view CodebookDto::name() const noexcept
	{
		return m_name;
	}

	inline const CodebookDto::ValuesMap& CodebookDto::values() const noexcept
	{
		return m_values;
	}

	//=====================================================================
	// Codebooks Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline CodebooksDto::CodebooksDto( std::string visVersion, Items items ) noexcept
		: m_visVersion{ std::move( visVersion ) },
		  m_items{ std::move( items ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline std::string_view CodebooksDto::visVersion() const noexcept
	{
		return m_visVersion;
	}

	inline const CodebooksDto::Items& CodebooksDto::items() const noexcept
	{
		return m_items;
	}
}
