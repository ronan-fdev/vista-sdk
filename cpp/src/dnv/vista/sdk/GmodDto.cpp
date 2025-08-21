/**
 * @file GmodDto.cpp
 * @brief Implementation of Generic Product Model (GMOD) data transfer objects
 */

#include "pch.h"

#include "dnv/vista/sdk/GmodDto.h"

#include "dnv/vista/sdk/constants/DtoKeysConstants.h"
#include "dnv/vista/sdk/internal/StringBuilderPool.h"

namespace dnv::vista::sdk
{
	namespace GmodDtoInternal
	{
		//=====================================================================
		// JSON parsing helper functions
		//=====================================================================

		inline std::string_view extractVisHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				const auto visIt = json.find( constants::dto::GMOD_DTO_KEY_VIS_RELEASE );
				if ( visIt != json.end() && visIt->is_string() )
				{
					const auto& str = visIt->get_ref<const std::string&>();

					return std::string_view{ str };
				}

				return constants::dto::GMOD_DTO_UNKNOWN_VERSION;
			}
			catch ( ... )
			{
				return constants::dto::GMOD_DTO_UNKNOWN_VERSION;
			}
		}

		inline std::string_view extractNameHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				const auto codeIt = json.find( constants::dto::GMOD_DTO_KEY_CODE );
				if ( codeIt != json.end() && codeIt->is_string() )
				{
					const auto& str = codeIt->get_ref<const std::string&>();

					return std::string_view{ str };
				}

				return constants::dto::GMOD_DTO_UNKNOWN_CODE;
			}
			catch ( ... )
			{
				return constants::dto::GMOD_DTO_UNKNOWN_CODE;
			}
		}
	}

	//=====================================================================
	// GMOD Node Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<GmodNodeDto> GmodNodeDto::tryFromJson( const nlohmann::json& json )
	{
		const auto codeHint = GmodDtoInternal::extractNameHint( json );

		try
		{
			const auto codeIt = json.find( constants::dto::GMOD_DTO_KEY_CODE );
			if ( codeIt == json.end() || !codeIt->is_string() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "ERROR: GMOD Node JSON missing required '" );
				builder.append( constants::dto::GMOD_DTO_KEY_CODE );
				builder.append( "' field or not a string\n" );
				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}
			const auto categoryIt = json.find( constants::dto::GMOD_DTO_KEY_CATEGORY );
			if ( categoryIt == json.end() || !categoryIt->is_string() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "ERROR: GMOD Node JSON missing required '" );
				builder.append( constants::dto::GMOD_DTO_KEY_CATEGORY );
				builder.append( "' field or not a string\n" );

				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}
			const auto typeIt = json.find( constants::dto::GMOD_DTO_KEY_TYPE );
			if ( typeIt == json.end() || !typeIt->is_string() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "ERROR: GMOD Node JSON missing required '" );
				builder.append( constants::dto::GMOD_DTO_KEY_TYPE );
				builder.append( "' field or not a string\n" );

				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}

			std::string tempCode = codeIt->get<std::string>();
			std::string tempCategory = categoryIt->get<std::string>();
			std::string tempType = typeIt->get<std::string>();

			std::string tempName;
			const auto nameIt = json.find( constants::dto::GMOD_DTO_KEY_NAME );
			if ( nameIt != json.end() )
			{
				if ( nameIt->is_string() )
				{
					tempName = nameIt->get<std::string>();
				}
				else
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "ERROR: GMOD Node JSON (code='" );
					builder.append( tempCode );
					builder.append( "') field '" );
					builder.append( constants::dto::GMOD_DTO_KEY_NAME );
					builder.append( "' is present but not a string\n" );

					fmt::print( stderr, "{}", lease.toString() );

					return std::nullopt;
				}
			}
			else
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: GMOD Node JSON (code='" );
				builder.append( tempCode );
				builder.append( "') missing '" );
				builder.append( constants::dto::GMOD_DTO_KEY_NAME );
				builder.append( "' field. Defaulting name to empty string.\n" );

				fmt::print( stderr, "{}", lease.toString() );

				tempName = "";
			}

			if ( tempCode.empty() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: Empty code field found in GMOD node\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}
			if ( tempCategory.empty() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: Empty category field found in GMOD node code='" );
				builder.append( tempCode );
				builder.append( "'\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}
			if ( tempType.empty() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: Empty type field found in GMOD node code='" );
				builder.append( tempCode );
				builder.append( "'\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}
			if ( tempName.empty() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: Empty name field used for GMOD node code='" );
				builder.append( tempCode );
				builder.append( "'\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}

			std::optional<std::string> tempCommonName = std::nullopt;
			const auto commonNameIt = json.find( constants::dto::GMOD_DTO_KEY_COMMON_NAME );
			if ( commonNameIt != json.end() )
			{
				if ( commonNameIt->is_string() )
				{
					tempCommonName = commonNameIt->get<std::string>();
				}
				else if ( !commonNameIt->is_null() )
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "WARN: GMOD Node code='" );
					builder.append( tempCode );
					builder.append( "' has non-string '" );
					builder.append( constants::dto::GMOD_DTO_KEY_COMMON_NAME );
					builder.append( "'\n" );

					fmt::print( stderr, "{}", lease.toString() );
				}
			}

			std::optional<std::string> tempDefinition = std::nullopt;
			const auto definitionIt = json.find( constants::dto::GMOD_DTO_KEY_DEFINITION );
			if ( definitionIt != json.end() )
			{
				if ( definitionIt->is_string() )
				{
					tempDefinition = definitionIt->get<std::string>();
				}
				else if ( !definitionIt->is_null() )
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "WARN: GMOD Node code='" );
					builder.append( tempCode );
					builder.append( "' has non-string '" );
					builder.append( constants::dto::GMOD_DTO_KEY_DEFINITION );
					builder.append( "'\n" );

					fmt::print( stderr, "{}", lease.toString() );
				}
			}

			std::optional<std::string> tempCommonDefinition = std::nullopt;
			const auto commonDefinitionIt = json.find( constants::dto::GMOD_DTO_KEY_COMMON_DEFINITION );
			if ( commonDefinitionIt != json.end() )
			{
				if ( commonDefinitionIt->is_string() )
				{
					tempCommonDefinition = commonDefinitionIt->get<std::string>();
				}
				else if ( !commonDefinitionIt->is_null() )
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "WARN: GMOD Node code='" );
					builder.append( tempCode );
					builder.append( "' has non-string '" );
					builder.append( constants::dto::GMOD_DTO_KEY_COMMON_DEFINITION );
					builder.append( "'\n" );

					fmt::print( stderr, "{}", lease.toString() );
				}
			}

			std::optional<bool> tempInstallSubstructure = std::nullopt;
			const auto installIt = json.find( constants::dto::GMOD_DTO_KEY_INSTALL_SUBSTRUCTURE );
			if ( installIt != json.end() )
			{
				if ( installIt->is_boolean() )
				{
					tempInstallSubstructure = installIt->get<bool>();
				}
				else if ( !installIt->is_null() )
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "WARN: GMOD Node code='" );
					builder.append( tempCode );
					builder.append( "' has non-bool '" );
					builder.append( constants::dto::GMOD_DTO_KEY_INSTALL_SUBSTRUCTURE );
					builder.append( "'\n" );

					fmt::print( stderr, "{}", lease.toString() );
				}
			}

			std::optional<NormalAssignmentNamesMap> tempNormalAssignmentNames = std::nullopt;
			const auto assignmentIt = json.find( constants::dto::GMOD_DTO_KEY_NORMAL_ASSIGNMENT_NAMES );
			if ( assignmentIt != json.end() )
			{
				if ( assignmentIt->is_object() )
				{
					try
					{
						auto assignments = assignmentIt->get<NormalAssignmentNamesMap>();
						if ( !assignments.empty() )
						{
							tempNormalAssignmentNames = std::move( assignments );
						}
					}
					catch ( const nlohmann::json::exception& ex )
					{
						auto lease = internal::StringBuilderPool::instance();
						auto builder = lease.builder();
						builder.append( "WARN: GMOD Node code='" );
						builder.append( tempCode );
						builder.append( "' failed to parse '" );
						builder.append( constants::dto::GMOD_DTO_KEY_NORMAL_ASSIGNMENT_NAMES );
						builder.append( "' object: " );
						builder.append( ex.what() );
						builder.append( "\n" );

						fmt::print( stderr, "{}", lease.toString() );
					}
				}
				else if ( !assignmentIt->is_null() )
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "WARN: GMOD Node code='" );
					builder.append( tempCode );
					builder.append( "' has non-object '" );
					builder.append( constants::dto::GMOD_DTO_KEY_NORMAL_ASSIGNMENT_NAMES );
					builder.append( "'\n" );

					fmt::print( stderr, "{}", lease.toString() );
				}
			}

			/* Construct the final DTO using successfully parsed data */
			GmodNodeDto result( std::move( tempCategory ),
				std::move( tempType ),
				std::move( tempCode ),
				std::move( tempName ),
				std::move( tempCommonName ),
				std::move( tempDefinition ),
				std::move( tempCommonDefinition ),
				tempInstallSubstructure,
				std::move( tempNormalAssignmentNames ) );

			return result;
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during GmodNodeDto parsing (hint: code='" );
			builder.append( codeHint );
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
			builder.append( "ERROR: Standard exception during GmodNodeDto parsing (hint: code='" );
			builder.append( codeHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
	}

	GmodNodeDto GmodNodeDto::fromJson( const nlohmann::json& json )
	{
		const auto codeHint = GmodDtoInternal::extractNameHint( json );
		auto dtoOpt = GmodNodeDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Failed to deserialize GmodNodeDto from JSON (hint: code='" );
			builder.append( codeHint );
			builder.append( "')" );

			throw std::invalid_argument( lease.toString() );
		}

		return std::move( dtoOpt.value() );
	}

	nlohmann::json GmodNodeDto::toJson() const
	{
		nlohmann::json result;
		to_json( result, *this );

		return result;
	}

	//----------------------------------------------
	// Private serialization methods
	//----------------------------------------------

	void from_json( const nlohmann::json& j, GmodNodeDto& dto )
	{
		/* ADL hook for nlohmann::json deserialization. */
		const auto categoryIt = j.find( constants::dto::GMOD_DTO_KEY_CATEGORY );
		if ( categoryIt == j.end() || !categoryIt->is_string() )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "GMOD Node JSON missing required '" );
			builder.append( constants::dto::GMOD_DTO_KEY_CATEGORY );
			builder.append( "' field or not a string" );

			throw nlohmann::json::parse_error::create( 101, 0u, lease.toString(), &j );
		}

		const auto typeIt = j.find( constants::dto::GMOD_DTO_KEY_TYPE );
		if ( typeIt == j.end() || !typeIt->is_string() )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "GMOD Node JSON missing required '" );
			builder.append( constants::dto::GMOD_DTO_KEY_TYPE );
			builder.append( "' field or not a string" );

			throw nlohmann::json::parse_error::create( 102, 0u, lease.toString(), &j );
		}

		const auto codeIt = j.find( constants::dto::GMOD_DTO_KEY_CODE );
		if ( codeIt == j.end() || !codeIt->is_string() )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "GMOD Node JSON missing required '" );
			builder.append( constants::dto::GMOD_DTO_KEY_CODE );
			builder.append( "' field or not a string" );

			throw nlohmann::json::parse_error::create( 103, 0u, lease.toString(), &j );
		}

		/* Extract required fields with cached iterators */
		std::string tempCategory = categoryIt->get<std::string>();
		std::string tempType = typeIt->get<std::string>();
		std::string tempCode = codeIt->get<std::string>();

		/* SPECIAL CASE: Name field handling - matches tryFromJson logic */
		std::string tempName;
		const auto nameIt = j.find( constants::dto::GMOD_DTO_KEY_NAME );
		if ( nameIt != j.end() )
		{
			if ( nameIt->is_string() )
			{
				tempName = nameIt->get<std::string>();
			}
			else if ( nameIt->is_null() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: GMOD Node JSON (code='" );
				builder.append( tempCode );
				builder.append( "') has null '" );
				builder.append( constants::dto::GMOD_DTO_KEY_NAME );
				builder.append( "' field. Defaulting name to empty string.\n" );
				fmt::print( stderr, "{}", lease.toString() );
				tempName = "";
			}
			else
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: GMOD Node JSON (code='" );
				builder.append( tempCode );
				builder.append( "') has non-string '" );
				builder.append( constants::dto::GMOD_DTO_KEY_NAME );
				builder.append( "' field. Defaulting name to empty string.\n" );
				fmt::print( stderr, "{}", lease.toString() );
				tempName = "";
			}
		}
		else
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "WARN: GMOD Node JSON (code='" );
			builder.append( tempCode );
			builder.append( "') missing '" );
			builder.append( constants::dto::GMOD_DTO_KEY_NAME );
			builder.append( "' field. Defaulting name to empty string.\n" );

			fmt::print( stderr, "{}", lease.toString() );
			tempName = "";
		}

		/* Optional fields with robust validation (matches tryFromJson pattern) */
		std::optional<std::string> tempCommonName = std::nullopt;
		const auto commonNameIt = j.find( constants::dto::GMOD_DTO_KEY_COMMON_NAME );
		if ( commonNameIt != j.end() )
		{
			if ( commonNameIt->is_string() )
			{
				tempCommonName = commonNameIt->get<std::string>();
			}
			else if ( !commonNameIt->is_null() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: GMOD Node code='" );
				builder.append( tempCode );
				builder.append( "' has non-string '" );
				builder.append( constants::dto::GMOD_DTO_KEY_COMMON_NAME );
				builder.append( "' in from_json\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}
		}

		std::optional<std::string> tempDefinition = std::nullopt;
		const auto definitionIt = j.find( constants::dto::GMOD_DTO_KEY_DEFINITION );
		if ( definitionIt != j.end() )
		{
			if ( definitionIt->is_string() )
			{
				tempDefinition = definitionIt->get<std::string>();
			}
			else if ( !definitionIt->is_null() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: GMOD Node code='" );
				builder.append( tempCode );
				builder.append( "' has non-string '" );
				builder.append( constants::dto::GMOD_DTO_KEY_DEFINITION );
				builder.append( "' in from_json\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}
		}

		std::optional<std::string> tempCommonDefinition = std::nullopt;
		const auto commonDefinitionIt = j.find( constants::dto::GMOD_DTO_KEY_COMMON_DEFINITION );
		if ( commonDefinitionIt != j.end() )
		{
			if ( commonDefinitionIt->is_string() )
			{
				tempCommonDefinition = commonDefinitionIt->get<std::string>();
			}
			else if ( !commonDefinitionIt->is_null() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: GMOD Node code='" );
				builder.append( tempCode );
				builder.append( "' has non-string '" );
				builder.append( constants::dto::GMOD_DTO_KEY_COMMON_DEFINITION );
				builder.append( "' in from_json\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}
		}

		std::optional<bool> tempInstallSubstructure = std::nullopt;
		const auto installIt = j.find( constants::dto::GMOD_DTO_KEY_INSTALL_SUBSTRUCTURE );
		if ( installIt != j.end() )
		{
			if ( installIt->is_boolean() )
			{
				tempInstallSubstructure = installIt->get<bool>();
			}
			else if ( !installIt->is_null() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: GMOD Node code='" );
				builder.append( tempCode );
				builder.append( "' has non-bool '" );
				builder.append( constants::dto::GMOD_DTO_KEY_INSTALL_SUBSTRUCTURE );
				builder.append( "' in from_json\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}
		}

		std::optional<GmodNodeDto::NormalAssignmentNamesMap> tempNormalAssignmentNames = std::nullopt;
		const auto assignmentIt = j.find( constants::dto::GMOD_DTO_KEY_NORMAL_ASSIGNMENT_NAMES );
		if ( assignmentIt != j.end() )
		{
			if ( assignmentIt->is_object() )
			{
				try
				{
					auto assignments = assignmentIt->get<GmodNodeDto::NormalAssignmentNamesMap>();
					if ( !assignments.empty() )
					{
						tempNormalAssignmentNames = std::move( assignments );
					}
				}
				catch ( const nlohmann::json::exception& ex )
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "WARN: GMOD Node code='" );
					builder.append( tempCode );
					builder.append( "' failed to parse '" );
					builder.append( constants::dto::GMOD_DTO_KEY_NORMAL_ASSIGNMENT_NAMES );
					builder.append( "' object in from_json: " );
					builder.append( ex.what() );
					builder.append( "\n" );

					fmt::print( stderr, "{}", lease.toString() );
				}
			}
			else if ( !assignmentIt->is_null() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: GMOD Node code='" );
				builder.append( tempCode );
				builder.append( "' has non-object '" );
				builder.append( constants::dto::GMOD_DTO_KEY_NORMAL_ASSIGNMENT_NAMES );
				builder.append( "' in from_json\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}
		}

		/* Assign all fields to DTO using successfully parsed data */
		dto.m_category = std::move( tempCategory );
		dto.m_type = std::move( tempType );
		dto.m_code = std::move( tempCode );
		dto.m_name = std::move( tempName );
		dto.m_commonName = std::move( tempCommonName );
		dto.m_definition = std::move( tempDefinition );
		dto.m_commonDefinition = std::move( tempCommonDefinition );
		dto.m_installSubstructure = tempInstallSubstructure;
		dto.m_normalAssignmentNames = std::move( tempNormalAssignmentNames );
	}

	void to_json( nlohmann::json& j, const GmodNodeDto& dto )
	{
		/* ADL hook for nlohmann::json serialization. */
		j = { { constants::dto::GMOD_DTO_KEY_CATEGORY, dto.category() }, { constants::dto::GMOD_DTO_KEY_TYPE, dto.type() },
			{ constants::dto::GMOD_DTO_KEY_CODE, dto.code() }, { constants::dto::GMOD_DTO_KEY_NAME, dto.name() } };

		if ( dto.commonName().has_value() )
		{
			j[constants::dto::GMOD_DTO_KEY_COMMON_NAME] = dto.commonName().value();
		}
		if ( dto.definition().has_value() )
		{
			j[constants::dto::GMOD_DTO_KEY_DEFINITION] = dto.definition().value();
		}
		if ( dto.commonDefinition().has_value() )
		{
			j[constants::dto::GMOD_DTO_KEY_COMMON_DEFINITION] = dto.commonDefinition().value();
		}
		if ( dto.installSubstructure().has_value() )
		{
			j[constants::dto::GMOD_DTO_KEY_INSTALL_SUBSTRUCTURE] = dto.installSubstructure().value();
		}
		if ( dto.normalAssignmentNames().has_value() )
		{
			j[constants::dto::GMOD_DTO_KEY_NORMAL_ASSIGNMENT_NAMES] = dto.normalAssignmentNames().value();
		}
	}

	//=====================================================================
	// GMOD Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<GmodDto> GmodDto::tryFromJson( const nlohmann::json& json )
	{
		const auto visHint = GmodDtoInternal::extractVisHint( json );

		try
		{
			const auto visReleaseIt = json.find( constants::dto::GMOD_DTO_KEY_VIS_RELEASE );
			if ( visReleaseIt == json.end() || !visReleaseIt->is_string() )
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "ERROR: GMOD JSON missing required '" );
				builder.append( constants::dto::GMOD_DTO_KEY_VIS_RELEASE );
				builder.append( "' field or not a string\n" );

				fmt::print( stderr, "{}", lease.toString() );

				return std::nullopt;
			}

			std::string tempVisVersion = visReleaseIt->get<std::string>();

			Items tempItems;
			size_t totalItems = 0;
			size_t successCount = 0;

			const auto itemsIt = json.find( constants::dto::GMOD_DTO_KEY_ITEMS );
			if ( itemsIt != json.end() )
			{
				if ( !itemsIt->is_array() )
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "WARN: '" );
					builder.append( constants::dto::GMOD_DTO_KEY_ITEMS );
					builder.append( "' field is not an array for VIS version " );
					builder.append( tempVisVersion );
					builder.append( "\n" );

					fmt::print( stderr, "{}", lease.toString() );
				}
				else
				{
					const auto& itemsArray = *itemsIt;
					totalItems = itemsArray.size();
					tempItems.reserve( totalItems + totalItems / 8 );

					for ( const auto& itemJson : itemsArray )
					{
						auto nodeOpt = GmodNodeDto::tryFromJson( itemJson );
						if ( nodeOpt.has_value() )
						{
							tempItems.emplace_back( std::move( *nodeOpt ) );
							successCount++;
						}
						else
						{
							auto lease = internal::StringBuilderPool::instance();
							auto builder = lease.builder();
							builder.append( "WARN: Skipping malformed GMOD node during GmodDto parsing for VIS version " );
							builder.append( tempVisVersion );
							builder.append( "\n" );
							fmt::print( stderr, "{}", lease.toString() );
						}
					}

					/* If parsing failed for more than 10% of items, shrink the vector to potentially save memory */
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
				builder.append( constants::dto::GMOD_DTO_KEY_ITEMS );
				builder.append( "' array found in GmodDto for VIS version " );
				builder.append( tempVisVersion );
				builder.append( "\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}

			Relations tempRelations;
			size_t relationCount = 0;
			size_t validRelationCount = 0;

			const auto relationsIt = json.find( constants::dto::GMOD_DTO_KEY_RELATIONS );
			if ( relationsIt != json.end() )
			{
				if ( !relationsIt->is_array() )
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "WARN: '" );
					builder.append( constants::dto::GMOD_DTO_KEY_RELATIONS );
					builder.append( "' field is not an array for VIS version " );
					builder.append( tempVisVersion );
					builder.append( "\n" );

					fmt::print( stderr, "{}", lease.toString() );
				}
				else
				{
					const auto& relationsArray = *relationsIt;
					relationCount = relationsArray.size();
					tempRelations.reserve( relationCount + relationCount / 8 );

					for ( const auto& relationJson : relationsArray )
					{
						if ( relationJson.is_array() )
						{
							Relation relationPair;
							relationPair.reserve( relationJson.size() + 1 );
							bool validPair = true;

							for ( const auto& relJson : relationJson )
							{
								if ( relJson.is_string() )
								{
									relationPair.emplace_back( relJson.get<std::string>() );
								}
								else
								{
									auto lease = internal::StringBuilderPool::instance();
									auto builder = lease.builder();
									builder.append( "WARN: Non-string value found in relation entry for VIS version " );
									builder.append( tempVisVersion );
									builder.append( "\n" );

									fmt::print( stderr, "{}", lease.toString() );
									validPair = false;
									break;
								}
							}

							if ( validPair && !relationPair.empty() )
							{
								tempRelations.emplace_back( std::move( relationPair ) );
								validRelationCount++;
							}
						}
						else
						{
							auto lease = internal::StringBuilderPool::instance();
							auto builder = lease.builder();
							builder.append( "WARN: Non-array entry found in '" );
							builder.append( constants::dto::GMOD_DTO_KEY_RELATIONS );
							builder.append( "' array for VIS version " );
							builder.append( tempVisVersion );
							builder.append( "\n" );

							fmt::print( stderr, "{}", lease.toString() );
						}
					}

					/* If parsing failed for more than 10% of relations, shrink the vector to potentially save memory */
					if ( relationCount > 0 && validRelationCount < relationCount * 9 / 10 )
					{
						if ( tempRelations.capacity() > tempRelations.size() * 4 / 3 )
						{
							tempRelations.shrink_to_fit();
						}
					}
				}
			}
			else
			{
				auto lease = internal::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "WARN: No '" );
				builder.append( constants::dto::GMOD_DTO_KEY_RELATIONS );
				builder.append( "' array found in GmodDto for VIS version " );
				builder.append( tempVisVersion );
				builder.append( "\n" );

				fmt::print( stderr, "{}", lease.toString() );
			}

			if ( tempItems.size() > 10000 )
			{
			}

			/* Construct the final DTO using successfully parsed data */
			GmodDto resultDto( std::move( tempVisVersion ), std::move( tempItems ), std::move( tempRelations ) );

			return resultDto;
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during GmodDto parsing (hint: visRelease='" );
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
			builder.append( "ERROR: Standard exception during GmodDto parsing (hint: visRelease='" );
			builder.append( visHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			fmt::print( stderr, "{}", lease.toString() );

			return std::nullopt;
		}
	}

	GmodDto GmodDto::fromJson( const nlohmann::json& json )
	{
		const auto visHint = GmodDtoInternal::extractVisHint( json );
		auto dtoOpt = GmodDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Failed to deserialize GmodDto from JSON (hint: visRelease='" );
			builder.append( visHint );
			builder.append( "')" );

			throw std::invalid_argument( lease.toString() );
		}

		return std::move( dtoOpt.value() );
	}

	nlohmann::json GmodDto::toJson() const
	{
		nlohmann::json result;
		to_json( result, *this );

		return result;
	}

	//-------------------------------------------------------------------
	// Private serialization methods
	//-------------------------------------------------------------------

	void from_json( const nlohmann::json& j, GmodDto& dto )
	{
		/* ADL hook for nlohmann::json deserialization. */
		const auto visIt = j.find( constants::dto::GMOD_DTO_KEY_VIS_RELEASE );
		if ( visIt == j.end() || !visIt->is_string() )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "GMOD JSON missing required '" );
			builder.append( constants::dto::GMOD_DTO_KEY_VIS_RELEASE );
			builder.append( "' field or not a string" );

			throw nlohmann::json::parse_error::create( 201, 0u, lease.toString(), &j );
		}

		const auto itemsIt = j.find( constants::dto::GMOD_DTO_KEY_ITEMS );
		if ( itemsIt == j.end() || !itemsIt->is_array() )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "GMOD JSON missing required '" );
			builder.append( constants::dto::GMOD_DTO_KEY_ITEMS );
			builder.append( "' field or not an array" );

			throw nlohmann::json::parse_error::create( 202, 0u, lease.toString(), &j );
		}

		const auto relationsIt = j.find( constants::dto::GMOD_DTO_KEY_RELATIONS );
		if ( relationsIt == j.end() || !relationsIt->is_array() )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "GMOD JSON missing required '" );
			builder.append( constants::dto::GMOD_DTO_KEY_RELATIONS );
			builder.append( "' field or not an array" );

			throw nlohmann::json::parse_error::create( 203, 0u, lease.toString(), &j );
		}

		/* Extract required fields with cached iterators */
		std::string tempVisVersion = visIt->get<std::string>();

		GmodDto::Items tempItems;
		if ( itemsIt != j.end() && itemsIt->is_array() )
		{
			const auto& itemsArray = *itemsIt;
			tempItems.reserve( itemsArray.size() );

			for ( const auto& itemJson : itemsArray )
			{
				auto itemDtoOpt = GmodNodeDto::tryFromJson( itemJson );
				if ( itemDtoOpt.has_value() )
				{
					tempItems.emplace_back( std::move( itemDtoOpt.value() ) );
				}
				else
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "ERROR: Failed to parse GMOD item in collection during from_json" );

					throw nlohmann::json::parse_error::create( 203, 0u, lease.toString(), &itemJson );
				}
			}
			tempItems.shrink_to_fit();
		}

		GmodDto::Relations tempRelations;
		if ( relationsIt != j.end() && relationsIt->is_array() )
		{
			const auto& relationsArray = *relationsIt;
			tempRelations.reserve( relationsArray.size() );

			for ( const auto& relationJson : relationsArray )
			{
				if ( relationJson.is_array() )
				{
					try
					{
						GmodDto::Relation relation = relationJson.get<GmodDto::Relation>();
						tempRelations.emplace_back( std::move( relation ) );
					}
					catch ( const nlohmann::json::exception& ex )
					{
						auto lease = internal::StringBuilderPool::instance();
						auto builder = lease.builder();
						builder.append( "ERROR: Failed to parse GMOD relation in collection during from_json: " );
						builder.append( ex.what() );

						throw nlohmann::json::parse_error::create( 203, 0u, lease.toString(), &relationJson );
					}
				}
				else
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "ERROR: GMOD relation must be an array during from_json" );

					throw nlohmann::json::parse_error::create( 203, 0u, lease.toString(), &relationJson );
				}
			}
			tempRelations.shrink_to_fit();
		}

		/* Assign all fields to DTO using successfully parsed data */
		dto.m_visVersion = std::move( tempVisVersion );
		dto.m_items = std::move( tempItems );
		dto.m_relations = std::move( tempRelations );
	}

	void to_json( nlohmann::json& j, const GmodDto& dto )
	{
		/* ADL hook for nlohmann::json serialization. */
		j = { { constants::dto::GMOD_DTO_KEY_VIS_RELEASE, dto.visVersion() },
			{ constants::dto::GMOD_DTO_KEY_ITEMS, dto.items() },
			{ constants::dto::GMOD_DTO_KEY_RELATIONS, dto.relations() } };
	}
}
