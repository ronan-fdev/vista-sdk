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
 * @file GmodVersioningDto.h
 * @brief Data Transfer Objects for Gmod Versioning JSON resources
 * @details Header-only DTOs for deserializing Gmod versioning data from Vista SDK resources.
 */

#pragma once

#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/SerializationTraits.h>

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace dnv::vista::sdk
{
	/**
	 * @brief DTO for Gmod node assignment changes
	 * @details Represents changes in assignment names between versions.
	 */
	struct GmodVersioningAssignmentChangeDto
	{
		std::string oldAssignment;	   ///< Old assignment name
		std::string currentAssignment; ///< Current assignment name

		/**
		 * @brief Equality comparison operator
		 * @param other Assignment change to compare with
		 * @return true if assignment changes are equal
		 */
		bool operator==( const GmodVersioningAssignmentChangeDto& other ) const
		{
			return oldAssignment == other.oldAssignment &&
				   currentAssignment == other.currentAssignment;
		}

		/**
		 * @brief Inequality comparison operator
		 * @param other Assignment change to compare with
		 * @return true if assignment changes are not equal
		 */
		bool operator!=( const GmodVersioningAssignmentChangeDto& other ) const
		{
			return !( *this == other );
		}
	};

	/**
	 * @brief DTO for Gmod node conversion information
	 * @details Contains conversion operations and mappings between versions.
	 */
	struct GmodNodeConversionDto
	{
		using Operations = std::unordered_set<std::string>;

		Operations operations;						  ///< Set of operations (e.g., "changeCode", "merge")
		std::optional<std::string> source;			  ///< Source node identifier
		std::optional<std::string> target;			  ///< Target node identifier
		std::optional<std::string> oldAssignment;	  ///< Old assignment name
		std::optional<std::string> newAssignment;	  ///< New assignment name
		std::optional<bool> deleteAssignment = false; ///< Whether assignment should be deleted

		/**
		 * @brief Equality comparison operator
		 * @param other Conversion to compare with
		 * @return true if conversions are equal
		 */
		bool operator==( const GmodNodeConversionDto& other ) const
		{
			return operations == other.operations && source == other.source && target == other.target &&
				   oldAssignment == other.oldAssignment && newAssignment == other.newAssignment &&
				   deleteAssignment == other.deleteAssignment;
		}

		/**
		 * @brief Inequality comparison operator
		 * @param other Conversion to compare with
		 * @return true if conversions are not equal
		 */
		bool operator!=( const GmodNodeConversionDto& other ) const { return !( *this == other ); }
	};

	/**
	 * @brief DTO for the complete Gmod versioning structure
	 * @details Contains the VIS version and all node conversion mappings.
	 */
	struct GmodVersioningDto
	{
		using Items = std::unordered_map<std::string, GmodNodeConversionDto>;

		std::string visVersion; ///< VIS version string (maps to "visRelease" in JSON)
		Items items;			///< Map of node code to conversion information

		/**
		 * @brief Equality comparison operator
		 * @param other Versioning to compare with
		 * @return true if versioning data are equal
		 */
		bool operator==( const GmodVersioningDto& other ) const
		{
			return visVersion == other.visVersion && items == other.items;
		}

		/**
		 * @brief Inequality comparison operator
		 * @param other Versioning to compare with
		 * @return true if versioning data are not equal
		 */
		bool operator!=( const GmodVersioningDto& other ) const
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
		 * @throws std::runtime_error if required fields are missing or invalid
		 */
		static void deserialize( dnv::vista::sdk::GmodVersioningAssignmentChangeDto& obj, const Document& doc )
		{
			auto oldAssignmentOpt = doc.get<std::string>( "/oldAssignment" );
			if ( !oldAssignmentOpt )
			{
				throw std::runtime_error(
					"GmodVersioningAssignmentChangeDto: Missing required field 'oldAssignment'" );
			}
			obj.oldAssignment = std::move( *oldAssignmentOpt );

			auto currentAssignmentOpt = doc.get<std::string>( "/currentAssignment" );
			if ( !currentAssignmentOpt )
			{
				throw std::runtime_error(
					"GmodVersioningAssignmentChangeDto: Missing required field 'currentAssignment'" );
			}
			obj.currentAssignment = std::move( *currentAssignmentOpt );
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

			if ( obj.source )
			{
				doc.set( "/source", *obj.source );
			}
			if ( obj.target )
			{
				doc.set( "/target", *obj.target );
			}
			if ( obj.oldAssignment )
			{
				doc.set( "/oldAssignment", *obj.oldAssignment );
			}
			if ( obj.newAssignment )
			{
				doc.set( "/newAssignment", *obj.newAssignment );
			}
			if ( obj.deleteAssignment )
			{
				doc.set( "/deleteAssignment", *obj.deleteAssignment );
			}
		}

		/**
		 * @brief Deserialize GmodNodeConversionDto from JSON document
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 * @throws std::runtime_error if required fields are missing or invalid
		 */
		static void deserialize( dnv::vista::sdk::GmodNodeConversionDto& obj, const Document& doc )
		{
			// Required fields
			auto operationsDoc = doc.get<Document>( "/operations" );
			if ( !operationsDoc )
			{
				throw std::runtime_error( "GmodNodeConversionDto: Missing required field 'operations'" );
			}
			Serializer<dnv::vista::sdk::GmodNodeConversionDto::Operations> operationsSerializer;
			obj.operations = operationsSerializer.deserialize( *operationsDoc );

			auto sourceOpt = doc.get<std::string>( "/source" );
			if ( !sourceOpt )
			{
				throw std::runtime_error( "GmodNodeConversionDto: Missing required field 'source'" );
			}
			obj.source = std::move( *sourceOpt );

			// Optional fields
			if ( auto val = doc.get<std::string>( "/target" ) )
			{
				obj.target = std::move( *val );
			}
			if ( auto val = doc.get<std::string>( "/oldAssignment" ) )
			{
				obj.newAssignment = std::move( *val );
			}
			if ( auto val = doc.get<std::string>( "/newAssignment" ) )
			{
				obj.newAssignment = std::move( *val );
			}
			if ( auto val = doc.get<std::string>( "/deleteAssignment" ) )
			{
				obj.newAssignment = std::move( *val );
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
		 * @throws std::runtime_error if required fields are missing or invalid
		 * @note JSON field "visRelease" maps to member visVersion
		 */
		static void deserialize( dnv::vista::sdk::GmodVersioningDto& obj, const Document& doc )
		{
			auto visReleaseOpt = doc.get<std::string>( "/visRelease" );
			if ( !visReleaseOpt )
			{
				throw std::runtime_error( "GmodVersioningDto: Missing required field 'visRelease'" );
			}
			obj.visVersion = std::move( *visReleaseOpt );

			if ( obj.visVersion.empty() )
			{
				throw std::runtime_error( "GmodVersioningDto: Field 'visRelease' cannot be empty" );
			}

			auto itemsDoc = doc.get<Document>( "/items" );
			if ( !itemsDoc )
			{
				throw std::runtime_error( "GmodVersioningDto: Missing required field 'items'" );
			}

			Serializer<dnv::vista::sdk::GmodVersioningDto::Items> itemsSerializer;
			obj.items = itemsSerializer.deserialize( *itemsDoc );
		}
	};
} // namespace nfx::serialization::json
