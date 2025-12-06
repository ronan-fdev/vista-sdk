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
 * @file CodebooksDto.h
 * @brief Data Transfer Objects for Codebooks JSON resources
 * @details Header-only DTOs for deserializing Codebooks data from Vista SDK resources.
 */

#pragma once

#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/SerializationTraits.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace dnv::vista::sdk
{
	/**
	 * @brief DTO for a single Codebook
	 * @details Represents a codebook with its name and mapping of codes to value arrays.
	 */
	struct CodebookDto final
	{
		using Values = std::unordered_map<std::string, std::vector<std::string>>;

		std::string name; ///< Codebook name
		Values values;	  ///< Map of code to array of values
	};

	/**
	 * @brief DTO for the complete Codebooks structure
	 * @details Contains the VIS version and all codebook items.
	 */
	struct CodebooksDto final
	{
		using Items = std::vector<CodebookDto>;

		std::string visVersion; ///< VIS version string
		Items items;			///< All codebooks
	};
} // namespace dnv::vista::sdk

//=====================================================================
// nfx SerializationTraits specializations
//=====================================================================

namespace nfx::serialization::json
{
	/**
	 * @brief Serialization traits for CodebookDto
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::CodebookDto>
	{
		/**
		 * @brief Serialize CodebookDto to JSON document
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::CodebookDto& obj, Document& doc )
		{
			doc.set( "/name", obj.name );

			Serializer<dnv::vista::sdk::CodebookDto::Values> valuesSerializer;
			doc.set( "/values", valuesSerializer.serialize( obj.values ) );
		}

		/**
		 * @brief Deserialize CodebookDto from JSON document
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 */
		static void deserialize( dnv::vista::sdk::CodebookDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/name" ) )
			{
				obj.name = *val;
			}

			if ( auto valuesDoc = doc.get<Document>( "/values" ) )
			{
				Serializer<dnv::vista::sdk::CodebookDto::Values> valuesSerializer;
				obj.values = valuesSerializer.deserialize( *valuesDoc );
			}
		}
	};

	/**
	 * @brief Serialization traits for CodebooksDto
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::CodebooksDto>
	{
		/**
		 * @brief Serialize CodebooksDto to JSON document
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::CodebooksDto& obj, Document& doc )
		{
			doc.set( "/visRelease", obj.visVersion );

			Serializer<dnv::vista::sdk::CodebooksDto::Items> itemsSerializer;
			doc.set( "/items", itemsSerializer.serialize( obj.items ) );
		}

		/**
		 * @brief Deserialize CodebooksDto from JSON document
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 */
		static void deserialize( dnv::vista::sdk::CodebooksDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/visRelease" ) )
			{
				obj.visVersion = *val;
			}

			if ( auto itemsDoc = doc.get<Document>( "/items" ) )
			{
				Serializer<dnv::vista::sdk::CodebooksDto::Items> itemsSerializer;
				obj.items = itemsSerializer.deserialize( *itemsDoc );
			}
		}
	};
} // namespace nfx::serialization::json
