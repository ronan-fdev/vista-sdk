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
 * @file EmbeddedTestData.h
 * @brief Access embedded test data files for unit tests
 * @details Provides access to testdata/ *.json, testdata/ *.txt, etc. embedded at build time
 */

#pragma once

#include <nfx/serialization/json/Serializer.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace dnv::vista::sdk
{
	/**
	 * @brief Access to embedded test data resources
	 *
	 * This class provides access to testdata/ files embedded at compile time.
	 * Unlike EmbeddedResource (for production data), this is test-specific.
	 *
	 * Example:
	 * @code
	 *   // Load and deserialize Codebook test data
	 *   auto testData = EmbeddedTestData::load<CodebookTestDataDto>("Codebook.json");
	 *
	 *   // List all available test files
	 *   auto files = EmbeddedTestData::listFiles();
	 * @endcode
	 */
	class EmbeddedTestData
	{
	public:
		/**
		 * @brief List all available test data filenames
		 * @return Vector of filenames (e.g., ["Codebook.json", "LocalIds.txt", ...])
		 */
		static std::vector<std::string> listFiles();

		/**
		 * @brief Load and deserialize JSON test data to DTO
		 * @tparam TDto DTO type to deserialize into
		 * @param filename Name of the JSON file (without path, e.g., "Codebook.json")
		 * @return Deserialized DTO, or std::nullopt if not found or parse error
		 */
		template <typename TDto>
		static std::optional<TDto> load( std::string_view filename );

	private:
		static std::vector<uint8_t> getBytes( std::string_view filename );
		static std::string getText( std::string_view filename );
	};

	template <typename TDto>
	std::optional<TDto> EmbeddedTestData::load( std::string_view filename )
	{
		auto text = getText( filename );
		if ( text.empty() )
		{
			return std::nullopt;
		}

		// Special case: std::string just returns the text directly
		if constexpr ( std::is_same_v<TDto, std::string> )
		{
			return text;
		}

		// Special case: Document just parses JSON directly
		if constexpr ( std::is_same_v<TDto, nfx::serialization::json::Document> )
		{
			return nfx::serialization::json::Document::fromString( text );
		}
		else
		{
			// Other DTOs use Serializer
			try
			{
				return nfx::serialization::json::Serializer<TDto>::fromString( text );
			}
			catch ( ... )
			{
				return std::nullopt;
			}
		}
	}
} // namespace dnv::vista::sdk
