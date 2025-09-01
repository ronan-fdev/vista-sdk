/**
 * @file LocationsDto.cpp
 * @brief Implementation of data transfer objects for locations in the VIS standard
 */

#include <nfx/string/StringBuilderPool.h>
#include <fmt/format.h>

#include "dnv/vista/sdk/LocationsDto.h"

#include "dnv/vista/sdk/constants/DtoKeysConstants.h"

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// JSON parsing helper functions
		//=====================================================================

		std::string_view extractCodeHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				const auto it = json.find( constants::dto::LOCATIONS_DTO_KEY_CODE );
				if ( it != json.end() && it->is_string() )
				{
					const auto& str = it->get_ref<const std::string&>();

					return std::string_view{ str };
				}

				return constants::dto::LOCATIONS_DTO_UNKNOWN_CODE;
			}
			catch ( ... )
			{
				return constants::dto::LOCATIONS_DTO_UNKNOWN_CODE;
			}
		}

		std::string_view extractVisHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				const auto it = json.find( constants::dto::LOCATIONS_DTO_KEY_VIS_RELEASE );
				if ( it != json.end() && it->is_string() )
				{
					const auto& str = it->get_ref<const std::string&>();

					return std::string_view{ str };
				}

				return constants::dto::LOCATIONS_DTO_UNKNOWN_VERSION;
			}
			catch ( ... )
			{
				return constants::dto::LOCATIONS_DTO_UNKNOWN_VERSION;
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
		const auto codeHint = extractCodeHint( json );

		try
		{
			if ( !json.is_object() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "ERROR: JSON value for RelativeLocationsDto is not an object\n" );

				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}

			/* Cache iterators to avoid multiple lookups */
			const auto codeIt = json.find( constants::dto::LOCATIONS_DTO_KEY_CODE );
			const auto nameIt = json.find( constants::dto::LOCATIONS_DTO_KEY_NAME );
			const auto defIt = json.find( constants::dto::LOCATIONS_DTO_KEY_DEFINITION );

			if ( codeIt == json.end() || !codeIt->is_string() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "ERROR: RelativeLocationsDto JSON missing required '" );
				builder.append( constants::dto::LOCATIONS_DTO_KEY_CODE );
				builder.append( "' field or not a string" );
				fmt::print( stderr, "{}\n", lease.toString() );

				return std::nullopt;
			}
			if ( nameIt == json.end() || !nameIt->is_string() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "ERROR: RelativeLocationsDto JSON missing required '" );
				builder.append( constants::dto::LOCATIONS_DTO_KEY_NAME );
				builder.append( "' field or not a string" );
				fmt::print( stderr, "{}\n", lease.toString() );

				return std::nullopt;
			}

			std::string codeStr = codeIt->get<std::string>();
			if ( codeStr.empty() || codeStr.length() != 1 )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "ERROR: RelativeLocationsDto (hint: code='" );
				builder.append( codeHint );
				builder.append( "') has invalid code format\n" );

				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}

			std::string tempName = nameIt->get<std::string>();
			if ( tempName.empty() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "WARN: Empty name field found in RelativeLocationsDto code='" );
				builder.append( codeStr );
				builder.append( "'\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}

			char tempCode = codeStr[0];
			std::optional<std::string> tempDefinition = std::nullopt;

			if ( defIt != json.end() )
			{
				if ( defIt->is_string() )
				{
					tempDefinition = defIt->get<std::string>();
				}
				else if ( !defIt->is_null() )
				{
					auto lease = nfx::string::StringBuilderPool::lease();
					auto builder = lease.builder();
					builder.append( "WARN: RelativeLocationsDto code='" );
					builder.append( codeStr );
					builder.append( "' has non-string definition field\n" );

					fmt::print( stderr, "{}", lease.toString() );
				}
			}

			RelativeLocationsDto result( std::move( tempCode ), std::move( tempName ), std::move( tempDefinition ) );

			return result;
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during RelativeLocationsDto parsing (hint: code='" );
			builder.append( codeHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
		catch ( const std::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "ERROR: Standard exception during RelativeLocationsDto parsing (hint: code='" );
			builder.append( codeHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

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
		j = nlohmann::json{ { constants::dto::LOCATIONS_DTO_KEY_CODE, std::string{ 1, dto.m_code } }, { constants::dto::LOCATIONS_DTO_KEY_NAME, dto.m_name } };

		if ( dto.m_definition.has_value() )
		{
			j[constants::dto::LOCATIONS_DTO_KEY_DEFINITION] = dto.m_definition.value();
		}
	}

	void from_json( const nlohmann::json& j, RelativeLocationsDto& dto )
	{
		const auto codeIt = j.find( constants::dto::LOCATIONS_DTO_KEY_CODE );
		const auto nameIt = j.find( constants::dto::LOCATIONS_DTO_KEY_NAME );
		const auto defIt = j.find( constants::dto::LOCATIONS_DTO_KEY_DEFINITION );

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
		dto.m_name = nameIt->get<std::string>();
		if ( dto.m_name.empty() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "WARN: Empty name field found in RelativeLocationsDto code='" );
			builder.append( std::string{ 1, dto.m_code } );
			builder.append( "'\n" );

			fmt::print( stderr, "{}", lease.toString() );
		}

		if ( defIt != j.end() && defIt->is_string() )
		{
			dto.m_definition = defIt->get<std::string>();

			if ( dto.m_definition.has_value() && dto.m_definition->empty() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "WARN: Empty definition field found in RelativeLocationsDto code='" );
				builder.append( std::string{ 1, dto.m_code } );
				builder.append( "'\n" );

				fmt::print( stderr, "{}", lease.toString() );
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
		const auto visHint = extractVisHint( json );

		try
		{
			if ( !json.is_object() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "ERROR: JSON value for LocationsDto is not an object\n" );

				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}

			/* Cache iterators to avoid multiple lookups */
			const auto visIt = json.find( constants::dto::LOCATIONS_DTO_KEY_VIS_RELEASE );
			const auto itemsIt = json.find( constants::dto::LOCATIONS_DTO_KEY_ITEMS );

			if ( visIt == json.end() || !visIt->is_string() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "ERROR: LocationsDto JSON missing required '" );
				builder.append( constants::dto::LOCATIONS_DTO_KEY_VIS_RELEASE );
				builder.append( "' field or not a string" );

				fmt::print( stderr, "{}\n", lease.toString() );

				return std::nullopt;
			}
			if ( itemsIt == json.end() || !itemsIt->is_array() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "ERROR: LocationsDto JSON missing required '" );
				builder.append( constants::dto::LOCATIONS_DTO_KEY_ITEMS );
				builder.append( "' array" );

				fmt::print( stderr, "{}\n", lease.toString() );

				return std::nullopt;
			}

			LocationsDto dto = json.get<LocationsDto>();

			return std::optional<LocationsDto>{ std::move( dto ) };
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during LocationsDto parsing (hint: visRelease='" );
			builder.append( visHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
		catch ( const std::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "ERROR: Standard exception during LocationsDto parsing (hint: visRelease='" );
			builder.append( visHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

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
			{ constants::dto::LOCATIONS_DTO_KEY_VIS_RELEASE, dto.m_visVersion },
			{ constants::dto::LOCATIONS_DTO_KEY_ITEMS, dto.m_items } };
	}

	void from_json( const nlohmann::json& j, LocationsDto& dto )
	{
		dto.m_items.clear();

		const auto visIt = j.find( constants::dto::LOCATIONS_DTO_KEY_VIS_RELEASE );
		const auto itemsIt = j.find( constants::dto::LOCATIONS_DTO_KEY_ITEMS );

		if ( visIt == j.end() || !visIt->is_string() )
		{
			throw nlohmann::json::parse_error::create(
				101, 0u,
				fmt::format(
					"LocationsDto JSON missing required '{}' field",
					constants::dto::LOCATIONS_DTO_KEY_VIS_RELEASE ),
				nullptr );
		}
		if ( itemsIt == j.end() || !itemsIt->is_array() )
		{
			throw nlohmann::json::parse_error::create(
				101, 0u,
				fmt::format(
					"LocationsDto JSON missing required '{}' field",
					constants::dto::LOCATIONS_DTO_KEY_ITEMS ),
				nullptr );
		}

		dto.m_visVersion = visIt->get<std::string>();

		if ( dto.m_visVersion.empty() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "WARN: Empty visVersion field found in LocationsDto\n" );

			fmt::print( stderr, "{}", lease.toString() );
		}

		const auto& jsonArray = *itemsIt;
		const size_t totalItems = jsonArray.size();
		size_t successCount = 0;

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
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "WARN: Skipping invalid RelativeLocationsDto item during parsing\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}
		}

		if ( totalItems > 0 && successCount < totalItems * 9 / 10 )
		{
			if ( dto.m_items.capacity() > dto.m_items.size() * 4 / 3 )
			{
				dto.m_items.shrink_to_fit();
			}
		}
	}
}
