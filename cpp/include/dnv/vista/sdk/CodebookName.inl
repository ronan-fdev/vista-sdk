/**
 * @file CodebookName.inl
 * @brief Inline implementations for performance-critical CodebookName operations
 */

#pragma once

#include "Config.h"

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// Prefix-to-CodebookName mapping table
		//=====================================================================

		struct PrefixMapping
		{
			std::string_view prefix;
			CodebookName name;
		};

		static constexpr std::array<PrefixMapping, 11> s_prefixMappings{
			{ { CODEBOOK_PREFIX_POSITION, CodebookName::Position },
				{ CODEBOOK_PREFIX_QUANTITY, CodebookName::Quantity },
				{ CODEBOOK_PREFIX_STATE, CodebookName::State },
				{ CODEBOOK_PREFIX_CONTENT, CodebookName::Content },
				{ CODEBOOK_PREFIX_COMMAND, CodebookName::Command },
				{ CODEBOOK_PREFIX_TYPE, CodebookName::Type },
				{ CODEBOOK_PREFIX_CALCULATION, CodebookName::Calculation },
				{ CODEBOOK_PREFIX_DETAIL, CodebookName::Detail },
				{ CODEBOOK_PREFIX_FUNCTIONAL_SERVICES, CodebookName::FunctionalServices },
				{ CODEBOOK_PREFIX_MAINTENANCE_CATEGORY, CodebookName::MaintenanceCategory },
				{ CODEBOOK_PREFIX_ACTIVITY_TYPE, CodebookName::ActivityType } } };
	}

	//=====================================================================
	// CodebookNames class
	//=====================================================================

	//----------------------------------------------
	// Public static methods
	//----------------------------------------------

	inline CodebookName CodebookNames::fromPrefix( std::string_view prefix )
	{
		if ( prefix.empty() )
		{
			throw std::invalid_argument( "Prefix cannot be empty." );
		}

		for ( const auto& mapping : s_prefixMappings )
		{
			if ( mapping.prefix == prefix )
			{
				return mapping.name;
			}
		}

		throw std::invalid_argument( fmt::format( "Unknown prefix: {}", prefix ) );
	}

	inline std::string_view CodebookNames::toPrefix( CodebookName name )
	{
		switch ( name )
		{
			case CodebookName::Position:
			{
				return CODEBOOK_PREFIX_POSITION;
			}
			case CodebookName::Quantity:
			{
				return CODEBOOK_PREFIX_QUANTITY;
			}
			case CodebookName::Calculation:
			{
				return CODEBOOK_PREFIX_CALCULATION;
			}
			case CodebookName::State:
			{
				return CODEBOOK_PREFIX_STATE;
			}
			case CodebookName::Content:
			{
				return CODEBOOK_PREFIX_CONTENT;
			}
			case CodebookName::Command:
			{
				return CODEBOOK_PREFIX_COMMAND;
			}
			case CodebookName::Type:
			{
				return CODEBOOK_PREFIX_TYPE;
			}
			case CodebookName::FunctionalServices:
			{
				return CODEBOOK_PREFIX_FUNCTIONAL_SERVICES;
			}
			case CodebookName::MaintenanceCategory:
			{
				return CODEBOOK_PREFIX_MAINTENANCE_CATEGORY;
			}
			case CodebookName::ActivityType:
			{
				return CODEBOOK_PREFIX_ACTIVITY_TYPE;
			}
			case CodebookName::Detail:
			{
				return CODEBOOK_PREFIX_DETAIL;
			}
			default:
			{
				throw std::invalid_argument( fmt::format( "Unknown codebook: {}", static_cast<int>( name ) ) );
			}
		}
	}
}
