/**
 * @file EmbeddedResource.cpp
 * @brief Implementation of resource loading and caching utilities for Vista SDK
 */

#include <cstdio>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>

#include <nfx/string/StringBuilderPool.h>
#include <zlib-ng.h>

#include "dnv/vista/sdk/EmbeddedResource.h"
#include "dnv/vista/sdk/transport/ISO19848Dtos.h"
#include "dnv/vista/sdk/CodebooksDto.h"
#include "dnv/vista/sdk/GmodDto.h"
#include "dnv/vista/sdk/GmodVersioningDto.h"
#include "dnv/vista/sdk/LocationsDto.h"

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// Constants
		//=====================================================================

		static constexpr std::string_view VIS_RELEASE_KEY = "visRelease";
		static constexpr std::string_view RESOURCES_DIR = "resources";

		// Compression buffer sizes for zlib streaming decompression
		static constexpr size_t CHUNK_IN_SIZE = 65536;	 // 64KB input buffer for reading compressed data
		static constexpr size_t CHUNK_OUT_SIZE = 131072; // 128KB output buffer for decompressed data

		// zlib window bits configuration for gzip format
		// 15 = maximum window size (32KB), +16 = gzip format detection
		static constexpr int GZIP_MAX_WINDOW_BITS = 15 + 16;
	}

	//=====================================================================
	// EmbeddedResource class
	//=====================================================================

	//----------------------------------------------
	// Public interface
	//----------------------------------------------

	std::vector<std::string> EmbeddedResource::visVersions()
	{
		auto names = resourceNames();
		std::vector<std::string> visVersions;
		visVersions.reserve( names.size() );

		for ( const auto& resourceName : names )
		{
			if ( isGmodResource( resourceName ) )
			{
				try
				{
					const auto stream = decompressedStream( resourceName );
					const nlohmann::json gmodJson = nlohmann::json::parse( *stream );

					if ( gmodJson.contains( VIS_RELEASE_KEY ) && gmodJson.at( VIS_RELEASE_KEY ).is_string() )
					{
						std::string version = gmodJson.at( VIS_RELEASE_KEY ).get<std::string>();
						visVersions.push_back( version );
					}
					else
					{
						std::fprintf(
							stderr,
							"WARN: GMOD resource %s missing or has invalid '%.*s' field.\n",
							resourceName.data(),
							static_cast<int>( VIS_RELEASE_KEY.size() ), VIS_RELEASE_KEY.data() );
					}
				}
				catch ( [[maybe_unused]] const nlohmann::json::parse_error& ex )
				{
					std::fprintf(
						stderr,
						"ERROR: JSON parse error in resource %s: %s\n",
						resourceName.data(),
						ex.what() );
				}
				catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
				{
					std::fprintf(
						stderr,
						"ERROR: JSON error processing resource %s: %s\n",
						resourceName.data(),
						ex.what() );
				}
				catch ( [[maybe_unused]] const std::exception& ex )
				{
					std::fprintf(
						stderr,
						"ERROR: Error reading/decompressing resource %s for version extraction: %s\n",
						resourceName.data(),
						ex.what() );
				}
			}
		}

		std::sort( visVersions.begin(), visVersions.end() );
		visVersions.erase( std::unique( visVersions.begin(), visVersions.end() ), visVersions.end() );

		{
			std::ostringstream versionsList;
			if ( !visVersions.empty() )
			{
				std::copy( visVersions.begin(), visVersions.end() - 1, std::ostream_iterator<std::string>( versionsList, ", " ) );
				versionsList << visVersions.back();
			}
			else
			{
				std::fprintf( stderr, "WARN: No VIS versions found in embedded GMOD resources.\n" );
			}
		}

		return visVersions;
	}

	const std::optional<nfx::containers::StringMap<GmodVersioningDto>>& EmbeddedResource::gmodVersioning()
	{
		static std::mutex gmodVersioningCacheMutex;
		static std::optional<nfx::containers::StringMap<GmodVersioningDto>> gmodVersioningCache;
		static bool cacheInitialized = false;

		{
			std::lock_guard<std::mutex> lock( gmodVersioningCacheMutex );
			if ( cacheInitialized )
			{
				return gmodVersioningCache;
			}
		}

		auto names = resourceNames();

		std::vector<std::string> matchingResources;
		matchingResources.reserve( names.size() / 4 );

		for ( const auto& resourceName : names )
		{
			if ( isGmodVersioningResource( resourceName ) )
			{
				matchingResources.push_back( resourceName );
			}
		}

		nfx::containers::StringMap<GmodVersioningDto> resultMap;
		std::mutex resultMutex;
		bool foundAnyResource = false;

		for ( const auto& resourceName : matchingResources )
		{
			try
			{
				const auto stream = decompressedStream( resourceName );
				const nlohmann::json versioningJson = nlohmann::json::parse( *stream );

				if ( versioningJson.contains( VIS_RELEASE_KEY ) && versioningJson.at( VIS_RELEASE_KEY ).is_string() )
				{
					std::string visVersion = versioningJson.at( VIS_RELEASE_KEY ).get<std::string>();

					auto dto = GmodVersioningDto::fromJson( versioningJson );

					{
						std::lock_guard<std::mutex> lock( resultMutex );
						resultMap.emplace( visVersion, std::move( dto ) );
						foundAnyResource = true;
					}
				}
				else
				{
					std::fprintf(
						stderr,
						"WARN: GMOD Versioning resource %s missing or has invalid '%.*s' field.\n",
						resourceName.data(),
						static_cast<int>( VIS_RELEASE_KEY.size() ), VIS_RELEASE_KEY.data() );
				}
			}
			catch ( [[maybe_unused]] const nlohmann::json::parse_error& ex )
			{
				std::fprintf(
					stderr,
					"ERROR: JSON parse error in GMOD Versioning resource %s: %s\n",
					resourceName.data(),
					ex.what() );
			}
			catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
			{
				std::fprintf(
					stderr,
					"ERROR: JSON validation/deserialization error in GMOD Versioning resource %s: %s\n",
					resourceName.data(),
					ex.what() );
			}
			catch ( [[maybe_unused]] const std::exception& ex )
			{
				std::fprintf(
					stderr,
					"ERROR: Error processing GMOD Versioning resource %s: %s\n",
					resourceName.data(),
					ex.what() );
			}
		}

		std::lock_guard<std::mutex> lock( gmodVersioningCacheMutex );
		cacheInitialized = true;
		if ( foundAnyResource )
		{
			gmodVersioningCache.emplace( std::move( resultMap ) );
		}
		else
		{
			gmodVersioningCache = std::nullopt;
		}
		return gmodVersioningCache;
	}

	std::optional<GmodDto> EmbeddedResource::gmod( std::string_view visVersion )
	{
		static std::mutex gmodCacheMutex;

		static nfx::containers::StringMap<std::optional<GmodDto>> gmodCache;

		{
			std::lock_guard<std::mutex> lock( gmodCacheMutex );
			auto cacheIt = gmodCache.find( visVersion );
			if ( cacheIt != gmodCache.end() )
			{
				return cacheIt->second;
			}
		}

		auto names = resourceNames();

		auto it = std::find_if( names.begin(), names.end(), [&visVersion]( const std::string& name ) {
			return nfx::string::contains( name, "gmod" ) && nfx::string::contains( name, visVersion ) &&
				   nfx::string::endsWith( name, ".json.gz" ) &&
				   !nfx::string::contains( name, "versioning" );
		} );

		if ( it == names.end() )
		{
			std::fprintf( stderr, "ERROR: GMOD resource not found for version: %.*s\n",
				static_cast<int>( visVersion.size() ), visVersion.data() );
			std::lock_guard<std::mutex> lock( gmodCacheMutex );
			gmodCache.emplace( visVersion, std::nullopt );

			return std::nullopt;
		}

		std::optional<GmodDto> resultForCache = std::nullopt;

		try
		{
			auto stream = decompressedStream( *it );
			nlohmann::json gmodJson = nlohmann::json::parse( *stream );

			GmodDto loadedDto = GmodDto::fromJson( gmodJson );

			resultForCache.emplace( std::move( loadedDto ) );
		}
		catch ( [[maybe_unused]] const nlohmann::json::parse_error& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON parse error in GMOD resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON validation/deserialization error in GMOD resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: Error processing GMOD resource %s: %s\n",
				it->c_str(), ex.what() );
		}

		std::lock_guard<std::mutex> lock( gmodCacheMutex );
		auto [emplaceIt, inserted] = gmodCache.emplace( visVersion, std::move( resultForCache ) );

		return emplaceIt->second;
	}

	std::optional<CodebooksDto> EmbeddedResource::codebooks( std::string_view visVersion )
	{
		static std::mutex codebooksCacheMutex;

		static nfx::containers::StringMap<std::optional<CodebooksDto>> codebooksCache;

		{
			std::lock_guard<std::mutex> lock( codebooksCacheMutex );
			auto cacheIt = codebooksCache.find( visVersion );
			if ( cacheIt != codebooksCache.end() )
			{
				return cacheIt->second;
			}
		}

		auto names = resourceNames();

		auto it = std::find_if( names.begin(), names.end(), [&visVersion]( const std::string& name ) {
			return nfx::string::contains( name, "codebooks" ) &&
				   nfx::string::contains( name, visVersion ) &&
				   nfx::string::endsWith( name, ".json.gz" );
		} );

		if ( it == names.end() )
		{
			std::fprintf( stderr, "ERROR: Codebooks resource not found for version: %.*s\n",
				static_cast<int>( visVersion.size() ), visVersion.data() );
			std::lock_guard<std::mutex> lock( codebooksCacheMutex );
			codebooksCache.emplace( visVersion, std::nullopt );

			return std::nullopt;
		}

		std::optional<CodebooksDto> resultForCache = std::nullopt;
		try
		{
			auto stream = decompressedStream( *it );
			nlohmann::json codebooksJson = nlohmann::json::parse( *stream );

			CodebooksDto loadedDto = CodebooksDto::fromJson( codebooksJson );

			resultForCache.emplace( std::move( loadedDto ) );
		}
		catch ( [[maybe_unused]] const nlohmann::json::parse_error& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON parse error in Codebooks resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON validation/deserialization error in Codebooks resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: Error processing Codebooks resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}

		std::lock_guard<std::mutex> lock( codebooksCacheMutex );
		auto [emplaceIt, inserted] = codebooksCache.emplace( visVersion, std::move( resultForCache ) );

		return emplaceIt->second;
	}

	std::optional<LocationsDto> EmbeddedResource::locations( std::string_view visVersion )
	{
		static std::mutex locationsCacheMutex;

		static nfx::containers::StringMap<std::optional<LocationsDto>> locationsCache;

		{
			std::lock_guard<std::mutex> lock( locationsCacheMutex );
			auto cacheIt = locationsCache.find( visVersion );
			if ( cacheIt != locationsCache.end() )
			{
				return cacheIt->second;
			}
		}

		auto names = resourceNames();

		auto it = std::find_if( names.begin(), names.end(),
			[&visVersion]( const std::string& name ) { return isLocationsResource( name ) &&
															  containsVersion( name, visVersion ); } );

		if ( it == names.end() )
		{
			std::fprintf( stderr, "ERROR: Locations resource not found for version: %.*s\n",
				static_cast<int>( visVersion.size() ), visVersion.data() );
			std::lock_guard<std::mutex> lock( locationsCacheMutex );
			locationsCache.emplace( visVersion, std::nullopt );

			return std::nullopt;
		}

		std::optional<LocationsDto> resultForCache = std::nullopt;
		try
		{
			auto stream = decompressedStream( *it );
			nlohmann::json locationsJson = nlohmann::json::parse( *stream );

			LocationsDto loadedDto = LocationsDto::fromJson( locationsJson );

			resultForCache.emplace( std::move( loadedDto ) );
		}
		catch ( [[maybe_unused]] const nlohmann::json::parse_error& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON parse error in Locations resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON validation/deserialization error in Locations resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: Error processing Locations resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}

		std::lock_guard<std::mutex> lock( locationsCacheMutex );
		auto [emplaceIt, inserted] = locationsCache.emplace( visVersion, std::move( resultForCache ) );

		return emplaceIt->second;
	}

	std::optional<transport::DataChannelTypeNamesDto> EmbeddedResource::dataChannelTypeNames( std::string_view version )
	{
		static std::mutex dataChannelTypeNamesCacheMutex;

		static nfx::containers::StringMap<std::optional<transport::DataChannelTypeNamesDto>> dataChannelTypeNamesCache;

		{
			std::lock_guard<std::mutex> lock( dataChannelTypeNamesCacheMutex );
			auto cacheIt = dataChannelTypeNamesCache.find( version );
			if ( cacheIt != dataChannelTypeNamesCache.end() )
			{
				return cacheIt->second;
			}
		}

		auto names = resourceNames();

		auto it = std::find_if( names.begin(), names.end(),
			[&version]( const std::string& name ) { return isDataChannelTypeNamesResource( name ) &&
														   containsVersion( name, version ); } );

		if ( it == names.end() )
		{
			std::fprintf( stderr, "ERROR: DataChannelTypeNames resource not found for version: %.*s\n",
				static_cast<int>( version.size() ), version.data() );

			std::lock_guard<std::mutex> lock( dataChannelTypeNamesCacheMutex );
			dataChannelTypeNamesCache.emplace( version, std::nullopt );

			return std::nullopt;
		}

		std::optional<transport::DataChannelTypeNamesDto> resultForCache = std::nullopt;
		try
		{
			auto stream = decompressedStream( *it );
			nlohmann::json dtNamesJson = nlohmann::json::parse( *stream );

			auto loadedDto = transport::DataChannelTypeNamesDto::fromJson( dtNamesJson );

			resultForCache.emplace( std::move( loadedDto ) );
		}
		catch ( [[maybe_unused]] const nlohmann::json::parse_error& ex )
		{
			std::fprintf(
				stderr,
				"ERROR : JSON parse error in DataChannelTypeNames resource %s : %s\n ",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON validation/deserialization error in DataChannelTypeNames resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: Error processing DataChannelTypeNames resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}

		std::lock_guard<std::mutex> lock( dataChannelTypeNamesCacheMutex );
		auto [emplaceIt, inserted] = dataChannelTypeNamesCache.emplace( version, std::move( resultForCache ) );

		return emplaceIt->second;
	}

	std::optional<transport::FormatDataTypesDto> EmbeddedResource::formatDataTypes( std::string_view version )
	{
		static std::mutex fdTypesCacheMutex;

		static nfx::containers::StringMap<std::optional<transport::FormatDataTypesDto>> fdTypesCache;

		{
			std::lock_guard<std::mutex> lock( fdTypesCacheMutex );
			auto cacheIt = fdTypesCache.find( version );
			if ( cacheIt != fdTypesCache.end() )
			{
				return cacheIt->second;
			}
		}

		auto names = resourceNames();

		auto it = std::find_if( names.begin(), names.end(),
			[&version]( const std::string& name ) { return isFormatDataTypesResource( name ) &&
														   containsVersion( name, version ); } );

		if ( it == names.end() )
		{
			std::fprintf( stderr, "ERROR: FormatDataTypes resource not found for version: %.*s\n",
				static_cast<int>( version.size() ), version.data() );
			std::lock_guard<std::mutex> lock( fdTypesCacheMutex );
			fdTypesCache.emplace( version, std::nullopt );

			return std::nullopt;
		}

		std::optional<transport::FormatDataTypesDto> resultForCache = std::nullopt;
		try
		{
			auto stream = decompressedStream( *it );
			nlohmann::json fdTypesJson = nlohmann::json::parse( *stream );

			auto loadedDto = transport::FormatDataTypesDto::fromJson( fdTypesJson );

			resultForCache.emplace( std::move( loadedDto ) );
		}
		catch ( [[maybe_unused]] const nlohmann::json::parse_error& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON parse error in FormatDataTypes resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: JSON validation/deserialization error in FormatDataTypes resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			std::fprintf(
				stderr,
				"ERROR: Error processing FormatDataTypes resource %s: %s\n",
				it->c_str(),
				ex.what() );
		}

		std::lock_guard<std::mutex> lock( fdTypesCacheMutex );
		auto [emplaceIt, inserted] = fdTypesCache.emplace( version, std::move( resultForCache ) );

		return emplaceIt->second;
	}

	//----------------------------------------------
	// Resource access
	//----------------------------------------------

	std::vector<std::string> EmbeddedResource::resourceNames()
	{
		static std::mutex cacheMutex;
		static std::vector<std::string> cachedResourceNames;
		static bool initialized = false;

		{
			std::lock_guard<std::mutex> lock( cacheMutex );
			if ( initialized )
			{
				return cachedResourceNames;
			}
		}

		const auto resourcesDir = std::filesystem::current_path() / RESOURCES_DIR;

		std::vector<std::string> foundNames;
		foundNames.reserve( 50 );

		try
		{
			if ( !std::filesystem::exists( resourcesDir ) || !std::filesystem::is_directory( resourcesDir ) )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Resources directory not found: " );
				builder.append( resourcesDir.string() );
				throw std::runtime_error{ lease.toString() };
			}

			for ( const auto& entry : std::filesystem::directory_iterator( resourcesDir ) )
			{
				if ( entry.is_regular_file() && nfx::string::endsWith( entry.path().filename().string(), ".json.gz" ) )
				{
					foundNames.emplace_back( entry.path().filename().string() );
				}
			}

			if ( foundNames.empty() )
			{
				std::fprintf(
					stderr,
					"WARN: No embedded resource files (.json.gz) found in resources directory: %s\n",
					resourcesDir.string().c_str() );
			}
		}
		catch ( [[maybe_unused]] const std::filesystem::filesystem_error& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Filesystem error scanning resources directory " );
			builder.append( resourcesDir.string() );
			builder.append( ": " );
			builder.append( ex.what() );
			throw std::runtime_error{ lease.toString() };
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Error scanning resources directory " );
			builder.append( resourcesDir.string() );
			builder.append( ": " );
			builder.append( ex.what() );
			throw std::runtime_error{ lease.toString() };
		}

		{
			std::lock_guard<std::mutex> lock( cacheMutex );
			cachedResourceNames = std::move( foundNames );
			initialized = true;

			return cachedResourceNames;
		}
	}

	std::shared_ptr<std::istream> EmbeddedResource::decompressedStream( std::string_view resourceName )
	{
		auto compressedStream = stream( resourceName );

		if ( !compressedStream || !compressedStream->good() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Failed to open compressed stream for resource: " );
			builder.append( resourceName );
			throw std::runtime_error{ lease.toString() };
		}

		compressedStream->seekg( 0, std::ios::end );
		auto pos = compressedStream->tellg();
		if ( pos < 0 )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Invalid stream position for resource: " );
			builder.append( resourceName );
			throw std::runtime_error{ lease.toString() };
		}
		size_t compressedSize = static_cast<size_t>( pos );
		compressedStream->seekg( 0, std::ios::beg );

		size_t estimatedDecompressedSize = std::min( compressedSize * 8, static_cast<size_t>( 128 * 1024 * 1024 ) );

		std::string decompressedData;
		decompressedData.reserve( estimatedDecompressedSize );

		zng_stream zs = {};
		zs.zalloc = nullptr;
		zs.zfree = nullptr;
		zs.opaque = nullptr;

		if ( ::zng_inflateInit2( &zs, GZIP_MAX_WINDOW_BITS ) != 0 )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Failed to initialize zlib for resource: " );
			builder.append( resourceName );
			throw std::runtime_error{ lease.toString() };
		}

		std::vector<char> inBuffer( CHUNK_IN_SIZE );
		std::vector<char> outBuffer( CHUNK_OUT_SIZE );
		int ret = Z_OK;

		do
		{
			compressedStream->read( inBuffer.data(), CHUNK_IN_SIZE );
			zs.avail_in = static_cast<uInt>( compressedStream->gcount() );

			if ( zs.avail_in == 0 && !compressedStream->eof() )
			{
				::zng_inflateEnd( &zs );

				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Error reading compressed stream for resource: " );
				builder.append( resourceName );
				throw std::runtime_error{ lease.toString() };
			}

			zs.next_in = reinterpret_cast<Bytef*>( inBuffer.data() );

			do
			{
				zs.avail_out = CHUNK_OUT_SIZE;
				zs.next_out = reinterpret_cast<Bytef*>( outBuffer.data() );

				ret = ::zng_inflate( &zs, Z_NO_FLUSH );

				if ( ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR )
				{
					auto errorMsg = ( zs.msg ) ? zs.msg : "Unknown zlib error";
					::zng_inflateEnd( &zs );

					// Use fprintf for the numeric error code to avoid append issues
					char errorBuffer[512];
					std::snprintf( errorBuffer, sizeof( errorBuffer ),
						"Zlib decompression failed for resource '%.*s' with error code %d: %s",
						static_cast<int>( resourceName.size() ), resourceName.data(),
						ret, errorMsg );
					throw std::runtime_error( errorBuffer );
				}

				size_t have = CHUNK_OUT_SIZE - zs.avail_out;
				decompressedData.append( outBuffer.data(), have );
			} while ( zs.avail_out == 0 );
		} while ( ret != Z_STREAM_END );

		::zng_inflateEnd( &zs );

		auto decompressedBuffer = std::make_shared<std::istringstream>( std::move( decompressedData ) );
		return decompressedBuffer;
	}

	std::shared_ptr<std::istream> EmbeddedResource::stream( std::string_view resourceName )
	{
		static std::mutex pathCacheMutex;
		static PathCache resourcePathCache;

		{
			std::lock_guard<std::mutex> lock( pathCacheMutex );

			auto it = resourcePathCache.find( resourceName );

			if ( it != resourcePathCache.end() )
			{
				if ( std::filesystem::exists( it->second ) && std::filesystem::is_regular_file( it->second ) )
				{
					auto fileStream = std::make_shared<std::ifstream>( it->second, std::ios::binary );
					if ( fileStream->is_open() )
					{
						return fileStream;
					}
				}
				std::fprintf(
					stderr,
					"WARN: Cached resource path '%s' for '%.*s' is invalid, removing from cache.\n",
					it->second.string().c_str(),
					static_cast<int>( resourceName.size() ), resourceName.data() );

				resourcePathCache.erase( it );
			}
		}

		const auto resourcePath = std::filesystem::current_path() / RESOURCES_DIR / resourceName;

		try
		{
			if ( std::filesystem::exists( resourcePath ) && std::filesystem::is_regular_file( resourcePath ) )
			{
				auto fileStream = std::make_shared<std::ifstream>( resourcePath, std::ios::binary );
				if ( fileStream->is_open() )
				{
					{
						std::lock_guard<std::mutex> lock( pathCacheMutex );
						resourcePathCache.emplace( resourceName, resourcePath );
					}

					return fileStream;
				}
				else
				{
					auto lease = nfx::string::StringBuilderPool::lease();
					auto builder = lease.builder();
					builder.append( "Found resource '" );
					builder.append( resourcePath.string() );
					builder.append( "' but failed to open stream" );
					throw std::runtime_error( lease.toString() );
				}
			}
		}
		catch ( [[maybe_unused]] const std::filesystem::filesystem_error& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Filesystem error accessing resource '" );
			builder.append( resourcePath.string() );
			builder.append( "': " );
			builder.append( ex.what() );
			throw std::runtime_error( lease.toString() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Error accessing resource '" );
			builder.append( resourcePath.string() );
			builder.append( "': " );
			builder.append( ex.what() );
			throw std::runtime_error( lease.toString() );
		}

		auto lease = nfx::string::StringBuilderPool::lease();
		auto builder = lease.builder();
		builder.append( "Resource file not found: " );
		builder.append( resourcePath.string() );
		throw std::runtime_error( lease.toString() );
	}
}
