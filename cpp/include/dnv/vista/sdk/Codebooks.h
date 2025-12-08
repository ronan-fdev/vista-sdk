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
 * @file Codebooks.h
 * @brief VIS codebooks container with version-specific codebook access
 * @details Container for all VIS codebooks for a specific version. Provides access
 *          to individual Codebook instances and convenience methods for creating tags.
 */

#pragma once

#include "Codebook.h"

#include <array>
#include <optional>

namespace dnv::vista::sdk
{
	struct CodebooksDto;
	enum class VisVersion : std::uint8_t;

	/**
	 * @brief Container for all VIS codebooks for a specific version
	 * @details Holds all 11 codebook types (Position, Quantity, Content, etc.) for a given
	 *          VIS version. Provides indexed access and convenience methods for tag creation.
	 */
	class Codebooks final
	{
		friend class VIS;

	private:
		/**
		 * @brief Internal constructor - creates codebooks from DTO
		 * @param version The VIS version these codebooks belong to
		 * @param dto DTO containing all codebook data
		 */
		explicit Codebooks( VisVersion version, const CodebooksDto& dto ) noexcept;

	public:
		/** @brief Default constructor */
		Codebooks() = delete;

		/** @brief Copy constructor */
		Codebooks( const Codebooks& ) = default;

		/** @brief Move constructor */
		Codebooks( Codebooks&& ) noexcept = default;

		/** @brief Destructor */
		~Codebooks() = default;

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this object
		 */
		Codebooks& operator=( const Codebooks& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this object
		 */
		Codebooks& operator=( Codebooks&& ) noexcept = default;

		/**
		 * @brief Get a codebook by name
		 * @param name The codebook name
		 * @return Const reference to the requested Codebook
		 * @throws std::out_of_range if name is invalid
		 */
		inline const Codebook& operator[]( CodebookName name ) const;

		/**
		 * @brief Get the VIS version for these codebooks
		 * @return The VisVersion enum value
		 */
		inline VisVersion version() const noexcept;

	private:
		/** @brief VIS version for these codebooks */
		VisVersion m_version;

		/** @brief Fixed-size array holding all codebooks */
		std::array<std::optional<Codebook>, static_cast<size_t>( CodebookName::Detail )> m_codebooks;
	};
} // namespace dnv::vista::sdk

#include "detail/Codebooks.inl"
