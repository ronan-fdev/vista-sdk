/**
 * @file Codebook.cpp
 * @brief Implementation of codebook-related components
 */

#include "pch.h"

#include "dnv/vista/sdk/Codebook.h"

#include "dnv/vista/sdk/Config.h"

#include "dnv/vista/sdk/Codebooks.h"
#include "dnv/vista/sdk/MetadataTag.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Magic numbers
	//=====================================================================

	namespace
	{
		/** @brief Stack allocation limit for position parsing arrays to avoid heap allocation during position validation. */
		static constexpr size_t MAX_POSITIONS = 16;

		/** @brief Stack allocation limit for group tracking arrays to avoid heap allocation during position grouping validation. */
		static constexpr size_t MAX_GROUPS = 16;

		/** @brief Stack allocation limit for non-numeric position tracking to avoid heap allocation during order validation. */
		static constexpr size_t MAX_NON_NUMERIC = 8;
	}

	//=====================================================================
	// Enum mapping tables
	//=====================================================================

	namespace
	{
		constexpr CodebookName codebookNameFromString( std::string_view name )
		{
			switch ( name.size() )
			{
				case 5:
					if ( name == CODEBOOK_NAME_TYPE )
					{
						return CodebookName::Type;
					}
					break;
				case 6:
					if ( name == CODEBOOK_NAME_STATE )
					{
						return CodebookName::State;
					}
					if ( name == CODEBOOK_NAME_DETAIL )
					{
						return CodebookName::Detail;
					}
					break;
				case 8:
					if ( name == CODEBOOK_NAME_CONTENT )
					{
						return CodebookName::Content;
					}
					if ( name == CODEBOOK_NAME_COMMAND )
					{
						return CodebookName::Command;
					}
					break;
				case 9:
					if ( name == CODEBOOK_NAME_POSITION )
					{
						return CodebookName::Position;
					}
					break;
				case 10:
					if ( name == CODEBOOK_NAME_QUANTITY )
					{
						return CodebookName::Quantity;
					}
					break;
				case 12:
					if ( name == CODEBOOK_NAME_CALCULATION )
					{
						return CodebookName::Calculation;
					}
					break;
				case 13:
					if ( name == CODEBOOK_NAME_ACTIVITY_TYPE )
					{
						return CodebookName::ActivityType;
					}
					break;
				case 19:
					if ( name == CODEBOOK_NAME_FUNCTIONAL_SERVICES )
					{
						return CodebookName::FunctionalServices;
					}
					break;
				case 20:
					if ( name == CODEBOOK_NAME_MAINTENANCE_CATEGORY )
					{
						return CodebookName::MaintenanceCategory;
					}
					break;
			}
			throw std::invalid_argument( "Unknown codebook name: " + std::string{ name } );
		}
	}

	//=====================================================================
	// Character lookup tables
	//=====================================================================

	namespace
	{
		/** @brief Standard whitespace characters for string trimming operations. */
		static constexpr std::string_view WHITESPACE = " \t\n\r\f\v";

		alignas( 64 ) constexpr std::array<bool, 256> s_whitespaceLookup = []() constexpr {
			std::array<bool, 256> lookup{};
			constexpr std::string_view whitespace = WHITESPACE;
			for ( char c : whitespace )
			{
				lookup[static_cast<unsigned char>( c )] = true;
			}

			return lookup;
		}();

		constexpr bool isWhitespace( char c ) noexcept
		{
			return s_whitespaceLookup[static_cast<unsigned char>( c )];
		}

		constexpr bool isDigit( char c ) noexcept
		{
			return static_cast<unsigned char>( c - '0' ) <= 9u;
		}

		constexpr std::string_view trimString( std::string_view str ) noexcept
		{
			if ( str.empty() )
			{
				return {};
			}

			/* Find first non-whitespace */
			size_t first = 0;
			const size_t size = str.size();
			while ( first < size && isWhitespace( str[first] ) )
			{
				++first;
			}

			if ( first == size )
			{
				return {};
			}

			/* Find last non-whitespace */
			size_t last = size - 1;
			while ( last > first && isWhitespace( str[last] ) )
			{
				--last;
			}

			return str.substr( first, last - first + 1 );
		}
	}

	//=====================================================================
	// PositionValidationResult string conversion
	//=====================================================================

	namespace
	{
		static constexpr std::array<std::pair<std::string_view, PositionValidationResult>, 5> s_validationResultMap{
			{ { CODEBOOK_POSITION_VALIDATION_INVALID, PositionValidationResult::Invalid },
				{ CODEBOOK_POSITION_VALIDATION_INVALID_ORDER, PositionValidationResult::InvalidOrder },
				{ CODEBOOK_POSITION_VALIDATION_INVALID_GROUPING, PositionValidationResult::InvalidGrouping },
				{ CODEBOOK_POSITION_VALIDATION_VALID, PositionValidationResult::Valid },
				{ CODEBOOK_POSITION_VALIDATION_CUSTOM, PositionValidationResult::Custom } } };

		[[nodiscard]] static std::string toLower( std::string_view input ) noexcept
		{
			std::string result;
			result.reserve( input.size() );

			for ( char c : input )
			{
				result.push_back( ( c >= 'A' && c <= 'Z' ) ? ( c + 32 ) : c );
			}

			return result;
		}
	}

	//=====================================================================
	// PositionValidationResults class
	//=====================================================================

	//----------------------------------------------
	// Static utility methods
	//----------------------------------------------

	PositionValidationResult PositionValidationResults::fromString( std::string_view name )
	{
		if ( name.empty() )
		{
			throw std::invalid_argument( "PositionValidationResult name cannot be empty" );
		}

		const std::string lowerName = toLower( name );

		for ( const auto& [key, value] : s_validationResultMap )
		{
			if ( key == lowerName )
			{
				return value;
			}
		}

		throw std::invalid_argument( "Unknown PositionValidationResult name: '" + std::string{ name } +
									 "'. Valid values are: Invalid, InvalidOrder, InvalidGrouping, Valid, Custom" );
	}

	//=====================================================================
	// Codebook class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Codebook::Codebook( const CodebookDto& dto )
		: m_name{ codebookNameFromString( dto.name() ) },
		  m_groupMap{},
		  m_standardValues{},
		  m_groups{},
		  m_rawData{}
	{
		/* Pre-calculate total size for better memory allocation */
		size_t totalEstimate = 0;
		const auto& dtoValues = dto.values();
		const size_t groupCount = dtoValues.size();

		for ( const auto& [_, values] : dtoValues )
		{
			totalEstimate += values.size();
		}

		/* Load factor calculation */
		const size_t capacity = totalEstimate + ( totalEstimate >> 1 ); /* 1.5x */

		m_groupMap.reserve( capacity );
		m_rawData.reserve( groupCount + ( groupCount >> 2 ) ); /* 1.25x */

		StringSet valueSet;
		StringSet groupSet;
		valueSet.reserve( capacity );
		groupSet.reserve( groupCount + ( groupCount >> 2 ) );

		for ( auto& [groupKey, values] : dto.values() )
		{
			std::string_view groupTrimmed = trimString( groupKey );
			std::string groupStr{ groupTrimmed };

			std::vector<std::string> trimmedValues;
			trimmedValues.reserve( values.size() );

			for ( auto& value : values )
			{
				std::string_view valueTrimmed = trimString( value );
				std::string valueStr{ valueTrimmed };
				trimmedValues.emplace_back( std::move( valueStr ) );

				if ( trimmedValues.back() != CODEBOOK_GROUP_NUMBER )
				{
					m_groupMap.emplace( trimmedValues.back(), groupStr );
					valueSet.emplace( trimmedValues.back() );
					groupSet.emplace( groupStr );
				}
			}

			m_rawData.emplace( std::move( groupStr ), std::move( trimmedValues ) );
		}

		m_standardValues = CodebookStandardValues{ m_name, std::move( valueSet ) };
		m_groups = CodebookGroups{ std::move( groupSet ) };
	}

	//----------------------------------------------
	// Metadata tag creation
	//----------------------------------------------

	std::optional<MetadataTag> Codebook::tryCreateTag( std::string_view valueView ) const
	{
		if ( valueView.empty() )
		{
			return std::nullopt;
		}

		bool isCustom = false;

		if ( m_name == CodebookName::Position )
		{
			PositionValidationResult positionValidity = validatePosition( valueView );

			if ( positionValidity < PositionValidationResult::Valid )
			{
				return std::nullopt;
			}

			if ( positionValidity == PositionValidationResult::Custom )
			{
				isCustom = true;
			}
		}
		else
		{
			if ( !VIS::isISOString( valueView ) )
			{
				return std::nullopt;
			}

			if ( m_name != CodebookName::Detail && !m_standardValues.contains( valueView ) )
			{
				isCustom = true;
			}
		}

		std::string value{ valueView };
		return MetadataTag( m_name, value, isCustom );
	}

	MetadataTag Codebook::createTag( std::string_view value ) const
	{
		auto tagOpt = tryCreateTag( value );
		if ( !tagOpt )
		{
			throw std::invalid_argument( "Invalid value for metadata tag: codebook=" + std::to_string( static_cast<int>( m_name ) ) + ", value=" + std::string{ value } );
		}

		return tagOpt.value();
	}

	//----------------------------------------------
	// Position validation
	//----------------------------------------------

	PositionValidationResult Codebook::validatePosition( std::string_view position ) const
	{
		if ( position.empty() )
		{
			return PositionValidationResult::Invalid;
		}

		bool allDigits = true;
		for ( char c : position )
		{
			if ( isWhitespace( c ) )
			{
				return PositionValidationResult::Invalid;
			}
			if ( allDigits && !isDigit( c ) )
			{
				allDigits = false;
			}
		}

		if ( !VIS::isISOString( position ) )
		{
			return PositionValidationResult::Invalid;
		}

		if ( m_standardValues.contains( position ) )
		{
			return PositionValidationResult::Valid;
		}

		if ( allDigits )
		{
			return PositionValidationResult::Valid;
		}

		const size_t hyphenPos = position.find( '-' );
		if ( hyphenPos == std::string_view::npos )
		{
			return PositionValidationResult::Custom;
		}

		std::array<std::string_view, MAX_POSITIONS> positions;
		size_t positionCount = 0;

		const char* data = position.data();
		const char* end = data + position.size();
		const char* start = data;

		while ( start < end && positionCount < MAX_POSITIONS )
		{
			const char* hyphen = std::find( start, end, '-' );
			positions[positionCount] = std::string_view( start, static_cast<size_t>( hyphen - start ) );
			++positionCount;

			if ( hyphen == end )
			{
				break;
			}

			start = hyphen + 1;
		}

		if ( positionCount >= MAX_POSITIONS )
		{
			return PositionValidationResult::Invalid;
		}

		std::array<bool, MAX_POSITIONS> isDigitArray;
		std::array<std::string_view, MAX_NON_NUMERIC> nonNumericPositions;
		size_t nonNumericCount = 0;
		bool hasNumberNotAtEnd = false;
		PositionValidationResult worstResult = PositionValidationResult::Valid;
		bool isNotSorted = false;

		for ( size_t i = 0; i < positionCount; ++i )
		{
			isDigitArray[i] = !positions[i].empty() && std::all_of( positions[i].begin(), positions[i].end(), isDigit );

			PositionValidationResult result;
			if ( m_standardValues.contains( positions[i] ) )
			{
				result = PositionValidationResult::Valid;
			}
			else if ( isDigitArray[i] )
			{
				result = PositionValidationResult::Valid;
			}
			else
			{
				result = PositionValidationResult::Custom;
			}

			if ( result < PositionValidationResult::Valid )
			{
				return result;
			}
			if ( result > worstResult )
			{
				worstResult = result;
			}

			if ( isDigitArray[i] )
			{
				if ( i < positionCount - 1 )
				{
					hasNumberNotAtEnd = true;
				}
			}
			else
			{
				if ( nonNumericCount < MAX_NON_NUMERIC )
				{
					if ( nonNumericCount > 0 && positions[i] < nonNumericPositions[nonNumericCount - 1] )
					{
						isNotSorted = true;
					}
					nonNumericPositions[nonNumericCount] = positions[i];
					++nonNumericCount;
				}
			}
		}

		if ( hasNumberNotAtEnd || isNotSorted )
		{
			return PositionValidationResult::InvalidOrder;
		}

		if ( worstResult == PositionValidationResult::Valid )
		{
			StringSet uniqueGroups;
			uniqueGroups.reserve( MAX_GROUPS );
			bool hasDefaultGroup = false;

			for ( size_t i = 0; i < positionCount; ++i )
			{
				std::string_view group;

				if ( isDigitArray[i] )
				{
					group = CODEBOOK_GROUP_NUMBER;
				}
				else
				{
					auto it = m_groupMap.find( positions[i] );
					group = ( it != m_groupMap.end() ) ? std::string_view{ it->second } : CODEBOOK_GROUP_UNKNOWN;
				}

				uniqueGroups.emplace( group );

				if ( group == CODEBOOK_GROUP_DEFAULT )
				{
					hasDefaultGroup = true;
				}
			}

			if ( !hasDefaultGroup && uniqueGroups.size() != positionCount )
			{
				return PositionValidationResult::InvalidGrouping;
			}
		}

		return worstResult;
	}
}
