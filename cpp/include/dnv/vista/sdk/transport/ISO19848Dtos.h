/**
 * @file ISO19848Dtos.h
 * @brief Data transfer objects for ISO 19848 standard data types
 * @details Provides data transfer objects used for serializing and deserializing
 *          ISO 19848 data channel type and format data type information.
 *          These DTOs serve as an intermediate representation when loading or
 *          saving this data, typically from JSON formatted files.
 * @see ISO 19848 - Ships and marine technology - Standard data for shipboard machinery and equipment
 */

#pragma once

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// Single Data Channel Type data transfer objects
	//=====================================================================

	/**
	 * @brief Data transfer object (DTO) for a single ISO 19848 data channel type name.
	 * @details Represents a specific data channel type name and its corresponding description,
	 *          as defined in the ISO 19848 standard. Used for serialization to and from JSON.
	 * @todo Consider refactoring for stricter immutability (e.g., const members) if direct modification
	 *       by deserialization (beyond initial construction) is not desired.
	 */
	class DataChannelTypeNameDto final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor with parameters
		 * @param type The type name
		 * @param description The description of the type
		 */
		inline explicit DataChannelTypeNameDto( std::string type, std::string description ) noexcept;

		/** @brief Default constructor. */
		DataChannelTypeNameDto() = default;

		/** @brief Copy constructor */
		DataChannelTypeNameDto( const DataChannelTypeNameDto& ) = default;

		/** @brief Move constructor */
		DataChannelTypeNameDto( DataChannelTypeNameDto&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DataChannelTypeNameDto() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		DataChannelTypeNameDto& operator=( const DataChannelTypeNameDto& ) = default;

		/** @brief Move assignment operator */
		DataChannelTypeNameDto& operator=( DataChannelTypeNameDto&& ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the type name
		 * @return Type name
		 */
		[[nodiscard]] inline std::string_view type() const noexcept;

		/**
		 * @brief Get the description
		 * @return Description of the type
		 */
		[[nodiscard]] inline std::string_view description() const noexcept;

		//----------------------------------------------
		// Serialization
		//----------------------------------------------

		/**
		 * @brief Try to deserialize a DataChannelTypeNameDto from an nlohmann::json object
		 * @param json The nlohmann::json object to deserialize
		 * @return Optional containing the deserialized object if successful, empty optional otherwise
		 */
		static std::optional<DataChannelTypeNameDto> tryFromJson( const nlohmann::json& json );

		/**
		 * @brief Deserialize a DataChannelTypeNameDto from an nlohmann::json object
		 * @param json The nlohmann::json object to deserialize
		 * @return The deserialized DataChannelTypeNameDto
		 * @throws std::invalid_argument If deserialization fails (e.g., missing fields, type errors)
		 * @throws nlohmann::json::exception If JSON parsing/access errors occur
		 */
		static DataChannelTypeNameDto fromJson( const nlohmann::json& json );

		/**
		 * @brief Serialize this DataChannelTypeNameDto to an nlohmann::json object
		 * @return The serialized nlohmann::json object
		 */
		[[nodiscard]] nlohmann::json toJson() const;

	private:
		//----------------------------------------------
		// Private serialization methods
		//----------------------------------------------

		/**
		 * @brief ADL hook for nlohmann::json deserialization
		 * @details Friend function that enables automatic deserialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert JSON to DataChannelTypeNameDto objects.
		 * @param j The JSON object to deserialize from
		 * @param dto The DataChannelTypeNameDto object to deserialize into
		 * @throws nlohmann::json::parse_error If required fields are missing or have wrong types
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void from_json( const nlohmann::json& j, DataChannelTypeNameDto& dto );

		/**
		 * @brief ADL hook for nlohmann::json serialization
		 * @details Friend function that enables automatic serialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert DataChannelTypeNameDto objects to JSON.
		 * @param j The JSON object to serialize into
		 * @param dto The DataChannelTypeNameDto object to serialize from
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void to_json( nlohmann::json& j, const DataChannelTypeNameDto& dto );

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Type name (JSON: "type") */
		std::string m_type;

		/** @brief Description of the type (JSON: "description") */
		std::string m_description;
	};

	//=====================================================================
	// Collection of Data Channel Type data transfer objects
	//=====================================================================

	/**
	 * @brief Data transfer object (DTO) for a collection of ISO 19848 data channel type names.
	 * @details Represents a list of `DataChannelTypeNameDto` objects, effectively serializing
	 *          a set of data channel types and their descriptions as per ISO 19848.
	 *          Used for serialization to and from JSON.
	 * @todo Consider refactoring for stricter immutability (e.g., const members) if direct modification
	 *       by deserialization (beyond initial construction) is not desired.
	 */
	class DataChannelTypeNamesDto final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor with parameters
		 * @param values A collection of data channel type name values
		 */
		inline explicit DataChannelTypeNamesDto( std::vector<DataChannelTypeNameDto> values ) noexcept;

		/** @brief Default constructor. */
		DataChannelTypeNamesDto() = default;

		/** @brief Copy constructor */
		DataChannelTypeNamesDto( const DataChannelTypeNamesDto& ) = default;

		/** @brief Move constructor */
		DataChannelTypeNamesDto( DataChannelTypeNamesDto&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DataChannelTypeNamesDto() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		DataChannelTypeNamesDto& operator=( const DataChannelTypeNamesDto& ) = default;

		/** @brief Move assignment operator */
		DataChannelTypeNamesDto& operator=( DataChannelTypeNamesDto&& ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the collection of data channel type names
		 * @return Collection of data channel type names
		 */
		[[nodiscard]] inline const std::vector<DataChannelTypeNameDto>& values() const noexcept;

		//----------------------------------------------
		// Serialization
		//----------------------------------------------

		/**
		 * @brief Try to deserialize a DataChannelTypeNamesDto from an nlohmann::json object
		 * @param json The nlohmann::json object to deserialize
		 * @return Optional containing the deserialized object if successful, empty optional otherwise
		 */
		static std::optional<DataChannelTypeNamesDto> tryFromJson( const nlohmann::json& json );

		/**
		 * @brief Deserialize a DataChannelTypeNamesDto from an nlohmann::json object
		 * @param json The nlohmann::json object to deserialize
		 * @return The deserialized DataChannelTypeNamesDto
		 * @throws std::invalid_argument If deserialization fails (e.g., missing fields, type errors)
		 * @throws nlohmann::json::exception If JSON parsing/access errors occur
		 */
		static DataChannelTypeNamesDto fromJson( const nlohmann::json& json );

		/**
		 * @brief Serialize this DataChannelTypeNamesDto to an nlohmann::json object
		 * @return The serialized nlohmann::json object
		 */
		[[nodiscard]] nlohmann::json toJson() const;

	private:
		//----------------------------------------------
		// Private serialization methods
		//----------------------------------------------

		/**
		 * @brief ADL hook for nlohmann::json deserialization
		 * @details Friend function that enables automatic deserialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert JSON to DataChannelTypeNamesDto objects.
		 * @param j The JSON object to deserialize from
		 * @param dto The DataChannelTypeNamesDto object to deserialize into
		 * @throws nlohmann::json::parse_error If required fields are missing or have wrong types
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void from_json( const nlohmann::json& j, DataChannelTypeNamesDto& dto );

		/**
		 * @brief ADL hook for nlohmann::json serialization
		 * @details Friend function that enables automatic serialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert DataChannelTypeNamesDto objects to JSON.
		 * @param j The JSON object to serialize into
		 * @param dto The DataChannelTypeNamesDto object to serialize from
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void to_json( nlohmann::json& j, const DataChannelTypeNamesDto& dto );

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Collection of data channel type name values (JSON: "values") */
		std::vector<DataChannelTypeNameDto> m_values;
	};

	//=====================================================================
	// Single Format Data Type data transfer objects
	//=====================================================================

	/**
	 * @brief Data transfer object (DTO) for a single ISO 19848 format data type.
	 * @details Represents a specific format data type and its corresponding description,
	 *          as defined in the ISO 19848 standard. Used for serialization to and from JSON.
	 * @todo Consider refactoring for stricter immutability (e.g., const members) if direct modification
	 *       by deserialization (beyond initial construction) is not desired.
	 */
	class FormatDataTypeDto final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor with parameters
		 * @param type The type name
		 * @param description The description of the type
		 */
		inline explicit FormatDataTypeDto( std::string type, std::string description ) noexcept;

		/** @brief Default constructor. */
		FormatDataTypeDto() = default;

		/** @brief Copy constructor */
		FormatDataTypeDto( const FormatDataTypeDto& ) = default;

		/** @brief Move constructor */
		FormatDataTypeDto( FormatDataTypeDto&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~FormatDataTypeDto() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		FormatDataTypeDto& operator=( const FormatDataTypeDto& ) = default;

		/** @brief Move assignment operator */
		FormatDataTypeDto& operator=( FormatDataTypeDto&& ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the type name
		 * @return Type name
		 */
		[[nodiscard]] inline std::string_view type() const noexcept;

		/**
		 * @brief Get the description
		 * @return Description of the type
		 */
		[[nodiscard]] inline std::string_view description() const noexcept;

		//----------------------------------------------
		// Serialization
		//----------------------------------------------

		/**
		 * @brief Try to deserialize a FormatDataTypeDto from an nlohmann::json object
		 * @param json The nlohmann::json object to deserialize
		 * @return Optional containing the deserialized object if successful, empty optional otherwise
		 */
		static std::optional<FormatDataTypeDto> tryFromJson( const nlohmann::json& json );

		/**
		 * @brief Deserialize a FormatDataTypeDto from an nlohmann::json object
		 * @param json The nlohmann::json object to deserialize
		 * @return The deserialized FormatDataTypeDto
		 * @throws std::invalid_argument If deserialization fails (e.g., missing fields, type errors)
		 * @throws nlohmann::json::exception If JSON parsing/access errors occur
		 */
		static FormatDataTypeDto fromJson( const nlohmann::json& json );

		/**
		 * @brief Serialize this FormatDataTypeDto to an nlohmann::json object
		 * @return The serialized nlohmann::json object
		 */
		[[nodiscard]] nlohmann::json toJson() const;

	private:
		//----------------------------------------------
		// Private serialization methods
		//----------------------------------------------

		/**
		 * @brief ADL hook for nlohmann::json deserialization
		 * @details Friend function that enables automatic deserialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert JSON to FormatDataTypeDto objects.
		 * @param j The JSON object to deserialize from
		 * @param dto The FormatDataTypeDto object to deserialize into
		 * @throws nlohmann::json::parse_error If required fields are missing or have wrong types
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void from_json( const nlohmann::json& j, FormatDataTypeDto& dto );

		/**
		 * @brief ADL hook for nlohmann::json serialization
		 * @details Friend function that enables automatic serialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert FormatDataTypeDto objects to JSON.
		 * @param j The JSON object to serialize into
		 * @param dto The FormatDataTypeDto object to serialize from
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void to_json( nlohmann::json& j, const FormatDataTypeDto& dto );

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Type name (JSON: "type") */
		std::string m_type;

		/** @brief Description of the type (JSON: "description") */
		std::string m_description;
	};

	//=====================================================================
	// Collection of  Format Data Type data transfer objects
	//=====================================================================

	/**
	 * @brief Data transfer object (DTO) for a collection of ISO 19848 format data types.
	 * @details Represents a list of `FormatDataTypeDto` objects, effectively serializing
	 *          a set of format data types and their descriptions as per ISO 19848.
	 *          Used for serialization to and from JSON.
	 * @todo Consider refactoring for stricter immutability (e.g., const members) if direct modification
	 *       by deserialization (beyond initial construction) is not desired.
	 */
	class FormatDataTypesDto final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor with parameters
		 * @param values A collection of format data type values
		 */
		inline explicit FormatDataTypesDto( std::vector<FormatDataTypeDto> values ) noexcept;

		/** @brief Default constructor. */
		FormatDataTypesDto() = default;

		/** @brief Copy constructor */
		FormatDataTypesDto( const FormatDataTypesDto& ) = default;

		/** @brief Move constructor */
		FormatDataTypesDto( FormatDataTypesDto&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~FormatDataTypesDto() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		FormatDataTypesDto& operator=( const FormatDataTypesDto& ) = default;

		/** @brief Move assignment operator */
		FormatDataTypesDto& operator=( FormatDataTypesDto&& ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the collection of format data types
		 * @return Collection of format data types
		 */
		[[nodiscard]] inline const std::vector<FormatDataTypeDto>& values() const noexcept;

		//----------------------------------------------
		// Serialization
		//----------------------------------------------

		/**
		 * @brief Try to deserialize a FormatDataTypesDto from an nlohmann::json object
		 * @param json The nlohmann::json object to deserialize
		 * @return Optional containing the deserialized object if successful, empty optional otherwise
		 */
		static std::optional<FormatDataTypesDto> tryFromJson( const nlohmann::json& json );

		/**
		 * @brief Deserialize a FormatDataTypesDto from an nlohmann::json object
		 * @param json The nlohmann::json object to deserialize
		 * @return The deserialized FormatDataTypesDto
		 * @throws std::invalid_argument If deserialization fails (e.g., missing fields, type errors)
		 * @throws nlohmann::json::exception If JSON parsing/access errors occur
		 */
		static FormatDataTypesDto fromJson( const nlohmann::json& json );

		/**
		 * @brief Serialize this FormatDataTypesDto to an nlohmann::json object
		 * @return The serialized nlohmann::json object
		 */
		[[nodiscard]] nlohmann::json toJson() const;

	private:
		//----------------------------------------------
		// Private serialization methods
		//----------------------------------------------

		/**
		 * @brief ADL hook for nlohmann::json deserialization
		 * @details Friend function that enables automatic deserialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert JSON to FormatDataTypesDto objects.
		 * @param j The JSON object to deserialize from
		 * @param dto The FormatDataTypesDto object to deserialize into
		 * @throws nlohmann::json::parse_error If required fields are missing or have wrong types
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void from_json( const nlohmann::json& j, FormatDataTypesDto& dto );

		/**
		 * @brief ADL hook for nlohmann::json serialization
		 * @details Friend function that enables automatic serialization via nlohmann::json.
		 *          This function is found through Argument-Dependent Lookup (ADL) and allows
		 *          nlohmann::json to automatically convert FormatDataTypesDto objects to JSON.
		 * @param j The JSON object to serialize into
		 * @param dto The FormatDataTypesDto object to serialize from
		 * @note This function accesses private members and is called automatically by nlohmann::json
		 */
		friend void to_json( nlohmann::json& j, const FormatDataTypesDto& dto );

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Collection of format data type values (JSON: "values") */
		std::vector<FormatDataTypeDto> m_values;
	};
}

#include "ISO19848Dtos.inl"
