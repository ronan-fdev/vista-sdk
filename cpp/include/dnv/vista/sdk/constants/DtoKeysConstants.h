/**
 * @file DtoKeysConstants.h
 * @brief JSON DTO parsing key constants for all Vista SDK data structures
 * @details String constants for parsing Codebooks, GMOD, Locations,
 *          and Versioning JSON data with zero-allocation string_view usage
 * @todo Development-only file - remove before production release
 */

#pragma once

namespace dnv::vista::sdk::constants::dto
{
	//=====================================================================
	// JSON DTO parsing key constants
	//=====================================================================

	//----------------------------------------------
	// Codebook DTO
	//----------------------------------------------

	//-----------------------------
	// Codebook DTO parsing keys
	//-----------------------------

	/** @brief VIS release version key for codebook JSON parsing. */
	inline constexpr std::string_view CODEBOOK_DTO_KEY_VIS_RELEASE = "visRelease";

	/** @brief Codebook name key for JSON parsing. */
	inline constexpr std::string_view CODEBOOK_DTO_KEY_NAME = "name";

	/** @brief Items array key for codebook JSON parsing. */
	inline constexpr std::string_view CODEBOOK_DTO_KEY_ITEMS = "items";

	/** @brief Values array key for codebook JSON parsing. */
	inline constexpr std::string_view CODEBOOK_DTO_KEY_VALUES = "values";

	//-----------------------------
	// Codebook DTO error constants
	//-----------------------------

	/** @brief Unknown codebook name fallback constant. */
	inline constexpr std::string_view CODEBOOK_DTO_UNKNOWN_NAME = "[unknown name]";

	/** @brief Unknown codebook version fallback constant. */
	inline constexpr std::string_view CODEBOOK_DTO_UNKNOWN_VERSION = "[unknown version]";

	//----------------------------------------------
	// GMOD DTO
	//----------------------------------------------

	//-----------------------------
	// GMOD DTO parsing keys
	//-----------------------------

	/** @brief VIS release version key for GMOD JSON parsing. */
	inline constexpr std::string_view GMOD_DTO_KEY_VIS_RELEASE = "visRelease";

	/** @brief GMOD name key for JSON parsing. */
	inline constexpr std::string_view GMOD_DTO_KEY_NAME = "name";

	/** @brief Items array key for GMOD JSON parsing. */
	inline constexpr std::string_view GMOD_DTO_KEY_ITEMS = "items";

	//-----------------------------
	// GMOD DTO classification keys
	//-----------------------------

	/** @brief Category classification key for GMOD JSON parsing. */
	inline constexpr std::string_view GMOD_DTO_KEY_CATEGORY = "category";

	/** @brief Type classification key for GMOD JSON parsing. */
	inline constexpr std::string_view GMOD_DTO_KEY_TYPE = "type";

	/** @brief Code identifier key for GMOD JSON parsing. */
	inline constexpr std::string_view GMOD_DTO_KEY_CODE = "code";

	/** @brief Common name key for GMOD JSON parsing. */
	inline constexpr std::string_view GMOD_DTO_KEY_COMMON_NAME = "commonName";

	/** @brief Definition key for GMOD JSON parsing. */
	inline constexpr std::string_view GMOD_DTO_KEY_DEFINITION = "definition";

	/** @brief Common definition key for GMOD JSON parsing. */
	inline constexpr std::string_view GMOD_DTO_KEY_COMMON_DEFINITION = "commonDefinition";

	/** @brief Install substructure key for GMOD JSON parsing. */
	inline constexpr std::string_view GMOD_DTO_KEY_INSTALL_SUBSTRUCTURE = "installSubstructure";

	/** @brief Normal assignment names key for GMOD JSON parsing. */
	inline constexpr std::string_view GMOD_DTO_KEY_NORMAL_ASSIGNMENT_NAMES = "normalAssignmentNames";

	/** @brief Relations key for GMOD JSON parsing. */
	inline constexpr std::string_view GMOD_DTO_KEY_RELATIONS = "relations";

	//-----------------------------
	// GMOD DTO error constants
	//-----------------------------

	/** @brief Unknown GMOD code fallback constant. */
	inline constexpr std::string_view GMOD_DTO_UNKNOWN_CODE = "[unknown code]";

	/** @brief Unknown GMOD version fallback constant. */
	inline constexpr std::string_view GMOD_DTO_UNKNOWN_VERSION = "[unknown version]";

	//----------------------------------------------
	// GMOD versioning DTO
	//----------------------------------------------

	//-----------------------------
	// GMOD versioning DTO parsing keys
	//-----------------------------

