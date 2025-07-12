/**
 * @file GmodVersioningDto.cpp
 * @brief Implementation of GMOD versioning data transfer objects
 */

#include "pch.h"

#include "dnv/vista/sdk/GmodVersioningDto.h"

#include "dnv/vista/sdk/Config.h"

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// JSON parsing helper functions
		//=====================================================================

		static constexpr std::string_view UNKNOWN_VERSION = "[unknown version]";
		static constexpr std::string_view UNKNOWN_OLD_ASSIGNMENT = "[unknown oldAssignment]";
		static constexpr std::string_view UNKNOWN_SOURCE = "[unknown source]";

		std::string_view extractVisHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				if ( json.contains( GMODVERSIONING_DTO_KEY_VIS_RELEASE ) && json.at( GMODVERSIONING_DTO_KEY_VIS_RELEASE ).is_string() )
				{
					const auto& str = json.at( GMODVERSIONING_DTO_KEY_VIS_RELEASE ).get_ref<const std::string&>();
					return std::string_view{ str };
				}
				return UNKNOWN_VERSION;
			}
			catch ( ... )
			{
				return UNKNOWN_VERSION;
			}
		}

		std::string_view extractOldAssignmentHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				if ( json.contains( GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT ) && json.at( GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT ).is_string() )
				{
					const auto& str = json.at( GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT ).get_ref<const std::string&>();
					return std::string_view{ str };
				}
				return UNKNOWN_OLD_ASSIGNMENT;
			}
			catch ( ... )
			{
				return UNKNOWN_OLD_ASSIGNMENT;
			}
		}

		std::string_view extractSourceHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				if ( json.contains( GMODVERSIONING_DTO_KEY_SOURCE ) && json.at( GMODVERSIONING_DTO_KEY_SOURCE ).is_string() )
				{
					const auto& str = json.at( GMODVERSIONING_DTO_KEY_SOURCE ).get_ref<const std::string&>();
					return std::string_view{ str };
				}
				return UNKNOWN_SOURCE;
			}
			catch ( ... )
			{
				return UNKNOWN_SOURCE;
			}
		}
	}

	//=====================================================================
	// GMOD versioning assignement data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<GmodVersioningAssignmentChangeDto> GmodVersioningAssignmentChangeDto::tryFromJson( const nlohmann::json& json )
	{
		[[maybe_unused]] const auto oldAssignmentHint = extractOldAssignmentHint( json );

		try
		{
			if ( !json.is_object() )
			{
				SPDLOG_ERROR( "JSON value for GmodVersioningAssignmentChangeDto is not an object" );
				return std::nullopt;
			}

			GmodVersioningAssignmentChangeDto dto = json.get<GmodVersioningAssignmentChangeDto>();
			return std::optional<GmodVersioningAssignmentChangeDto>{ std::move( dto ) };
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			SPDLOG_ERROR( "JSON exception during GmodVersioningAssignmentChangeDto parsing (hint: oldAssignment='{}'): {}",
				oldAssignmentHint, ex.what() );
			return std::nullopt;
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			SPDLOG_ERROR( "Standard exception during GmodVersioningAssignmentChangeDto parsing (hint: oldAssignment='{}'): {}",
				oldAssignmentHint, ex.what() );
			return std::nullopt;
		}
	}

	GmodVersioningAssignmentChangeDto GmodVersioningAssignmentChangeDto::fromJson( const nlohmann::json& json )
	{
		const auto oldAssignmentHint = extractOldAssignmentHint( json );
		auto dtoOpt = GmodVersioningAssignmentChangeDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			throw std::invalid_argument( fmt::format( "Failed to deserialize GmodVersioningAssignmentChangeDto from JSON (hint: oldAssignment='{}')",
				oldAssignmentHint ) );
		}
		return dtoOpt.value();
	}

	nlohmann::json GmodVersioningAssignmentChangeDto::toJson() const
	{
		nlohmann::json result;
		to_json( result, *this );
		return result;
	}

	//----------------------------------------------
	// Private serialization methods
	//----------------------------------------------

	void from_json( const nlohmann::json& j, GmodVersioningAssignmentChangeDto& dto )
	{
		if ( !j.contains( GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT ) || !j.at( GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT ).is_string() )
		{
			throw nlohmann::json::parse_error::create( 101, 0u, fmt::format( "GmodVersioningAssignmentChangeDto JSON missing required '{}' field or not a string", GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT ), nullptr );
		}
		if ( !j.contains( GMODVERSIONING_DTO_KEY_CURRENT_ASSIGNMENT ) || !j.at( GMODVERSIONING_DTO_KEY_CURRENT_ASSIGNMENT ).is_string() )
		{
			throw nlohmann::json::parse_error::create( 101, 0u, fmt::format( "GmodVersioningAssignmentChangeDto JSON missing required '{}' field or not a string", GMODVERSIONING_DTO_KEY_CURRENT_ASSIGNMENT ), nullptr );
		}

		dto.m_oldAssignment = j.at( GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT ).get<std::string>();
		dto.m_currentAssignment = j.at( GMODVERSIONING_DTO_KEY_CURRENT_ASSIGNMENT ).get<std::string>();

		if ( dto.m_oldAssignment.empty() )
		{
			SPDLOG_WARN( "Empty 'oldAssignment' field found in GmodVersioningAssignmentChangeDto" );
		}
		if ( dto.m_currentAssignment.empty() )
		{
			SPDLOG_WARN( "Empty 'currentAssignment' field found in GmodVersioningAssignmentChangeDto" );
		}
	}

	void to_json( nlohmann::json& j, const GmodVersioningAssignmentChangeDto& dto )
	{
		j = nlohmann::json{
			{ GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT, dto.m_oldAssignment },
			{ GMODVERSIONING_DTO_KEY_CURRENT_ASSIGNMENT, dto.m_currentAssignment } };
	}

	//=====================================================================
	// GMOD Node Conversion Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<GmodNodeConversionDto> GmodNodeConversionDto::tryFromJson( const nlohmann::json& json )
	{
		[[maybe_unused]] const auto sourceHint = extractSourceHint( json );

		try
		{
			if ( !json.is_object() )
			{
				SPDLOG_ERROR( "JSON value for GmodNodeConversionDto is not an object" );
				return std::nullopt;
			}

			GmodNodeConversionDto dto = json.get<GmodNodeConversionDto>();
			return std::optional<GmodNodeConversionDto>{ std::move( dto ) };
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			SPDLOG_ERROR( "JSON exception during GmodNodeConversionDto parsing (hint: source='{}'): {}",
				sourceHint, ex.what() );
			return std::nullopt;
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			SPDLOG_ERROR( "Standard exception during GmodNodeConversionDto parsing (hint: source='{}'): {}",
				sourceHint, ex.what() );
			return std::nullopt;
		}
	}

	GmodNodeConversionDto GmodNodeConversionDto::fromJson( const nlohmann::json& json )
	{
		const auto sourceHint = extractSourceHint( json );
		auto dtoOpt = GmodNodeConversionDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			throw std::invalid_argument( fmt::format( "Failed to deserialize GmodNodeConversionDto from JSON (hint: source='{}')",
				sourceHint ) );
		}
		return dtoOpt.value();
	}

	nlohmann::json GmodNodeConversionDto::toJson() const
	{
		nlohmann::json result;
		to_json( result, *this );
		return result;
	}

	//----------------------------------------------
	// Private serialization methods
	//----------------------------------------------

	void from_json( const nlohmann::json& j, GmodNodeConversionDto& dto )
	{
		if ( j.contains( GMODVERSIONING_DTO_KEY_OPERATIONS ) )
		{
			if ( !j.at( GMODVERSIONING_DTO_KEY_OPERATIONS ).is_array() )
			{
				throw nlohmann::json::type_error::create( 302, fmt::format( "GmodNodeConversionDto JSON field '{}' is not an array", GMODVERSIONING_DTO_KEY_OPERATIONS ), nullptr );
			}
			dto.m_operations.clear();
			const auto& opsArray = j.at( GMODVERSIONING_DTO_KEY_OPERATIONS );

			dto.m_operations.reserve( opsArray.size() + opsArray.size() / 8 );
			for ( const auto& op : opsArray )
			{
				if ( !op.is_string() )
				{
					throw nlohmann::json::type_error::create( 302, fmt::format( "GmodNodeConversionDto JSON field '{}' contains non-string element", GMODVERSIONING_DTO_KEY_OPERATIONS ), nullptr );
				}
				dto.m_operations.insert( op.get<std::string>() );
			}
		}
		else
		{
			dto.m_operations.clear();
			SPDLOG_WARN( "GmodNodeConversionDto JSON missing optional '{}' field", GMODVERSIONING_DTO_KEY_OPERATIONS );
		}

		if ( j.contains( GMODVERSIONING_DTO_KEY_SOURCE ) )
		{
			if ( !j.at( GMODVERSIONING_DTO_KEY_SOURCE ).is_string() )
			{
				throw nlohmann::json::type_error::create( 302, fmt::format( "GmodNodeConversionDto JSON field '{}' is not a string", GMODVERSIONING_DTO_KEY_SOURCE ), nullptr );
			}
			dto.m_source = j.at( GMODVERSIONING_DTO_KEY_SOURCE ).get<std::string>();
		}
		else
		{
			dto.m_source.clear();
			SPDLOG_WARN( "GmodNodeConversionDto JSON missing optional '{}' field", GMODVERSIONING_DTO_KEY_SOURCE );
		}

		if ( j.contains( GMODVERSIONING_DTO_KEY_TARGET ) )
		{
			if ( !j.at( GMODVERSIONING_DTO_KEY_TARGET ).is_string() )
			{
				throw nlohmann::json::type_error::create( 302, fmt::format( "GmodNodeConversionDto JSON field '{}' is not a string", GMODVERSIONING_DTO_KEY_TARGET ), nullptr );
			}
			dto.m_target = j.at( GMODVERSIONING_DTO_KEY_TARGET ).get<std::string>();
		}
		else
		{
			dto.m_target.clear();
			SPDLOG_WARN( "GmodNodeConversionDto JSON missing optional '{}' field", GMODVERSIONING_DTO_KEY_TARGET );
		}

		if ( j.contains( GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT ) )
		{
			if ( !j.at( GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT ).is_string() )
			{
				throw nlohmann::json::type_error::create( 302, fmt::format( "GmodNodeConversionDto JSON field '{}' is not a string", GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT ), nullptr );
			}
			dto.m_oldAssignment = j.at( GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT ).get<std::string>();
		}
		else
		{
			dto.m_oldAssignment.clear();
			SPDLOG_DEBUG( "GmodNodeConversionDto JSON missing optional '{}' field", GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT );
		}

		if ( j.contains( GMODVERSIONING_DTO_KEY_NEW_ASSIGNMENT ) )
		{
			if ( !j.at( GMODVERSIONING_DTO_KEY_NEW_ASSIGNMENT ).is_string() )
			{
				throw nlohmann::json::type_error::create( 302, fmt::format( "GmodNodeConversionDto JSON field '{}' is not a string", GMODVERSIONING_DTO_KEY_NEW_ASSIGNMENT ), nullptr );
			}
			dto.m_newAssignment = j.at( GMODVERSIONING_DTO_KEY_NEW_ASSIGNMENT ).get<std::string>();
		}
		else
		{
			dto.m_newAssignment.clear();
			SPDLOG_DEBUG( "GmodNodeConversionDto JSON missing optional '{}' field", GMODVERSIONING_DTO_KEY_NEW_ASSIGNMENT );
		}

		if ( j.contains( GMODVERSIONING_DTO_KEY_DELETE_ASSIGNMENT ) )
		{
			if ( !j.at( GMODVERSIONING_DTO_KEY_DELETE_ASSIGNMENT ).is_boolean() )
			{
				throw nlohmann::json::type_error::create( 302, fmt::format( "GmodNodeConversionDto JSON field '{}' is not a boolean", GMODVERSIONING_DTO_KEY_DELETE_ASSIGNMENT ), nullptr );
			}
			dto.m_deleteAssignment = j.at( GMODVERSIONING_DTO_KEY_DELETE_ASSIGNMENT ).get<bool>();
		}
		else
		{
			dto.m_deleteAssignment = false;
			SPDLOG_DEBUG( "GmodNodeConversionDto JSON missing optional '{}' field, defaulting to false", GMODVERSIONING_DTO_KEY_DELETE_ASSIGNMENT );
		}

		if ( dto.m_operations.empty() )
		{
			SPDLOG_WARN( "Node conversion has no operations: source={}, target={}", dto.m_source, dto.m_target );
		}
		if ( dto.m_source.empty() && dto.m_target.empty() )
		{
			SPDLOG_WARN( "Node conversion has empty source and target" );
		}
	}

	void to_json( nlohmann::json& j, const GmodNodeConversionDto& dto )
	{
		j = nlohmann::json{
			{ GMODVERSIONING_DTO_KEY_OPERATIONS, dto.m_operations },
			{ GMODVERSIONING_DTO_KEY_SOURCE, dto.m_source },
			{ GMODVERSIONING_DTO_KEY_TARGET, dto.m_target },
			{ GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT, dto.m_oldAssignment },
			{ GMODVERSIONING_DTO_KEY_NEW_ASSIGNMENT, dto.m_newAssignment },
			{ GMODVERSIONING_DTO_KEY_DELETE_ASSIGNMENT, dto.m_deleteAssignment } };
	}

	//=====================================================================
	// GMOD Versioning Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<GmodVersioningDto> GmodVersioningDto::tryFromJson( const nlohmann::json& json )
	{
		[[maybe_unused]] const auto visHint = extractVisHint( json );

		try
		{
			if ( !json.is_object() )
			{
				SPDLOG_ERROR( "JSON value for GmodVersioningDto is not an object" );
				return std::nullopt;
			}

			GmodVersioningDto dto = json.get<GmodVersioningDto>();
			return std::optional<GmodVersioningDto>{ std::move( dto ) };
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			SPDLOG_ERROR( "JSON exception during GmodVersioningDto parsing (hint: visRelease='{}'): {}",
				visHint, ex.what() );
			return std::nullopt;
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			SPDLOG_ERROR( "Standard exception during GmodVersioningDto parsing (hint: visRelease='{}'): {}",
				visHint, ex.what() );
			return std::nullopt;
		}
	}

	GmodVersioningDto GmodVersioningDto::fromJson( const nlohmann::json& json )
	{
		const auto visHint = extractVisHint( json );
		auto dtoOpt = GmodVersioningDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			throw std::invalid_argument( fmt::format( "Failed to deserialize GmodVersioningDto from JSON (hint: visRelease='{}')",
				visHint ) );
		}
		return std::move( dtoOpt ).value();
	}

	nlohmann::json GmodVersioningDto::toJson() const
	{
		nlohmann::json result;
		to_json( result, *this );
		return result;
	}

	//----------------------------------------------
	// Private serialization methods
	//----------------------------------------------

	void from_json( const nlohmann::json& j, GmodVersioningDto& dto )
	{
		if ( !j.contains( GMODVERSIONING_DTO_KEY_VIS_RELEASE ) || !j.at( GMODVERSIONING_DTO_KEY_VIS_RELEASE ).is_string() )
		{
			throw nlohmann::json::parse_error::create( 101, 0u, fmt::format( "GmodVersioningDto JSON missing required '{}' field or not a string", GMODVERSIONING_DTO_KEY_VIS_RELEASE ), nullptr );
		}
		dto.m_visVersion = j.at( GMODVERSIONING_DTO_KEY_VIS_RELEASE ).get<std::string>();

		dto.m_items.clear();
		if ( j.contains( GMODVERSIONING_DTO_KEY_ITEMS ) )
		{
			if ( !j.at( GMODVERSIONING_DTO_KEY_ITEMS ).is_object() )
			{
				throw nlohmann::json::type_error::create( 302, fmt::format( "GmodVersioningDto JSON field '{}' is not an object", GMODVERSIONING_DTO_KEY_ITEMS ), nullptr );
			}

			const auto& itemsObj = j.at( GMODVERSIONING_DTO_KEY_ITEMS );
			size_t itemCount = itemsObj.size();

			if ( itemCount > 10000 )
			{
				[[maybe_unused]] const size_t approxMemoryUsage = ( itemCount * sizeof( GmodNodeConversionDto ) ) / ( 1024 * 1024 );
				SPDLOG_DEBUG( "Large GMOD versioning model loaded: ~{} MB estimated memory usage", approxMemoryUsage );
			}

			size_t successCount = 0;

			dto.m_items.reserve( itemCount + itemCount / 8 );

			for ( const auto& [key, value] : itemsObj.items() )
			{
				try
				{
					GmodNodeConversionDto nodeDto = value.get<GmodNodeConversionDto>();

					dto.m_items.emplace( key, std::move( nodeDto ) );
					successCount++;
				}
				catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
				{
					SPDLOG_ERROR( "Error parsing conversion item '{}': {}", key, ex.what() );
				}
				catch ( [[maybe_unused]] const std::exception& ex )
				{
					SPDLOG_ERROR( "Standard exception parsing conversion item '{}': {}", key, ex.what() );
				}
			}

			SPDLOG_DEBUG( "Successfully parsed {}/{} GMOD versioning nodes", successCount, itemCount );

			/* If parsing failed for more than 10% of items, rebuild the map to potentially save memory */
			if ( itemCount > 0 && successCount < itemCount * 9 / 10 )
			{
				if ( dto.m_items.bucket_count() > dto.m_items.size() * 4 / 3 )
				{
					GmodVersioningDto::ItemsMap optimizedMap;
					optimizedMap.reserve( dto.m_items.size() );
					optimizedMap = std::move( dto.m_items );
					dto.m_items = std::move( optimizedMap );
				}
			}
		}
		else
		{
			SPDLOG_WARN( "No '{}' object found in GMOD versioning data for VIS version {}",
				GMODVERSIONING_DTO_KEY_ITEMS, dto.m_visVersion );
		}
	}

	void to_json( nlohmann::json& j, const GmodVersioningDto& dto )
	{
		j = nlohmann::json{ { GMODVERSIONING_DTO_KEY_VIS_RELEASE, dto.m_visVersion } };

		if ( !dto.m_items.empty() )
		{
			j[GMODVERSIONING_DTO_KEY_ITEMS] = dto.m_items;

			size_t emptyOperationsCount = 0;
			for ( const auto& [key, value] : dto.m_items )
			{
				if ( value.operations().empty() )
				{
					emptyOperationsCount++;
				}
			}
			if ( emptyOperationsCount > 0 )
			{
				SPDLOG_WARN( "{} nodes have no operations defined during serialization", emptyOperationsCount );
			}
		}
		else
		{
			j[GMODVERSIONING_DTO_KEY_ITEMS] = nlohmann::json::object();
		}
	}
}
