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
 * @file LocationsDto.h
 * @brief Data Transfer Objects for Locations JSON resources
 * @details Header-only DTOs for deserializing Locations data from Vista SDK resources.
 */

#pragma once

#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/SerializationTraits.h>

#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace dnv::vista::sdk
{
	/**
	 * @brief DTO for a single relative location
	 * @details Represents a relative location with its code, name, and optional definition.
	 */
	struct RelativeLocationsDto
	{
		char code;							   ///< Location code (single character)
		std::string name;					   ///< Location name
		std::optional<std::string> definition; ///< Optional definition

		/**
		 * @brief Equality comparison operator
		 * @param other Relative location to compare with
		 * @return true if relative locations are equal
		 */
		bool operator==( const RelativeLocationsDto& other ) const
		{
			return code == other.code && name == other.name && definition == other.definition;
		}

		/**
		 * @brief Inequality comparison operator
		 * @param other Relative location to compare with
		 * @return true if relative locations are not equal
		 */
		bool operator!=( const RelativeLocationsDto& other ) const
		{
			return !( *this == other );
		}
	};

	/**
	 * @brief DTO for the complete Locations structure
	 * @details Contains the VIS version and all relative location items.
	 */
	struct LocationsDto
	{
		using Items = std::vector<RelativeLocationsDto>;

		std::string visVersion; ///< VIS version string (maps to "visRelease" in JSON)
		Items items;			///< All relative locations

		/**
		 * @brief Equality comparison operator
		 * @param other Locations to compare with
		 * @return true if locations are equal
		 */
		bool operator==( const LocationsDto& other ) const
		{
			return visVersion == other.visVersion && items == other.items;
		}

		/**
		 * @brief Inequality comparison operator
		 * @param other Locations to compare with
		 * @return true if locations are not equal
		 */
		bool operator!=( const LocationsDto& other ) const
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
	 * @brief Serialization traits for RelativeLocationsDto
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::RelativeLocationsDto>
	{
		/**
		 * @brief Serialize RelativeLocationsDto to JSON document
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::RelativeLocationsDto& obj, Document& doc )
		{
			doc.set( "/code", std::string( 1, obj.code ) );
			doc.set( "/name", obj.name );

			if ( obj.definition )
			{
				doc.set( "/definition", *obj.definition );
			}
		}

		/**
		 * @brief Deserialize RelativeLocationsDto from JSON document
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 * @throws std::runtime_error if required fields are missing or invalid
		 */
		static void deserialize( dnv::vista::sdk::RelativeLocationsDto& obj, const Document& doc )
		{
			auto codeOpt = doc.get<std::string>( "/code" );
			if ( !codeOpt || codeOpt->empty() )
			{
				throw std::runtime_error( "RelativeLocationsDto: Missing required field 'code'" );
			}
			obj.code = ( *codeOpt )[0];

			auto nameOpt = doc.get<std::string>( "/name" );
			if ( !nameOpt )
			{
				throw std::runtime_error( "RelativeLocationsDto: Missing required field 'name'" );
			}
			obj.name = std::move( *nameOpt );

			if ( auto val = doc.get<std::string>( "/definition" ) )
			{
				obj.definition = std::move( *val );
			}
		}
	};

	/**
	 * @brief Serialization traits for LocationsDto
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::LocationsDto>
	{
		/**
		 * @brief Serialize LocationsDto to JSON document
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::LocationsDto& obj, Document& doc )
		{
			doc.set( "/visRelease", obj.visVersion );

			Serializer<dnv::vista::sdk::LocationsDto::Items> itemsSerializer;
			doc.set( "/items", itemsSerializer.serialize( obj.items ) );
		}

		/**
		 * @brief Deserialize LocationsDto from JSON document
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 * @throws std::runtime_error if required fields are missing or invalid
		 * @note JSON field "visRelease" maps to member visVersion
		 */
		static void deserialize( dnv::vista::sdk::LocationsDto& obj, const Document& doc )
		{
			auto visReleaseOpt = doc.get<std::string>( "/visRelease" );
			if ( !visReleaseOpt )
			{
				throw std::runtime_error( "LocationsDto: Missing required field 'visRelease'" );
			}
			obj.visVersion = std::move( *visReleaseOpt );

			if ( obj.visVersion.empty() )
			{
				throw std::runtime_error( "LocationsDto: Field 'visRelease' cannot be empty" );
			}

			auto itemsDoc = doc.get<Document>( "/items" );
			if ( !itemsDoc )
			{
				throw std::runtime_error( "LocationsDto: Missing required field 'items'" );
			}

			Serializer<dnv::vista::sdk::LocationsDto::Items> itemsSerializer;
			obj.items = itemsSerializer.deserialize( *itemsDoc );
		}
	};
} // namespace nfx::serialization::json
