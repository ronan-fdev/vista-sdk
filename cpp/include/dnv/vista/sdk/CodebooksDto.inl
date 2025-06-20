/**
 * @file CodebooksDto.inl
 * @brief Inline implementations for performance-critical CodebooksDto operations
 */

namespace dnv::vista::sdk
{
	//=====================================================================
	// Codebook Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Accessor
	//----------------------------------------------

	inline std::string_view CodebookDto::name() const
	{
		return m_name;
	}

	inline const CodebookDto::ValuesMap& CodebookDto::values() const
	{
		return m_values;
	}

	//=====================================================================
	// Codebooks Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::string& CodebooksDto::visVersion() const
	{
		return m_visVersion;
	}

	inline const CodebooksDto::Items& CodebooksDto::items() const
	{
		return m_items;
	}
}
