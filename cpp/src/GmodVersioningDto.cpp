/**
 * @file GmodVersioningDto.cpp
 * @brief Implementation of GMOD versioning data transfer objects
 */

#include <cstdio>
#include <nfx/string/StringBuilderPool.h>

#include "dnv/vista/sdk/GmodVersioningDto.h"

#include "dnv/vista/sdk/constants/DtoKeysConstants.h"

namespace dnv::vista::sdk
{
	namespace GmodVersioningDtoInternal
	{
		//=====================================================================
		// JSON parsing helper functions
		//=====================================================================

		inline std::string_view extractVisHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				const auto it = json.find( constants::dto::GMODVERSIONING_DTO_KEY_VIS_RELEASE );
				if ( it != json.end() && it->is_string() )
				{
					const auto& str = it->get_ref<const std::string&>();

					return std::string_view{ str };
				}

				return constants::dto::GMODVERSIONING_DTO_UNKNOWN_VERSION;
			}
			catch ( ... )
			{
				return constants::dto::GMODVERSIONING_DTO_UNKNOWN_VERSION;
			}
		}

		inline std::string_view extractOldAssignmentHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				const auto it = json.find( constants::dto::GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT );
				if ( it != json.end() && it->is_string() )
				{
					const auto& str = it->get_ref<const std::string&>();

					return std::string_view{ str };
				}

				return constants::dto::GMODVERSIONING_DTO_UNKNOWN_OLD_ASSIGNMENT;
			}
			catch ( ... )
			{
				return constants::dto::GMODVERSIONING_DTO_UNKNOWN_OLD_ASSIGNMENT;
			}
		}

		inline std::string_view extractSourceHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				const auto it = json.find( constants::dto::GMODVERSIONING_DTO_KEY_SOURCE );
				if ( it != json.end() && it->is_string() )
				{
					const auto& str = it->get_ref<const std::string&>();

					return std::string_view{ str };
				}

				return constants::dto::GMODVERSIONING_DTO_UNKNOWN_SOURCE;
			}
			catch ( ... )
			{
				return constants::dto::GMODVERSIONING_DTO_UNKNOWN_SOURCE;
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
		const auto oldAssignmentHint = GmodVersioningDtoInternal::extractOldAssignmentHint( json );

		try
		{
			if ( !json.is_object() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				lease.builder().append( "ERROR: JSON value for GmodVersioningAssignmentChangeDto is not an object\n" );

				std::fprintf( stderr, "%s", lease.toString().c_str() );

				return std::nullopt;
			}

			GmodVersioningAssignmentChangeDto dto = json.get<GmodVersioningAssignmentChangeDto>();

			return std::optional<GmodVersioningAssignmentChangeDto>{ std::move( dto ) };
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during GmodVersioningAssignmentChangeDto parsing (hint: oldAssignment='" );
			builder.append( oldAssignmentHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "'\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );

			return std::nullopt;
		}
		catch ( const std::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "ERROR: Standard exception during GmodVersioningAssignmentChangeDto parsing (hint: oldAssignment='" );
			builder.append( oldAssignmentHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "'\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );

			return std::nullopt;
		}
	}

	GmodVersioningAssignmentChangeDto GmodVersioningAssignmentChangeDto::fromJson( const nlohmann::json& json )
	{
		const auto oldAssignmentHint = GmodVersioningDtoInternal::extractOldAssignmentHint( json );
		auto dtoOpt = GmodVersioningAssignmentChangeDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Failed to deserialize GmodVersioningAssignmentChangeDto from JSON (hint: oldAssignment='" );
			builder.append( oldAssignmentHint );
			builder.append( "')" );
			throw std::invalid_argument{ lease.toString() };
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
		const auto oldAssignmentIt = j.find( constants::dto::GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT );
		if ( oldAssignmentIt == j.end() || !oldAssignmentIt->is_string() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "GmodVersioningAssignmentChangeDto JSON missing required '" );
			builder.append( constants::dto::GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT );
			builder.append( "' field or not a string" );
			throw nlohmann::json::parse_error::create( 101, 0u, lease.toString(), nullptr );
		}

		const auto currentAssignmentIt = j.find( constants::dto::GMODVERSIONING_DTO_KEY_CURRENT_ASSIGNMENT );
		if ( currentAssignmentIt == j.end() || !currentAssignmentIt->is_string() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "GmodVersioningAssignmentChangeDto JSON missing required '" );
			builder.append( constants::dto::GMODVERSIONING_DTO_KEY_CURRENT_ASSIGNMENT );
			builder.append( "' field or not a string" );
			throw nlohmann::json::parse_error::create( 101, 0u, lease.toString(), nullptr );
		}

		dto.m_oldAssignment = oldAssignmentIt->get<std::string>();
		dto.m_currentAssignment = currentAssignmentIt->get<std::string>();

		if ( dto.m_oldAssignment.empty() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			lease.builder().append( "WARN: Empty 'oldAssignment' field found in GmodVersioningAssignmentChangeDto\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );
		}
		if ( dto.m_currentAssignment.empty() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			lease.builder().append( "WARN: Empty 'currentAssignment' field found in GmodVersioningAssignmentChangeDto\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );
		}
	}

	void to_json( nlohmann::json& j, const GmodVersioningAssignmentChangeDto& dto )
	{
		j = nlohmann::json{
			{ constants::dto::GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT, dto.m_oldAssignment },
			{ constants::dto::GMODVERSIONING_DTO_KEY_CURRENT_ASSIGNMENT, dto.m_currentAssignment } };
	}

	//=====================================================================
	// GMOD Node Conversion Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<GmodNodeConversionDto> GmodNodeConversionDto::tryFromJson( const nlohmann::json& json )
	{
		const auto sourceHint = GmodVersioningDtoInternal::extractSourceHint( json );

		try
		{
			if ( !json.is_object() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				lease.builder().append( "ERROR: JSON value for GmodNodeConversionDto is not an object\n" );

				std::fprintf( stderr, "%s", lease.toString().c_str() );

				return std::nullopt;
			}

			GmodNodeConversionDto dto = json.get<GmodNodeConversionDto>();
			return std::optional<GmodNodeConversionDto>{ std::move( dto ) };
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during GmodNodeConversionDto parsing (hint: source='" );
			builder.append( sourceHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "'\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );

			return std::nullopt;
		}
		catch ( const std::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "ERROR: Standard exception during GmodNodeConversionDto parsing (hint: source='" );
			builder.append( sourceHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "'\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );

			return std::nullopt;
		}
	}

	GmodNodeConversionDto GmodNodeConversionDto::fromJson( const nlohmann::json& json )
	{
		const auto sourceHint = GmodVersioningDtoInternal::extractSourceHint( json );
		auto dtoOpt = GmodNodeConversionDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Failed to deserialize GmodNodeConversionDto from JSON (hint: source='" );
			builder.append( sourceHint );
			builder.append( "')" );
			throw std::invalid_argument{ lease.toString() };
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
		const auto opsIt = j.find( constants::dto::GMODVERSIONING_DTO_KEY_OPERATIONS );
		if ( opsIt != j.end() )
		{
			if ( !opsIt->is_array() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "GmodNodeConversionDto JSON field '" );
				builder.append( constants::dto::GMODVERSIONING_DTO_KEY_OPERATIONS );
				builder.append( "' is not an array" );
				throw nlohmann::json::type_error::create( 302, lease.toString(), nullptr );
			}
			dto.m_operations.clear();
			const auto& opsArray = *opsIt;

			dto.m_operations.reserve( opsArray.size() + opsArray.size() / 8 );
			for ( const auto& op : opsArray )
			{
				if ( !op.is_string() )
				{
					auto lease = nfx::string::StringBuilderPool::lease();
					auto builder = lease.builder();
					builder.append( "GmodNodeConversionDto JSON field '" );
					builder.append( constants::dto::GMODVERSIONING_DTO_KEY_OPERATIONS );
					builder.append( "' contains non-string element" );
					throw nlohmann::json::type_error::create( 302, lease.toString(), nullptr );
				}
				dto.m_operations.insert( op.get<std::string>() );
			}
		}
		else
		{
			dto.m_operations.clear();
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "WARN: GmodNodeConversionDto JSON missing optional '" );
			builder.append( constants::dto::GMODVERSIONING_DTO_KEY_OPERATIONS );
			builder.append( "' field\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );
		}

		const auto sourceIt = j.find( constants::dto::GMODVERSIONING_DTO_KEY_SOURCE );
		if ( sourceIt != j.end() )
		{
			if ( !sourceIt->is_string() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "GmodNodeConversionDto JSON field '" );
				builder.append( constants::dto::GMODVERSIONING_DTO_KEY_SOURCE );
				builder.append( "' is not a string" );
				throw nlohmann::json::type_error::create( 302, lease.toString(), nullptr );
			}
			dto.m_source = sourceIt->get<std::string>();
		}
		else
		{
			dto.m_source.clear();
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "WARN: GmodNodeConversionDto JSON missing optional '" );
			builder.append( constants::dto::GMODVERSIONING_DTO_KEY_SOURCE );
			builder.append( "' field\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );
		}

		const auto targetIt = j.find( constants::dto::GMODVERSIONING_DTO_KEY_TARGET );
		if ( targetIt != j.end() )
		{
			if ( !targetIt->is_string() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "GmodNodeConversionDto JSON field '" );
				builder.append( constants::dto::GMODVERSIONING_DTO_KEY_TARGET );
				builder.append( "' is not a string" );
				throw nlohmann::json::type_error::create( 302, lease.toString(), nullptr );
			}
			dto.m_target = targetIt->get<std::string>();
		}
		else
		{
			dto.m_target.clear();
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "WARN: GmodNodeConversionDto JSON missing optional '" );
			builder.append( constants::dto::GMODVERSIONING_DTO_KEY_TARGET );
			builder.append( "' field\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );
		}

		const auto oldAssignmentIt = j.find( constants::dto::GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT );
		if ( oldAssignmentIt != j.end() )
		{
			if ( !oldAssignmentIt->is_string() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "GmodNodeConversionDto JSON field '" );
				builder.append( constants::dto::GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT );
				builder.append( "' is not a string" );
				throw nlohmann::json::type_error::create( 302, lease.toString(), nullptr );
			}
			dto.m_oldAssignment = oldAssignmentIt->get<std::string>();
		}
		else
		{
			dto.m_oldAssignment.clear();
		}

		const auto newAssignmentIt = j.find( constants::dto::GMODVERSIONING_DTO_KEY_NEW_ASSIGNMENT );
		if ( newAssignmentIt != j.end() )
		{
			if ( !newAssignmentIt->is_string() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "GmodNodeConversionDto JSON field '" );
				builder.append( constants::dto::GMODVERSIONING_DTO_KEY_NEW_ASSIGNMENT );
				builder.append( "' is not a string" );
				throw nlohmann::json::type_error::create( 302, lease.toString(), nullptr );
			}
			dto.m_newAssignment = newAssignmentIt->get<std::string>();
		}
		else
		{
			dto.m_newAssignment.clear();
		}

		const auto deleteAssignmentIt = j.find( constants::dto::GMODVERSIONING_DTO_KEY_DELETE_ASSIGNMENT );
		if ( deleteAssignmentIt != j.end() )
		{
			if ( !deleteAssignmentIt->is_boolean() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "GmodNodeConversionDto JSON field '" );
				builder.append( constants::dto::GMODVERSIONING_DTO_KEY_DELETE_ASSIGNMENT );
				builder.append( "' is not a boolean" );
				throw nlohmann::json::type_error::create( 302, lease.toString(), nullptr );
			}
			dto.m_deleteAssignment = deleteAssignmentIt->get<bool>();
		}
		else
		{
			dto.m_deleteAssignment = false;
		}

		if ( dto.m_operations.empty() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "WARN: Node conversion has no operations: source=" );
			builder.append( dto.m_source );
			builder.append( ", target=" );
			builder.append( dto.m_target );
			builder.append( "'\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );
		}
		if ( dto.m_source.empty() && dto.m_target.empty() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			lease.builder().append( "WARN: Node conversion has empty source and target\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );
		}
	}

	void to_json( nlohmann::json& j, const GmodNodeConversionDto& dto )
	{
		j = nlohmann::json{
			{ constants::dto::GMODVERSIONING_DTO_KEY_OPERATIONS, dto.m_operations },
			{ constants::dto::GMODVERSIONING_DTO_KEY_SOURCE, dto.m_source },
			{ constants::dto::GMODVERSIONING_DTO_KEY_TARGET, dto.m_target },
			{ constants::dto::GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT, dto.m_oldAssignment },
			{ constants::dto::GMODVERSIONING_DTO_KEY_NEW_ASSIGNMENT, dto.m_newAssignment },
			{ constants::dto::GMODVERSIONING_DTO_KEY_DELETE_ASSIGNMENT, dto.m_deleteAssignment } };
	}

	//=====================================================================
	// GMOD Versioning Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<GmodVersioningDto> GmodVersioningDto::tryFromJson( const nlohmann::json& json )
	{
		const auto visHint = GmodVersioningDtoInternal::extractVisHint( json );

		try
		{
			if ( !json.is_object() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				lease.builder().append( "ERROR: JSON value for GmodVersioningDto is not an object\n" );

				std::fprintf( stderr, "%s", lease.toString().c_str() );

				return std::nullopt;
			}

			GmodVersioningDto dto = json.get<GmodVersioningDto>();

			return std::optional<GmodVersioningDto>{ std::move( dto ) };
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during GmodVersioningDto parsing (hint: visRelease='" );
			builder.append( visHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "'\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );

			return std::nullopt;
		}
		catch ( const std::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "ERROR: Standard exception during GmodVersioningDto parsing (hint: visRelease='" );
			builder.append( visHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "'\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );

			return std::nullopt;
		}
	}

	GmodVersioningDto GmodVersioningDto::fromJson( const nlohmann::json& json )
	{
		const auto visHint = GmodVersioningDtoInternal::extractVisHint( json );
		auto dtoOpt = GmodVersioningDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Failed to deserialize GmodVersioningDto from JSON (hint: visRelease='" );
			builder.append( visHint );
			builder.append( "')" );
			throw std::invalid_argument{ lease.toString() };
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
		const auto visReleaseIt = j.find( constants::dto::GMODVERSIONING_DTO_KEY_VIS_RELEASE );
		if ( visReleaseIt == j.end() || !visReleaseIt->is_string() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "GmodVersioningDto JSON missing required '" );
			builder.append( constants::dto::GMODVERSIONING_DTO_KEY_VIS_RELEASE );
			builder.append( "' field or not a string" );
			throw nlohmann::json::parse_error::create( 101, 0u, lease.toString(), nullptr );
		}
		dto.m_visVersion = visReleaseIt->get<std::string>();

		dto.m_items.clear();
		const auto itemsIt = j.find( constants::dto::GMODVERSIONING_DTO_KEY_ITEMS );
		if ( itemsIt != j.end() )
		{
			if ( !itemsIt->is_object() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "GmodVersioningDto JSON field '" );
				builder.append( constants::dto::GMODVERSIONING_DTO_KEY_ITEMS );
				builder.append( "' is not an object" );
				throw nlohmann::json::type_error::create( 302, lease.toString(), nullptr );
			}

			const auto& itemsObj = *itemsIt;
			size_t itemCount = itemsObj.size();

			if ( itemCount > 10000 )
			{
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
				catch ( const nlohmann::json::exception& ex )
				{
					auto lease = nfx::string::StringBuilderPool::lease();
					auto builder = lease.builder();
					builder.append( "ERROR: Error parsing conversion item '" );
					builder.append( key );
					builder.append( "': " );
					builder.append( ex.what() );
					builder.append( "'\n" );

					std::fprintf( stderr, "%s", lease.toString().c_str() );
				}
				catch ( const std::exception& ex )
				{
					auto lease = nfx::string::StringBuilderPool::lease();
					auto builder = lease.builder();
					builder.append( "ERROR: Standard exception parsing conversion item '" );
					builder.append( key );
					builder.append( "': " );
					builder.append( ex.what() );
					builder.append( "'\n" );

					std::fprintf( stderr, "%s", lease.toString().c_str() );
				}
			}

			// If parsing failed for more than 10% of items, rebuild the map to potentially save memory
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
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "WARN: No '" );
			builder.append( constants::dto::GMODVERSIONING_DTO_KEY_ITEMS );
			builder.append( "' object found in GMOD versioning data for VIS version " );
			builder.append( dto.m_visVersion );
			builder.append( "'\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );
		}
	}

	void to_json( nlohmann::json& j, const GmodVersioningDto& dto )
	{
		j = nlohmann::json{ { constants::dto::GMODVERSIONING_DTO_KEY_VIS_RELEASE, dto.m_visVersion } };

		if ( !dto.m_items.empty() )
		{
			j[constants::dto::GMODVERSIONING_DTO_KEY_ITEMS] = dto.m_items;

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
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "WARN: " );
				builder.append( std::to_string( emptyOperationsCount ) );
				builder.append( " nodes have no operations defined during serialization\n" );

				std::fprintf( stderr, "%s", lease.toString().c_str() );
			}
		}
		else
		{
			j[constants::dto::GMODVERSIONING_DTO_KEY_ITEMS] = nlohmann::json::object();
		}
	}
}
