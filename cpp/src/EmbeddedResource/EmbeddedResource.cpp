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
 * @file EmbeddedResource.cpp
 * @brief Implementation of resource loading utilities for Vista SDK
 */

#include "EmbeddedResource.h"

#include <CodebooksDto.h>
#include <GmodDto.h>
#include <LocationsDto.h>
#include <GmodVersioningDto.h>
#include <ISO19848Dtos.h>

#include <nfx/serialization/json/Serializer.h>
#include <nfx/string/Utils.h>
#include <zlib-ng.h>

#include <algorithm>
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace dnv::vista::sdk::registry
{
	struct Resource
	{
		std::string_view name;
		const uint8_t* data;
		size_t size;
	};

	const std::vector<Resource>& all();
} // namespace dnv::vista::sdk::registry

namespace dnv::vista::sdk
{
	namespace
	{
		std::vector<uint8_t> decompressedStream( std::string_view resourceName )
		{
			const auto& resources = registry::all();
			const auto it = std::ranges::find_if( resources,
				[resourceName]( const auto& res ) { return res.name == resourceName; } );

			if ( it == resources.end() )
			{
				throw std::runtime_error( "Resource not found: " + std::string( resourceName ) );
			}

			const auto* compressedData = it->data;
			const auto compressedSize = it->size;

			// Initialize zlib-ng for gzip decompression
			// windowBits = 15 + 16: 15 = max window size (32KB), +16 = gzip format with header/trailer
			// Reference: https://www.zlib.net/manual.html#Advanced (inflateInit2 documentation)
			::zng_stream gz = {};
			if ( ::zng_inflateInit2( &gz, 15 + 16 ) != Z_OK )
			{
				throw std::runtime_error( "Failed to init zlib-ng for: " + std::string( resourceName ) );
			}

			// Decompress with dynamic buffer growth
			std::vector<uint8_t> decompressedData;
			decompressedData.reserve( static_cast<size_t>( compressedSize ) * 16 );

			gz.avail_in = static_cast<uint32_t>( compressedSize );
			gz.next_in = compressedData;

			constexpr size_t chunkSize = 32768;
			uint8_t chunk[chunkSize];

			int ret;
			do
			{
				gz.avail_out = chunkSize;
				gz.next_out = chunk;

				ret = ::zng_inflate( &gz, Z_NO_FLUSH );

				if ( ret != Z_OK && ret != Z_STREAM_END )
				{
					::zng_inflateEnd( &gz );
					throw std::runtime_error( "Failed to decompress: " + std::string( resourceName ) );
				}

				size_t have = chunkSize - gz.avail_out;
				decompressedData.insert( decompressedData.end(), chunk, chunk + have );
			} while ( ret != Z_STREAM_END );

			::zng_inflateEnd( &gz );

			return decompressedData;
		}

		template <typename TDto>
		std::optional<TDto> loadResource( std::string_view resourceKey, std::string_view version,
			std::vector<uint8_t> ( *decompressFn )( std::string_view ) )
		{
			const auto& resources = registry::all();

			auto it = std::ranges::find_if( resources, [resourceKey, version]( const auto& r ) {
				const bool matchesKey = nfx::string::contains( r.name, resourceKey );
				const bool isGzipped = nfx::string::endsWith( r.name, ".gz" );
				const bool matchesVersion = version.empty() || nfx::string::contains( r.name, version );
				return matchesKey && isGzipped && matchesVersion;
			} );

			if ( it == resources.end() )
			{
				return std::nullopt;
			}

			auto decompressed = decompressFn( it->name );
			std::string_view jsonStr{ reinterpret_cast<const char*>( decompressed.data() ), decompressed.size() };

			return nfx::serialization::json::Serializer<TDto>::fromString( jsonStr );
		}
	} // namespace

