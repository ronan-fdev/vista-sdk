/**
 * @file EmbeddedResource.inl
 * @brief Inline implementations for performance-critical resource operations
 */

#pragma once

namespace dnv::vista::sdk
{
	/**
	 * @brief Fast check if filename is a GMOD resource (without versioning)
	 */
	inline bool isGmodResource( std::string_view filename ) noexcept
	{
		return utils::contains( filename, "gmod" ) && !utils::contains( filename, "versioning" ) && utils::endsWith( filename, ".json.gz" );
	}

	/**
	 * @brief Fast check if filename is a GMOD versioning resource
	 */
	inline bool isGmodVersioningResource( std::string_view filename ) noexcept
	{
		return utils::contains( filename, "gmod" ) && utils::contains( filename, "versioning" ) && utils::endsWith( filename, ".json.gz" );
	}

	/**
	 * @brief Fast check if filename is a codebooks resource
	 */
	inline bool isCodebooksResource( std::string_view filename ) noexcept
	{
		return utils::contains( filename, "codebooks" ) && utils::endsWith( filename, ".json.gz" );
	}

	/**
	 * @brief Fast check if filename is a locations resource
	 */
	inline bool isLocationsResource( std::string_view filename ) noexcept
	{
		return utils::contains( filename, "locations" ) && utils::endsWith( filename, ".json.gz" );
	}

	/**
	 * @brief Fast check if filename is an ISO 19848 resource
	 */
	inline bool isISO19848Resource( std::string_view filename ) noexcept
	{
		return utils::contains( filename, "iso19848" ) && utils::endsWith( filename, ".json.gz" );
	}

	/**
	 * @brief Fast check if filename contains version
	 */
	inline bool containsVersion( std::string_view filename, std::string_view version ) noexcept
	{
		return utils::contains( filename, version );
	}

	/**
	 * @brief Fast check if filename is a data channel type names resource
	 */
	inline bool isDataChannelTypeNamesResource( std::string_view filename ) noexcept
	{
		return utils::contains( filename, "data-channel-type-names" ) && utils::contains( filename, "iso19848" ) && utils::endsWith( filename, ".json.gz" );
	}

	/**
	 * @brief Fast check if filename is a format data types resource
	 */
	inline bool isFormatDataTypesResource( std::string_view filename ) noexcept
	{
		return utils::contains( filename, "format-data-types" ) && utils::contains( filename, "iso19848" ) && utils::endsWith( filename, ".json.gz" );
	}

	/**
	 * @brief Fast check if filename matches version
	 */
	inline bool matchesVersion( std::string_view filename, std::string_view version ) noexcept
	{
		return utils::contains( filename, version );
	}
}
