/**
 * @file LocationsDto.cpp
 * @brief Implementation of data transfer objects for locations in the VIS standard
 */

#include "pch.h"

#include "dnv/vista/sdk/LocationsDto.h"

#include "dnv/vista/sdk/Config.h"

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// JSON parsing helper functions
		//=====================================================================

		static constexpr std::string_view UNKNOWN_CODE = "[unknown code]";
		static constexpr std::string_view UNKNOWN_VERSION = "[unknown version]";

		std::string_view extractCodeHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				if ( json.contains( LOCATIONS_DTO_KEY_CODE ) && json.at( LOCATIONS_DTO_KEY_CODE ).is_string() )
				{
					const auto& str = json.at( LOCATIONS_DTO_KEY_CODE ).get_ref<const std::string&>();
					return std::string_view{ str };
				}
				return UNKNOWN_CODE;
			}
			catch ( ... )
			{
				return UNKNOWN_CODE;
			}
		}

		std::string_view extractVisHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				if ( json.contains( LOCATIONS_DTO_KEY_VIS_RELEASE ) && json.at( LOCATIONS_DTO_KEY_VIS_RELEASE ).is_string() )
				{
					const auto& str = json.at( LOCATIONS_DTO_KEY_VIS_RELEASE ).get_ref<const std::string&>();
					return std::string_view{ str };
				}
				return UNKNOWN_VERSION;
			}
			catch ( ... )
			{
				return UNKNOWN_VERSION;
			}
		}
	}

	//=====================================================================
	// Relative Location data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<RelativeLocationsDto> RelativeLocationsDto::tryFromJson( const nlohmann::json& json )
	{
		[[maybe_unused]] const auto codeHint = extractCodeHint( json );

		try
		{
			if ( !json.is_object() )
			{
				SPDLOG_ERROR( "JSON value for RelativeLocationsDto is not an object" );
				return std::nullopt;
			}

			if ( !json.contains( LOCATIONS_DTO_KEY_CODE ) || !json.at( LOCATIONS_DTO_KEY_CODE ).is_string() )
			{
				SPDLOG_ERROR( "RelativeLocationsDto JSON missing required '{}' field or not a string",
					LOCATIONS_DTO_KEY_CODE );
				return std::nullopt;
			}
			if ( !json.contains( LOCATIONS_DTO_KEY_NAME ) || !json.at( LOCATIONS_DTO_KEY_NAME ).is_string() )
			{
				SPDLOG_ERROR( "RelativeLocationsDto JSON missing required '{}' field or not a string",
					LOCATIONS_DTO_KEY_NAME );
				return std::nullopt;
			}

			std::string codeStr = json.at( LOCATIONS_DTO_KEY_CODE ).get<std::string>();
			if ( codeStr.empty() || codeStr.length() != 1 )
			{
				SPDLOG_ERROR( "RelativeLocationsDto (hint: code='{}') has invalid code format", codeHint );
				return std::nullopt;
			}

			std::string tempName = json.at( LOCATIONS_DTO_KEY_NAME ).get<std::string>();
			if ( tempName.empty() )
			{
				SPDLOG_WARN( "Empty name field found in RelativeLocationsDto code='{}'", codeStr );
			}

			char tempCode = codeStr[0];
			std::optional<std::string> tempDefinition = std::nullopt;

			if ( json.contains( LOCATIONS_DTO_KEY_DEFINITION ) )
			{
				if ( json.at( LOCATIONS_DTO_KEY_DEFINITION ).is_string() )
				{
					tempDefinition = json.at( LOCATIONS_DTO_KEY_DEFINITION ).get<std::string>();
				}
				else if ( !json.at( LOCATIONS_DTO_KEY_DEFINITION ).is_null() )
				{
					SPDLOG_WARN( "RelativeLocationsDto code='{}' has non-string definition field",
						codeStr );
				}
			}

			RelativeLocationsDto result(
				std::move( tempCode ),
				std::move( tempName ),
				std::move( tempDefinition ) );

			return result;
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			SPDLOG_ERROR( "JSON exception during RelativeLocationsDto parsing (hint: code='{}'): {}",
				codeHint, ex.what() );
			return std::nullopt;
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			SPDLOG_ERROR( "Standard exception during RelativeLocationsDto parsing (hint: code='{}'): {}",
				codeHint, ex.what() );
			return std::nullopt;
		}
	}

	RelativeLocationsDto RelativeLocationsDto::fromJson( const nlohmann::json& json )
	{
		auto dtoOpt = RelativeLocationsDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			throw std::invalid_argument( "Failed to deserialize RelativeLocationsDto from JSON" );
		}
		return std::move( dtoOpt ).value();
	}

	//----------------------------------------------
	// Private serialization methods
	//---------------------------------------------

	nlohmann::json RelativeLocationsDto::toJson() const
	{
		nlohmann::json j = *this;
		return j;
	}

	void to_json( nlohmann::json& j, const RelativeLocationsDto& dto )
	{
		j = nlohmann::json{
			{ LOCATIONS_DTO_KEY_CODE, std::string( 1, dto.m_code ) },
			{ LOCATIONS_DTO_KEY_NAME, dto.m_name } };
		if ( dto.m_definition.has_value() )
		{
			j[LOCATIONS_DTO_KEY_DEFINITION] = dto.m_definition.value();
		}
	}

	void from_json( const nlohmann::json& j, RelativeLocationsDto& dto )
	{
		const auto codeIt = j.find( LOCATIONS_DTO_KEY_CODE );
		const auto nameIt = j.find( LOCATIONS_DTO_KEY_NAME );
		const auto defIt = j.find( LOCATIONS_DTO_KEY_DEFINITION );

		if ( codeIt == j.end() || !codeIt->is_string() )
		{
			throw nlohmann::json::parse_error::create( 101, 0u, "Missing or invalid 'code' field", nullptr );
		}
		if ( nameIt == j.end() || !nameIt->is_string() )
		{
			throw nlohmann::json::parse_error::create( 101, 0u, "Missing or invalid 'name' field", nullptr );
		}

		std::string codeStr = codeIt->get<std::string>();
		if ( codeStr.empty() || codeStr.length() != 1 )
		{
			throw nlohmann::json::type_error::create( 302, "'code' field must be single character", nullptr );
		}

		dto.m_code = codeStr[0];
		dto.m_name = ( nameIt->get<std::string>() );
		if ( dto.m_name.empty() )
		{
			SPDLOG_WARN( "Empty name field found in RelativeLocationsDto code='{}'", dto.m_code );
		}

		if ( defIt != j.end() && defIt->is_string() )
		{
			dto.m_definition = ( defIt->get<std::string>() );

			if ( dto.m_definition.has_value() && dto.m_definition->empty() )
			{
				SPDLOG_WARN( "Empty definition field found in RelativeLocationsDto code='{}'", dto.m_code );
			}
		}
		else
		{
			dto.m_definition.reset();
		}
	}

	//=====================================================================
	// Location data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<LocationsDto> LocationsDto::tryFromJson( const nlohmann::json& json )
	{
		[[maybe_unused]] const auto visHint = extractVisHint( json );

		try
		{
			if ( !json.is_object() )
			{
				SPDLOG_ERROR( "JSON value for LocationsDto is not an object" );
				return std::nullopt;
			}

			if ( !json.contains( LOCATIONS_DTO_KEY_VIS_RELEASE ) || !json.at( LOCATIONS_DTO_KEY_VIS_RELEASE ).is_string() )
			{
				SPDLOG_ERROR( "LocationsDto JSON missing required '{}' field or not a string",
					LOCATIONS_DTO_KEY_VIS_RELEASE );
				return std::nullopt;
			}
			if ( !json.contains( LOCATIONS_DTO_KEY_ITEMS ) || !json.at( LOCATIONS_DTO_KEY_ITEMS ).is_array() )
			{
				SPDLOG_ERROR( "LocationsDto JSON missing required '{}' array", LOCATIONS_DTO_KEY_ITEMS );
				return std::nullopt;
			}

			LocationsDto dto = json.get<LocationsDto>();
			return std::optional<LocationsDto>{ std::move( dto ) };
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			SPDLOG_ERROR( "JSON exception during LocationsDto parsing (hint: visRelease='{}'): {}",
				visHint, ex.what() );
			return std::nullopt;
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			SPDLOG_ERROR( "Standard exception during LocationsDto parsing (hint: visRelease='{}'): {}",
				visHint, ex.what() );
			return std::nullopt;
		}
	}

	LocationsDto LocationsDto::fromJson( const nlohmann::json& json )
	{
		auto dtoOpt = LocationsDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			throw std::invalid_argument( "Failed to deserialize LocationsDto from JSON" );
		}
		return std::move( dtoOpt ).value();
	}

	nlohmann::json LocationsDto::toJson() const
	{
		return *this;
	}

	//----------------------------------------------
	// Private serialization methods
	//----------------------------------------------

	void to_json( nlohmann::json& j, const LocationsDto& dto )
	{
		j = nlohmann::json{
			{ LOCATIONS_DTO_KEY_VIS_RELEASE, dto.m_visVersion },
			{ LOCATIONS_DTO_KEY_ITEMS, dto.m_items } };
	}

	void from_json( const nlohmann::json& j, LocationsDto& dto )
	{
		dto.m_items.clear();

		const auto visIt = j.find( LOCATIONS_DTO_KEY_VIS_RELEASE );
		const auto itemsIt = j.find( LOCATIONS_DTO_KEY_ITEMS );

		if ( visIt == j.end() || !visIt->is_string() )
		{
			throw nlohmann::json::parse_error::create( 101, 0u,
				fmt::format( "LocationsDto JSON missing required '{}' field", LOCATIONS_DTO_KEY_VIS_RELEASE ), nullptr );
		}
		if ( itemsIt == j.end() || !itemsIt->is_array() )
		{
			throw nlohmann::json::parse_error::create( 101, 0u,
				fmt::format( "LocationsDto JSON missing required '{}' field", LOCATIONS_DTO_KEY_ITEMS ), nullptr );
		}

		dto.m_visVersion = visIt->get<std::string>();

		if ( dto.m_visVersion.empty() )
		{
			SPDLOG_WARN( "Empty visVersion field found in LocationsDto" );
		}

		const auto& jsonArray = *itemsIt;
		const size_t totalItems = jsonArray.size();
		size_t successCount = 0;

		if ( totalItems > 10000 )
		{
			[[maybe_unused]] const size_t approxMemoryUsage = ( totalItems * sizeof( RelativeLocationsDto ) ) / ( 1024 * 1024 );
			SPDLOG_DEBUG( "Large locations dataset loaded: ~{} MB estimated memory usage", approxMemoryUsage );
		}

		const size_t reserveSize = totalItems < 1000
									   ? totalItems + totalItems / 4
									   : totalItems + totalItems / 16;
		dto.m_items.reserve( reserveSize );

		for ( const auto& itemJson : jsonArray )
		{
			auto itemOpt = RelativeLocationsDto::tryFromJson( itemJson );
			if ( itemOpt.has_value() )
			{
				dto.m_items.emplace_back( std::move( *itemOpt ) );
				successCount++;
			}
			else
			{
				SPDLOG_WARN( "Skipping invalid RelativeLocationsDto item during parsing" );
			}
		}

		SPDLOG_DEBUG( "Successfully parsed {}/{} relative locations", successCount, totalItems );

		if ( totalItems > 0 && successCount < totalItems * 9 / 10 )
		{
			if ( dto.m_items.capacity() > dto.m_items.size() * 4 / 3 )
			{
				dto.m_items.shrink_to_fit();
			}
		}
	}
}
