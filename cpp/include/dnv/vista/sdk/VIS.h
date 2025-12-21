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

#include "Codebooks.h"
#include "Gmod.h"
#include "GmodPath.h"
#include "ImoNumber.h"
#include "LocationBuilder.h"
#include "ParsingErrors.h"
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
	};
} // namespace dnv::vista::sdk