	/** @brief VIS release version key for GMOD versioning JSON parsing. */
	inline constexpr std::string_view GMODVERSIONING_DTO_KEY_VIS_RELEASE = "visRelease";

	/** @brief Items array key for GMOD versioning JSON parsing. */
	inline constexpr std::string_view GMODVERSIONING_DTO_KEY_ITEMS = "items";

	/** @brief Old assignment key for GMOD versioning JSON parsing. */
	inline constexpr std::string_view GMODVERSIONING_DTO_KEY_OLD_ASSIGNMENT = "oldAssignment";

	/** @brief Current assignment key for GMOD versioning JSON parsing. */
	inline constexpr std::string_view GMODVERSIONING_DTO_KEY_CURRENT_ASSIGNMENT = "currentAssignment";

	/** @brief New assignment key for GMOD versioning JSON parsing. */
	inline constexpr std::string_view GMODVERSIONING_DTO_KEY_NEW_ASSIGNMENT = "newAssignment";

	/** @brief Delete assignment key for GMOD versioning JSON parsing. */
	inline constexpr std::string_view GMODVERSIONING_DTO_KEY_DELETE_ASSIGNMENT = "deleteAssignment";

	/** @brief Operations key for GMOD versioning JSON parsing. */
	inline constexpr std::string_view GMODVERSIONING_DTO_KEY_OPERATIONS = "operations";

	/** @brief Source key for GMOD versioning JSON parsing. */
	inline constexpr std::string_view GMODVERSIONING_DTO_KEY_SOURCE = "source";

	/** @brief Target key for GMOD versioning JSON parsing. */
	inline constexpr std::string_view GMODVERSIONING_DTO_KEY_TARGET = "target";

	//-----------------------------
	// GMOD versioning DTO error constants
	//-----------------------------

	/** @brief Unknown GMOD versioning version fallback constant. */
	inline constexpr std::string_view GMODVERSIONING_DTO_UNKNOWN_VERSION = "[unknown version]";

	/** @brief Unknown old assignment fallback constant. */
	inline constexpr std::string_view GMODVERSIONING_DTO_UNKNOWN_OLD_ASSIGNMENT = "[unknown oldAssignment]";

	/** @brief Unknown source fallback constant. */
	inline constexpr std::string_view GMODVERSIONING_DTO_UNKNOWN_SOURCE = "[unknown source]";

	//----------------------------------------------
	// ISO19848 DTO
	//----------------------------------------------

	//-----------------------------
	// ISO19848 DTO parsing keys
	//-----------------------------

	/** @brief Values key for ISO19848 JSON parsing. */
	inline constexpr std::string_view ISO19848_DTO_KEY_VALUES = "values";

	/** @brief Type key for ISO19848 JSON parsing. */
	inline constexpr std::string_view ISO19848_DTO_KEY_TYPE = "type";

	/** @brief Description key for ISO19848 JSON parsing. */
	inline constexpr std::string_view ISO19848_DTO_KEY_DESCRIPTION = "description";

	//-----------------------------
	// ISO19848 DTO error constants
	//-----------------------------

	/** @brief Unknown ISO19848 type fallback constant. */
	inline constexpr std::string_view ISO19848_DTO_UNKNOWN_TYPE = "[unknown type]";

	//----------------------------------------------
	// Locations DTO
	//----------------------------------------------

	//-----------------------------
	// Locations DTO parsing keys
	//-----------------------------

	/** @brief Code key for locations JSON parsing. */
	inline constexpr std::string_view LOCATIONS_DTO_KEY_CODE = "code";

	/** @brief Name key for locations JSON parsing. */
	inline constexpr std::string_view LOCATIONS_DTO_KEY_NAME = "name";

	/** @brief Definition key for locations JSON parsing. */
	inline constexpr std::string_view LOCATIONS_DTO_KEY_DEFINITION = "definition";

	/** @brief VIS release version key for locations JSON parsing. */
	inline constexpr std::string_view LOCATIONS_DTO_KEY_VIS_RELEASE = "visRelease";

	/** @brief Items array key for locations JSON parsing. */
	inline constexpr std::string_view LOCATIONS_DTO_KEY_ITEMS = "items";

	//-----------------------------
	// Locations DTO error constants
	//-----------------------------

	/** @brief Unknown location code fallback constant. */
	inline constexpr std::string_view LOCATIONS_DTO_UNKNOWN_CODE = "[unknown code]";

	/** @brief Unknown location version fallback constant. */
	inline constexpr std::string_view LOCATIONS_DTO_UNKNOWN_VERSION = "[unknown version]";
}
