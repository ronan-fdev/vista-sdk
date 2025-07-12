/**
 * @file EmbeddedResource.inl
 * @brief Inline implementations for performance-critical resource operations
 * @details Contains performance-critical inline implementations for
 *          fast resource access operations.
 */

#pragma once

namespace dnv::vista::sdk
{
	/**
	 * @brief Fast check if filename is a GMOD resource (without versioning)
	 */
	inline bool isGmodResource( std::string_view filename ) noexcept
	{
		return contains( filename, "gmod" ) &&
			   !contains( filename, "versioning" ) &&
			   endsWith( filename, ".json.gz" );
	}

	/**
	 * @brief Fast check if filename is a GMOD versioning resource
	 */
	inline bool isGmodVersioningResource( std::string_view filename ) noexcept
	{
		return contains( filename, "gmod" ) &&
			   contains( filename, "versioning" ) &&
			   endsWith( filename, ".json.gz" );
	}

	/**
	 * @brief Fast check if filename is a codebooks resource
	 */
	inline bool isCodebooksResource( std::string_view filename ) noexcept
	{
		return contains( filename, "codebooks" ) && endsWith( filename, ".json.gz" );
	}

	/**
	 * @brief Fast check if filename is a locations resource
	 */
	inline bool isLocationsResource( std::string_view filename ) noexcept
	{
		return contains( filename, "locations" ) && endsWith( filename, ".json.gz" );
	}

	/**
	 * @brief Fast check if filename is an ISO 19848 resource
	 */
	inline bool isISO19848Resource( std::string_view filename ) noexcept
	{
		return contains( filename, "iso19848" ) && endsWith( filename, ".json.gz" );
	}

	/**
	 * @brief Fast check if filename contains version
	 */
	inline bool containsVersion( std::string_view filename, std::string_view version ) noexcept
	{
		return contains( filename, version );
	}

	/**
	 * @brief Fast check if filename is a data channel type names resource
	 */
	inline bool isDataChannelTypeNamesResource( std::string_view filename ) noexcept
	{
		return contains( filename, "data-channel-type-names" ) &&
			   contains( filename, "iso19848" ) &&
			   endsWith( filename, ".json.gz" );
	}

	/**
	 * @brief Fast check if filename is a format data types resource
	 */
	inline bool isFormatDataTypesResource( std::string_view filename ) noexcept
	{
		return contains( filename, "format-data-types" ) &&
			   contains( filename, "iso19848" ) &&
			   endsWith( filename, ".json.gz" );
	}

	/**
	 * @brief Fast check if filename matches version
	 */
	inline bool matchesVersion( std::string_view filename, std::string_view version ) noexcept
	{
		return contains( filename, version );
	}
}
