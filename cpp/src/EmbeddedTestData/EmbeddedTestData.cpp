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
 * @file EmbeddedTestData.cpp
 * @brief Implementation of test data resource loading
 */

#include "EmbeddedTestData.h"

#include <nfx/serialization/json/Serializer.h>
#include <algorithm>
#include <ranges>
#include <sstream>
#include <stdexcept>

namespace dnv::vista::sdk::testdata
{
	struct TestResource
	{
		std::string_view name;
		const uint8_t* data;
		size_t size;
	};

	const std::vector<TestResource>& all();
} // namespace dnv::vista::sdk::testdata

namespace dnv::vista::sdk
{
	std::vector<uint8_t> EmbeddedTestData::getBytes( std::string_view filename )
	{
		const auto& resources = testdata::all();
		auto it = std::ranges::find_if( resources, [filename]( const auto& r ) {
			return r.name == filename;
		} );

		if ( it == resources.end() )
		{
			return {};
		}

		return std::vector<uint8_t>( it->data, it->data + it->size );
	}

	std::string EmbeddedTestData::getText( std::string_view filename )
	{
		const auto bytes = getBytes( filename );
		if ( bytes.empty() )
		{
			return {};
		}

		return std::string( reinterpret_cast<const char*>( bytes.data() ), bytes.size() );
	}

	std::vector<std::string> EmbeddedTestData::listFiles()
	{
		const auto& resources = testdata::all();
		std::vector<std::string> filenames;
		filenames.reserve( resources.size() );

		for ( const auto& r : resources )
		{
			filenames.emplace_back( r.name );
		}

		return filenames;
	}
} // namespace dnv::vista::sdk
