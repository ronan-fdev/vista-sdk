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
 * @file VIS.h
 * @brief VIS singleton registry for accessing Gmod, Codebooks, and Locations
 * @details Central entry point for Vista SDK - provides cached access to versioned resources
 */

#pragma once

#include "mqtt/LocalId.h"
#include "Codebooks.h"
#include "Gmod.h"
#include "GmodPath.h"
#include "GmodPathQuery.h"
#include "ImoNumber.h"
#include "LocalId.h"
#include "LocalIdQuery.h"
#include "LocationBuilder.h"
#include "MetadataTagsQuery.h"
#include "ParsingErrors.h"
#include "UniversalId.h"
#include "VisVersions.h"

#include <vector>

namespace dnv::vista::sdk
{
	/**
	 * @brief VIS singleton registry providing access to Vista SDK versioned resources
	 * @details Central entry point for Vista SDK. Provides version management and cached
	 *          access to Gmod, Codebooks, and Locations for all VIS versions.
	 *          Uses Meyer's singleton pattern for thread-safe lazy initialization.
	 */
	class VIS final
	{
	private:
		/** @brief Default constructor */
		VIS() = default;

		/** @brief Copy constructor */
		VIS( const VIS& ) = delete;

		/** @brief Move constructor */
		VIS( VIS&& ) noexcept = delete;

	public:
		/** @brief Destructor */
		~VIS() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		VIS& operator=( const VIS& ) = delete;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		VIS& operator=( VIS&& ) noexcept = delete;

		/**
		 * @brief Get the singleton instance
		 * @return Reference to the singleton VIS instance
		 */
		const static VIS& instance();

		/**
		 * @brief Get all available VIS versions
		 * @return Vector containing all supported VIS versions in lexicographical order
		 */
		const std::vector<VisVersion>& versions() const noexcept;

		/**
		 * @brief Get the latest VIS version
		 * @return The most recent VIS version
		 */
		VisVersion latest() const noexcept;

		/**
		 * @brief Get Gmod for a specific VIS version
		 * @param visVersion The VIS version for which to retrieve the Gmod
		 * @return Reference to a `Gmod` container for the requested version
		 * @throws std::out_of_range if the requested version is not available
		 * @note The returned reference is to an internal cached object owned by the `VIS` singleton.
		 */
		const Gmod& gmod( VisVersion visVersion ) const;

		/**
		 * @brief Get codebooks for a specific VIS version
		 * @param visVersion The VIS version for which to retrieve codebooks
		 * @return Reference to a `Codebooks` container for the requested version
		 * @throws std::out_of_range if the requested version is not available
		 * @note The returned reference is to an internal cached object owned by the `VIS` singleton.
		 */
		const Codebooks& codebooks( VisVersion visVersion ) const;

		/**
		 * @brief Get locations for a specific VIS version
		 * @param visVersion The VIS version for which to retrieve locations
		 * @return Reference to a `Locations` container for the requested version
		 * @throws std::out_of_range if the requested version is not available
		 * @note The returned reference is to an internal cached object owned by the `VIS` singleton.
		 */
		const Locations& locations( VisVersion visVersion ) const;

		/**
		 * @brief Convert a Gmod node from one VIS version to another
		 * @param sourceVersion The VIS version of the source node
		 * @param sourceNode The Gmod node to convert
		 * @param targetVersion The target VIS version
		 * @return The converted node in the target version, or std::nullopt if conversion failed
		 * @details Converts a Gmod node across VIS versions by applying versioning transformations
		 *          step-by-step from source to target version. The location information is preserved
		 *          during conversion when possible.
		 */
		std::optional<GmodNode> convertNode( VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion ) const;

		/**
		 * @brief Convert a Gmod node to a target VIS version (convenience overload)
		 * @param sourceNode The Gmod node to convert (contains source version)
		 * @param targetVersion The target VIS version
		 * @param sourceParent Optional parent node for context (currently unused)
		 * @return The converted node in the target version, or std::nullopt if conversion failed
		 * @details Convenience method that extracts the source version from the node and delegates
		 *          to the full convertNode() method.
		 */
		std::optional<GmodNode> convertNode( const GmodNode& sourceNode, VisVersion targetVersion, const GmodNode* sourceParent = nullptr ) const;

