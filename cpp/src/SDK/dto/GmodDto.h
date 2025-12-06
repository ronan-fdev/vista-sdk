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
 * @file GmodDto.h
 * @brief Data Transfer Objects for Gmod (Generic product model) JSON resources
 * @details Header-only DTOs for deserializing Gmod data from Vista SDK resources.
 */

#pragma once

#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/SerializationTraits.h>

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace dnv::vista::sdk
{
	/**
	 * @brief DTO for a single Gmod node
	 * @details Represents a node in the Generic product model hierarchy with its
	 *          category, type, code, name, and optional metadata.
	 */
	struct GmodNodeDto final
	{
		using NormalAssignmentNames = std::unordered_map<std::string, std::string>;

		std::string category;										///< Node category
		std::string type;											///< Node type
		std::string code;											///< Node code identifier
		std::string name;											///< Node name
		std::optional<std::string> commonName;						///< Optional common name
		std::optional<std::string> definition;						///< Optional definition
		std::optional<std::string> commonDefinition;				///< Optional common definition
		std::optional<bool> installSubstructure;					///< Optional install substructure flag
		std::optional<NormalAssignmentNames> normalAssignmentNames; ///< Optional assignment names map
	};

	/**
	 * @brief DTO for the complete Gmod (Generic product model) structure
	 * @details Contains the VIS version, all Gmod nodes (items), and their relationships.
	 */
	struct GmodDto final
	{
		using Items = std::vector<GmodNodeDto>;
		using Relations = std::vector<std::vector<std::string>>;

		std::string visVersion; ///< VIS version string
		Items items;			///< All Gmod nodes
		Relations relations;	///< Node relationships
	};
} // namespace dnv::vista::sdk

//=====================================================================
// nfx SerializationTraits specializations
//=====================================================================

namespace nfx::serialization::json
{
	/**
	 * @brief Serialization traits for GmodNodeDto
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::GmodNodeDto>
	{
		/**
		 * @brief Serialize GmodNodeDto to JSON document
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::GmodNodeDto& obj, Document& doc )
		{
			doc.set( "/category", obj.category );
			doc.set( "/type", obj.type );
			doc.set( "/code", obj.code );
			doc.set( "/name", obj.name );

			if ( obj.commonName )
			{
				doc.set( "/commonName", *obj.commonName );
			}
			if ( obj.definition )
			{
				doc.set( "/definition", *obj.definition );
			}
			if ( obj.commonDefinition )
			{
				doc.set( "/commonDefinition", *obj.commonDefinition );
			}
			if ( obj.installSubstructure )
			{
				doc.set( "/installSubstructure", *obj.installSubstructure );
			}

			if ( obj.normalAssignmentNames )
			{
				Serializer<dnv::vista::sdk::GmodNodeDto::NormalAssignmentNames> namesSerializer;
				doc.set( "/normalAssignmentNames", namesSerializer.serialize( *obj.normalAssignmentNames ) );
			}
		}

		/**
		 * @brief Deserialize GmodNodeDto from JSON document
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 */
		static void deserialize( dnv::vista::sdk::GmodNodeDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/category" ) )
			{
				obj.category = *val;
			}
			if ( auto val = doc.get<std::string>( "/type" ) )
			{
				obj.type = *val;
			}
			if ( auto val = doc.get<std::string>( "/code" ) )
			{
				obj.code = *val;
			}
			if ( auto val = doc.get<std::string>( "/name" ) )
			{
				obj.name = *val;
			}
			if ( auto val = doc.get<std::string>( "/commonName" ) )
			{
				obj.commonName = *val;
			}
			if ( auto val = doc.get<std::string>( "/definition" ) )
			{
				obj.definition = *val;
			}
			if ( auto val = doc.get<std::string>( "/commonDefinition" ) )
			{
				obj.commonDefinition = *val;
			}
			if ( auto val = doc.get<bool>( "/installSubstructure" ) )
			{
				obj.installSubstructure = *val;
			}

			if ( auto namesDoc = doc.get<Document>( "/normalAssignmentNames" ) )
			{
				Serializer<dnv::vista::sdk::GmodNodeDto::NormalAssignmentNames> namesSerializer;
				obj.normalAssignmentNames = namesSerializer.deserialize( *namesDoc );
			}
		}
	};

	/**
	 * @brief Serialization traits for GmodDto
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::GmodDto>
	{
		/**
		 * @brief Serialize GmodDto to JSON document
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::GmodDto& obj, Document& doc )
		{
			doc.set( "/visRelease", obj.visVersion );

			Serializer<dnv::vista::sdk::GmodDto::Items> itemsSerializer;
			doc.set( "/items", itemsSerializer.serialize( obj.items ) );

			Serializer<dnv::vista::sdk::GmodDto::Relations> relationsSerializer;
			doc.set( "/relations", relationsSerializer.serialize( obj.relations ) );
		}

		/**
		 * @brief Deserialize GmodDto from JSON document
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 */
		static void deserialize( dnv::vista::sdk::GmodDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/visRelease" ) )
			{
				obj.visVersion = *val;
			}

			if ( auto itemsDoc = doc.get<Document>( "/items" ) )
			{
				Serializer<dnv::vista::sdk::GmodDto::Items> itemsSerializer;
				obj.items = itemsSerializer.deserialize( *itemsDoc );
			}

			if ( auto relationsDoc = doc.get<Document>( "/relations" ) )
			{
				Serializer<dnv::vista::sdk::GmodDto::Relations> relationsSerializer;
				obj.relations = relationsSerializer.deserialize( *relationsDoc );
			}
		}
	};
} // namespace nfx::serialization::json
