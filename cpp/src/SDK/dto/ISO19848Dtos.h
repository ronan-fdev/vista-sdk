/*
 * MIT License
 *
 * Copyright (c) 2025 DNV
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file ISO19848Dtos.h
 * @brief Data Transfer Objects for ISO 19848 JSON resources
 * @details Header-only DTOs for deserializing ISO 19848 data channel type names and format data types from Vista SDK resources.
 */

#pragma once

#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/SerializationTraits.h>

#include <stdexcept>
#include <string>
#include <vector>

namespace dnv::vista::sdk
{
	/**
	 * @brief DTO for a single data channel type name
	 * @details Represents a data channel type with its type identifier and description.
	 */
	struct DataChannelTypeNameDto
	{
		std::string type;		 ///< Data channel type identifier
		std::string description; ///< Data channel type description

		/**
		 * @brief Equality comparison operator
		 * @param other Data channel type name to compare with
		 * @return true if data channel type names are equal
		 */
		bool operator==( const DataChannelTypeNameDto& other ) const
		{
			return type == other.type && description == other.description;
		}

		/**
		 * @brief Inequality comparison operator
		 * @param other Data channel type name to compare with
		 * @return true if data channel type names are not equal
		 */
		bool operator!=( const DataChannelTypeNameDto& other ) const
		{
			return !( *this == other );
		}
	};

	/**
	 * @brief DTO for the complete data channel type names structure
	 * @details Contains all data channel type name entries for ISO 19848.
	 */
	struct DataChannelTypeNamesDto
	{
		using Values = std::vector<DataChannelTypeNameDto>;

		Values values; ///< All data channel type names

		/**
		 * @brief Equality comparison operator
		 * @param other Data channel type names to compare with
		 * @return true if data channel type names are equal
		 */
		bool operator==( const DataChannelTypeNamesDto& other ) const
		{
			return values == other.values;
		}

		/**
		 * @brief Inequality comparison operator
		 * @param other Data channel type names to compare with
		 * @return true if data channel type names are not equal
		 */
		bool operator!=( const DataChannelTypeNamesDto& other ) const
		{
			return !( *this == other );
		}
	};

	/**
	 * @brief DTO for a single format data type
	 * @details Represents a format data type with its type identifier and description.
	 */
	struct FormatDataTypeDto
	{
		std::string type;		 ///< Format data type identifier
		std::string description; ///< Format data type description

		/**
		 * @brief Equality comparison operator
		 * @param other Format data type to compare with
		 * @return true if format data types are equal
		 */
		bool operator==( const FormatDataTypeDto& other ) const
		{
			return type == other.type && description == other.description;
		}

		/**
		 * @brief Inequality comparison operator
		 * @param other Format data type to compare with
		 * @return true if format data types are not equal
		 */
		bool operator!=( const FormatDataTypeDto& other ) const
		{
			return !( *this == other );
		}
	};

	/**
	 * @brief DTO for the complete format data types structure
	 * @details Contains all format data type entries for ISO 19848.
	 */
	struct FormatDataTypesDto
	{
		using Values = std::vector<FormatDataTypeDto>;

		Values values; ///< All format data types

		/**
		 * @brief Equality comparison operator
		 * @param other Format data types to compare with
		 * @return true if format data types are equal
		 */
		bool operator==( const FormatDataTypesDto& other ) const
		{
			return values == other.values;
		}

		/**
		 * @brief Inequality comparison operator
		 * @param other Format data types to compare with
		 * @return true if format data types are not equal
		 */
		bool operator!=( const FormatDataTypesDto& other ) const
		{
			return !( *this == other );
		}
	};
} // namespace dnv::vista::sdk

//=====================================================================
// nfx SerializationTraits specializations
//=====================================================================

