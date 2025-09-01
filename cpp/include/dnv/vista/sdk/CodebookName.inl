/**
 * @file CodebookName.inl
 * @brief Inline implementations for performance-critical CodebookName operations
 */

#include <array>
#include <stdexcept>

#include <fmt/format.h>

#include "config/Platform.h"
#include "constants/CodebookConstants.h"

namespace dnv::vista::sdk
{
	namespace internal
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
			{ { constants::codebook::CODEBOOK_PREFIX_POSITION, CodebookName::Position },
				{ constants::codebook::CODEBOOK_PREFIX_QUANTITY, CodebookName::Quantity },
				{ constants::codebook::CODEBOOK_PREFIX_STATE, CodebookName::State },
				{ constants::codebook::CODEBOOK_PREFIX_CONTENT, CodebookName::Content },
				{ constants::codebook::CODEBOOK_PREFIX_COMMAND, CodebookName::Command },
				{ constants::codebook::CODEBOOK_PREFIX_TYPE, CodebookName::Type },
				{ constants::codebook::CODEBOOK_PREFIX_CALCULATION, CodebookName::Calculation },
				{ constants::codebook::CODEBOOK_PREFIX_DETAIL, CodebookName::Detail },
				{ constants::codebook::CODEBOOK_PREFIX_FUNCTIONAL_SERVICES, CodebookName::FunctionalServices },
				{ constants::codebook::CODEBOOK_PREFIX_MAINTENANCE_CATEGORY, CodebookName::MaintenanceCategory },
				{ constants::codebook::CODEBOOK_PREFIX_ACTIVITY_TYPE, CodebookName::ActivityType } } };
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
			throw std::invalid_argument{ "Prefix cannot be empty." };
		}

		for ( const auto& mapping : internal::s_prefixMappings )
		{
			if ( mapping.prefix == prefix )
			{
				return mapping.name;
			}
		}

		throw std::invalid_argument{ fmt::format( "Unknown prefix: {}", prefix ) };
	}

	inline std::string_view CodebookNames::toPrefix( CodebookName name )
	{
		switch ( name )
		{
			case CodebookName::Position:
			{
				return constants::codebook::CODEBOOK_PREFIX_POSITION;
			}
			case CodebookName::Quantity:
			{
				return constants::codebook::CODEBOOK_PREFIX_QUANTITY;
			}
			case CodebookName::Calculation:
			{
				return constants::codebook::CODEBOOK_PREFIX_CALCULATION;
			}
			case CodebookName::State:
			{
				return constants::codebook::CODEBOOK_PREFIX_STATE;
			}
			case CodebookName::Content:
			{
				return constants::codebook::CODEBOOK_PREFIX_CONTENT;
			}
			case CodebookName::Command:
			{
				return constants::codebook::CODEBOOK_PREFIX_COMMAND;
			}
			case CodebookName::Type:
			{
				return constants::codebook::CODEBOOK_PREFIX_TYPE;
			}
			case CodebookName::FunctionalServices:
			{
				return constants::codebook::CODEBOOK_PREFIX_FUNCTIONAL_SERVICES;
			}
			case CodebookName::MaintenanceCategory:
			{
				return constants::codebook::CODEBOOK_PREFIX_MAINTENANCE_CATEGORY;
			}
			case CodebookName::ActivityType:
			{
				return constants::codebook::CODEBOOK_PREFIX_ACTIVITY_TYPE;
			}
			case CodebookName::Detail:
			{
				return constants::codebook::CODEBOOK_PREFIX_DETAIL;
			}
			default:
			{
				throw std::invalid_argument{ fmt::format( "Unknown codebook: {}", static_cast<int>( name ) ) };
			}
		}
	}
}
