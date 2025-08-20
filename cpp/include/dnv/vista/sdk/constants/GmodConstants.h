/**
 * @file GmodConstants.h
 * @brief GMOD node classification and categorization constants
 * @details Category, type, and composite classification constants for
 *          GMOD node identification and traversal operations
 * @todo Development-only file - remove before production release
 */

#pragma once

namespace dnv::vista::sdk::constants::gmod
{
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
}
