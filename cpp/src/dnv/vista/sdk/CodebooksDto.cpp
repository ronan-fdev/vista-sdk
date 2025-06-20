/**
 * @file CodebooksDto.cpp
 * @brief Implementation of ISO 19848 codebook data transfer objects
 */

#include "pch.h"

#include "dnv/vista/sdk/CodebooksDto.h"

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// Constants
		//=====================================================================

		static constexpr const char* NAME_KEY = "name";
		static constexpr const char* VALUES_KEY = "values";
		static constexpr const char* ITEMS_KEY = "items";
		static constexpr const char* VIS_RELEASE_KEY = "visRelease";
	}

	//=====================================================================
	// Codebook Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Construction / destruction
	//----------------------------------------------

	CodebookDto::CodebookDto( std::string name, ValuesMap values )
		: m_name{ std::move( name ) },
		  m_values{ std::move( values ) }
	{
	}

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<CodebookDto> CodebookDto::tryFromJson( const nlohmann::json& json )
	{
		try
		{
			if ( !json.contains( NAME_KEY ) || !json.at( NAME_KEY ).is_string() )
			{
				SPDLOG_ERROR( "Codebook JSON missing required '{}' field or field is not a string", NAME_KEY );

				return std::nullopt;
			}

			std::string tempName = json.at( NAME_KEY ).get<std::string>();

			ValuesMap tempValues;

			if ( json.contains( VALUES_KEY ) )
			{
				if ( !json.at( VALUES_KEY ).is_object() )
				{
					SPDLOG_WARN( "No '{}' object found or not an object for codebook '{}'", VALUES_KEY, tempName );
				}
				else
				{
					/* Each key is a group name, and its value should be an array of strings. */
					const auto& valuesObject = json.at( VALUES_KEY );
					tempValues.reserve( valuesObject.size() );

					for ( const auto& [groupName, groupValueJson] : valuesObject.items() )
					{
						if ( !groupValueJson.is_array() )
						{
							SPDLOG_WARN( "Group '{}' values are not in array format for codebook '{}', skipping", groupName, tempName );

							continue;
						}

						ValueGroup groupValues;
						try
						{
							/* Attempt to parse the array of strings for the current group */
							groupValues = groupValueJson.get<ValueGroup>();
							tempValues.emplace( groupName, std::move( groupValues ) );
						}
						catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
						{
							SPDLOG_WARN( "Error parsing values for group '{}' in codebook '{}': {}. Skipping group.", groupName, tempName, ex.what() );
						}
					}
				}
			}
			else
			{
				SPDLOG_WARN( "No '{}' object found for codebook '{}'", VALUES_KEY, tempName );
			}

			/* Construct the final DTO using successfully parsed data */
			CodebookDto resultDto( std::move( tempName ), std::move( tempValues ) );

			return std::optional<CodebookDto>{ std::move( resultDto ) };
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			std::string nameHint = "[unknown name]";
			if ( json.contains( NAME_KEY ) && json.at( NAME_KEY ).is_string() )
			{
				nameHint = json.at( NAME_KEY ).get<std::string>();
			}

			SPDLOG_ERROR( "JSON exception during CodebookDto parsing (hint: name='{}'): {}", nameHint, ex.what() );

			return std::nullopt;
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			std::string nameHint = "[unknown name]";
			if ( json.contains( NAME_KEY ) && json.at( NAME_KEY ).is_string() )
			{
				nameHint = json.at( NAME_KEY ).get<std::string>();
			}

			SPDLOG_ERROR( "Standard exception during CodebookDto parsing (hint: name='{}'): {}", nameHint, ex.what() );

			return std::nullopt;
		}
	}

	CodebookDto CodebookDto::fromJson( const nlohmann::json& json )
	{
		auto dtoOpt = CodebookDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			std::string nameHint = "[unknown name]";
			if ( json.is_object() && json.contains( NAME_KEY ) && json.at( NAME_KEY ).is_string() )
			{
				nameHint = json.at( NAME_KEY ).get<std::string>();
			}

			std::string errorMsg = fmt::format( "Failed to deserialize CodebookDto from JSON (hint: name='{}')", nameHint );
			throw std::invalid_argument( errorMsg );
		}

		return std::move( dtoOpt.value() );
	}

	nlohmann::json CodebookDto::toJson() const
	{
		/* Directly construct JSON object from members */
		nlohmann::json obj = { { NAME_KEY, m_name }, { VALUES_KEY, m_values } };

		return obj;
	}

	//-------------------------------------------------------------------
	// Private serialization methods
	//-------------------------------------------------------------------

	void to_json( nlohmann::json& j, const CodebookDto& dto )
	{
		/* ADL hook for nlohmann::json serialization */
		j = nlohmann::json{ { NAME_KEY, dto.name() }, { VALUES_KEY, dto.values() } };
	}

	void from_json( const nlohmann::json& j, CodebookDto& dto )
	{
		/* ADL hook for nlohmann::json deserialization. Direct member assignment and throws on error, unlike tryFromJson which returns std::optional. */
		if ( !j.contains( NAME_KEY ) || !j.at( NAME_KEY ).is_string() )
		{
			throw nlohmann::json::parse_error::create( 101, 0u, "Codebook JSON missing required 'name' field or field is not a string", nullptr );
		}

		std::string tempName = j.at( NAME_KEY ).get<std::string>();
		CodebookDto::ValuesMap tempValues;
		if ( j.contains( VALUES_KEY ) )
		{
			if ( !j.at( VALUES_KEY ).is_object() )
			{
				SPDLOG_WARN( "Codebook JSON 'values' field is not an object for name '{}'", tempName );
			}
			else
			{
				tempValues = j.at( VALUES_KEY ).get<CodebookDto::ValuesMap>();
			}
		}
		else
		{
			SPDLOG_WARN( "Codebook JSON missing 'values' field for name '{}'", tempName );
		}

		dto.m_name = std::move( tempName );
		dto.m_values = std::move( tempValues );
	}

	//=====================================================================
	// Codebooks Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Construction / destruction
	//----------------------------------------------

	CodebooksDto::CodebooksDto( std::string visVersion, Items items )
		: m_visVersion{ std::move( visVersion ) },
		  m_items{ std::move( items ) }
	{
	}

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<CodebooksDto> CodebooksDto::tryFromJson( const nlohmann::json& json )
	{
		try
		{
			if ( !json.contains( VIS_RELEASE_KEY ) || !json.at( VIS_RELEASE_KEY ).is_string() )
			{
				SPDLOG_ERROR( "Codebooks JSON missing required '{}' field or field is not a string", VIS_RELEASE_KEY );

				return std::nullopt;
			}

			std::string tempVisVersion = json.at( VIS_RELEASE_KEY ).get<std::string>();

			Items tempItems;
			size_t totalItems = 0;
			size_t successCount = 0;

			if ( json.contains( ITEMS_KEY ) )
			{
				if ( !json.at( ITEMS_KEY ).is_array() )
				{
					SPDLOG_WARN( "'{}' field is not an array for VIS version {}", ITEMS_KEY, tempVisVersion );
				}
				else
				{
					/* The "items" key should contain a JSON array of codebook objects. */
					const auto& itemsArray = json.at( ITEMS_KEY );
					totalItems = itemsArray.size();
					tempItems.reserve( totalItems );

					for ( const auto& itemJson : itemsArray )
					{
						/* Recursively parse each codebook item using its own tryFromJson. */
						auto codebookOpt = CodebookDto::tryFromJson( itemJson );
						if ( codebookOpt.has_value() )
						{
							tempItems.emplace_back( std::move( codebookOpt.value() ) );
							successCount++;
						}
						else
						{
							SPDLOG_WARN( "Skipping invalid codebook item during CodebooksDto parsing for VIS version {}.", tempVisVersion );
						}
					}

					/* If parsing failed for more than 10% of items, shrink the vector to potentially save memory. */
					if ( totalItems > 0 && static_cast<double>( successCount ) < static_cast<double>( totalItems ) * 0.9 )
					{
						SPDLOG_WARN( "Shrinking items vector due to high parsing failure rate ({}/{}) for VIS version {}", successCount, totalItems, tempVisVersion );
						tempItems.shrink_to_fit();
					}
				}
			}
			else
			{
				SPDLOG_WARN( "No '{}' array found in CodebooksDto for VIS version {}", ITEMS_KEY, tempVisVersion );
			}

			/* Construct the final DTO using successfully parsed data */
			CodebooksDto resultDto( std::move( tempVisVersion ), std::move( tempItems ) );

			return std::optional<CodebooksDto>{ std::move( resultDto ) };
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			std::string visHint = "[unknown version]";
			if ( json.contains( VIS_RELEASE_KEY ) && json.at( VIS_RELEASE_KEY ).is_string() )
			{
				visHint = json.at( VIS_RELEASE_KEY ).get<std::string>();
			}

			SPDLOG_ERROR( "JSON exception during CodebooksDto parsing (hint: visRelease='{}'): {}", visHint, ex.what() );

			return std::nullopt;
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			std::string visHint = "[unknown version]";
			if ( json.contains( VIS_RELEASE_KEY ) && json.at( VIS_RELEASE_KEY ).is_string() )
			{
				visHint = json.at( VIS_RELEASE_KEY ).get<std::string>();
			}

			SPDLOG_ERROR( "Standard exception during CodebooksDto parsing (hint: visRelease='{}'): {}", visHint, ex.what() );

			return std::nullopt;
		}
	}

	CodebooksDto CodebooksDto::fromJson( const nlohmann::json& json )
	{
		auto dtoOpt = CodebooksDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			std::string visHint = "[unknown version]";
			if ( json.is_object() && json.contains( VIS_RELEASE_KEY ) && json.at( VIS_RELEASE_KEY ).is_string() )
			{
				visHint = json.at( VIS_RELEASE_KEY ).get<std::string>();
			}

			std::string errorMsg = fmt::format( "Failed to deserialize CodebooksDto from JSON (hint: visRelease='{}')", visHint );
			throw std::invalid_argument( errorMsg );
		}

		return std::move( dtoOpt.value() );
	}

	nlohmann::json CodebooksDto::toJson() const
	{
		return { { VIS_RELEASE_KEY, m_visVersion }, { ITEMS_KEY, m_items } };
	}
}