	std::vector<std::string> EmbeddedResource::visVersions()
	{
		const auto& resources = registry::all();
		std::vector<std::string> versions;

		for ( const auto& res : resources )
		{
			// Filter: ends with ".gz", contains "gmod", NOT "versioning"
			// Filename pattern: "gmod-vis-{version}.json.gz" e.g. "gmod-vis-3-9a.json.gz"
			if ( nfx::string::endsWith( res.name, ".gz" ) &&
				 nfx::string::contains( res.name, "gmod" ) &&
				 !nfx::string::contains( res.name, "versioning" ) )
			{
				// Extract version from filename
				// "gmod-vis-3-9a.json.gz" -> "3-9a"
				constexpr std::string_view prefix = "gmod-vis-";
				constexpr std::string_view suffix = ".json.gz";

				auto prefixPos = res.name.find( prefix );
				if ( prefixPos != std::string_view::npos )
				{
					auto versionStart = prefixPos + prefix.size();
					auto suffixPos = res.name.find( suffix, versionStart );
					if ( suffixPos != std::string_view::npos )
					{
						versions.emplace_back( res.name.substr( versionStart, suffixPos - versionStart ) );
					}
				}
			}
		}

		if ( versions.empty() )
		{
			throw std::runtime_error( "Did not find required resources (expected gmod-vis-*.json.gz resources)" );
		}

		return versions;
	}

	std::optional<GmodDto> EmbeddedResource::gmod( std::string_view visVersion )
	{
		const auto& resources = registry::all();

		auto it = std::ranges::find_if( resources, [visVersion]( const auto& r ) {
			return nfx::string::contains( r.name, "gmod" ) &&
				   nfx::string::endsWith( r.name, ".gz" ) &&
				   !nfx::string::contains( r.name, "versioning" ) &&
				   nfx::string::contains( r.name, visVersion );
		} );

		if ( it == resources.end() )
		{
			return std::nullopt;
		}

		auto decompressed = decompressedStream( it->name );
		std::string_view jsonStr{ reinterpret_cast<const char*>( decompressed.data() ), decompressed.size() };

		return nfx::serialization::json::Serializer<GmodDto>::fromString( jsonStr );
	}

	std::optional<CodebooksDto> EmbeddedResource::codebooks( std::string_view visVersion )
	{
		return loadResource<CodebooksDto>( "codebooks", visVersion, &decompressedStream );
	}

	std::optional<LocationsDto> EmbeddedResource::locations( std::string_view visVersion )
	{
		return loadResource<LocationsDto>( "locations", visVersion, &decompressedStream );
	}

	std::optional<std::unordered_map<std::string, GmodVersioningDto>> EmbeddedResource::gmodVersioning()
	{
		const auto& resources = registry::all();

		std::unordered_map<std::string, GmodVersioningDto> dtos;

		for ( const auto& res : resources )
		{
			if ( nfx::string::contains( res.name, "gmod-vis-versioning" ) &&
				 nfx::string::endsWith( res.name, ".gz" ) )
			{
				auto decompressed = decompressedStream( res.name );
				std::string_view jsonStr{ reinterpret_cast<const char*>( decompressed.data() ), decompressed.size() };

				auto gmodVersioning = nfx::serialization::json::Serializer<GmodVersioningDto>::fromString( jsonStr );

				dtos[gmodVersioning.visVersion] = std::move( gmodVersioning );
			}
		}

		if ( dtos.empty() )
		{
			return std::nullopt;
		}

		return dtos;
	}

	std::optional<DataChannelTypeNamesDto> EmbeddedResource::dataChannelTypeNames( std::string_view version )
	{
		return loadResource<DataChannelTypeNamesDto>( "data-channel-type-names", version, &decompressedStream );
	}

	std::optional<FormatDataTypesDto> EmbeddedResource::formatDataTypes( std::string_view version )
	{
		return loadResource<FormatDataTypesDto>( "format-data-types", version, &decompressedStream );
	}
} // namespace dnv::vista::sdk
