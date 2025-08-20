/**
 * @file VIS.h
 * @brief Vessel Information Structure (VIS) interface and implementation
 *
 * Provides access to VIS data like GMOD, Codebooks, and Locations,
 * along with version conversion utilities and ISO string validation.
 */

#pragma once

#include "internal/MemoryCache.h"
#include "utils/StringUtils.h"

#include "Codebooks.h"
#include "Gmod.h"
#include "Locations.h"
#include "VISVersion.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	class CodebooksDto;
	class GmodDto;
	class GmodNode;
	class GmodPath;
	class GmodVersioning;
	class GmodVersioningDto;
	class LocalId;
	class LocalIdBuilder;
	class LocationsDto;

	//=====================================================================
	// VIS singleton
	//=====================================================================

	/**
	 * @brief Singleton implementation for Vessel Information Structure operations.
	 *
	 * This class provides efficient access to VIS data (GMOD, Codebooks, Locations)
	 * using thread-safe caching mechanisms. It follows the Singleton pattern to ensure
	 * a single point of access and consistent data management throughout the application.
	 */
	class VIS final
	{
	private:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Private constructor (singleton pattern) */
		inline VIS();

		/** @brief Copy constructor */
		VIS( const VIS& ) = delete;

		/** @brief Move constructor */
		VIS( VIS&& ) noexcept = delete;

	public:
		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~VIS() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		VIS& operator=( const VIS& ) = delete;

		/** @brief Move assignment operator */
		VIS& operator=( VIS&& ) noexcept = delete;

		//----------------------------------------------
		// Singleton access
		//----------------------------------------------

		/**
		 * @brief Provides access to the singleton instance of the VIS class.
		 * @return A reference to the unique VIS instance.
		 */
		[[nodiscard]] inline static VIS& instance();

		//----------------------------------------------
		// DTO Loading
		//----------------------------------------------

		/**
		 * @brief Statically loads the GMOD Data Transfer Object (DTO) for a specific VIS version.
		 * This method performs direct loading (e.g., from a file or resource) without instance caching.
		 * @param visVersion The VIS version for which to load the GMOD DTO.
		 * @return An std::optional<GmodDto> containing the DTO if loading is successful, or std::nullopt otherwise.
		 */
		[[nodiscard]] static std::optional<GmodDto> loadGmodDto( VisVersion visVersion );

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		//-----------------------------
		// VisVersion
		//-----------------------------

		/**
		 * @brief Get all available and supported VIS versions.
		 * @return A const reference to vector containing all supported VisVersion enumerators.
		 */
		[[nodiscard]] inline const std::vector<VisVersion>& visVersions() const noexcept;

		/**
		 * @brief Get the latest known and supported VIS version.
		 * @return The latest VisVersion.
		 */
		[[nodiscard]] inline VisVersion latestVisVersion() const noexcept;

		//-----------------------------
		// Cached objects
		//-----------------------------

		/**
		 * @brief Retrieves the GMOD versioning information processed into a GmodVersioning object.
		 * This object provides higher-level access to version conversion logic.
		 * @return The GmodVersioning object.
		 * @throws std::runtime_error If the GMOD versioning data cannot be loaded or processed.
		 */
		[[nodiscard]] const GmodVersioning& gmodVersioning();

		/**
		 * @brief Get the GMOD (Generic Product Model) for a specific VIS version.
		 * @param visVersion The VIS version for which to retrieve the GMOD.
		 * @return A constant reference to the GMOD object.
		 * @throws std::invalid_argument If the provided VIS version is invalid or not supported.
		 */
		[[nodiscard]] inline const Gmod& gmod( VisVersion visVersion );

		/**
		 * @brief Get the Codebooks for a specific VIS version.
		 * @param visVersion The VIS version for which to retrieve the Codebooks.
		 * @return A constant reference to the Codebooks object.
		 * @throws std::invalid_argument If the provided VIS version is invalid or not supported.
		 */
		[[nodiscard]] inline const Codebooks& codebooks( VisVersion visVersion );

		/**
		 * @brief Get the Locations for a specific VIS version.
		 * @param visVersion The VIS version for which to retrieve the Locations.
		 * @return A constant reference to the Locations object.
		 * @throws std::invalid_argument If the provided VIS version is invalid or not supported.
		 */
		[[nodiscard]] inline const Locations& locations( VisVersion visVersion );

		//-----------------------------
		// Cached maps
		//-----------------------------

		/**
		 * @brief Get GMODs for multiple VIS versions.
		 * @param visVersions A vector of VIS versions.
		 * @return A new unordered_map where keys are VisVersion and values are Gmod objects.
		 * @throws std::invalid_argument If any provided VIS version is invalid or not supported.
		 */
		[[nodiscard]] std::unordered_map<VisVersion, Gmod> gmodsMap( const std::vector<VisVersion>& visVersions );

		/**
		 * @brief Get Codebooks for multiple VIS versions.
		 * @param visVersions A vector of VIS versions.
		 * @return A new unordered_map where keys are VisVersion and values are Codebooks objects.
		 * @throws std::invalid_argument If any provided VIS version is invalid or not supported.
		 */
		[[nodiscard]] std::unordered_map<VisVersion, Codebooks> codebooksMap( const std::vector<VisVersion>& visVersions );

		/**
		 * @brief Get Locations for multiple VIS versions.
		 * @param visVersions A vector of VIS versions.
		 * @return A new unordered_map where keys are VisVersion and values are Locations objects.
		 * @throws std::invalid_argument If any provided VIS version is invalid or not supported.
		 */
		[[nodiscard]] std::unordered_map<VisVersion, Locations> locationsMap( const std::vector<VisVersion>& visVersions );

		//----------------------------------------------
		// Cached DTO
		//----------------------------------------------

		/**
		 * @brief Retrieves the GMOD Data Transfer Object (DTO) for a specific VIS version.
		 * @param visVersion The VIS version for which to retrieve the GMOD DTO.
		 * @return The GmodDto object.
		 * @throws std::invalid_argument If the provided VIS version is invalid or not supported.
		 * @throws std::runtime_error If the GMOD DTO cannot be loaded or found for the specified version.
		 */
		[[nodiscard]] inline const GmodDto& gmodDto( VisVersion visVersion );

		/**
		 * @brief Retrieves the Codebooks Data Transfer Object (DTO) for a specific VIS version.
		 * @param visVersion The VIS version for which to retrieve the Codebooks DTO.
		 * @return The CodebooksDto object.
		 * @throws std::invalid_argument If the provided VIS version is invalid or not supported.
		 * @throws std::runtime_error If the Codebooks DTO cannot be loaded or found.
		 */
		[[nodiscard]] inline const CodebooksDto& codebooksDto( VisVersion visVersion );

		/**
		 * @brief Retrieves the Locations Data Transfer Object (DTO) for a specific VIS version.
		 * @param visVersion The VIS version for which to retrieve the Locations DTO.
		 * @return The LocationsDto object.
		 * @throws std::invalid_argument If the provided VIS version is invalid or not supported.
		 * @throws std::runtime_error If the Locations DTO cannot be loaded or found.
		 */
		[[nodiscard]] inline const LocationsDto& locationsDto( VisVersion visVersion );

		/**
		 * @brief Retrieves the GMOD versioning DTOs.
		 * This method provides access to the data structures defining how GMOD nodes convert between versions.
		 * @return A const reference to StringMap where keys are version transition strings with heterogeneous lookup support,
		 *         and values are GmodVersioningDto objects.
		 * @throws std::runtime_error If the versioning DTOs cannot be loaded.
		 */
		[[nodiscard]] static const utils::StringMap<GmodVersioningDto>& gmodVersioningDto();

		//----------------------------------------------
		// Conversion
		//----------------------------------------------

		//-----------------------------
		// GmodNode
		//-----------------------------

		/**
		 * @brief Convert a GMOD node from one VIS version to another.
		 * @param sourceVersion The VIS version of the source GMOD node.
		 * @param sourceNode The GMOD node to convert.
		 * @param targetVersion The target VIS version for the conversion.
		 * @return An std::optional containing the converted GmodNode if successful, or std::nullopt if conversion is not possible or fails.
		 * @throws std::invalid_argument If any provided VIS version is invalid or not supported.
		 */
		[[nodiscard]] std::optional<GmodNode> convertNode( VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion );

		/**
		 * @brief Convert a GMOD node to a different VIS version, inferring source version from the node.
		 * @param sourceNode The GMOD node to convert (must have its VIS version embedded or retrievable).
		 * @param targetVersion The target VIS version for the conversion.
		 * @param sourceParent Optional parent GMOD node for context during conversion, can be nullptr.
		 * @return An std::optional containing the converted GmodNode if successful, or std::nullopt if conversion is not possible or fails.
		 * @throws std::invalid_argument If the target VIS version is invalid or not supported.
		 */
		[[nodiscard]] std::optional<GmodNode> convertNode( const GmodNode& sourceNode, VisVersion targetVersion, const GmodNode* sourceParent = nullptr );

		//-----------------------------
		// GmodPath
		//-----------------------------

		/**
		 * @brief Convert a GMOD path from one VIS version to another.
		 * @param sourceVersion The VIS version of the source GMOD path.
		 * @param sourcePath The GMOD path to convert.
		 * @param targetVersion The target VIS version for the conversion.
		 * @return An std::optional containing the converted GmodPath if successful, or std::nullopt if conversion is not possible or fails.
		 * @throws std::invalid_argument If any provided VIS version is invalid or not supported.
		 */
		[[nodiscard]] std::optional<GmodPath> convertPath( VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion );

		/**
		 * @brief Convert a GMOD path to a different VIS version, inferring source version from the path.
		 * @param sourcePath The GMOD path to convert (must have its VIS version embedded or retrievable).
		 * @param targetVersion The target VIS version for the conversion.
		 * @return An std::optional containing the converted GmodPath if successful, or std::nullopt if conversion is not possible or fails.
		 * @throws std::invalid_argument If the target VIS version is invalid or not supported.
		 */
		[[nodiscard]] std::optional<GmodPath> convertPath( const GmodPath& sourcePath, VisVersion targetVersion );

		//-----------------------------
		// LocalId
		//-----------------------------

		/**
		 * @brief Convert a LocalIdBuilder instance to a different VIS version.
		 * @param sourceLocalId The LocalIdBuilder to convert.
		 * @param targetVersion The target VIS version for the conversion.
		 * @return An std::optional containing the converted LocalIdBuilder if successful, or std::nullopt if conversion is not possible or fails.
		 * @throws std::invalid_argument If the target VIS version is invalid or not supported.
		 */
		[[nodiscard]] std::optional<LocalIdBuilder> convertLocalId( const LocalIdBuilder& sourceLocalId, VisVersion targetVersion );

		/**
		 * @brief Convert a LocalId instance to a different VIS version.
		 * @param sourceLocalId The LocalId to convert.
		 * @param targetVersion The target VIS version for the conversion.
		 * @return An std::optional containing the converted LocalId if successful, or std::nullopt if conversion is not possible or fails.
		 * @throws std::invalid_argument If the target VIS version is invalid or not supported.
		 */
		[[nodiscard]] std::optional<LocalId> convertLocalId( const LocalId& sourceLocalId, VisVersion targetVersion );

		//----------------------------------------------
		// ISO string validation
		//----------------------------------------------

		/**
		 * @brief Validates ISO Local ID format for any string-like container.
		 * @details Rules according to: "ISO19848 5.2.1, Note 1" and "RFC3986 2.3 - Unreserved characters"
		 *          Allows '/' characters within the string, validates all other characters against ISO rules.
		 * @tparam StringLike Any type convertible to string_view (string, string_view, const char*, etc.)
		 * @param value The string-like value to validate
		 * @return True if the value matches the ISO Local ID format, false otherwise
		 */
		template <typename StringLike>
		[[nodiscard]] inline static bool matchISOLocalIdString( const StringLike& value ) noexcept;

		/**
		 * @brief Validates that all characters are valid ISO characters for any string-like container.
		 * @details Rules according to: "ISO19848 5.2.1, Note 1" and "RFC3986 2.3 - Unreserved characters"
		 *          Does NOT allow '/' characters - strict ISO character validation only.
		 * @tparam StringLike Any type convertible to string_view (string, string_view, const char*, etc.)
		 * @param value The string-like value to validate
		 * @return True if all characters are valid ISO characters, false otherwise
		 */
		template <typename StringLike>
		[[nodiscard]] inline static bool isISOString( const StringLike& value ) noexcept;

		/**
		 * @brief Alias for matchISOLocalIdString providing semantic clarity.
		 * @details Comprehensive check combining ISO character validation with Local ID format rules.
		 *          Identical behavior to matchISOLocalIdString - provided for API consistency.
		 * @tparam StringLike Any type convertible to string_view (string, string_view, const char*, etc.)
		 * @param value The string-like value to validate
		 * @return True if the value is a valid ISO Local ID string, false otherwise
		 */
		template <typename StringLike>
		[[nodiscard]] inline static bool isISOLocalIdString( const StringLike& value ) noexcept;

		/**
		 * @brief Validates a single character against ISO character rules.
		 * @details Rules according to: "ISO19848 5.2.1, Note 1" and "RFC3986 2.3 - Unreserved characters"
		 *          Fast single-character validation using ASCII decimal code checking.
		 *          Supports: 0-9, A-Z, a-z, -, ., _, ~
		 * @param c The character to check
		 * @return True if the character is valid for ISO strings, false otherwise
		 */
		[[nodiscard]] inline static bool isISOString( char c ) noexcept;

		/**
		 * @brief Core ASCII character validation using decimal code ranges.
		 * @details Validates character codes against ISO-compliant ranges:
		 *          - Numbers: 48-57 (0-9)
		 *          - Uppercase: 65-90 (A-Z)
		 *          - Lowercase: 97-122 (a-z)
		 *          - Special: 45(-), 46(.), 95(_), 126(~)
		 * @param code The integer ASCII code to check
		 * @return True if the code represents a valid ISO character, false otherwise
		 */
		[[nodiscard]] inline static constexpr bool matchAsciiDecimal( int code ) noexcept;

	private:
		//----------------------------------------------
		// Private helper methods for static caches
		//----------------------------------------------

		/**
		 * @brief Access static thread-safe cache for GMOD DTOs by VisVersion
		 */
		static internal::MemoryCache<VisVersion, GmodDto>& gmodDtoCache();

		/**
		 * @brief Access static thread-safe cache for Codebooks DTOs by VisVersion
		 */
		static internal::MemoryCache<VisVersion, CodebooksDto>& codebooksDtoCache();

		/**
		 * @brief Access static thread-safe cache for Locations DTOs by VisVersion
		 */
		static internal::MemoryCache<VisVersion, LocationsDto>& locationsDtoCache();

		/**
		 * @brief Access static thread-safe cache for processed Codebooks objects by VisVersion
		 */
		static internal::MemoryCache<VisVersion, Codebooks>& codebooksCache();

		/**
		 * @brief Access static thread-safe cache for processed GMOD objects by VisVersion
		 */
		static internal::MemoryCache<VisVersion, Gmod>& gmodsCache();

		/**
		 * @brief Access static thread-safe cache for processed Locations objects by VisVersion
		 */
		static internal::MemoryCache<VisVersion, Locations>& locationsCache();

		/**
		 * @brief Access static thread-safe cache for GMOD versioning object
		 */
		static internal::MemoryCache<int, GmodVersioning>& gmodVersioningCache();
	};
}

#include "VIS.inl"
