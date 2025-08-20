/**
 * @file LocationsDto.h
 * @brief Data transfer objects for locations in the VIS standard
 */

#pragma once

namespace dnv::vista::sdk
{
	//=====================================================================
	// Relative Location data transfer objects
	//=====================================================================

	/**
	 * @brief Data transfer object (DTO) for a relative location.
	 *
	 * Represents a relative location with a code, name, and optional definition.
	 * This class is immutable - all properties are set during construction.
	 */
	class RelativeLocationsDto final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor with parameters
		 *
		 * @param code The character code representing the location
		 * @param name The name of the location
		 * @param definition An optional definition of the location
		 */
		inline explicit RelativeLocationsDto(
			char code,
			std::string name,
			std::optional<std::string> definition = std::nullopt ) noexcept;

		/** @brief Default constructor. */
		RelativeLocationsDto() = default;

		/** @brief Copy constructor */
		RelativeLocationsDto( const RelativeLocationsDto& ) = default;

		/** @brief Move constructor */
		RelativeLocationsDto( RelativeLocationsDto&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~RelativeLocationsDto() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		RelativeLocationsDto& operator=( const RelativeLocationsDto& ) = default;

		/** @brief Move assignment operator */
		RelativeLocationsDto& operator=( RelativeLocationsDto&& ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the location code
		 * @return The character code representing the location
		 */
		[[nodiscard]] inline char code() const noexcept;

		/**
		 * @brief Get the location name
		 * @return The name of the location
		 */
		[[nodiscard]] inline std::string_view name() const noexcept;

		/**
		 * @brief Get the location definition
		 * @return The optional definition of the location
		 */
		[[nodiscard]] inline const std::optional<std::string>& definition() const noexcept;

		//----------------------------------------------
		// Serialization
		//----------------------------------------------

		/**
		 * @brief Try to deserialize a RelativeLocationsDto from an nlohmann::json object.
		 *
		 * @param json The nlohmann::json object to deserialize.
		 * @return An optional containing the deserialized DTO if successful, or std::nullopt if parsing failed
		 */
		static std::optional<RelativeLocationsDto> tryFromJson( const nlohmann::json& json );

		/**
		 * @brief Deserialize a RelativeLocationsDto from an nlohmann::json object.
		 *
		 * @param json The nlohmann::json object to deserialize.
		 * @return The deserialized RelativeLocationsDto.
		 * @throws std::invalid_argument If deserialization fails (e.g., missing fields, type errors)
		 * @throws nlohmann::json::exception If JSON parsing/access errors occur
		 */
		static RelativeLocationsDto fromJson( const nlohmann::json& json );

		/**
		 * @brief Serialize this RelativeLocationsDto to an nlohmann::json object
		 *
		 * @return The serialized nlohmann::json object
		 */
		nlohmann::json toJson() const;

	private:
		//----------------------------------------------
		// Private serialization methods
		//---------------------------------------------

		/**
		 * @brief ADL hook for nlohmann::json deserialization
		 * @details Friend function that enables automatic deserialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert JSON to RelativeLocationsDto objects.
		 * @param j The JSON object to deserialize from
		 * @param dto The RelativeLocationsDto object to deserialize into
		 * @throws nlohmann::json::parse_error If required fields are missing or have wrong types
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void from_json( const nlohmann::json& j, RelativeLocationsDto& dto );

		/**
		 * @brief ADL hook for nlohmann::json serialization
		 * @details Friend function that enables automatic serialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert RelativeLocationsDto objects to JSON.
		 * @param j The JSON object to serialize into
		 * @param dto The RelativeLocationsDto object to serialize from
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void to_json( nlohmann::json& j, const RelativeLocationsDto& dto );

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief The character code representing the location (JSON: "code"). */
		char m_code;

		/** @brief The name of the location (JSON: "name"). */
		std::string m_name;

		/** @brief An optional definition of the location (JSON: "definition"). */
		std::optional<std::string> m_definition;
	};

	//=====================================================================
	// Location data transfer objects
	//=====================================================================

	/**
	 * @brief Data transfer object (DTO) for a collection of locations.
	 *
	 * Represents a collection of relative locations and the VIS version they belong to.
	 * This class is immutable - all properties are set during construction.
	 */
	class LocationsDto final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor with parameters
		 *
		 * @param visVersion The VIS version string
		 * @param items A collection of relative locations
		 */
		inline explicit LocationsDto( std::string visVersion, std::vector<RelativeLocationsDto> items ) noexcept;

		/** @brief Default constructor. */
		LocationsDto() = default;

		/** @brief Copy constructor */
		LocationsDto( const LocationsDto& ) = default;

		/** @brief Move constructor */
		LocationsDto( LocationsDto&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~LocationsDto() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		LocationsDto& operator=( const LocationsDto& ) = default;

		/** @brief Move assignment operator */
		LocationsDto& operator=( LocationsDto&& ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the VIS version string
		 * @return The VIS version string
		 */
		[[nodiscard]] inline std::string_view visVersion() const noexcept;

		/**
		 * @brief Get the collection of relative locations
		 * @return A vector of relative locations
		 */
		[[nodiscard]] inline const std::vector<RelativeLocationsDto>& items() const noexcept;

		//----------------------------------------------
		// Serialization
		//----------------------------------------------

		/**
		 * @brief Try to deserialize a LocationsDto from an nlohmann::json object.
		 *
		 * @param json The nlohmann::json object to deserialize.
		 * @return An optional containing the deserialized DTO if successful, or std::nullopt if parsing failed
		 */
		static std::optional<LocationsDto> tryFromJson( const nlohmann::json& json );

		/**
		 * @brief Deserialize a LocationsDto from an nlohmann::json object.
		 *
		 * @param json The nlohmann::json object to deserialize.
		 * @return The deserialized LocationsDto.
		 * @throws std::invalid_argument If deserialization fails (e.g., missing fields, type errors)
		 * @throws nlohmann::json::exception If JSON parsing/access errors occur
		 */
		static LocationsDto fromJson( const nlohmann::json& json );

		/**
		 * @brief Serialize this LocationsDto to an nlohmann::json object
		 * @return The serialized nlohmann::json object
		 */
		nlohmann::json toJson() const;

	private:
		//----------------------------------------------
		// Private serialization methods
		//----------------------------------------------

		/**
		 * @brief ADL hook for nlohmann::json deserialization
		 * @details Friend function that enables automatic deserialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert JSON to LocationsDto objects.
		 * @param j The JSON object to deserialize from
		 * @param dto The LocationsDto object to deserialize into
		 * @throws nlohmann::json::parse_error If required fields are missing or have wrong types
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void from_json( const nlohmann::json& j, LocationsDto& dto );

		/**
		 * @brief ADL hook for nlohmann::json serialization
		 * @details Friend function that enables automatic serialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert LocationsDto objects to JSON.
		 * @param j The JSON object to serialize into
		 * @param dto The LocationsDto object to serialize from
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void to_json( nlohmann::json& j, const LocationsDto& dto );

	private:
		//----------------------------------------------
		// Private member variables
		//--------------------------------------------

		/** @brief The VIS version string (JSON: "visRelease"). */
		std::string m_visVersion;

		/** @brief A vector of relative locations (JSON: "items"). */
		std::vector<RelativeLocationsDto> m_items;
	};
}

#include "LocationsDto.inl"
