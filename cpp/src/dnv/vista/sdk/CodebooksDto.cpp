/**
 * @file CodebooksDto.cpp
 * @brief Implementation of ISO 19848 codebook data transfer objects
 */

#include "dnv/vista/sdk/CodebooksDto.h"

#include "dnv/vista/sdk/constants/DtoKeysConstants.h"
#include "dnv/vista/sdk/internal/StringBuilderPool.h"

namespace dnv::vista::sdk
{
	namespace CodebooksDtoInternal
	{
		//=====================================================================
		// JSON parsing helper functions
		//=====================================================================

		inline std::string_view extractNameHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				const auto nameIt = json.find( constants::dto::CODEBOOK_DTO_KEY_NAME );
				if ( nameIt != json.end() && nameIt->is_string() )
				{
					const auto& str = nameIt->get_ref<const std::string&>();

					return std::string_view{ str };
				}

				return constants::dto::CODEBOOK_DTO_UNKNOWN_NAME;
			}
			catch ( ... )
			{
				return constants::dto::CODEBOOK_DTO_UNKNOWN_NAME;
			}
		}

		inline std::string_view extractVisHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				const auto visIt = json.find( constants::dto::CODEBOOK_DTO_KEY_VIS_RELEASE );
				if ( visIt != json.end() && visIt->is_string() )
				{
					const auto& str = visIt->get_ref<const std::string&>();

					return std::string_view{ str };
				}

				return constants::dto::CODEBOOK_DTO_UNKNOWN_VERSION;
			}
			catch ( ... )
			{
				return constants::dto::CODEBOOK_DTO_UNKNOWN_VERSION;
			}
		}
	}

	//=====================================================================
	// Codebook Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<CodebookDto> CodebookDto::tryFromJson( const nlohmann::json& json )
	{
		const auto nameHint = CodebooksDtoInternal::extractNameHint( json );

		try
		{
			const auto nameIt = json.find( constants::dto::CODEBOOK_DTO_KEY_NAME );
			if ( nameIt == json.end() || !nameIt->is_string() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "ERROR: Codebook JSON missing required '" );
				builder.append( constants::dto::CODEBOOK_DTO_KEY_NAME );
				builder.append( "' field or field is not a string\n" );

				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}

			std::string tempName = nameIt->get<std::string>();

			ValuesMap tempValues;

			const auto valuesIt = json.find( constants::dto::CODEBOOK_DTO_KEY_VALUES );
			if ( valuesIt != json.end() )
			{
				if ( !valuesIt->is_object() )
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "WARN: No '" );
					builder.append( constants::dto::CODEBOOK_DTO_KEY_VALUES );
					builder.append( "' object found or not an object for codebook '" );
					builder.append( std::string_view{ tempName } );
					builder.append( "'\n" );

					fmt::print( stderr, "{}", lease.toString() );
				}
				else
				{
					/* Each key is a group name, and its value should be an array of strings. */
					const auto& valuesObject = *valuesIt;
					tempValues.reserve( valuesObject.size() + valuesObject.size() / 4 );

					for ( const auto& [groupName, groupValueJson] : valuesObject.items() )
					{
						if ( !groupValueJson.is_array() )
						{
							auto lease = internal::StringBuilderPool::instance();
							auto builder = lease.builder();
							builder.append( "WARN: Group '" );
							builder.append( std::string_view{ groupName } );
							builder.append( "' values are not in array format for codebook '" );
							builder.append( std::string_view{ tempName } );
							builder.append( "', skipping\n" );

							fmt::print( stderr, "{}", lease.toString() );

							continue;
						}

						ValueGroup groupValues;
						groupValues.reserve( groupValueJson.size() );
						try
						{
							/* Attempt to parse the array of strings for the current group */
							groupValues = groupValueJson.get<ValueGroup>();
							tempValues.emplace( groupName, std::move( groupValues ) );
						}
						catch ( const nlohmann::json::exception& ex )
						{
							auto lease = internal::StringBuilderPool::instance();
							auto builder = lease.builder();
							builder.append( "WARN: Error parsing values for group '" );
							builder.append( std::string_view{ groupName } );
							builder.append( "' in codebook '" );
							builder.append( std::string_view{ tempName } );
							builder.append( "': " );
							builder.append( ex.what() );
							builder.append( ". Skipping group.\n" );

							fmt::print( stderr, "{}", lease.toString() );
						}
					}
				}
			}
			else
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: No '" );
				builder.append( constants::dto::CODEBOOK_DTO_KEY_VALUES );
				builder.append( "' object found for codebook '" );
				builder.append( std::string_view{ tempName } );
				builder.append( "'\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}

			/* Construct the final DTO using successfully parsed data */
			CodebookDto resultDto( std::move( tempName ), std::move( tempValues ) );

			return resultDto;
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during CodebookDto parsing (hint: name='" );
			builder.append( nameHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
		catch ( const std::exception& ex )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: Standard exception during CodebookDto parsing (hint: name='" );
			builder.append( nameHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
	}

	CodebookDto CodebookDto::fromJson( const nlohmann::json& json )
	{
		const auto nameHint = CodebooksDtoInternal::extractNameHint( json );
		auto dtoOpt = CodebookDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Failed to deserialize CodebookDto from JSON (hint: name='" );
			builder.append( nameHint );
			builder.append( "')" );

			throw std::invalid_argument( lease.toString() );
		}

		return dtoOpt.value();
	}

	nlohmann::json CodebookDto::toJson() const
	{
		nlohmann::json result;
		to_json( result, *this );

		return result;
	}

	//-------------------------------------------------------------------
	// Private serialization methods
	//-------------------------------------------------------------------

	void from_json( const nlohmann::json& j, CodebookDto& dto )
	{
		/* ADL hook for nlohmann::json deserialization. */
		const auto nameIt = j.find( constants::dto::CODEBOOK_DTO_KEY_NAME );
		if ( nameIt == j.end() || !nameIt->is_string() )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "CodebookDto JSON missing required '" );
			builder.append( constants::dto::CODEBOOK_DTO_KEY_NAME );
			builder.append( "' field or field is not a string" );

			throw nlohmann::json::parse_error::create( 101, 0u, lease.toString(), nullptr );
		}

		/* Extract required fields */
		std::string tempName = nameIt->get<std::string>();
		if ( tempName.empty() )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "WARN: Empty name field found in CodebookDto\n" );

			fmt::print( stderr, "{}", lease.toString() );
		}

		CodebookDto::ValuesMap tempValues;
		const auto valuesIt = j.find( constants::dto::CODEBOOK_DTO_KEY_VALUES );
		if ( valuesIt != j.end() && valuesIt->is_object() )
		{
			tempValues = valuesIt->get<CodebookDto::ValuesMap>();
		}

		dto.m_name = std::move( tempName );
		dto.m_values = std::move( tempValues );
	}

	void to_json( nlohmann::json& j, const CodebookDto& dto )
	{
		/* ADL hook for nlohmann::json serialization. */
		j = nlohmann::json{ { constants::dto::CODEBOOK_DTO_KEY_NAME, dto.name() },
			{ constants::dto::CODEBOOK_DTO_KEY_VALUES, dto.values() } };
	}

	//=====================================================================
	// Codebooks Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<CodebooksDto> CodebooksDto::tryFromJson( const nlohmann::json& json )
	{
		const auto visHint = CodebooksDtoInternal::extractVisHint( json );

		try
		{
			const auto visIt = json.find( constants::dto::CODEBOOK_DTO_KEY_VIS_RELEASE );
			if ( visIt == json.end() || !visIt->is_string() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "ERROR: Codebooks JSON missing required '" );
				builder.append( constants::dto::CODEBOOK_DTO_KEY_VIS_RELEASE );
				builder.append( "' field or field is not a string\n" );

				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}

			std::string tempVisVersion = visIt->get<std::string>();

			Items tempItems;
			size_t totalItems = 0;
			size_t successCount = 0;

			const auto itemsIt = json.find( constants::dto::CODEBOOK_DTO_KEY_ITEMS );
			if ( itemsIt != json.end() )
			{
				if ( !itemsIt->is_array() )
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "WARN: '" );
					builder.append( constants::dto::CODEBOOK_DTO_KEY_ITEMS );
					builder.append( "' field is not an array for VIS version " );
					builder.append( std::string_view{ tempVisVersion } );
					builder.append( "\n" );

					fmt::print( stderr, "{}", lease.toString() );
				}
				else
				{
					/* The "items" key should contain a JSON array of codebook objects. */
					const auto& itemsArray = *itemsIt;
					totalItems = itemsArray.size();
					tempItems.reserve( totalItems );

					for ( const auto& itemJson : itemsArray )
					{
						/* Recursively parse each codebook item using its own tryFromJson. */
						auto codebookOpt = CodebookDto::tryFromJson( itemJson );
						if ( codebookOpt.has_value() )
						{
							tempItems.emplace_back( std::move( *codebookOpt ) );
							successCount++;
						}
						else
						{
							auto lease = internal::StringBuilderPool::instance();
							auto builder = lease.builder();
							builder.append( "WARN: Skipping invalid codebook item during CodebooksDto parsing for VIS version " );
							builder.append( std::string_view{ tempVisVersion } );
							builder.append( ".\n" );

							fmt::print( stderr, "{}", lease.toString() );
						}
					}

					/* If parsing failed for more than 10% of items, shrink the vector to potentially save memory. */
					if ( totalItems > 0 && successCount < totalItems * 9 / 10 )
					{
						if ( tempItems.capacity() > tempItems.size() * 4 / 3 )
						{
							tempItems.shrink_to_fit();
						}
					}
				}
			}
			else
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: No '" );
				builder.append( constants::dto::CODEBOOK_DTO_KEY_ITEMS );
				builder.append( "' array found in CodebooksDto for VIS version " );
				builder.append( std::string_view{ tempVisVersion } );
				builder.append( "\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}

			/* Construct the final DTO using successfully parsed data */
			CodebooksDto resultDto( std::move( tempVisVersion ), std::move( tempItems ) );

			return resultDto;
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during CodebooksDto parsing (hint: visRelease='" );
			builder.append( visHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
		catch ( const std::exception& ex )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: Standard exception during CodebooksDto parsing (hint: visRelease='" );
			builder.append( visHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
	}

	CodebooksDto CodebooksDto::fromJson( const nlohmann::json& json )
	{
		const auto visHint = CodebooksDtoInternal::extractVisHint( json );
		auto dtoOpt = CodebooksDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Failed to deserialize CodebooksDto from JSON (hint: visRelease='" );
			builder.append( visHint );
			builder.append( "')" );

			throw std::invalid_argument( lease.toString() );
		}

		return std::move( dtoOpt ).value();
	}

	nlohmann::json CodebooksDto::toJson() const
	{
		nlohmann::json result;
		to_json( result, *this );

		return result;
	}

	//-------------------------------------------------------------------
	// Private serialization methods
	//-------------------------------------------------------------------

	void from_json( const nlohmann::json& j, CodebooksDto& dto )
	{
		/* ADL hook for nlohmann::json deserialization. */
		const auto visIt = j.find( constants::dto::CODEBOOK_DTO_KEY_VIS_RELEASE );
		if ( visIt == j.end() || !visIt->is_string() )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Codebooks JSON missing required '" );
			builder.append( constants::dto::CODEBOOK_DTO_KEY_VIS_RELEASE );
			builder.append( "' field" );

			throw nlohmann::json::parse_error::create( 201, 0u, lease.toString(), nullptr );
		}

		const auto itemsIt = j.find( constants::dto::CODEBOOK_DTO_KEY_ITEMS );
		if ( itemsIt == j.end() || !itemsIt->is_array() )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Codebooks JSON missing required '" );
			builder.append( constants::dto::CODEBOOK_DTO_KEY_ITEMS );
			builder.append( "' array" );

			throw nlohmann::json::parse_error::create( 202, 0u, lease.toString(), nullptr );
		}

		/* Extract required fields */
		dto.m_visVersion = visIt->get<std::string>();

		if ( itemsIt->is_array() )
		{
			const auto& itemsArray = *itemsIt;
			dto.m_items.reserve( itemsArray.size() );

			for ( const auto& itemJson : itemsArray )
			{
				auto itemDtoOpt = CodebookDto::tryFromJson( itemJson );
				if ( itemDtoOpt.has_value() )
				{
					dto.m_items.emplace_back( std::move( itemDtoOpt.value() ) );
				}
				else
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "ERROR: Failed to parse Codebook item in collection during from_json" );

					throw nlohmann::json::parse_error::create( 203, 0u, lease.toString(), &itemJson );
				}
			}
			dto.m_items.shrink_to_fit();
		}
		else
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: Expected '" );
			builder.append( constants::dto::CODEBOOK_DTO_KEY_ITEMS );
			builder.append( "' to be array in CodebooksDto::from_json" );

			throw nlohmann::json::parse_error::create( 202, 0u, lease.toString(), &j );
		}
	}

	void to_json( nlohmann::json& j, const CodebooksDto& dto )
	{
		/* ADL hook for nlohmann::json serialization. */
		j = { { constants::dto::CODEBOOK_DTO_KEY_VIS_RELEASE, dto.visVersion() },
			{ constants::dto::CODEBOOK_DTO_KEY_ITEMS, dto.items() } };
	}
}
