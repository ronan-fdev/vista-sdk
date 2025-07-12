/**
 * @file GmodDto.inl
 * @brief Inline implementations for performance-critical GmodDto operations
 */

#pragma once

namespace dnv::vista::sdk
{
	//=====================================================================
	// GMOD Node Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline GmodNodeDto::GmodNodeDto(
		std::string category,
		std::string type,
		std::string code,
		std::string name,
		std::optional<std::string> commonName,
		std::optional<std::string> definition,
		std::optional<std::string> commonDefinition,
		std::optional<bool> installSubstructure,
		std::optional<NormalAssignmentNamesMap> normalAssignmentNames ) noexcept
		: m_category{ std::move( category ) },
		  m_type{ std::move( type ) },
		  m_code{ std::move( code ) },
		  m_name{ std::move( name ) },
		  m_commonName{ std::move( commonName ) },
		  m_definition{ std::move( definition ) },
		  m_commonDefinition{ std::move( commonDefinition ) },
		  m_installSubstructure{ installSubstructure },
		  m_normalAssignmentNames{ std::move( normalAssignmentNames ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline std::string_view GmodNodeDto::category() const noexcept
	{
		return m_category;
	}

	inline std::string_view GmodNodeDto::type() const noexcept
	{
		return m_type;
	}

	inline std::string_view GmodNodeDto::code() const noexcept
	{
		return m_code;
	}

	inline std::string_view GmodNodeDto::name() const noexcept
	{
		return m_name;
	}

	inline const std::optional<std::string>& GmodNodeDto::commonName() const
	{
		return m_commonName;
	}

	inline const std::optional<std::string>& GmodNodeDto::definition() const
	{
		return m_definition;
	}

	inline const std::optional<std::string>& GmodNodeDto::commonDefinition() const
	{
		return m_commonDefinition;
	}

	inline const std::optional<bool>& GmodNodeDto::installSubstructure() const
	{
		return m_installSubstructure;
	}

	inline const std::optional<GmodNodeDto::NormalAssignmentNamesMap>& GmodNodeDto::normalAssignmentNames() const
	{
		return m_normalAssignmentNames;
	}

	//=====================================================================
	// GMOD Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline GmodDto::GmodDto( std::string visVersion, Items items, Relations relations ) noexcept
		: m_visVersion{ std::move( visVersion ) },
		  m_items{ std::move( items ) },
		  m_relations{ std::move( relations ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline std::string_view GmodDto::visVersion() const noexcept
	{
		return m_visVersion;
	}

	inline const GmodDto::Items& GmodDto::items() const noexcept
	{
		return m_items;
	}

	inline const GmodDto::Relations& GmodDto::relations() const noexcept
	{
		return m_relations;
	}
}
