/**
 * @file EmbeddedResource.cpp
 * @brief Implementation of resource loading and caching utilities for Vista SDK
 */

#include "pch.h"

#include "dnv/vista/sdk/EmbeddedResource.h"

#include "dnv/vista/sdk/CodebooksDto.h"
#include "dnv/vista/sdk/GmodDto.h"
#include "dnv/vista/sdk/GmodVersioningDto.h"
#include "dnv/vista/sdk/ISO19848Dtos.h"
#include "dnv/vista/sdk/LocationsDto.h"

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// Constants
		//=====================================================================

		static inline constexpr std::string_view VIS_RELEASE_KEY = "visRelease";

		static constexpr size_t CHUNK_IN_SIZE = 65536;
		static constexpr size_t CHUNK_OUT_SIZE = 131072;
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
						SPDLOG_WARN( "GMOD resource {} missing or has invalid '{}' field.", resourceName, VIS_RELEASE_KEY );
					}
				}
				catch ( [[maybe_unused]] const nlohmann::json::parse_error& ex )
				{
					SPDLOG_ERROR( "JSON parse error in resource {}: {}", resourceName, ex.what() );
				}
				catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
				{
					SPDLOG_ERROR( "JSON error processing resource {}: {}", resourceName, ex.what() );
				}
				catch ( [[maybe_unused]] const std::exception& ex )
				{
					SPDLOG_ERROR( "Error reading/decompressing resource {} for version extraction: {}", resourceName, ex.what() );
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
				SPDLOG_WARN( "No VIS versions found in embedded GMOD resources." );
			}
		}

		return visVersions;
	}

	const std::optional<StringMap<GmodVersioningDto>>& EmbeddedResource::gmodVersioning()
	{
		static std::mutex gmodVersioningCacheMutex;
		static std::optional<StringMap<GmodVersioningDto>> gmodVersioningCache;
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

		StringMap<GmodVersioningDto> resultMap;
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
					SPDLOG_WARN( "GMOD Versioning resource {} missing or has invalid '{}' field.", resourceName, VIS_RELEASE_KEY );
				}
			}
			catch ( [[maybe_unused]] const nlohmann::json::parse_error& ex )
			{
				SPDLOG_ERROR( "JSON parse error in GMOD Versioning resource {}: {}", resourceName, ex.what() );
			}
			catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
			{
				SPDLOG_ERROR( "JSON validation/deserialization error in GMOD Versioning resource {}: {}", resourceName, ex.what() );
			}
			catch ( [[maybe_unused]] const std::exception& ex )
			{
				SPDLOG_ERROR( "Error processing GMOD Versioning resource {}: {}", resourceName, ex.what() );
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

		static StringMap<std::optional<GmodDto>> gmodCache;

		{
			std::lock_guard<std::mutex> lock( gmodCacheMutex );
			auto cacheIt = gmodCache.find( visVersion );
			if ( cacheIt != gmodCache.end() )
			{
				return cacheIt->second;
			}
		}

		auto names = resourceNames();

		auto it = std::find_if( names.begin(), names.end(),
			[&visVersion]( const std::string& name ) {
				return dnv::vista::sdk::contains( name, "gmod" ) &&
					   dnv::vista::sdk::contains( name, visVersion ) &&
					   dnv::vista::sdk::endsWith( name, ".json.gz" ) &&
					   !dnv::vista::sdk::contains( name, "versioning" );
			} );

		if ( it == names.end() )
		{
			SPDLOG_ERROR( "GMOD resource not found for version: {}", visVersion );
			std::lock_guard<std::mutex> lock( gmodCacheMutex );
			gmodCache.emplace( std::string{ visVersion }, std::nullopt );

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
			SPDLOG_ERROR( "JSON parse error in GMOD resource {}: {}", *it, ex.what() );
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			SPDLOG_ERROR( "JSON validation/deserialization error in GMOD resource {}: {}", *it, ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			SPDLOG_ERROR( "Error processing GMOD resource {}: {}", *it, ex.what() );
		}

		std::lock_guard<std::mutex> lock( gmodCacheMutex );
		auto [emplaceIt, inserted] = gmodCache.emplace( std::string{ visVersion }, std::move( resultForCache ) );

		return emplaceIt->second;
	}

	std::optional<CodebooksDto> EmbeddedResource::codebooks( std::string_view visVersion )
	{
		static std::mutex codebooksCacheMutex;

		static StringMap<std::optional<CodebooksDto>> codebooksCache;

		{
			std::lock_guard<std::mutex> lock( codebooksCacheMutex );
			auto cacheIt = codebooksCache.find( visVersion );
			if ( cacheIt != codebooksCache.end() )
			{
				return cacheIt->second;
			}
		}

		auto names = resourceNames();

		auto it = std::find_if( names.begin(), names.end(),
			[&visVersion]( const std::string& name ) {
				return dnv::vista::sdk::contains( name, "codebooks" ) &&
					   dnv::vista::sdk::contains( name, visVersion ) &&
					   dnv::vista::sdk::endsWith( name, ".json.gz" );
			} );

		if ( it == names.end() )
		{
			SPDLOG_ERROR( "Codebooks resource not found for version: {}", visVersion );
			std::lock_guard<std::mutex> lock( codebooksCacheMutex );
			codebooksCache.emplace( std::string{ visVersion }, std::nullopt );

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
			SPDLOG_ERROR( "JSON parse error in Codebooks resource {}: {}", *it, ex.what() );
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			SPDLOG_ERROR( "JSON validation/deserialization error in Codebooks resource {}: {}", *it, ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			SPDLOG_ERROR( "Error processing Codebooks resource {}: {}", *it, ex.what() );
		}

		std::lock_guard<std::mutex> lock( codebooksCacheMutex );
		auto [emplaceIt, inserted] = codebooksCache.emplace( std::string{ visVersion }, std::move( resultForCache ) );

		return emplaceIt->second;
	}

	std::optional<LocationsDto> EmbeddedResource::locations( std::string_view visVersion )
	{
		static std::mutex locationsCacheMutex;

		static StringMap<std::optional<LocationsDto>> locationsCache;

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
			[&visVersion]( const std::string& name ) {
				return isLocationsResource( name ) &&
					   containsVersion( name, visVersion );
			} );

		if ( it == names.end() )
		{
			SPDLOG_ERROR( "Locations resource not found for version: {}", visVersion );
			std::lock_guard<std::mutex> lock( locationsCacheMutex );
			locationsCache.emplace( std::string{ visVersion }, std::nullopt );

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
			SPDLOG_ERROR( "JSON parse error in Locations resource {}: {}", *it, ex.what() );
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			SPDLOG_ERROR( "JSON validation/deserialization error in Locations resource {}: {}", *it, ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			SPDLOG_ERROR( "Error processing Locations resource {}: {}", *it, ex.what() );
		}

		std::lock_guard<std::mutex> lock( locationsCacheMutex );
		auto [emplaceIt, inserted] = locationsCache.emplace( std::string{ visVersion }, std::move( resultForCache ) );

		return emplaceIt->second;
	}

	std::optional<DataChannelTypeNamesDto> EmbeddedResource::dataChannelTypeNames( std::string_view version )
	{
		static std::mutex dataChannelTypeNamesCacheMutex;

		static StringMap<std::optional<DataChannelTypeNamesDto>> dataChannelTypeNamesCache;

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
			[&version]( const std::string& name ) {
				return isDataChannelTypeNamesResource( name ) &&
					   containsVersion( name, version );
			} );

		if ( it == names.end() )
		{
			SPDLOG_ERROR( "DataChannelTypeNames resource not found for version: {}", version );
			std::lock_guard<std::mutex> lock( dataChannelTypeNamesCacheMutex );
			dataChannelTypeNamesCache.emplace( std::string{ version }, std::nullopt );

			return std::nullopt;
		}

		std::optional<DataChannelTypeNamesDto> resultForCache = std::nullopt;
		try
		{
			auto stream = decompressedStream( *it );
			nlohmann::json dtNamesJson = nlohmann::json::parse( *stream );

			DataChannelTypeNamesDto loadedDto = DataChannelTypeNamesDto::fromJson( dtNamesJson );

			resultForCache.emplace( std::move( loadedDto ) );
		}
		catch ( [[maybe_unused]] const nlohmann::json::parse_error& ex )
		{
			SPDLOG_ERROR( "JSON parse error in DataChannelTypeNames resource {}: {}", *it, ex.what() );
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			SPDLOG_ERROR( "JSON validation/deserialization error in DataChannelTypeNames resource {}: {}", *it, ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			SPDLOG_ERROR( "Error processing DataChannelTypeNames resource {}: {}", *it, ex.what() );
		}

		std::lock_guard<std::mutex> lock( dataChannelTypeNamesCacheMutex );
		auto [emplaceIt, inserted] = dataChannelTypeNamesCache.emplace( std::string{ version }, std::move( resultForCache ) );

		return emplaceIt->second;
	}

	std::optional<FormatDataTypesDto> EmbeddedResource::formatDataTypes( std::string_view version )
	{
		static std::mutex fdTypesCacheMutex;

		static StringMap<std::optional<FormatDataTypesDto>> fdTypesCache;

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
			[&version]( const std::string& name ) {
				return isFormatDataTypesResource( name ) &&
					   containsVersion( name, version );
			} );

		if ( it == names.end() )
		{
			SPDLOG_ERROR( "FormatDataTypes resource not found for version: {}", version );
			std::lock_guard<std::mutex> lock( fdTypesCacheMutex );
			fdTypesCache.emplace( std::string{ version }, std::nullopt );

			return std::nullopt;
		}

		std::optional<FormatDataTypesDto> resultForCache = std::nullopt;
		try
		{
			auto stream = decompressedStream( *it );
			nlohmann::json fdTypesJson = nlohmann::json::parse( *stream );

			FormatDataTypesDto loadedDto = FormatDataTypesDto::fromJson( fdTypesJson );

			resultForCache.emplace( std::move( loadedDto ) );
		}
		catch ( [[maybe_unused]] const nlohmann::json::parse_error& ex )
		{
			SPDLOG_ERROR( "JSON parse error in FormatDataTypes resource {}: {}", *it, ex.what() );
		}
		catch ( [[maybe_unused]] const nlohmann::json::exception& ex )
		{
			SPDLOG_ERROR( "JSON validation/deserialization error in FormatDataTypes resource {}: {}", *it, ex.what() );
		}
		catch ( [[maybe_unused]] const std::exception& ex )
		{
			SPDLOG_ERROR( "Error processing FormatDataTypes resource {}: {}", *it, ex.what() );
		}

		std::lock_guard<std::mutex> lock( fdTypesCacheMutex );
		auto [emplaceIt, inserted] = fdTypesCache.emplace( std::string{ version }, std::move( resultForCache ) );

		return emplaceIt->second;
	}

	//----------------------------------------------
	// Resource access
	//----------------------------------------------

	std::vector<std::string> EmbeddedResource::resourceNames()
	{
		static std::mutex cacheMutex;
		static std::vector<std::string> cachedResourceNames;
		static std::optional<std::filesystem::path> successfulDir;
		static bool initialized = false;

		{
			std::lock_guard<std::mutex> lock( cacheMutex );
			if ( initialized )
			{
				return cachedResourceNames;
			}
		}

		std::vector<std::filesystem::path> possibleDirs;

		if ( successfulDir )
		{
			possibleDirs.push_back( *successfulDir );
		}

		/* TODO: resources will always be at the same location in the future */
		possibleDirs.push_back( std::filesystem::current_path() / "resources" );
		possibleDirs.push_back( std::filesystem::current_path() / "../resources" );
		possibleDirs.push_back( std::filesystem::current_path() / "../../resources" );
		possibleDirs.push_back( std::filesystem::current_path() );

		std::vector<std::string> foundNames;
		foundNames.reserve( 50 );

		for ( const auto& dir : possibleDirs )
		{
			try
			{
				if ( !std::filesystem::exists( dir ) || !std::filesystem::is_directory( dir ) )
				{
					continue;
				}

				for ( const auto& entry : std::filesystem::directory_iterator( dir ) )
				{
					if ( entry.is_regular_file() )
					{
						if ( dnv::vista::sdk::endsWith( entry.path().filename().string(), ".json.gz" ) )
						{
							foundNames.emplace_back( entry.path().filename().string() );
						}
					}
				}

				if ( !foundNames.empty() )
				{
					successfulDir = dir;
					break;
				}
			}
			catch ( [[maybe_unused]] const std::filesystem::filesystem_error& ex )
			{
				SPDLOG_ERROR( "Filesystem error scanning directory {}: {}", dir.string(), ex.what() );
			}
			catch ( [[maybe_unused]] const std::exception& ex )
			{
				SPDLOG_ERROR( "Error scanning directory {}: {}", dir.string(), ex.what() );
			}
		}

		if ( foundNames.empty() )
		{
			SPDLOG_WARN( "No embedded resource files (.json.gz) found in search paths." );
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
			throw std::runtime_error( fmt::format( "Failed to open compressed stream for resource: {}", resourceName ) );
		}

		compressedStream->seekg( 0, std::ios::end );
		auto pos = compressedStream->tellg();
		if ( pos < 0 )
		{
			throw std::runtime_error( fmt::format( "Invalid stream position for resource: {}", resourceName ) );
		}
		size_t compressedSize = static_cast<size_t>( pos );
		compressedStream->seekg( 0, std::ios::beg );

		size_t estimatedDecompressedSize = std::min(
			compressedSize * 8,
			static_cast<size_t>( 128 * 1024 * 1024 ) );

		std::string decompressedData;
		decompressedData.reserve( estimatedDecompressedSize );

		::z_stream zs = {};
		zs.zalloc = Z_NULL;
		zs.zfree = Z_NULL;
		zs.opaque = Z_NULL;

		if ( ::inflateInit2( &zs, 15 + 16 ) != Z_OK )
		{
			throw std::runtime_error( fmt::format( "Failed to initialize zlib for resource: {}", resourceName ) );
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
				::inflateEnd( &zs );

				throw std::runtime_error( fmt::format( "Error reading compressed stream for resource: {}", resourceName ) );
			}

			zs.next_in = reinterpret_cast<Bytef*>( inBuffer.data() );

			do
			{
				zs.avail_out = CHUNK_OUT_SIZE;
				zs.next_out = reinterpret_cast<Bytef*>( outBuffer.data() );

				ret = ::inflate( &zs, Z_NO_FLUSH );

				if ( ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR )
				{
					std::string errorMsg = ( zs.msg ) ? zs.msg : "Unknown zlib error";
					::inflateEnd( &zs );

					throw std::runtime_error( fmt::format( "Zlib decompression failed for resource '{}' with error code {}: {}",
						resourceName, ret, errorMsg ) );
				}

				size_t have = CHUNK_OUT_SIZE - zs.avail_out;
				decompressedData.append( outBuffer.data(), have );
			} while ( zs.avail_out == 0 );
		} while ( ret != Z_STREAM_END );

		::inflateEnd( &zs );

		auto decompressedBuffer = std::make_shared<std::istringstream>( std::move( decompressedData ) );
		return decompressedBuffer;
	}

	std::shared_ptr<std::istream> EmbeddedResource::stream( std::string_view resourceName )
	{
		static std::mutex pathCacheMutex;
		static PathCache resourcePathCache;
		static std::optional<std::filesystem::path> lastSuccessfulBaseDir;
		static size_t cacheHits = 0;
		static size_t cacheMisses = 0;

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
						cacheHits++;
						SPDLOG_DEBUG( "Resource path cache hit: '{}' -> '{}'", resourceName, it->second.string() );
						if ( ( cacheHits + cacheMisses ) % 50 == 0 && ( cacheHits + cacheMisses ) > 0 )
						{
							SPDLOG_DEBUG( "Path Cache effectiveness: {:.1f}% hit rate ({} hits, {} misses)",
								( static_cast<double>( cacheHits ) * 100.0 ) / static_cast<double>( cacheHits + cacheMisses ),
								cacheHits, cacheMisses );
						}
						return fileStream;
					}
				}
				SPDLOG_WARN( "Cached resource path '{}' for '{}' is invalid, removing from cache.",
					it->second.string(), resourceName );
				resourcePathCache.erase( it );
			}
			cacheMisses++;
		}

		std::vector<std::filesystem::path> possiblePaths;
		possiblePaths.reserve( 5 );

		if ( lastSuccessfulBaseDir )
		{
			possiblePaths.push_back( *lastSuccessfulBaseDir / resourceName );
		}

		/* TODO: resources will always be at the same location in the future */
		possiblePaths.push_back( std::filesystem::current_path() / "resources" / resourceName );
		possiblePaths.push_back( std::filesystem::current_path() / "../resources" / resourceName );
		possiblePaths.push_back( std::filesystem::current_path() / "../../resources" / resourceName );

		if ( !lastSuccessfulBaseDir || *lastSuccessfulBaseDir != std::filesystem::current_path() )
		{
			possiblePaths.push_back( std::filesystem::current_path() / resourceName );
		}

		std::ostringstream attemptedPaths;
		for ( const auto& path : possiblePaths )
		{
			if ( attemptedPaths.tellp() > 0 )
				attemptedPaths << ", ";
			attemptedPaths << "'" << path.string() << "'";
		}
		std::string attemptedPathsStr = attemptedPaths.str();

		for ( const auto& path : possiblePaths )
		{
			try
			{
				if ( std::filesystem::exists( path ) && std::filesystem::is_regular_file( path ) )
				{
					auto fileStream = std::make_shared<std::ifstream>( path, std::ios::binary );
					if ( fileStream->is_open() )
					{
						{
							std::lock_guard<std::mutex> lock( pathCacheMutex );

							resourcePathCache.emplace( std::string{ resourceName }, path );

							if ( path.has_parent_path() )
							{
								lastSuccessfulBaseDir = path.parent_path();
							}
						}
						return fileStream;
					}
					else
					{
						SPDLOG_WARN( "Found file '{}' but failed to open stream.", path.string() );
					}
				}
			}
			catch ( [[maybe_unused]] const std::filesystem::filesystem_error& ex )
			{
				SPDLOG_ERROR( "Filesystem error accessing path '{}': {}", path.string(), ex.what() );
			}
			catch ( [[maybe_unused]] const std::exception& ex )
			{
				SPDLOG_ERROR( "Error checking/opening path '{}': {}", path.string(), ex.what() );
			}
		}

		throw std::runtime_error( fmt::format( "Failed to find or open resource file: {}. Attempted paths: [{}]",
			resourceName, attemptedPathsStr ) );
	}
}