		/**
		 * @brief Convert a Gmod path from one VIS version to another
		 * @param sourceVersion The VIS version of the source path
		 * @param sourcePath The Gmod path to convert
		 * @param targetVersion The target VIS version
		 * @return The converted path in the target version, or std::nullopt if conversion failed
		 * @details Converts a Gmod path across VIS versions by applying node conversions to each
		 *          element in the path. If any node in the path cannot be converted, the entire
		 *          conversion fails.
		 */
		std::optional<GmodPath> convertPath( VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion ) const;

		/**
		 * @brief Convert a Gmod path to a target VIS version (convenience overload)
		 * @param sourcePath The Gmod path to convert (contains source version)
		 * @param targetVersion The target VIS version
		 * @return The converted path in the target version, or std::nullopt if conversion failed
		 * @details Convenience method that extracts the source version from the path and delegates
		 *          to the full convertPath() method.
		 */
		std::optional<GmodPath> convertPath( const GmodPath& sourcePath, VisVersion targetVersion ) const;

		/**
		 * @brief Convert a LocalIdBuilder from one VIS version to another
		 * @param sourceLocalId The LocalIdBuilder to convert
		 * @param targetVersion The target VIS version
		 * @return The converted LocalIdBuilder in the target version, or std::nullopt if conversion failed
		 * @details Converts a LocalIdBuilder by converting its primary and secondary item paths to the
		 *          target version. If either path conversion fails, the entire conversion fails.
		 * @throws std::invalid_argument if the source LocalIdBuilder doesn't have a VIS version
		 */
		std::optional<LocalIdBuilder> convertLocalId( const LocalIdBuilder& sourceLocalId, VisVersion targetVersion ) const;

		/**
		 * @brief Convert a LocalId from one VIS version to another
		 * @param sourceLocalId The LocalId to convert
		 * @param targetVersion The target VIS version
		 * @return The converted LocalId in the target version, or std::nullopt if conversion failed
		 * @details Converts a LocalId by first converting it to a LocalIdBuilder, applying version
		 *          conversion, and then building the final LocalId. Preserves all metadata and quantity
		 *          information during conversion.
		 */
		std::optional<LocalId> convertLocalId( const LocalId& sourceLocalId, VisVersion targetVersion ) const;

		/**
		 * @brief Get multiple Gmod instances for specified VIS versions
		 * @param visVersions Vector of VIS versions to retrieve Gmods for
		 * @return Unordered map containing Gmod references keyed by VIS version
		 * @throws std::out_of_range if any requested version is not available
		 * @details Efficiently loads multiple Gmod versions in a single call. Useful for
		 *          operations that need to work with multiple VIS versions simultaneously,
		 *          such as cross-version comparison or batch conversion operations.
		 *          Each returned reference points to an internal cached object owned by the VIS singleton.
		 */
		std::unordered_map<VisVersion, const Gmod&> gmodsMap( const std::vector<VisVersion>& visVersions ) const;

		/**
		 * @brief Get multiple Codebooks instances for specified VIS versions
		 * @param visVersions Vector of VIS versions to retrieve Codebooks for
		 * @return Unordered map containing Codebooks references keyed by VIS version
		 * @throws std::out_of_range if any requested version is not available
		 * @details Efficiently loads multiple Codebooks versions in a single call. Useful for
		 *          operations that need to work with multiple VIS versions simultaneously.
		 *          Each returned reference points to an internal cached object owned by the VIS singleton.
		 */
		std::unordered_map<VisVersion, const Codebooks&> codebooksMap( const std::vector<VisVersion>& visVersions ) const;

		/**
		 * @brief Get multiple Locations instances for specified VIS versions
		 * @param visVersions Vector of VIS versions to retrieve Locations for
		 * @return Unordered map containing Locations references keyed by VIS version
		 * @throws std::out_of_range if any requested version is not available
		 * @details Efficiently loads multiple Locations versions in a single call. Useful for
		 *          operations that need to work with multiple VIS versions simultaneously.
		 *          Each returned reference points to an internal cached object owned by the VIS singleton.
		 */
		std::unordered_map<VisVersion, const Locations&> locationsMap( const std::vector<VisVersion>& visVersions ) const;
	};
} // namespace dnv::vista::sdk
