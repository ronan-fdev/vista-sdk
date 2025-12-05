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

	class EmbeddedResource final
	{
	public:
		static std::vector<std::string> visVersions();
		static std::optional<GmodDto> gmod( std::string_view visVersion );
		static std::optional<CodebooksDto> codebooks( std::string_view visVersion );
		static std::optional<LocationsDto> locations( std::string_view visVersion );
		static std::optional<std::unordered_map<std::string, GmodVersioningDto>> gmodVersioning();
		static std::optional<DataChannelTypeNamesDto> dataChannelTypeNames( std::string_view version );
		static std::optional<FormatDataTypesDto> formatDataTypes( std::string_view version );

	private:
		static std::vector<std::string> resourceNames();
		static std::vector<uint8_t> decompressedStream( std::string_view resourceName );
	};
} // namespace dnv::vista::sdk
