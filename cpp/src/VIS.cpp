/**
 * @file VIS.cpp
 * @brief Implementation of Vessel Information Structure (VIS) interface
 */

#include <nfx/string/StringBuilderPool.h>

#include "dnv/vista/sdk/VIS.h"

#include "dnv/vista/sdk/Codebooks.h"
#include "dnv/vista/sdk/CodebooksDto.h"
#include "dnv/vista/sdk/EmbeddedResource.h"
#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodDto.h"
#include "dnv/vista/sdk/GmodVersioning.h"
#include "dnv/vista/sdk/LocalIdBuilder.h"

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// Helper functions
		//=====================================================================

		[[noreturn]] void throwInvalidVersionError( VisVersion version )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Invalid VIS version provided: " );
			builder.append( VisVersionExtensions::toVersionString( version ) );

			throw std::invalid_argument{ lease.toString() };
		}
	}

	//=====================================================================
	// VIS singleton
	//=====================================================================

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	//-----------------------------
	// Cached objects
	//-----------------------------

	nfx::memory::MemoryCache<VisVersion, GmodDto>& VIS::gmodDtoCache()
	{
		static nfx::memory::MemoryCache<VisVersion, GmodDto> cache{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours( 1 ) } };

		return cache;
	}

	nfx::memory::MemoryCache<VisVersion, CodebooksDto>& VIS::codebooksDtoCache()
	{
		static nfx::memory::MemoryCache<VisVersion, CodebooksDto> cache{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours( 1 ) } };

		return cache;
	}

	nfx::memory::MemoryCache<VisVersion, LocationsDto>& VIS::locationsDtoCache()
	{
		static nfx::memory::MemoryCache<VisVersion, LocationsDto> cache{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours( 1 ) } };

		return cache;
	}

	nfx::memory::MemoryCache<VisVersion, Codebooks>& VIS::codebooksCache()
	{
		static nfx::memory::MemoryCache<VisVersion, Codebooks> cache{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours( 1 ) } };

		return cache;
	}

	nfx::memory::MemoryCache<VisVersion, Gmod>& VIS::gmodsCache()
	{
		static nfx::memory::MemoryCache<VisVersion, Gmod> cache{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours( 1 ) } };

		return cache;
	}

	nfx::memory::MemoryCache<VisVersion, Locations>& VIS::locationsCache()
	{
		static nfx::memory::MemoryCache<VisVersion, Locations> cache{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours( 1 ) } };

		return cache;
	}

	nfx::memory::MemoryCache<VisVersion, GmodVersioning>& VIS::gmodVersioningCache()
	{
		static nfx::memory::MemoryCache<VisVersion, GmodVersioning> cache{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours( 1 ) } };

		return cache;
	}

	const GmodVersioning& VIS::gmodVersioning()
	{
		static constexpr VisVersion cacheKey = VisVersion::LATEST;

		return gmodVersioningCache().getOrCreate( cacheKey, []() {
			auto dto = EmbeddedResource::gmodVersioning();
			if ( !dto )
			{
				throw std::runtime_error{ "Failed to load GMOD versioning data" };
			}
			return GmodVersioning( *dto );
		} );
	}

	//-----------------------------
	// Cached maps
	//-----------------------------

	std::unordered_map<VisVersion, Gmod> VIS::gmodsMap( const std::vector<VisVersion>& visVersions )
	{
		std::unordered_map<VisVersion, Gmod> result;
		result.reserve( visVersions.size() );
		for ( const auto& version : visVersions )
		{
			if ( !VisVersionExtensions::isValid( version ) )
			{
				throwInvalidVersionError( version );
			}

			result.try_emplace( version, gmod( version ) );
		}

		return result;
	}

	std::unordered_map<VisVersion, Codebooks> VIS::codebooksMap( const std::vector<VisVersion>& visVersions )
	{
		std::unordered_map<VisVersion, Codebooks> result;
		result.reserve( visVersions.size() );

		for ( const auto& version : visVersions )
		{
			if ( !VisVersionExtensions::isValid( version ) )
			{
				throwInvalidVersionError( version );
			}

			result.try_emplace( version, codebooks( version ) );
		}

		return result;
	}

	std::unordered_map<VisVersion, Locations> VIS::locationsMap( const std::vector<VisVersion>& visVersions )
	{
		std::unordered_map<VisVersion, Locations> result;
		result.reserve( visVersions.size() );

		for ( const auto& version : visVersions )
		{
			if ( !VisVersionExtensions::isValid( version ) )
			{
				throwInvalidVersionError( version );
			}

			result.try_emplace( version, locations( version ) );
		}

		return result;
	}

	//----------------------------------------------
	// DTO Loading
	//----------------------------------------------

	const nfx::containers::StringMap<GmodVersioningDto>& VIS::gmodVersioningDto()
	{
		static auto versioningDto = []() -> nfx::containers::StringMap<GmodVersioningDto> {
			auto dto = EmbeddedResource::gmodVersioning();
			if ( !dto )
			{
				throw std::runtime_error{ "Failed to load GMOD versioning data" };
			}
			return std::move( *dto );
		}();

		return versioningDto;
	}

	//----------------------------------------------
	// Conversion
	//----------------------------------------------

	//-----------------------------
	// GmodNode
	//-----------------------------

	std::optional<GmodNode> VIS::convertNode( VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion )
	{
		return gmodVersioning().convertNode( sourceVersion, sourceNode, targetVersion );
	}

	std::optional<GmodNode> VIS::convertNode( const GmodNode& sourceNode, VisVersion targetVersion, [[maybe_unused]] const GmodNode* sourceParent )
	{
		return convertNode( sourceNode.visVersion(), sourceNode, targetVersion );
	}

	//-----------------------------
	// GmodPath
	//-----------------------------

	std::optional<GmodPath> VIS::convertPath( VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion )
	{
		return gmodVersioning().convertPath( sourceVersion, sourcePath, targetVersion );
	}

	std::optional<GmodPath> VIS::convertPath( const GmodPath& sourcePath, VisVersion targetVersion )
	{
		return convertPath( sourcePath.visVersion(), sourcePath, targetVersion );
	}

	//-----------------------------
	// LocalId
	//-----------------------------

	std::optional<LocalIdBuilder> VIS::convertLocalId( const LocalIdBuilder& sourceLocalId, VisVersion targetVersion )
	{
		return gmodVersioning().convertLocalId( sourceLocalId, targetVersion );
	}

	std::optional<LocalId> VIS::convertLocalId( const LocalId& sourceLocalId, VisVersion targetVersion )
	{
		return gmodVersioning().convertLocalId( sourceLocalId, targetVersion );
	}
}
