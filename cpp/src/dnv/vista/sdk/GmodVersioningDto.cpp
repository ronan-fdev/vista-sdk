/**
 * @file GmodVersioningDto.cpp
 * @brief Implementation of GMOD versioning data transfer objects
 */

#include "pch.h"

#include "dnv/vista/sdk/GmodVersioningDto.h"

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// Constants
		//=====================================================================

		static constexpr const char* VIS_RELEASE_KEY = "visRelease";
		static constexpr const char* ITEMS_KEY = "items";

		static constexpr const char* OLD_ASSIGNMENT_KEY = "oldAssignment";
		static constexpr const char* CURRENT_ASSIGNMENT_KEY = "currentAssignment";
		static constexpr const char* NEW_ASSIGNMENT_KEY = "newAssignment";
		static constexpr const char* DELETE_ASSIGNMENT_KEY = "deleteAssignment";

		static constexpr const char* OPERATIONS_KEY = "operations";
		static constexpr const char* SOURCE_KEY = "source";
		static constexpr const char* TARGET_KEY = "target";

		//=====================================================================
		// Helper functions
		//=====================================================================

		static const std::string& internString( const std::string& value )
		{
			if ( value.length() <= 4 )
			{
				static std::unordered_map<std::string, std::string> shortCache;
				static std::mutex shortMutex;

				std::lock_guard<std::mutex> lock( shortMutex );
				auto [it, inserted] = shortCache.try_emplace( value, value );
				return it->second;
			}

			if ( value.length() > 30 )
			{
				return value;
			}

			static std::unordered_map<std::string, std::string> cache;
			static std::mutex cacheMutex;

			std::lock_guard<std::mutex> lock( cacheMutex );
			auto [it, inserted] = cache.try_emplace( value, value );
			return it->second;
		}
	}

	//=====================================================================
	// GMOD Versioning Assignement Data Transfer Objects
	//=====================================================================

	//----------------------------------------------
	// Construction / destruction
	//----------------------------------------------

	GmodVersioningAssignmentChangeDto::GmodVersioningAssignmentChangeDto( std::string oldAssignment, std::string currentAssignment )
		: m_oldAssignment{ std::move( oldAssignment ) },
		  m_currentAssignment{ std::move( currentAssignment ) }
	{
	}

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<GmodVersioningAssignmentChangeDto> GmodVersioningAssignmentChangeDto::tryFromJson( const nlohmann::json& json )
	{
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
			SPDLOG_ERROR( "nlohmann::json exception during GmodVersioningAssignmentChangeDto parsing: {}", ex.what() );

			return std::nullopt;
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			SPDLOG_ERROR( "Standard exception during GmodVersioningAssignmentChangeDto parsing: {}", ex.what() );

			return std::nullopt;
		}
	}

	GmodVersioningAssignmentChangeDto GmodVersioningAssignmentChangeDto::fromJson( const nlohmann::json& json )
	{
		try
		{
			return json.get<GmodVersioningAssignmentChangeDto>();
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			throw std::invalid_argument( fmt::format( "Failed to deserialize GmodVersioningAssignmentChangeDto from JSON: {}", ex.what() ) );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			throw std::invalid_argument( fmt::format( "Failed to deserialize GmodVersioningAssignmentChangeDto from JSON: {}", ex.what() ) );
		}
	}

	nlohmann::json GmodVersioningAssignmentChangeDto::toJson() const
	{
		return *this;
	}

	//----------------------------------------------
	// Private serialization methods
	//----------------------------------------------

	void to_json( nlohmann::json& j, const GmodVersioningAssignmentChangeDto& dto )
	{
		j = nlohmann::json{
			{ OLD_ASSIGNMENT_KEY, dto.m_oldAssignment },
			{ CURRENT_ASSIGNMENT_KEY, dto.m_currentAssignment } };
	}

	void from_json( const nlohmann::json& j, GmodVersioningAssignmentChangeDto& dto )
	{
		if ( !j.contains( OLD_ASSIGNMENT_KEY ) || !j.at( OLD_ASSIGNMENT_KEY ).is_string() )
		{
			throw nlohmann::json::parse_error::create( 101, 0u, fmt::format( "GmodVersioningAssignmentChangeDto JSON missing required '{}' field or not a string", OLD_ASSIGNMENT_KEY ), nullptr );
		}
		if ( !j.contains( CURRENT_ASSIGNMENT_KEY ) || !j.at( CURRENT_ASSIGNMENT_KEY ).is_string() )
		{
			throw nlohmann::json::parse_error::create( 101, 0u, fmt::format( "GmodVersioningAssignmentChangeDto JSON missing required '{}' field or not a string", CURRENT_ASSIGNMENT_KEY ), nullptr );
		}

		dto.m_oldAssignment = internString( j.at( OLD_ASSIGNMENT_KEY ).get<std::string>() );
		dto.m_currentAssignment = internString( j.at( CURRENT_ASSIGNMENT_KEY ).get<std::string>() );

		if ( dto.m_oldAssignment.empty() )
		{
			SPDLOG_WARN( "Empty 'oldAssignment' field found in GmodVersioningAssignmentChangeDto" );
		}
		if ( dto.m_currentAssignment.empty() )
		{
			SPDLOG_WARN( "Empty 'currentAssignment' field found in GmodVersioningAssignmentChangeDto" );
		}
	}

	//=====================================================================
	// GMOD Node Conversion Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Construction / destruction
	//----------------------------------------------

	GmodNodeConversionDto::GmodNodeConversionDto(
		OperationSet operations, std::string source,
		std::string target, std::string oldAssignment,
		std::string newAssignment, bool deleteAssignment )
		: m_operations{ std::move( operations ) },
		  m_source{ std::move( source ) },
		  m_target{ std::move( target ) },
		  m_oldAssignment{ std::move( oldAssignment ) },
		  m_newAssignment{ std::move( newAssignment ) },
		  m_deleteAssignment{ deleteAssignment }
	{
	}

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<GmodNodeConversionDto> GmodNodeConversionDto::tryFromJson( const nlohmann::json& json )
	{
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
			SPDLOG_ERROR( "nlohmann::json exception during GmodNodeConversionDto parsing: {}", ex.what() );

			return std::nullopt;
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			SPDLOG_ERROR( "Standard exception during GmodNodeConversionDto parsing: {}", ex.what() );

			return std::nullopt;
		}
	}

	GmodNodeConversionDto GmodNodeConversionDto::fromJson( const nlohmann::json& json )
	{
		try
		{
			return json.get<GmodNodeConversionDto>();
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			throw std::invalid_argument( fmt::format( "Failed to deserialize GmodNodeConversionDto from JSON: {}", ex.what() ) );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			throw std::invalid_argument( fmt::format( "Failed to deserialize GmodNodeConversionDto from JSON: {}", ex.what() ) );
		}
	}

	nlohmann::json GmodNodeConversionDto::toJson() const
	{
		nlohmann::json j = *this;
		return j;
	}

	//----------------------------------------------
	// Private serialization methods
	//----------------------------------------------

	void to_json( nlohmann::json& j, const GmodNodeConversionDto& dto )
	{
		j = nlohmann::json{
			{ OPERATIONS_KEY, dto.m_operations },
			{ SOURCE_KEY, dto.m_source },
			{ TARGET_KEY, dto.m_target },
			{ OLD_ASSIGNMENT_KEY, dto.m_oldAssignment },
			{ NEW_ASSIGNMENT_KEY, dto.m_newAssignment },
			{ DELETE_ASSIGNMENT_KEY, dto.m_deleteAssignment } };
	}

	void from_json( const nlohmann::json& j, GmodNodeConversionDto& dto )
	{
		if ( j.contains( OPERATIONS_KEY ) )
		{
			if ( !j.at( OPERATIONS_KEY ).is_array() )
			{
				throw nlohmann::json::type_error::create( 302, fmt::format( "GmodNodeConversionDto JSON field '{}' is not an array", OPERATIONS_KEY ), nullptr );
			}
			dto.m_operations.clear();
			const auto& opsArray = j.at( OPERATIONS_KEY );
			dto.m_operations.reserve( opsArray.size() );
			for ( const auto& op : opsArray )
			{
				if ( !op.is_string() )
				{
					throw nlohmann::json::type_error::create( 302, fmt::format( "GmodNodeConversionDto JSON field '{}' contains non-string element", OPERATIONS_KEY ), nullptr );
				}
				dto.m_operations.insert( internString( op.get<std::string>() ) );
			}
		}
		else
		{
			dto.m_operations.clear();
			SPDLOG_WARN( "GmodNodeConversionDto JSON missing optional '{}' field", OPERATIONS_KEY );
		}

		if ( j.contains( SOURCE_KEY ) )
		{
			if ( !j.at( SOURCE_KEY ).is_string() )
			{
				throw nlohmann::json::type_error::create( 302, fmt::format( "GmodNodeConversionDto JSON field '{}' is not a string", SOURCE_KEY ), nullptr );
			}
			dto.m_source = internString( j.at( SOURCE_KEY ).get<std::string>() );
		}
		else
		{
			dto.m_source.clear();
			SPDLOG_WARN( "GmodNodeConversionDto JSON missing optional '{}' field", SOURCE_KEY );
		}

		if ( j.contains( TARGET_KEY ) )
		{
			if ( !j.at( TARGET_KEY ).is_string() )
			{
				throw nlohmann::json::type_error::create( 302, fmt::format( "GmodNodeConversionDto JSON field '{}' is not a string", TARGET_KEY ), nullptr );
			}
			dto.m_target = internString( j.at( TARGET_KEY ).get<std::string>() );
		}
		else
		{
			dto.m_target.clear();
			SPDLOG_WARN( "GmodNodeConversionDto JSON missing optional '{}' field", TARGET_KEY );
		}

		if ( j.contains( OLD_ASSIGNMENT_KEY ) )
		{
			if ( !j.at( OLD_ASSIGNMENT_KEY ).is_string() )
			{
				throw nlohmann::json::type_error::create( 302, fmt::format( "GmodNodeConversionDto JSON field '{}' is not a string", OLD_ASSIGNMENT_KEY ), nullptr );
			}
			dto.m_oldAssignment = internString( j.at( OLD_ASSIGNMENT_KEY ).get<std::string>() );
		}
		else
		{
			dto.m_oldAssignment.clear();
			SPDLOG_DEBUG( "GmodNodeConversionDto JSON missing optional '{}' field", OLD_ASSIGNMENT_KEY );
		}

		if ( j.contains( NEW_ASSIGNMENT_KEY ) )
		{
			if ( !j.at( NEW_ASSIGNMENT_KEY ).is_string() )
			{
				throw nlohmann::json::type_error::create( 302, fmt::format( "GmodNodeConversionDto JSON field '{}' is not a string", NEW_ASSIGNMENT_KEY ), nullptr );
			}
			dto.m_newAssignment = internString( j.at( NEW_ASSIGNMENT_KEY ).get<std::string>() );
		}
		else
		{
			dto.m_newAssignment.clear();
			SPDLOG_DEBUG( "GmodNodeConversionDto JSON missing optional '{}' field", NEW_ASSIGNMENT_KEY );
		}

		if ( j.contains( DELETE_ASSIGNMENT_KEY ) )
		{
			if ( !j.at( DELETE_ASSIGNMENT_KEY ).is_boolean() )
			{
				throw nlohmann::json::type_error::create( 302, fmt::format( "GmodNodeConversionDto JSON field '{}' is not a boolean", DELETE_ASSIGNMENT_KEY ), nullptr );
			}
			dto.m_deleteAssignment = j.at( DELETE_ASSIGNMENT_KEY ).get<bool>();
		}
		else
		{
			dto.m_deleteAssignment = false;
			SPDLOG_DEBUG( "GmodNodeConversionDto JSON missing optional '{}' field, defaulting to false", DELETE_ASSIGNMENT_KEY );
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

	//=====================================================================
	// GMOD Versioning Data Transfer Object
	//=====================================================================

	//----------------------------------------------
	// Construction / destruction
	//----------------------------------------------

	GmodVersioningDto::GmodVersioningDto( std::string visVersion, ItemsMap items )
		: m_visVersion{ std::move( visVersion ) },
		  m_items{ std::move( items ) }
	{
	}

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<GmodVersioningDto> GmodVersioningDto::tryFromJson( const nlohmann::json& json )
	{
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
			std::string visHint = "[unknown version]";
			try
			{
				if ( json.contains( VIS_RELEASE_KEY ) && json.at( VIS_RELEASE_KEY ).is_string() )
				{
					visHint = json.at( VIS_RELEASE_KEY ).get<std::string>();
				}
			}
			catch ( ... )
			{
			}
			SPDLOG_ERROR( "nlohmann::json exception during GmodVersioningDto parsing (hint: visRelease='{}'): {}", visHint, ex.what() );

			return std::nullopt;
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			std::string visHint = "[unknown version]";
			try
			{
				if ( json.contains( VIS_RELEASE_KEY ) && json.at( VIS_RELEASE_KEY ).is_string() )
				{
					visHint = json.at( VIS_RELEASE_KEY ).get<std::string>();
				}
			}
			catch ( ... )
			{
			}
			SPDLOG_ERROR( "Standard exception during GmodVersioningDto parsing (hint: visRelease='{}'): {}", visHint, ex.what() );

			return std::nullopt;
		}
	}

	GmodVersioningDto GmodVersioningDto::fromJson( const nlohmann::json& json )
	{
		try
		{
			return json.get<GmodVersioningDto>();
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			std::string visHint = "[unknown version]";
			try
			{
				if ( json.is_object() && json.contains( VIS_RELEASE_KEY ) && json.at( VIS_RELEASE_KEY ).is_string() )
				{
					visHint = json.at( VIS_RELEASE_KEY ).get<std::string>();
				}
			}
			catch ( ... )
			{
			}
			throw std::invalid_argument( fmt::format( "Failed to deserialize GmodVersioningDto from JSON (hint: visRelease='{}'): {}", visHint, ex.what() ) );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			std::string visHint = "[unknown version]";
			try
			{
				if ( json.is_object() && json.contains( VIS_RELEASE_KEY ) && json.at( VIS_RELEASE_KEY ).is_string() )
				{
					visHint = json.at( VIS_RELEASE_KEY ).get<std::string>();
				}
			}
			catch ( ... )
			{
			}

			throw std::invalid_argument( fmt::format( "Failed to deserialize GmodVersioningDto from JSON (hint: visRelease='{}'): {}", visHint, ex.what() ) );
		}
	}

	nlohmann::json GmodVersioningDto::toJson() const
	{
		nlohmann::json j = *this;
		return j;
	}

	//----------------------------------------------
	// Private serialization methods
	//----------------------------------------------

	void to_json( nlohmann::json& j, const GmodVersioningDto& dto )
	{
		j = nlohmann::json{
			{ VIS_RELEASE_KEY, dto.m_visVersion } };

		if ( !dto.m_items.empty() )
		{
			[[maybe_unused]] auto serializationStartTime = std::chrono::steady_clock::now();

			j[ITEMS_KEY] = dto.m_items;

			auto serializationEndTime = std::chrono::steady_clock::now();
			auto serializationDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
				serializationEndTime - serializationStartTime );

			if ( !dto.m_items.empty() && serializationDuration.count() > 0 )
			{
				[[maybe_unused]] double serializationRatePerSecond = static_cast<double>( dto.m_items.size() ) * 1000.0 / static_cast<double>( serializationDuration.count() );
				SPDLOG_DEBUG( "Node serialization rate: {:.1f} items/sec", serializationRatePerSecond );
			}

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
			j[ITEMS_KEY] = nlohmann::json::object();
		}
	}

	void from_json( const nlohmann::json& j, GmodVersioningDto& dto )
	{
		if ( !j.contains( VIS_RELEASE_KEY ) || !j.at( VIS_RELEASE_KEY ).is_string() )
		{
			throw nlohmann::json::parse_error::create( 101, 0u, fmt::format( "GmodVersioningDto JSON missing required '{}' field or not a string", VIS_RELEASE_KEY ), nullptr );
		}
		dto.m_visVersion = j.at( VIS_RELEASE_KEY ).get<std::string>();

		dto.m_items.clear();
		if ( j.contains( ITEMS_KEY ) )
		{
			if ( !j.at( ITEMS_KEY ).is_object() )
			{
				throw nlohmann::json::type_error::create( 302, fmt::format( "GmodVersioningDto JSON field '{}' is not an object", ITEMS_KEY ), nullptr );
			}

			const auto& itemsObj = j.at( ITEMS_KEY );
			size_t itemCount = itemsObj.size();

			if ( itemCount > 10000 )
			{
				SPDLOG_WARN( "Large versioning dataset detected ({}), consider performance implications", itemCount );
			}

			dto.m_items.reserve( itemCount );

			for ( const auto& [key, value] : itemsObj.items() )
			{
				try
				{
					GmodNodeConversionDto nodeDto = value.get<GmodNodeConversionDto>();
					(void)dto.m_items.emplace( key, std::move( nodeDto ) ).first->second;
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
		}
		else
		{
			SPDLOG_WARN( "No '{}' object found in GMOD versioning data for VIS version {}", ITEMS_KEY, dto.m_visVersion );
		}
	}
}
