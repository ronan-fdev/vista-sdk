/**
 * @file CodebooksDto.h
 * @brief Data transfer objects for ISO 19848 codebook serialization
 * @details Provides data transfer objects used for serializing and deserializing
 *          codebook information according to the ISO 19848 standard.
 *          These DTOs serve as an intermediate representation when loading or saving codebook data.
 * @see ISO 19848 - Ships and marine technology - Standard data for shipboard machinery and equipment
 */

#pragma once

#include "utils/StringUtils.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// CodebookDto class
	//=====================================================================

	/**
	 * @brief Data transfer object for a single codebook
	 * @details Represents serialized information about a codebook from the ISO 19848 standard.
	 *          Each codebook contains a name identifier and a collection of values organized by groups.
	 * @todo Consider refactoring for stricter immutability (e.g., const members) if direct modification
	 *       by deserialization (beyond initial construction) is not desired.
	 */
	class CodebookDto final
	{
	public:
		//----------------------------------------------
		// Types and aliases
		//----------------------------------------------

		/** @brief Type representing a collection of values within a group */
		using ValueGroup = std::vector<std::string>;

		/** @brief Type representing a mapping of group names to their values with heterogeneous lookup */
		using ValuesMap = StringMap<ValueGroup>;

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor with parameters
		 * @param name The codebook name
		 * @param values The map of group names to values
		 */
		inline explicit CodebookDto( std::string name, ValuesMap values ) noexcept;

		/** @brief Default constructor. */
		CodebookDto() = default;

		/** @brief Copy constructor */
		CodebookDto( const CodebookDto& ) = default;

		/** @brief Move constructor */
		CodebookDto( CodebookDto&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~CodebookDto() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		CodebookDto& operator=( const CodebookDto& ) = default;

		/** @brief Move assignment operator */
		CodebookDto& operator=( CodebookDto&& ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the name of this codebook
		 * @return The codebook name
		 */
		[[nodiscard]] inline std::string_view name() const noexcept;

		/**
		 * @brief Get the values map of this codebook
		 * @return The map of group names to their corresponding values
		 */
		[[nodiscard]] inline const ValuesMap& values() const noexcept;

		//----------------------------------------------
		// Serialization
		//----------------------------------------------

		/**
		 * @brief Try to deserialize a CodebookDto from a nlohmann::json object
		 * @param json The nlohmann::json object to deserialize
		 * @return Optional containing the deserialized object if successful, empty optional otherwise
		 */
		static std::optional<CodebookDto> tryFromJson( const nlohmann::json& json );

		/**
		 * @brief Deserialize a CodebookDto from a nlohmann::json object
		 * @param json The nlohmann::json object to deserialize
		 * @return The deserialized CodebookDto
		 * @throws std::invalid_argument If required fields are missing or invalid
		 * @throws nlohmann::json::exception If JSON parsing/access errors occur
		 */
		static CodebookDto fromJson( const nlohmann::json& json );

		/**
		 * @brief Serialize this CodebookDto to a nlohmann::json object
		 * @return The serialized nlohmann::json object
		 */
		[[nodiscard]] nlohmann::json toJson() const;

	private:
		//-------------------------------------------------------------------
		// Private serialization methods
		//-------------------------------------------------------------------

		/**
		 * @brief ADL hook for nlohmann::json deserialization
		 * @details Friend function that enables automatic deserialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert JSON to CodebookDto objects.
		 * @param j The JSON object to deserialize from
		 * @param dto The CodebookDto object to deserialize into
		 * @throws nlohmann::json::parse_error If required fields are missing or have wrong types
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void from_json( const nlohmann::json& j, CodebookDto& dto );

		/**
		 * @brief ADL hook for nlohmann::json serialization
		 * @details Friend function that enables automatic serialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert CodebookDto objects to JSON.
		 * @param j The JSON object to serialize into
		 * @param dto The CodebookDto object to serialize from
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void to_json( nlohmann::json& j, const CodebookDto& dto );

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Name identifier of the codebook (e.g., "positions", "quantities") */
		std::string m_name;

		/** @brief Map of group names to their corresponding values */
		ValuesMap m_values;
	};

	//=====================================================================
	// CodebooksDto class
	//=====================================================================

	/**
	 * @brief Data transfer object for a collection of codebooks
	 * @details Represents a complete set of codebooks for a specific VIS version,
	 *          used for serialization to and from JSON format.
	 * @todo Consider refactoring for stricter immutability (e.g., const members) if direct modification
	 *       by deserialization (beyond initial construction) is not desired.
	 */
	class CodebooksDto final
	{
	public:
		//----------------------------------------------
		// Types and aliases
		//----------------------------------------------

		/** @brief Type representing a collection of codebook DTOs */
		using Items = std::vector<CodebookDto>;

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor with parameters
		 * @param visVersion The VIS version
		 * @param items The collection of codebook DTOs
		 */
		inline explicit CodebooksDto( std::string visVersion, Items items ) noexcept;

		/** @brief Default constructor. */
		CodebooksDto() = default;

		/** @brief Copy constructor */
		CodebooksDto( const CodebooksDto& ) = default;

		/** @brief Move constructor */
		CodebooksDto( CodebooksDto&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~CodebooksDto() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		CodebooksDto& operator=( const CodebooksDto& ) = default;

		/** @brief Move assignment operator */
		CodebooksDto& operator=( CodebooksDto&& ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the VIS version string
		 * @return The VIS version string
		 */
		[[nodiscard]] inline std::string_view visVersion() const noexcept;

		/**
		 * @brief Get the collection of codebooks
		 * @return The vector of codebook DTOs
		 */
		[[nodiscard]] inline const Items& items() const noexcept;

		//----------------------------------------------
		// Serialization
		//----------------------------------------------

		/**
		 * @brief Try to deserialize a CodebooksDto from a nlohmann::json object
		 * @param json The nlohmann::json object to deserialize
		 * @return Optional containing the deserialized object if successful, empty optional otherwise
		 */
		static std::optional<CodebooksDto> tryFromJson( const nlohmann::json& json );

		/**
		 * @brief Deserialize a CodebooksDto from a nlohmann::json object
		 * @param json The nlohmann::json object to deserialize
		 * @return The deserialized CodebooksDto
		 * @throws std::invalid_argument If required fields are missing or invalid
		 * @throws nlohmann::json::exception If JSON parsing/access errors occur
		 */
		static CodebooksDto fromJson( const nlohmann::json& json );

		/**
		 * @brief Serialize this CodebooksDto to a nlohmann::json object
		 * @return The serialized nlohmann::json object
		 */
		[[nodiscard]] nlohmann::json toJson() const;

	private:
		//-------------------------------------------------------------------
		// Private serialization methods
		//-------------------------------------------------------------------

		/**
		 * @brief ADL hook for nlohmann::json deserialization
		 * @details Friend function that enables automatic deserialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert JSON to CodebooksDto objects.
		 * @param j The JSON object to deserialize from
		 * @param dto The CodebooksDto object to deserialize into
		 * @throws nlohmann::json::parse_error If required fields are missing or have wrong types
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void from_json( const nlohmann::json& j, CodebooksDto& dto );

		/**
		 * @brief ADL hook for nlohmann::json serialization
		 * @details Friend function that enables automatic serialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert CodebooksDto objects to JSON.
		 * @param j The JSON object to serialize into
		 * @param dto The CodebooksDto object to serialize from
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void to_json( nlohmann::json& j, const CodebooksDto& dto );

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief VIS version string (e.g., "3.8a") */
		std::string m_visVersion;

		/** @brief Collection of codebook DTOs contained in this version */
		Items m_items;
	};
}

#include "CodebooksDto.inl"
