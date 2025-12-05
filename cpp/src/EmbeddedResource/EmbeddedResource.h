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
 * @file EmbeddedResource.h
 * @brief Resource loading utilities for Vista SDK
 * @details Provides access to compile-time embedded VIS resources with automatic decompression
 */

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace dnv::vista::sdk
{
	struct GmodDto;
	struct GmodVersioningDto;
	struct CodebooksDto;
	struct LocationsDto;
	struct DataChannelTypeNamesDto;
	struct FormatDataTypesDto;

	/**
	 * @brief Access to embedded VIS resource data
	 * @details Loads and decompresses compile-time embedded .gz resources.
	 *          All methods are thread-safe and cache results internally.
	 */
	class EmbeddedResource final
	{
	public:
		/**
		 * @brief Get list of available VIS versions
		 * @return Vector of version strings (e.g., ["3-4a", "3-5a", "3-6a"])
		 */
		static std::vector<std::string> visVersions();

		/**
		 * @brief Load Gmod data for a VIS version
		 * @param visVersion VIS version string (e.g., "3-4a")
		 * @return Gmod DTO if found, nullopt otherwise
		 */
		static std::optional<GmodDto> gmod( std::string_view visVersion );

		/**
		 * @brief Load Codebooks data for a VIS version
		 * @param visVersion VIS version string (e.g., "3-4a")
		 * @return Codebooks DTO if found, nullopt otherwise
		 */
		static std::optional<CodebooksDto> codebooks( std::string_view visVersion );

		/**
		 * @brief Load Locations data for a VIS version
		 * @param visVersion VIS version string (e.g., "3-4a")
		 * @return Locations DTO if found, nullopt otherwise
		 */
		static std::optional<LocationsDto> locations( std::string_view visVersion );

		/**
		 * @brief Load Gmod versioning data
		 * @return Map of Gmod versioning data if available, nullopt otherwise
		 */
		static std::optional<std::unordered_map<std::string, GmodVersioningDto>> gmodVersioning();

		/**
		 * @brief Load DataChannelType names for ISO19848
		 * @param version Version string
		 * @return DataChannelType names DTO if found, nullopt otherwise
		 */
		static std::optional<DataChannelTypeNamesDto> dataChannelTypeNames( std::string_view version );

		/**
		 * @brief Load FormatDataTypes for ISO19848
		 * @param version Version string
		 * @return FormatDataTypes DTO if found, nullopt otherwise
		 */
		static std::optional<FormatDataTypesDto> formatDataTypes( std::string_view version );
	};
} // namespace dnv::vista::sdk
