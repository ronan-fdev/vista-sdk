/**
 * @file ISO19848Dtos.inl
 * @brief Inline implementations for performance-critical ISO19848Dtos operations
 */

#pragma once

namespace dnv::vista::sdk
{
	//=====================================================================
	// Single Data Channel Type data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline DataChannelTypeNameDto::DataChannelTypeNameDto( std::string type, std::string description ) noexcept
		: m_type{ std::move( type ) },
		  m_description{ std::move( description ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline std::string_view DataChannelTypeNameDto::type() const noexcept
	{
		return m_type;
	}

	inline std::string_view DataChannelTypeNameDto::description() const noexcept
	{
		return m_description;
	}

	//=====================================================================
	// Collection of Data Channel Type data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline DataChannelTypeNamesDto::DataChannelTypeNamesDto( std::vector<DataChannelTypeNameDto> values ) noexcept
		: m_values{ std::move( values ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::vector<DataChannelTypeNameDto>& DataChannelTypeNamesDto::values() const noexcept
	{
		return m_values;
	}

	//=====================================================================
	// Single Format Data Type data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline FormatDataTypeDto::FormatDataTypeDto( std::string type, std::string description ) noexcept
		: m_type{ std::move( type ) },
		  m_description{ std::move( description ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline std::string_view FormatDataTypeDto::type() const noexcept
	{
		return m_type;
	}

	inline std::string_view FormatDataTypeDto::description() const noexcept
	{
		return m_description;
	}

	//=====================================================================
	// Collection of  Format Data Type data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline FormatDataTypesDto::FormatDataTypesDto( std::vector<FormatDataTypeDto> values ) noexcept
		: m_values{ std::move( values ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::vector<FormatDataTypeDto>& FormatDataTypesDto::values() const noexcept
	{
		return m_values;
	}
}
