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
 * @file CodebookName.h
 * @brief Codebook name enumeration and conversion utilities
 * @details Defines the CodebookName enum representing various VIS codebook types
 *          (quantity, content, calculation, etc.) and provides utilities for converting
 *          between enum values and their string prefixes used in VIS identifiers.
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace dnv::vista::sdk
{
	/**
	 * @brief Enumeration of VIS codebook types
	 * @details Represents the different types of codebooks used in the VIS (Vessel Information Structure)
	 *          standard. Each codebook type has a corresponding string prefix used in VIS local identifiers.
	 *
	 *          The enum values map to the following string prefixes:
	 *          - Quantity: "qty"
	 *          - Content: "cnt"
	 *          - Calculation: "calc"
	 *          - State: "state"
	 *          - Command: "cmd"
	 *          - Type: "type"
	 *          - FunctionalServices: "funct.svc"
	 *          - MaintenanceCategory: "maint.cat"
	 *          - ActivityType: "act.type"
	 *          - Position: "pos"
	 *          - Detail: "detail"
	 */
	enum class CodebookName : std::uint8_t
	{
		Quantity = 1,
		Content,
		Calculation,
		State,
		Command,
		Type,
		FunctionalServices,
		MaintenanceCategory,
		ActivityType,
		Position,
		Detail
	};

	/**
	 * @brief Utility functions for converting between CodebookName enum values and string prefixes
	 * @details Provides functions to convert codebook name prefixes (e.g., "qty", "cnt", "calc")
	 *          to their corresponding CodebookName enum values and vice versa. This is used when
	 *          parsing and serializing VIS local identifiers and paths.
	 */
	namespace CodebookNames
	{
		/**
		 * @brief Convert a string prefix to a CodebookName enum value
		 * @param prefix The string prefix (e.g., "qty", "cnt", "calc", "state", etc.)
		 * @return The corresponding CodebookName enum value
		 * @throws std::invalid_argument if the prefix is not recognized
		 */
		inline CodebookName fromPrefix( std::string_view prefix );

		/**
		 * @brief Convert a CodebookName enum value to its string prefix
		 * @param name The CodebookName enum value
		 * @return The corresponding string prefix (e.g., "qty" for Quantity, "cnt" for Content)
		 * @throws std::invalid_argument if the codebook name is invalid
		 */
		inline std::string toPrefix( CodebookName name );
	} // namespace CodebookNames
} // namespace dnv::vista::sdk

#include "detail/CodebookName.inl"
