/*
 * MIT License
 *
 * Copyright (c) 2025 dnv
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
 * @file GmodVersioningDto.h
 * @brief Data Transfer Objects for Gmod Versioning JSON resources
 * @details Header-only DTOs for deserializing Gmod versioning data from Vista SDK resources.
 */

#pragma once

#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/SerializationTraits.h>

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace dnv::vista::sdk
{
	/**
	 * @brief DTO for Gmod node assignment changes
	 * @details Represents changes in assignment names between versions.
	 */
	struct GmodVersioningAssignmentChangeDto final
	{
		std::string oldAssignment;	   ///< Old assignment name
		std::string currentAssignment; ///< Current assignment name
	};

	/**
	 * @brief DTO for Gmod node conversion information
	 * @details Contains conversion operations and mappings between versions.
	 */
	struct GmodNodeConversionDto final
	{
		using Operations = std::unordered_set<std::string>;

		Operations operations;	   ///< Set of operations (e.g., "rename", "delete")
		std::string source;		   ///< Source node identifier
		std::string target;		   ///< Target node identifier
		std::string oldAssignment; ///< Old assignment name
		std::string newAssignment; ///< New assignment name
		bool deleteAssignment;	   ///< Whether assignment should be deleted
	};

	/**
	 * @brief DTO for the complete Gmod versioning structure
	 * @details Contains the VIS version and all node conversion mappings.
	 */
	struct GmodVersioningDto final
	{
		using Items = std::unordered_map<std::string, GmodNodeConversionDto>;

		std::string visVersion; ///< VIS version string
		Items items;			///< Map of node code to conversion information
	};
} // namespace dnv::vista::sdk

//=====================================================================
// nfx SerializationTraits specializations
//=====================================================================

namespace nfx::serialization::json
{
	/**
	 * @brief Serialization traits for GmodVersioningAssignmentChangeDto
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::GmodVersioningAssignmentChangeDto>
	{
		/**
		 * @brief Serialize GmodVersioningAssignmentChangeDto to JSON document
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::GmodVersioningAssignmentChangeDto& obj, Document& doc )
		{
			doc.set( "/oldAssignment", obj.oldAssignment );
			doc.set( "/currentAssignment", obj.currentAssignment );
		}

		/**
		 * @brief Deserialize GmodVersioningAssignmentChangeDto from JSON document
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 */
		static void deserialize( dnv::vista::sdk::GmodVersioningAssignmentChangeDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/oldAssignment" ) )
			{
				obj.oldAssignment = *val;
			}
			if ( auto val = doc.get<std::string>( "/currentAssignment" ) )
			{
				obj.currentAssignment = *val;
			}
		}
	};

	/**
	 * @brief Serialization traits for GmodNodeConversionDto
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::GmodNodeConversionDto>
	{
		/**
		 * @brief Serialize GmodNodeConversionDto to JSON document
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::GmodNodeConversionDto& obj, Document& doc )
		{
			Serializer<dnv::vista::sdk::GmodNodeConversionDto::Operations> operationsSerializer;
			doc.set( "/operations", operationsSerializer.serialize( obj.operations ) );
			doc.set( "/source", obj.source );
			doc.set( "/target", obj.target );
			doc.set( "/oldAssignment", obj.oldAssignment );
			doc.set( "/newAssignment", obj.newAssignment );
			doc.set( "/deleteAssignment", obj.deleteAssignment );
		}

		/**
		 * @brief Deserialize GmodNodeConversionDto from JSON document
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 */
		static void deserialize( dnv::vista::sdk::GmodNodeConversionDto& obj, const Document& doc )
		{
			if ( auto operationsDoc = doc.get<Document>( "/operations" ) )
			{
				Serializer<dnv::vista::sdk::GmodNodeConversionDto::Operations> operationsSerializer;
				obj.operations = operationsSerializer.deserialize( *operationsDoc );
			}
			if ( auto val = doc.get<std::string>( "/source" ) )
			{
				obj.source = *val;
			}
			if ( auto val = doc.get<std::string>( "/target" ) )
			{
				obj.target = *val;
			}
			if ( auto val = doc.get<std::string>( "/oldAssignment" ) )
			{
				obj.oldAssignment = *val;
			}
			if ( auto val = doc.get<std::string>( "/newAssignment" ) )
			{
				obj.newAssignment = *val;
			}
			if ( auto val = doc.get<bool>( "/deleteAssignment" ) )
			{
				obj.deleteAssignment = *val;
			}
		}
	};

	/**
	 * @brief Serialization traits for GmodVersioningDto
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::GmodVersioningDto>
	{
		/**
		 * @brief Serialize GmodVersioningDto to JSON document
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::GmodVersioningDto& obj, Document& doc )
		{
			doc.set( "/visRelease", obj.visVersion );

			Serializer<dnv::vista::sdk::GmodVersioningDto::Items> itemsSerializer;
			doc.set( "/items", itemsSerializer.serialize( obj.items ) );
		}

		/**
		 * @brief Deserialize GmodVersioningDto from JSON document
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 */
		static void deserialize( dnv::vista::sdk::GmodVersioningDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/visRelease" ) )
			{
				obj.visVersion = *val;
			}

			if ( auto itemsDoc = doc.get<Document>( "/items" ) )
			{
				Serializer<dnv::vista::sdk::GmodVersioningDto::Items> itemsSerializer;
				obj.items = itemsSerializer.deserialize( *itemsDoc );
			}
		}
	};
} // namespace nfx::serialization::json