namespace nfx::serialization::json
{
	/**
	 * @brief Serialization traits for DataChannelTypeNameDto
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::DataChannelTypeNameDto>
	{
		/**
		 * @brief Serialize DataChannelTypeNameDto to JSON document
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::DataChannelTypeNameDto& obj, Document& doc )
		{
			doc.set( "/type", obj.type );
			doc.set( "/description", obj.description );
		}

		/**
		 * @brief Deserialize DataChannelTypeNameDto from JSON document
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 * @throws std::runtime_error if required fields are missing or invalid
		 */
		static void deserialize( dnv::vista::sdk::DataChannelTypeNameDto& obj, const Document& doc )
		{
			auto typeOpt = doc.get<std::string>( "/type" );
			if ( !typeOpt )
			{
				throw std::runtime_error( "DataChannelTypeNameDto: Missing required field 'type'" );
			}
			obj.type = std::move( *typeOpt );

			auto descriptionOpt = doc.get<std::string>( "/description" );
			if ( !descriptionOpt )
			{
				throw std::runtime_error( "DataChannelTypeNameDto: Missing required field 'description'" );
			}
			obj.description = std::move( *descriptionOpt );
		}
	};

	/**
	 * @brief Serialization traits for DataChannelTypeNamesDto
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::DataChannelTypeNamesDto>
	{
		/**
		 * @brief Serialize DataChannelTypeNamesDto to JSON document
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::DataChannelTypeNamesDto& obj, Document& doc )
		{
			Serializer<dnv::vista::sdk::DataChannelTypeNamesDto::Values> valuesSerializer;
			doc.set( "/values", valuesSerializer.serialize( obj.values ) );
		}

		/**
		 * @brief Deserialize DataChannelTypeNamesDto from JSON document
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 * @throws std::runtime_error if required fields are missing or invalid
		 */
		static void deserialize( dnv::vista::sdk::DataChannelTypeNamesDto& obj, const Document& doc )
		{
			auto valuesDoc = doc.get<Document>( "/values" );
			if ( !valuesDoc )
			{
				throw std::runtime_error( "DataChannelTypeNamesDto: Missing required field 'values'" );
			}

			Serializer<dnv::vista::sdk::DataChannelTypeNamesDto::Values> valuesSerializer;
			obj.values = valuesSerializer.deserialize( *valuesDoc );
		}
	};

	/**
	 * @brief Serialization traits for FormatDataTypeDto
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::FormatDataTypeDto>
	{
		/**
		 * @brief Serialize FormatDataTypeDto to JSON document
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::FormatDataTypeDto& obj, Document& doc )
		{
			doc.set( "/type", obj.type );
			doc.set( "/description", obj.description );
		}

		/**
		 * @brief Deserialize FormatDataTypeDto from JSON document
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 * @throws std::runtime_error if required fields are missing or invalid
		 */
		static void deserialize( dnv::vista::sdk::FormatDataTypeDto& obj, const Document& doc )
		{
			auto typeOpt = doc.get<std::string>( "/type" );
			if ( !typeOpt )
			{
				throw std::runtime_error( "FormatDataTypeDto: Missing required field 'type'" );
			}
			obj.type = std::move( *typeOpt );

			auto descriptionOpt = doc.get<std::string>( "/description" );
			if ( !descriptionOpt )
			{
				throw std::runtime_error( "FormatDataTypeDto: Missing required field 'description'" );
			}
			obj.description = std::move( *descriptionOpt );
		}
	};

	/**
	 * @brief Serialization traits for FormatDataTypesDto
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::FormatDataTypesDto>
	{
		/**
		 * @brief Serialize FormatDataTypesDto to JSON document
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::FormatDataTypesDto& obj, Document& doc )
		{
			Serializer<dnv::vista::sdk::FormatDataTypesDto::Values> valuesSerializer;
			doc.set( "/values", valuesSerializer.serialize( obj.values ) );
		}

		/**
		 * @brief Deserialize FormatDataTypesDto from JSON document
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 * @throws std::runtime_error if required fields are missing or invalid
		 */
		static void deserialize( dnv::vista::sdk::FormatDataTypesDto& obj, const Document& doc )
		{
			auto valuesDoc = doc.get<Document>( "/values" );
			if ( !valuesDoc )
			{
				throw std::runtime_error( "FormatDataTypesDto: Missing required field 'values'" );
			}

			Serializer<dnv::vista::sdk::FormatDataTypesDto::Values> valuesSerializer;
			obj.values = valuesSerializer.deserialize( *valuesDoc );
		}
	};
} // namespace nfx::serialization::json
