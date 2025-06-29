/**
 * @file CodebooksDto.cpp
 * @brief Implementation of ISO 19848 codebook data transfer objects
 */

#include "pch.h"

#include "dnv/vista/sdk/CodebooksDto.h"

#include "dnv/vista/sdk/Config.h"

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// JSON parsing helper functions
		//=====================================================================

		static constexpr std::string_view UNKNOWN_NAME = "[unknown name]";
		static constexpr std::string_view UNKNOWN_VERSION = "[unknown version]";

		std::string_view extractNameHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				if ( json.contains( CODEBOOK_DTO_KEY_NAME ) && json.at( CODEBOOK_DTO_KEY_NAME ).is_string() )
				{
					const auto& str = json.at( CODEBOOK_DTO_KEY_NAME ).get_ref<const std::string&>();
					return std::string_view{ str };
				}
				return UNKNOWN_NAME;
			}
			catch ( ... )
			{
				return UNKNOWN_NAME;
			}
		}

		std::string_view extractVisHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				if ( json.contains( CODEBOOK_DTO_KEY_VIS_RELEASE ) && json.at( CODEBOOK_DTO_KEY_VIS_RELEASE ).is_string() )
				{
					const auto& str = json.at( CODEBOOK_DTO_KEY_VIS_RELEASE ).get_ref<const std::string&>();
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
	// Codebook Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<CodebookDto> CodebookDto::tryFromJson( const nlohmann::json& json )
	{
		[[maybe_unused]] const auto nameHint = extractNameHint( json );

		try
		{
			if ( !json.contains( CODEBOOK_DTO_KEY_NAME ) || !json.at( CODEBOOK_DTO_KEY_NAME ).is_string() )
			{
				SPDLOG_ERROR( "Codebook JSON missing required '{}' field or field is not a string",
					CODEBOOK_DTO_KEY_NAME );

				return std::nullopt;
			}

			std::string tempName = json.at( CODEBOOK_DTO_KEY_NAME ).get<std::string>();

			ValuesMap tempValues;

			if ( json.contains( CODEBOOK_DTO_KEY_VALUES ) )
			{
				if ( !json.at( CODEBOOK_DTO_KEY_VALUES ).is_object() )
				{
					SPDLOG_WARN( "No '{}' object found or not an object for codebook '{}'",
						CODEBOOK_DTO_KEY_VALUES, std::string_view{ tempName } );
				}
				else
				{
					/* Each key is a group name, and its value should be an array of strings. */
					const auto& valuesObject = json.at( CODEBOOK_DTO_KEY_VALUES );
					tempValues.reserve( valuesObject.size() + valuesObject.size() / 4 );

					for ( const auto& [groupName, groupValueJson] : valuesObject.items() )
					{
						if ( !groupValueJson.is_array() )
						{
							SPDLOG_WARN( "Group '{}' values are not in array format for codebook '{}', skipping",
								std::string_view{ groupName }, std::string_view{ tempName } );

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
						catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
						{
							SPDLOG_WARN( "Error parsing values for group '{}' in codebook '{}': {}. Skipping group.",
								std::string_view{ groupName }, std::string_view{ tempName }, ex.what() );
						}
					}
				}
			}
			else
			{
				SPDLOG_WARN( "No '{}' object found for codebook '{}'",
					CODEBOOK_DTO_KEY_VALUES, std::string_view{ tempName } );
			}

			/* Construct the final DTO using successfully parsed data */
			CodebookDto resultDto( std::move( tempName ), std::move( tempValues ) );

			return resultDto;
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			SPDLOG_ERROR( "JSON exception during CodebookDto parsing (hint: name='{}'): {}",
				nameHint, ex.what() );

			return std::nullopt;
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			SPDLOG_ERROR( "Standard exception during CodebookDto parsing (hint: name='{}'): {}",
				nameHint, ex.what() );

			return std::nullopt;
		}
	}

	CodebookDto CodebookDto::fromJson( const nlohmann::json& json )
	{
		const auto nameHint = extractNameHint( json );
		auto dtoOpt = CodebookDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			throw std::invalid_argument( fmt::format( "Failed to deserialize CodebookDto from JSON (hint: name='{}')", nameHint ) );
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
		if ( !j.contains( CODEBOOK_DTO_KEY_NAME ) || !j.at( CODEBOOK_DTO_KEY_NAME ).is_string() )
		{
			throw nlohmann::json::parse_error::create( 101, 0u,
				fmt::format( "CodebookDto JSON missing required '{}' field or field is not a string",
					CODEBOOK_DTO_KEY_NAME ),
				nullptr );
		}

		/* Extract required fields */
		std::string tempName = j.at( CODEBOOK_DTO_KEY_NAME ).get<std::string>();
		if ( tempName.empty() )
		{
			SPDLOG_WARN( "Empty name field found in CodebookDto" );
		}

		CodebookDto::ValuesMap tempValues;
		if ( j.contains( CODEBOOK_DTO_KEY_VALUES ) && j.at( CODEBOOK_DTO_KEY_VALUES ).is_object() )
		{
			tempValues = j.at( CODEBOOK_DTO_KEY_VALUES ).get<CodebookDto::ValuesMap>();
		}

		dto.m_name = std::move( tempName );
		dto.m_values = std::move( tempValues );
	}

	void to_json( nlohmann::json& j, const CodebookDto& dto )
	{
		/* ADL hook for nlohmann::json serialization. */
		j = nlohmann::json{ { CODEBOOK_DTO_KEY_NAME, dto.name() }, { CODEBOOK_DTO_KEY_VALUES, dto.values() } };
	}

	//=====================================================================
	// Codebooks Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<CodebooksDto> CodebooksDto::tryFromJson( const nlohmann::json& json )
	{
		[[maybe_unused]] const auto visHint = extractVisHint( json );

		try
		{
			if ( !json.contains( CODEBOOK_DTO_KEY_VIS_RELEASE ) || !json.at( CODEBOOK_DTO_KEY_VIS_RELEASE ).is_string() )
			{
				SPDLOG_ERROR( "Codebooks JSON missing required '{}' field or field is not a string",
					CODEBOOK_DTO_KEY_VIS_RELEASE );

				return std::nullopt;
			}

			std::string tempVisVersion = json.at( CODEBOOK_DTO_KEY_VIS_RELEASE ).get<std::string>();

			Items tempItems;
			size_t totalItems = 0;
			size_t successCount = 0;

			if ( json.contains( CODEBOOK_DTO_KEY_ITEMS ) )
			{
				if ( !json.at( CODEBOOK_DTO_KEY_ITEMS ).is_array() )
				{
					SPDLOG_WARN( "'{}' field is not an array for VIS version {}",
						CODEBOOK_DTO_KEY_ITEMS, std::string_view{ tempVisVersion } );
				}
				else
				{
					/* The "items" key should contain a JSON array of codebook objects. */
					const auto& itemsArray = json.at( CODEBOOK_DTO_KEY_ITEMS );
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
							SPDLOG_WARN( "Skipping invalid codebook item during CodebooksDto parsing for VIS version {}.",
								std::string_view{ tempVisVersion } );
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
				SPDLOG_WARN( "No '{}' array found in CodebooksDto for VIS version {}",
					CODEBOOK_DTO_KEY_ITEMS, std::string_view{ tempVisVersion } );
			}

			/* Construct the final DTO using successfully parsed data */
			CodebooksDto resultDto( std::move( tempVisVersion ), std::move( tempItems ) );

			return resultDto;
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			SPDLOG_ERROR( "JSON exception during CodebooksDto parsing (hint: visRelease='{}'): {}",
				visHint, ex.what() );

			return std::nullopt;
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			SPDLOG_ERROR( "Standard exception during CodebooksDto parsing (hint: visRelease='{}'): {}",
				visHint, ex.what() );

			return std::nullopt;
		}
	}

	CodebooksDto CodebooksDto::fromJson( const nlohmann::json& json )
	{
		const auto visHint = extractVisHint( json );
		auto dtoOpt = CodebooksDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			throw std::invalid_argument( fmt::format( "Failed to deserialize CodebooksDto from JSON (hint: visRelease='{}')", visHint ) );
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
		if ( !j.contains( CODEBOOK_DTO_KEY_VIS_RELEASE ) || !j.at( CODEBOOK_DTO_KEY_VIS_RELEASE ).is_string() )
		{
			throw nlohmann::json::parse_error::create( 201, 0u,
				fmt::format( "Codebooks JSON missing required '{}' field", CODEBOOK_DTO_KEY_VIS_RELEASE ), nullptr );
		}

		if ( !j.contains( CODEBOOK_DTO_KEY_ITEMS ) || !j.at( CODEBOOK_DTO_KEY_ITEMS ).is_array() )
		{
			throw nlohmann::json::parse_error::create( 202, 0u,
				fmt::format( "Codebooks JSON missing required '{}' array", CODEBOOK_DTO_KEY_ITEMS ), nullptr );
		}

		/* Extract required fields */
		dto.m_visVersion = j.at( CODEBOOK_DTO_KEY_VIS_RELEASE ).get<std::string>();
		dto.m_items = j.at( CODEBOOK_DTO_KEY_ITEMS ).get<CodebooksDto::Items>();
	}

	void to_json( nlohmann::json& j, const CodebooksDto& dto )
	{
		/* ADL hook for nlohmann::json serialization. */
		j = { { CODEBOOK_DTO_KEY_VIS_RELEASE, dto.visVersion() }, { CODEBOOK_DTO_KEY_ITEMS, dto.items() } };
	}
}
