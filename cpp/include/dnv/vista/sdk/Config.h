/**
 * @file Config.h
 * @brief Performance-optimized constants for GMOD node classification and operations
 */

#pragma once

//=====================================================================
// Cross-platform compiler support
//=====================================================================

//----------------------------------------------
// Compiler intrinsics headers
//----------------------------------------------

/** @brief Platform-specific intrinsics headers for SIMD and hardware optimization support */
#ifdef _MSC_VER
#	include <intrin.h>
#elif defined( __GNUC__ ) || defined( __clang__ )
#	include <immintrin.h>
#	include <x86intrin.h>
#endif

//----------------------------------------------
// Cross-compiler performance macros
//----------------------------------------------

/** @brief Cross-compiler force inline directive for performance-critical functions */
#if defined( _MSC_VER )
#	define VISTA_SDK_CPP_FORCE_INLINE __forceinline
#elif defined( __GNUC__ ) || defined( __clang__ )
#	define VISTA_SDK_CPP_FORCE_INLINE __attribute__( ( always_inline ) ) inline
#else
#	define VISTA_SDK_CPP_FORCE_INLINE inline
#endif

//----------------------------------------------
// Compiler-specific C++20 feature support
//----------------------------------------------

/** @brief Conditional constexpr support for GCC 11.x which has incomplete constexpr std::string support */
#if defined( __GNUC__ ) && __GNUC__ >= 11 && __GNUC__ < 12
#	define VISTA_SDK_CPP_CONDITIONAL_CONSTEXPR
#else
#	define VISTA_SDK_CPP_CONDITIONAL_CONSTEXPR constexpr
#endif

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// Hash algorithm constants
		//=====================================================================

		/** @brief FNV offset basis constant for hash calculations. */
		inline constexpr uint32_t FNV_OFFSET_BASIS{ 0x811C9DC5 };

		/** @brief FNV prime constant for hash calculations. */
		inline constexpr uint32_t FNV_PRIME{ 0x01000193 };

		//=====================================================================
		// CHD dictionary configuration
		//=====================================================================

		/** @brief Maximum multiplier for seed search iterations in CHD construction. */
		inline constexpr uint32_t MAX_SEED_SEARCH_MULTIPLIER = 100;

		//=====================================================================
		// GMOD node classification constants
		//=====================================================================

		//----------------------------------------------
		// Primary node categories
		//----------------------------------------------

		/** @brief Asset category identifier for GMOD nodes. */
		inline constexpr std::string_view GMODNODE_CATEGORY_ASSET = "ASSET";

		/** @brief Function category identifier for GMOD nodes. */
		inline constexpr std::string_view GMODNODE_CATEGORY_FUNCTION = "FUNCTION";

		/** @brief Product category identifier for GMOD nodes. */
		inline constexpr std::string_view GMODNODE_CATEGORY_PRODUCT = "PRODUCT";

		//----------------------------------------------
		// Composite node categories
		//----------------------------------------------

		/** @brief Asset function composite category for GMOD nodes. */
		inline constexpr std::string_view GMODNODE_CATEGORY_ASSET_FUNCTION = "ASSET FUNCTION";

		/** @brief Product function composite category for GMOD nodes. */
		inline constexpr std::string_view GMODNODE_CATEGORY_PRODUCT_FUNCTION = "PRODUCT FUNCTION";

		//----------------------------------------------
		// Node Type Classifications
		//----------------------------------------------

		/** @brief Selection type identifier for GMOD nodes. */
		inline constexpr std::string_view GMODNODE_TYPE_SELECTION = "SELECTION";

		/** @brief Group type identifier for GMOD nodes. */
		inline constexpr std::string_view GMODNODE_TYPE_GROUP = "GROUP";

		/** @brief Leaf type identifier for GMOD nodes. */
		inline constexpr std::string_view GMODNODE_TYPE_LEAF = "LEAF";

		/** @brief Type type identifier for GMOD nodes. */
		inline constexpr std::string_view GMODNODE_TYPE_TYPE = "TYPE";

		/** @brief Composition type identifier for GMOD nodes. */
		inline constexpr std::string_view GMODNODE_TYPE_COMPOSITION = "COMPOSITION";

		//----------------------------------------------
		// Full Type Specifications
		//----------------------------------------------

		/** @brief Complete asset function leaf type specification. */
		inline constexpr std::string_view GMODNODE_FULLTYPE_ASSET_FUNCTION_LEAF = "ASSET FUNCTION LEAF";

		/** @brief Complete product function leaf type specification. */
		inline constexpr std::string_view GMODNODE_FULLTYPE_PRODUCT_FUNCTION_LEAF = "PRODUCT FUNCTION LEAF";

		//=====================================================================
		// Codebooks
		//=====================================================================

		//----------------------------------------------
		// Codebooks names
		//----------------------------------------------

		inline constexpr std::string_view CODEBOOK_NAME_POSITION = "positions";
		inline constexpr std::string_view CODEBOOK_NAME_CALCULATION = "calculations";
		inline constexpr std::string_view CODEBOOK_NAME_QUANTITY = "quantities";
		inline constexpr std::string_view CODEBOOK_NAME_STATE = "states";
		inline constexpr std::string_view CODEBOOK_NAME_CONTENT = "contents";
		inline constexpr std::string_view CODEBOOK_NAME_COMMAND = "commands";
		inline constexpr std::string_view CODEBOOK_NAME_TYPE = "types";
		inline constexpr std::string_view CODEBOOK_NAME_FUNCTIONAL_SERVICES = "functional_services";
		inline constexpr std::string_view CODEBOOK_NAME_MAINTENANCE_CATEGORY = "maintenance_category";
		inline constexpr std::string_view CODEBOOK_NAME_ACTIVITY_TYPE = "activity_type";
		inline constexpr std::string_view CODEBOOK_NAME_DETAIL = "detail";

		//----------------------------------------------
		// Codebooks prefix constants
		//----------------------------------------------

		inline constexpr std::string_view CODEBOOK_PREFIX_POSITION = "pos";
		inline constexpr std::string_view CODEBOOK_PREFIX_QUANTITY = "qty";
		inline constexpr std::string_view CODEBOOK_PREFIX_CALCULATION = "calc";
		inline constexpr std::string_view CODEBOOK_PREFIX_STATE = "state";
		inline constexpr std::string_view CODEBOOK_PREFIX_CONTENT = "cnt";
		inline constexpr std::string_view CODEBOOK_PREFIX_COMMAND = "cmd";
		inline constexpr std::string_view CODEBOOK_PREFIX_TYPE = "type";
		inline constexpr std::string_view CODEBOOK_PREFIX_FUNCTIONAL_SERVICES = "funct.svc";
		inline constexpr std::string_view CODEBOOK_PREFIX_MAINTENANCE_CATEGORY = "maint.cat";
		inline constexpr std::string_view CODEBOOK_PREFIX_ACTIVITY_TYPE = "act.type";
		inline constexpr std::string_view CODEBOOK_PREFIX_DETAIL = "detail";

		//----------------------------------------------
		// Position validation group constants
		//----------------------------------------------

		/** @brief Special group identifier for numeric-only values in VISTA codebooks. */
		inline constexpr std::string_view CODEBOOK_GROUP_NUMBER = "<number>";

		/** @brief Default group name for ungrouped position components. */
		inline constexpr std::string_view CODEBOOK_GROUP_DEFAULT = "DEFAULT_GROUP";

		/** @brief Fallback group identifier for unrecognized position components. */
		inline constexpr std::string_view CODEBOOK_GROUP_UNKNOWN = "UNKNOWN";

		//----------------------------------------------
		// Position validation results constants
		//----------------------------------------------

		inline constexpr std::string_view CODEBOOK_POSITION_VALIDATION_INVALID = "invalid";
		inline constexpr std::string_view CODEBOOK_POSITION_VALIDATION_INVALID_ORDER = "invalidorder";
		inline constexpr std::string_view CODEBOOK_POSITION_VALIDATION_INVALID_GROUPING = "invalidgrouping";
		inline constexpr std::string_view CODEBOOK_POSITION_VALIDATION_VALID = "valid";
		inline constexpr std::string_view CODEBOOK_POSITION_VALIDATION_CUSTOM = "custom";

		//=====================================================================
		// DTO
		//=====================================================================

		//----------------------------------------------
		// Codebooks DTO keys
		//----------------------------------------------

		inline constexpr std::string_view CODEBOOK_DTO_KEY_VIS_RELEASE = "visRelease";
		inline constexpr std::string_view CODEBOOK_DTO_KEY_NAME = "name";
		inline constexpr std::string_view CODEBOOK_DTO_KEY_ITEMS = "items";

		inline constexpr std::string_view CODEBOOK_DTO_KEY_VALUES = "values";

		//----------------------------------------------
		// Gmod DTO keys
		//----------------------------------------------

		inline constexpr std::string_view GMOD_DTO_KEY_VIS_RELEASE = "visRelease";
		inline constexpr std::string_view GMOD_DTO_KEY_NAME = "name";
		inline constexpr std::string_view GMOD_DTO_KEY_ITEMS = "items";

		inline constexpr std::string_view GMOD_DTO_KEY_CATEGORY = "category";
		inline constexpr std::string_view GMOD_DTO_KEY_TYPE = "type";
		inline constexpr std::string_view GMOD_DTO_KEY_CODE = "code";
		inline constexpr std::string_view GMOD_DTO_KEY_COMMON_NAME = "commonName";
		inline constexpr std::string_view GMOD_DTO_KEY_DEFINITION = "definition";
		inline constexpr std::string_view GMOD_DTO_KEY_COMMON_DEFINITION = "commonDefinition";
		inline constexpr std::string_view GMOD_DTO_KEY_INSTALL_SUBSTRUCTURE = "installSubstructure";
		inline constexpr std::string_view GMOD_DTO_KEY_NORMAL_ASSIGNMENT_NAMES = "normalAssignmentNames";
		inline constexpr std::string_view GMOD_DTO_KEY_RELATIONS = "relations";

		//----------------------------------------------
		// Gmod versioning DTO keys
		//----------------------------------------------

		inline constexpr std::string_view GMODVERSIONING_DTO_KEY_VIS_RELEASE = "visRelease";
		inline constexpr std::string_view GMODVERSIONING_DTO_KEY_ITEMS = "items";

		inline constexpr std::string_view GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT = "oldAssignment";
		inline constexpr std::string_view GMODVERSIONING_DTO_KEY_CURRENT_ASSIGNMENT = "currentAssignment";
		inline constexpr std::string_view GMODVERSIONING_DTO_KEY_NEW_ASSIGNMENT = "newAssignment";
		inline constexpr std::string_view GMODVERSIONING_DTO_KEY_DELETE_ASSIGNMENT = "deleteAssignment";
		inline constexpr std::string_view GMODVERSIONING_DTO_KEY_OPERATIONS = "operations";
		inline constexpr std::string_view GMODVERSIONING_DTO_KEY_SOURCE = "source";
		inline constexpr std::string_view GMODVERSIONING_DTO_KEY_TARGET = "target";

		//----------------------------------------------
		// ISO19848 DTO keys
		//----------------------------------------------

		inline constexpr std::string_view ISO19848_DTO_KEY_VALUES = "values";
		inline constexpr std::string_view ISO19848_DTO_KEY_TYPE = "type";
		inline constexpr std::string_view ISO19848_DTO_KEY_DESCRIPTION = "description";

		//----------------------------------------------
		// Locations DTO keys
		//----------------------------------------------

		inline constexpr std::string_view LOCATIONS_DTO_KEY_CODE = "code";
		inline constexpr std::string_view LOCATIONS_DTO_KEY_NAME = "name";
		inline constexpr std::string_view LOCATIONS_DTO_KEY_DEFINITION = "definition";
		inline constexpr std::string_view LOCATIONS_DTO_KEY_VIS_RELEASE = "visRelease";
		inline constexpr std::string_view LOCATIONS_DTO_KEY_ITEMS = "items";

		//----------------------------------------------
		// String parsing constants
		//----------------------------------------------

		inline constexpr std::string_view NULL_OR_WHITESPACE = " \t\n\r\f\v";
	}
}
