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
 * @file EmbeddedSchemas.h
 * @brief Header for embedded schema resources
 * @details Provides compile-time embedded access to JSON and XSD schema files
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace dnv::vista::sdk::schemas
{
	/**
	 * @brief Represents an embedded schema resource
	 */
	struct SchemaResource
	{
		std::string_view name; ///< Schema file name (relative path from schemas/)
		const uint8_t* data;   ///< Pointer to embedded schema data
		size_t size;		   ///< Size of schema data in bytes
	};

	/**
	 * @brief Get all embedded schema resources
	 * @return Vector of all embedded schemas (JSON and XSD)
	 * @details Schemas include:
	 *          - JSON schemas for DataChannelList, TimeSeriesData
	 *          - XML schemas (XSD) for DataChannelList, TimeSeriesData
	 *          - Sample JSON files
	 */
	const std::vector<SchemaResource>& all();
} // namespace dnv::vista::sdk::schemas
