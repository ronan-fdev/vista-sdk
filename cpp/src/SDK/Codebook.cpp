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
 * @file Codebook.cpp
 * @brief Implementation of Codebook class methods
 * @details Provides constructor, validation logic, and MetadataTag creation methods
 */

#include "dnv/vista/sdk/Codebook.h"
#include "dto/CodebooksDto.h"

#include <nfx/StringUtils.h>

#include <algorithm>
#include <vector>

namespace dnv::vista::sdk
{
	//=====================================================================
	// Helper functions
	//=====================================================================

	namespace
	{
		constexpr CodebookName parseCodebookName( std::string_view name )
		{
			if ( name == "positions" )
			{
				return CodebookName::Position;
			}
			if ( name == "calculations" )
			{
				return CodebookName::Calculation;
			}
			if ( name == "quantities" )
			{
				return CodebookName::Quantity;
			}
			if ( name == "states" )
			{
				return CodebookName::State;
			}
			if ( name == "contents" )
			{
				return CodebookName::Content;
			}
			if ( name == "commands" )
			{
				return CodebookName::Command;
			}
			if ( name == "types" )
			{
				return CodebookName::Type;
			}
			if ( name == "functional_services" )
			{
				return CodebookName::FunctionalServices;
			}
			if ( name == "maintenance_category" )
			{
				return CodebookName::MaintenanceCategory;
			}
			if ( name == "activity_type" )
			{
				return CodebookName::ActivityType;
			}
			if ( name == "detail" )
			{
				return CodebookName::Detail;
			}

			throw std::invalid_argument( "Unknown codebook name: " + std::string{ name } );
		}
	} // namespace

	//=====================================================================
	// Constructor
	//=====================================================================

	Codebook::Codebook( const CodebookDto& dto )
		: m_name{ parseCodebookName( dto.name ) }
	{
		// Reserve capacity - single allocation for performance
		size_t estimatedSize = 0;
		for ( const auto& [group, values] : dto.values )
		{
			estimatedSize += values.size();
		}

		m_groupMap.reserve( estimatedSize );
		m_standardValues.reserve( estimatedSize );
		m_groups.reserve( dto.values.size() );

		// Build all data structures - cache-friendly single pass
		for ( const auto& [group, values] : dto.values )
		{
			auto trimmedGroup = std::string{ nfx::string::trim( group ) };

			// Add to groups set (skip <number> and Number as they're not real groups)
			if ( trimmedGroup != "<number>" && trimmedGroup != "Number" )
			{
				m_groups.emplace( trimmedGroup );
			}

			for ( const auto& value : values )
			{
				auto trimmedValue = std::string{ nfx::string::trim( value ) };

				// Skip "<number>" placeholder
				if ( trimmedValue == "<number>" )
				{
					continue;
				}

				// Map value → group
				m_groupMap.emplace( trimmedValue, trimmedGroup );

				// Add to standard values
				m_standardValues.emplace( std::move( trimmedValue ) );
			}
		}
	}

	//=====================================================================
	// MetadataTag creation
	//=====================================================================

	std::optional<MetadataTag> Codebook::createTag( std::string_view value ) const noexcept
	{
		if ( nfx::string::isNullOrWhiteSpace( value ) )
		{
			return std::nullopt;
		}

		bool isCustom = false;

		if ( m_name == CodebookName::Position )
		{
			// Validate position
			auto validity = validatePosition( value );
			if ( validity < PositionValidationResult::Valid )
			{
				return std::nullopt;
			}

			if ( validity == PositionValidationResult::Custom )
			{
				isCustom = true;
			}
		}
		else
		{
			if ( !nfx::string::isUriUnreserved( value ) )
			{
				return std::nullopt;
			}
			if ( m_name != CodebookName::Detail && m_standardValues.find( value ) == m_standardValues.end() )
			{
				isCustom = true;
			}
		}

		return MetadataTag{ m_name, std::string{ value }, isCustom };
	}

	//=====================================================================
	// Position validation
	//=====================================================================

	PositionValidationResult Codebook::validatePosition( std::string_view position ) const noexcept
	{
		if ( nfx::string::isNullOrWhiteSpace( position ) || !nfx::string::isUriUnreserved( position ) )
		{
			return PositionValidationResult::Invalid;
		}

		auto trimmed = nfx::string::trim( position );
		if ( trimmed.size() != position.size() )
		{
			return PositionValidationResult::Invalid;
		}

		if ( m_standardValues.find( position ) != m_standardValues.end() )
		{
			return PositionValidationResult::Valid;
		}

		if ( nfx::string::isAllDigits( position ) )
		{
			return PositionValidationResult::Valid;
		}

		if ( !nfx::string::contains( position, "-" ) )
		{
			return PositionValidationResult::Custom;
		}

		// Split and validate each part
		std::vector<std::string_view> parts;
		std::vector<PositionValidationResult> validations;
		parts.reserve( 8 );
		validations.reserve( 8 );

		for ( auto part : nfx::string::splitView( position, '-' ) )
		{
			parts.push_back( part );
			validations.push_back( validatePosition( part ) );
		}
		auto maxValidation = std::max_element( validations.begin(), validations.end(),
			[]( PositionValidationResult a, PositionValidationResult b ) {
				return static_cast<int>( a ) < static_cast<int>( b );
			} );

		bool anyErrors = maxValidation != validations.end() && static_cast<int>( *maxValidation ) < 100;

		// Phase 3: Order validation - numbers only at end
		// Check if any number appears before the last position
		bool numberNotAtEnd = false;
		for ( size_t i = 0; i < parts.size(); ++i )
		{
			if ( nfx::string::isAllDigits( parts[i] ) && i < parts.size() - 1 )
			{
				numberNotAtEnd = true;
				break;
			}
		}

		// Check alphabetical sorting of non-number parts
		std::vector<std::string_view> nonNumbers;
		nonNumbers.reserve( parts.size() );
		for ( auto part : parts )
		{
			if ( !nfx::string::isAllDigits( part ) )
			{
				nonNumbers.push_back( part );
			}
		}

		if ( !std::is_sorted( nonNumbers.begin(), nonNumbers.end() ) )
		{
			return PositionValidationResult::InvalidOrder;
		}

		// Combine order validation results
		if ( numberNotAtEnd )
		{
			return PositionValidationResult::InvalidOrder;
		}

		// If any part had errors, return now (after order check)
		if ( anyErrors && maxValidation != validations.end() )
		{
			return *maxValidation;
		}

		// Phase 4: Grouping validation - only if all parts are exactly Valid (100)
		if ( std::all_of( validations.begin(), validations.end(),
				 []( PositionValidationResult v ) { return static_cast<int>( v ) == 100; } ) )
		{
			std::vector<std::string_view> groups;
			groups.reserve( parts.size() );

			for ( auto part : parts )
			{
				if ( nfx::string::isAllDigits( part ) )
				{
					groups.push_back( "<number>" );
				}
				else
				{
					auto it = m_groupMap.find( part );
					if ( it != m_groupMap.end() )
					{
						groups.push_back( it->second );
					}
				}
			}

			// Check for duplicates (except DEFAULT_GROUP)
			bool hasDefaultGroup = std::find( groups.begin(), groups.end(), "DEFAULT_GROUP" ) != groups.end();
			if ( !hasDefaultGroup )
			{
				std::unordered_set<std::string_view> uniqueGroups( groups.begin(), groups.end() );
				if ( uniqueGroups.size() != groups.size() )
				{
					return PositionValidationResult::InvalidGrouping;
				}
			}
		}

		// Return maximum validation result
		return *maxValidation;
	}
} // namespace dnv::vista::sdk
