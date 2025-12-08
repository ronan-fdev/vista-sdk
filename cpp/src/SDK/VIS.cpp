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
 * @file VIS.cpp
 * @brief Implementation of VIS singleton registry
 * @details Provides version management and will provide cached access to Gmod, Codebooks, and Locations
 */

#include "dnv/vista/sdk/VIS.h"

#include "VisVersionsExtensions.h"

#include "dnv/vista/sdk/Codebooks.h"
#include "dto/CodebooksDto.h"

#include <EmbeddedResource/EmbeddedResource.h>

#include <stdexcept>

namespace dnv::vista::sdk
{
	const VIS& VIS::instance()
	{
		static VIS instance;

		return instance;
	}

	VisVersion VIS::latest() const noexcept
	{
		return VisVersions::latest();
	}

	const std::vector<VisVersion>& VIS::versions() const noexcept
	{
		static const std::vector<VisVersion> versions = []() {
			const auto arr = VisVersions::all();
			return std::vector<VisVersion>( arr.begin(), arr.end() );
		}();

		return versions;
	}

	const Codebooks& VIS::codebooks( VisVersion visVersion ) const
	{
		// Check cache first
		auto it = m_codebooksCache.find( visVersion );
		if ( it != m_codebooksCache.end() )
			return it->second;

		// Load from embedded resource
		auto versionStr = VisVersions::toString( visVersion );
		auto dto = EmbeddedResource::codebooks( versionStr );

		if ( !dto.has_value() )
			throw std::out_of_range{ "Codebooks not available for version: " + std::string{ versionStr } };

		// Construct and cache
		auto [cacheIt, inserted] = m_codebooksCache.emplace( visVersion, Codebooks{ visVersion, *dto } );
		return cacheIt->second;
	}
} // namespace dnv::vista::sdk
