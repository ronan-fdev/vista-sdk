/**
 * @file GmodVersioning.inl
 * @brief Inline implementations for performance-critical GmodVersioning operations
 */

#pragma once

namespace dnv::vista::sdk
{
	//=====================================================================
	// GMOD versioning assignement data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline GmodVersioningAssignmentChangeDto::GmodVersioningAssignmentChangeDto(
		std::string oldAssignment,
		std::string currentAssignment ) noexcept
		: m_oldAssignment{ std::move( oldAssignment ) },
		  m_currentAssignment{ std::move( currentAssignment ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline std::string_view GmodVersioningAssignmentChangeDto::oldAssignment() const noexcept
	{
		return m_oldAssignment;
	}

	inline std::string_view GmodVersioningAssignmentChangeDto::currentAssignment() const noexcept
	{
		return m_currentAssignment;
	}

	//=====================================================================
	// GMOD Node Conversion Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline GmodNodeConversionDto::GmodNodeConversionDto(
		OperationSet operations, std::string source,
		std::string target, std::string oldAssignment,
		std::string newAssignment, bool deleteAssignment ) noexcept
		: m_operations{ std::move( operations ) },
		  m_source{ std::move( source ) },
		  m_target{ std::move( target ) },
		  m_oldAssignment{ std::move( oldAssignment ) },
		  m_newAssignment{ std::move( newAssignment ) },
		  m_deleteAssignment{ deleteAssignment }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const GmodNodeConversionDto::OperationSet& GmodNodeConversionDto::operations() const noexcept
	{
		return m_operations;
	}

	inline std::string_view GmodNodeConversionDto::source() const noexcept
	{
		return m_source;
	}

	inline std::string_view GmodNodeConversionDto::target() const noexcept
	{
		return m_target;
	}

	inline std::string_view GmodNodeConversionDto::oldAssignment() const noexcept
	{
		return m_oldAssignment;
	}

	inline std::string_view GmodNodeConversionDto::newAssignment() const noexcept
	{
		return m_newAssignment;
	}

	inline bool GmodNodeConversionDto::deleteAssignment() const noexcept
	{
		return m_deleteAssignment;
	}

	//=====================================================================
	// GMOD Versioning Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline GmodVersioningDto::GmodVersioningDto( std::string visVersion, ItemsMap items ) noexcept
		: m_visVersion{ std::move( visVersion ) },
		  m_items{ std::move( items ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline std::string_view GmodVersioningDto::visVersion() const noexcept
	{
		return m_visVersion;
	}

	inline const GmodVersioningDto::ItemsMap& GmodVersioningDto::items() const noexcept
	{
		return m_items;
	}
}
