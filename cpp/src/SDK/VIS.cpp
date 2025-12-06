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
} // namespace dnv::vista::sdk
