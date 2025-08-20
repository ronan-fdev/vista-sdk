/**
 * @file ISO19848Dtos.cpp
 * @brief Implementation of ISO 19848 data transfer objects
 */

#include "dnv/vista/sdk/pch.h"

#include "dnv/vista/sdk/transport/ISO19848Dtos.h"

#include "dnv/vista/sdk/constants/DtoKeysConstants.h"
#include "dnv/vista/sdk/utils/StringBuilderPool.h"

namespace dnv::vista::sdk::transport
{
	namespace ISO19848DtosInternal
	{
		//=====================================================================
		// JSON parsing helper functions
		//=====================================================================

		inline std::string_view extractTypeHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				const auto typeIt = json.find( constants::dto::ISO19848_DTO_KEY_TYPE );
				if ( typeIt != json.end() && typeIt->is_string() )
				{
					const auto& str = typeIt->get_ref<const std::string&>();

					return std::string_view{ str };
				}

				return constants::dto::ISO19848_DTO_UNKNOWN_TYPE;
			}
			catch ( ... )
			{
				return constants::dto::ISO19848_DTO_UNKNOWN_TYPE;
			}
		}
	}

	//=====================================================================
	// Single Data Channel Type data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<DataChannelTypeNameDto> DataChannelTypeNameDto::tryFromJson( const nlohmann::json& json )
	{
		const auto typeHint = ISO19848DtosInternal::extractTypeHint( json );

		try
		{
			if ( !json.is_object() )
			{
				auto lease = utils::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "ERROR: JSON value for DataChannelTypeNameDto is not an object\n" );

				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}

			DataChannelTypeNameDto dto = json.get<DataChannelTypeNameDto>();

			return std::optional<DataChannelTypeNameDto>{ std::move( dto ) };
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during DataChannelTypeNameDto parsing (hint: type='" );
			builder.append( typeHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
		catch ( const std::exception& ex )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: Standard exception during DataChannelTypeNameDto parsing (hint: type='" );
			builder.append( typeHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
	}

	DataChannelTypeNameDto DataChannelTypeNameDto::fromJson( const nlohmann::json& json )
	{
		const auto typeHint = ISO19848DtosInternal::extractTypeHint( json );
		auto dtoOpt = DataChannelTypeNameDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Failed to deserialize DataChannelTypeNameDto from JSON (hint: type='" );
			builder.append( typeHint );
			builder.append( "')" );

			throw std::invalid_argument( lease.toString() );
		}

		return std::move( dtoOpt ).value();
	}

	nlohmann::json DataChannelTypeNameDto::toJson() const
	{
		nlohmann::json result;
		to_json( result, *this );

		return result;
	}

	//----------------------------------------------
	// Private serialization methods
	//----------------------------------------------

	void from_json( const nlohmann::json& j, DataChannelTypeNameDto& dto )
	{
		const auto typeIt = j.find( constants::dto::ISO19848_DTO_KEY_TYPE );
		const auto descIt = j.find( constants::dto::ISO19848_DTO_KEY_DESCRIPTION );

		if ( typeIt == j.end() || !typeIt->is_string() )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "DataChannelTypeNameDto JSON missing required '" );
			builder.append( constants::dto::ISO19848_DTO_KEY_TYPE );
			builder.append( "' field or not a string" );

			throw nlohmann::json::parse_error::create( 101, 0u, lease.toString(), nullptr );
		}
		if ( descIt == j.end() || !descIt->is_string() )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "DataChannelTypeNameDto JSON missing required '" );
			builder.append( constants::dto::ISO19848_DTO_KEY_DESCRIPTION );
			builder.append( "' field or not a string" );

			throw nlohmann::json::parse_error::create( 101, 0u, lease.toString(), nullptr );
		}

		dto.m_type = typeIt->get<std::string>();
		dto.m_description = descIt->get<std::string>();

		if ( dto.m_type.empty() )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "WARN: Empty 'type' field found in DataChannelTypeNameDto\n" );

			fmt::print( stderr, "{}", lease.toString() );
		}
		if ( dto.m_description.empty() )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "WARN: Empty 'description' field found in DataChannelTypeNameDto\n" );

			fmt::print( stderr, "{}", lease.toString() );
		}
	}

	void to_json( nlohmann::json& j, const DataChannelTypeNameDto& dto )
	{
		j = nlohmann::json{ { constants::dto::ISO19848_DTO_KEY_TYPE, dto.m_type }, { constants::dto::ISO19848_DTO_KEY_DESCRIPTION, dto.m_description } };
	}

	//=====================================================================
	// Collection of Data Channel Type data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<DataChannelTypeNamesDto> DataChannelTypeNamesDto::tryFromJson( const nlohmann::json& json )
	{
		try
		{
			if ( !json.is_object() )
			{
				auto lease = utils::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "ERROR: JSON value for DataChannelTypeNamesDto is not an object\n" );

				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}

			const auto valuesIt = json.find( constants::dto::ISO19848_DTO_KEY_VALUES );
			if ( valuesIt == json.end() || !valuesIt->is_array() )
			{
				auto lease = utils::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "ERROR: DataChannelTypeNamesDto JSON missing required '" );
				builder.append( constants::dto::ISO19848_DTO_KEY_VALUES );
				builder.append( "' array\n" );

				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}

			const auto& valuesArray = *valuesIt;
			size_t totalItems = valuesArray.size();
			size_t successCount = 0;

			std::vector<DataChannelTypeNameDto> tempValues;
			const size_t reserveSize = totalItems < 1000 ? totalItems + totalItems / 4 : totalItems + totalItems / 16;
			tempValues.reserve( reserveSize );

			for ( const auto& itemJson : valuesArray )
			{
				auto itemOpt = DataChannelTypeNameDto::tryFromJson( itemJson );
				if ( itemOpt.has_value() )
				{
					tempValues.emplace_back( std::move( *itemOpt ) );
					successCount++;
				}
				else
				{
					auto lease = utils::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "WARN: Skipping invalid DataChannelTypeNameDto item during parsing\n" );

					fmt::print( stderr, "{}", lease.toString() );
				}
			}

			/* If parsing failed for more than 10% of items, shrink the vector to potentially save memory */
			if ( totalItems > 0 && successCount < totalItems * 9 / 10 )
			{
				if ( tempValues.capacity() > tempValues.size() * 4 / 3 )
				{
					tempValues.shrink_to_fit();
				}
			}

			DataChannelTypeNamesDto resultDto( std::move( tempValues ) );
			return std::optional<DataChannelTypeNamesDto>{ std::move( resultDto ) };
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during DataChannelTypeNamesDto parsing: " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
		catch ( const std::exception& ex )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: Standard exception during DataChannelTypeNamesDto parsing: " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
	}

	DataChannelTypeNamesDto DataChannelTypeNamesDto::fromJson( const nlohmann::json& json )
	{
		auto dtoOpt = DataChannelTypeNamesDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			throw std::invalid_argument( "Failed to deserialize DataChannelTypeNamesDto from JSON" );
		}

		return std::move( dtoOpt ).value();
	}

	nlohmann::json DataChannelTypeNamesDto::toJson() const
	{
		nlohmann::json result;
		to_json( result, *this );

		return result;
	}

	//----------------------------------------------
	// Private serialization methods
	//----------------------------------------------

	void from_json( const nlohmann::json& j, DataChannelTypeNamesDto& dto )
	{
		const auto valuesIt = j.find( constants::dto::ISO19848_DTO_KEY_VALUES );
		if ( valuesIt == j.end() || !valuesIt->is_array() )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "DataChannelTypeNamesDto JSON missing required '" );
			builder.append( constants::dto::ISO19848_DTO_KEY_VALUES );
			builder.append( "' array" );

			throw nlohmann::json::parse_error::create( 101, 0u, lease.toString(), nullptr );
		}

		const auto& valuesArray = *valuesIt;
		const size_t totalItems = valuesArray.size();
		size_t successCount = 0;

		dto.m_values.clear();
		const size_t reserveSize = totalItems < 1000 ? totalItems + totalItems / 4 : totalItems + totalItems / 16;
		dto.m_values.reserve( reserveSize );

		for ( const auto& itemJson : valuesArray )
		{
			auto itemOpt = DataChannelTypeNameDto::tryFromJson( itemJson );
			if ( itemOpt.has_value() )
			{
				dto.m_values.emplace_back( std::move( *itemOpt ) );
				successCount++;
			}
			else
			{
				auto lease = utils::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: Skipping invalid DataChannelTypeNameDto item during ADL parsing\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}
		}

		/* If parsing failed for more than 10% of items, shrink the vector to potentially save memory */
		if ( totalItems > 0 && successCount < totalItems * 9 / 10 )
		{
			if ( dto.m_values.capacity() > dto.m_values.size() * 4 / 3 )
			{
				dto.m_values.shrink_to_fit();
			}
		}
	}

	void to_json( nlohmann::json& j, const DataChannelTypeNamesDto& dto ) { j = nlohmann::json{ { constants::dto::ISO19848_DTO_KEY_VALUES, dto.m_values } }; }

	//=====================================================================
	// Single Format Data Type data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<FormatDataTypeDto> FormatDataTypeDto::tryFromJson( const nlohmann::json& json )
	{
		const auto typeHint = ISO19848DtosInternal::extractTypeHint( json );

		try
		{
			if ( !json.is_object() )
			{
				auto lease = utils::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "ERROR: JSON value for FormatDataTypeDto is not an object\n" );

				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}

			FormatDataTypeDto dto = json.get<FormatDataTypeDto>();

			return std::optional<FormatDataTypeDto>{ std::move( dto ) };
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during FormatDataTypeDto parsing (hint: type='" );
			builder.append( typeHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
		catch ( const std::exception& ex )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: Standard exception during FormatDataTypeDto parsing (hint: type='" );
			builder.append( typeHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
	}

	FormatDataTypeDto FormatDataTypeDto::fromJson( const nlohmann::json& json )
	{
		const auto typeHint = ISO19848DtosInternal::extractTypeHint( json );
		auto dtoOpt = FormatDataTypeDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Failed to deserialize FormatDataTypeDto from JSON (hint: type='" );
			builder.append( typeHint );
			builder.append( "')" );

			throw std::invalid_argument( lease.toString() );
		}

		return std::move( dtoOpt ).value();
	}

	nlohmann::json FormatDataTypeDto::toJson() const
	{
		nlohmann::json result;
		to_json( result, *this );

		return result;
	}

	//----------------------------------------------
	// Private serialization methods
	//----------------------------------------------

	void from_json( const nlohmann::json& j, FormatDataTypeDto& dto )
	{
		const auto typeIt = j.find( constants::dto::ISO19848_DTO_KEY_TYPE );
		const auto descIt = j.find( constants::dto::ISO19848_DTO_KEY_DESCRIPTION );

		if ( typeIt == j.end() || !typeIt->is_string() )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "FormatDataTypeDto JSON missing required '" );
			builder.append( constants::dto::ISO19848_DTO_KEY_TYPE );
			builder.append( "' field or not a string" );

			throw nlohmann::json::parse_error::create( 101, 0u, lease.toString(), nullptr );
		}
		if ( descIt == j.end() || !descIt->is_string() )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "FormatDataTypeDto JSON missing required '" );
			builder.append( constants::dto::ISO19848_DTO_KEY_DESCRIPTION );
			builder.append( "' field or not a string" );

			throw nlohmann::json::parse_error::create( 101, 0u, lease.toString(), nullptr );
		}

		dto.m_type = typeIt->get<std::string>();
		dto.m_description = descIt->get<std::string>();

		if ( dto.m_type.empty() )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "WARN: Empty 'type' field found in FormatDataTypeDto\n" );

			fmt::print( stderr, "{}", lease.toString() );
		}
		if ( dto.m_description.empty() )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "WARN: Empty 'description' field found in FormatDataTypeDto\n" );

			fmt::print( stderr, "{}", lease.toString() );
		}
	}
	void to_json( nlohmann::json& j, const FormatDataTypeDto& dto )
	{
		j = nlohmann::json{ { constants::dto::ISO19848_DTO_KEY_TYPE, dto.m_type }, { constants::dto::ISO19848_DTO_KEY_DESCRIPTION, dto.m_description } };
	}

	//=====================================================================
	// Collection of Format Data Type data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<FormatDataTypesDto> FormatDataTypesDto::tryFromJson( const nlohmann::json& json )
	{
		try
		{
			if ( !json.is_object() )
			{
				auto lease = utils::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "ERROR: JSON value for FormatDataTypesDto is not an object\n" );

				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}

			const auto valuesIt = json.find( constants::dto::ISO19848_DTO_KEY_VALUES );
			if ( valuesIt == json.end() || !valuesIt->is_array() )
			{
				auto lease = utils::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "ERROR: FormatDataTypesDto JSON missing required '" );
				builder.append( constants::dto::ISO19848_DTO_KEY_VALUES );
				builder.append( "' array\n" );

				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}

			const auto& valuesArray = *valuesIt;
			size_t totalItems = valuesArray.size();
			size_t successCount = 0;

			std::vector<FormatDataTypeDto> tempValues;
			const size_t reserveSize = totalItems < 1000 ? totalItems + totalItems / 4 : totalItems + totalItems / 16;
			tempValues.reserve( reserveSize );

			for ( const auto& itemJson : valuesArray )
			{
				auto itemOpt = FormatDataTypeDto::tryFromJson( itemJson );
				if ( itemOpt.has_value() )
				{
					tempValues.emplace_back( std::move( *itemOpt ) );
					successCount++;
				}
				else
				{
					auto lease = utils::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "WARN: Skipping invalid FormatDataTypeDto item during parsing\n" );
					fmt::print( stderr, "{}", lease.toString() );
				}
			}

			/* If parsing failed for more than 10% of items, shrink the vector to potentially save memory */
			if ( totalItems > 0 && successCount < totalItems * 9 / 10 )
			{
				if ( tempValues.capacity() > tempValues.size() * 4 / 3 )
				{
					tempValues.shrink_to_fit();
				}
			}

			FormatDataTypesDto resultDto( std::move( tempValues ) );

			return std::optional<FormatDataTypesDto>{ std::move( resultDto ) };
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during FormatDataTypesDto parsing: " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
		catch ( const std::exception& ex )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: Standard exception during FormatDataTypesDto parsing: " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
	}

	FormatDataTypesDto FormatDataTypesDto::fromJson( const nlohmann::json& json )
	{
		auto dtoOpt = FormatDataTypesDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			throw std::invalid_argument( "Failed to deserialize FormatDataTypesDto from JSON" );
		}

		return std::move( dtoOpt ).value();
	}

	nlohmann::json FormatDataTypesDto::toJson() const
	{
		nlohmann::json result;
		to_json( result, *this );

		return result;
	}

	//----------------------------------------------
	// Private serialization methods
	//----------------------------------------------

	void from_json( const nlohmann::json& j, FormatDataTypesDto& dto )
	{
		const auto valuesIt = j.find( constants::dto::ISO19848_DTO_KEY_VALUES );
		if ( valuesIt == j.end() || !valuesIt->is_array() )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "FormatDataTypesDto JSON missing required '" );
			builder.append( constants::dto::ISO19848_DTO_KEY_VALUES );
			builder.append( "' array" );

			throw nlohmann::json::parse_error::create( 101, 0u, lease.toString(), nullptr );
		}

		const auto& valuesArray = *valuesIt;
		const size_t totalItems = valuesArray.size();
		size_t successCount = 0;

		dto.m_values.clear();

		const size_t reserveSize = totalItems < 1000 ? totalItems + totalItems / 4 : totalItems + totalItems / 16;
		dto.m_values.reserve( reserveSize );

		for ( const auto& itemJson : valuesArray )
		{
			auto itemOpt = FormatDataTypeDto::tryFromJson( itemJson );
			if ( itemOpt.has_value() )
			{
				dto.m_values.emplace_back( std::move( *itemOpt ) );
				successCount++;
			}
			else
			{
				auto lease = utils::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: Skipping invalid FormatDataTypeDto item during ADL parsing\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}
		}

		/* If parsing failed for more than 10% of items, shrink the vector to potentially save memory */
		if ( totalItems > 0 && successCount < totalItems * 9 / 10 )
		{
			if ( dto.m_values.capacity() > dto.m_values.size() * 4 / 3 )
			{
				dto.m_values.shrink_to_fit();
			}
		}
	}

	void to_json( nlohmann::json& j, const FormatDataTypesDto& dto )
	{
		j = nlohmann::json{ { constants::dto::ISO19848_DTO_KEY_VALUES, dto.m_values } };
	}
}
