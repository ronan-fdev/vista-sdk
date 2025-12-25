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

#include "dnv/vista/sdk/Codebooks.h"
#include "dnv/vista/sdk/Locations.h"
#include "dto/CodebooksDto.h"
#include "dto/GmodDto.h"
#include "dto/GmodVersioningDto.h"
#include "dto/LocationsDto.h"
#include "versioning/GmodVersioning.h"
#include "VisVersionsExtensions.h"

#include <EmbeddedResource/EmbeddedResource.h>

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace dnv::vista::sdk
{
	namespace internal
	{
		static std::unordered_map<VisVersion, Gmod> gmodCache;
		static std::shared_mutex gmodMutex;

		static std::unordered_map<VisVersion, Codebooks> codebooksCache;
		static std::shared_mutex codebooksMutex;

		static std::unordered_map<VisVersion, Locations> locationsCache;
		static std::shared_mutex locationsMutex;

		static std::unique_ptr<internal::GmodVersioning> gmodVersioningCache;
		static std::mutex gmodVersioningMutex;

		const GmodVersioning& gmodVersioning()
		{
			if ( gmodVersioningCache )
			{
				return *gmodVersioningCache;
			}

			std::lock_guard lock( gmodVersioningMutex );

			if ( gmodVersioningCache )
			{
				return *gmodVersioningCache;
			}

			auto dto = EmbeddedResource::gmodVersioning();

			if ( !dto.has_value() )
			{
				throw std::invalid_argument{ "Invalid state" };
			}

			gmodVersioningCache = std::make_unique<GmodVersioning>( *dto );
			return *gmodVersioningCache;
		}
	} // namespace internal

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

	const Gmod& VIS::gmod( VisVersion visVersion ) const
	{
		// Fast path: read-only access
		{
			std::shared_lock lock( internal::gmodMutex );
			auto it = internal::gmodCache.find( visVersion );
			if ( it != internal::gmodCache.end() )
			{
				return it->second;
			}
		}

		// Slow path: load and cache
		std::unique_lock lock( internal::gmodMutex );

		// Double-check (another thread might have loaded it)
		auto it = internal::gmodCache.find( visVersion );
		if ( it != internal::gmodCache.end() )
		{
			return it->second;
		}

		// Load from embedded resource
		auto versionStr = VisVersions::toString( visVersion );
		auto dto = EmbeddedResource::gmod( versionStr );

		if ( !dto.has_value() )
		{
			throw std::out_of_range{ "Gmod not available for version: " + std::string{ versionStr } };
		}

		auto [inserted, _] = internal::gmodCache.emplace( visVersion, Gmod{ visVersion, *dto } );
		return inserted->second;
	}

	const Codebooks& VIS::codebooks( VisVersion visVersion ) const
	{
		// Fast path: read-only access
		{
			std::shared_lock lock( internal::codebooksMutex );
			auto it = internal::codebooksCache.find( visVersion );
			if ( it != internal::codebooksCache.end() )
			{
				return it->second;
			}
		}

		// Slow path: load and cache
		std::unique_lock lock( internal::codebooksMutex );

		// Double-check
		auto it = internal::codebooksCache.find( visVersion );
		if ( it != internal::codebooksCache.end() )
		{
			return it->second;
		}

		// Load from embedded resource
		auto versionStr = VisVersions::toString( visVersion );
		auto dto = EmbeddedResource::codebooks( versionStr );

		if ( !dto.has_value() )
		{
			throw std::out_of_range{ "Codebooks not available for version: " + std::string{ versionStr } };
		}

		auto [inserted, _] = internal::codebooksCache.emplace( visVersion, Codebooks{ visVersion, *dto } );
		return inserted->second;
	}

	const Locations& VIS::locations( VisVersion visVersion ) const
	{
		// Fast path: read-only access
		{
			std::shared_lock lock( internal::locationsMutex );
			auto it = internal::locationsCache.find( visVersion );
			if ( it != internal::locationsCache.end() )
			{
				return it->second;
			}
		}

		// Slow path: load and cache
		std::unique_lock lock( internal::locationsMutex );

		// Double-check
		auto it = internal::locationsCache.find( visVersion );
		if ( it != internal::locationsCache.end() )
		{
			return it->second;
		}

		// Load from embedded resource
		auto versionStr = VisVersions::toString( visVersion );
		auto dto = EmbeddedResource::locations( versionStr );

		if ( !dto.has_value() )
		{
			throw std::out_of_range{ "Locations not available for version: " + std::string{ versionStr } };
		}

		auto [inserted, _] = internal::locationsCache.emplace( visVersion, Locations{ visVersion, *dto } );
		return inserted->second;
	}

	std::optional<GmodNode> VIS::convertNode( VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion ) const
	{
		return internal::gmodVersioning().convertNode( sourceVersion, sourceNode, targetVersion );
	}

	std::optional<GmodNode> VIS::convertNode( const GmodNode& sourceNode, VisVersion targetVersion, [[maybe_unused]] const GmodNode* sourceParent ) const
	{
		return convertNode( sourceNode.version(), sourceNode, targetVersion );
	}

	std::optional<GmodPath> VIS::convertPath( VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion ) const
	{
		return internal::gmodVersioning().convertPath( sourceVersion, sourcePath, targetVersion );
	}

	std::optional<GmodPath> VIS::convertPath( const GmodPath& sourcePath, VisVersion targetVersion ) const
	{
		return convertPath( sourcePath.version(), sourcePath, targetVersion );
	}

	std::optional<LocalIdBuilder> VIS::convertLocalId( const LocalIdBuilder& sourceLocalId, VisVersion targetVersion ) const
	{
		return internal::gmodVersioning().convertLocalId( sourceLocalId, targetVersion );
	}

	std::optional<LocalId> VIS::convertLocalId( const LocalId& sourceLocalId, VisVersion targetVersion ) const
	{
		return internal::gmodVersioning().convertLocalId( sourceLocalId, targetVersion );
	}

	std::unordered_map<VisVersion, const Gmod&> VIS::gmodsMap( const std::vector<VisVersion>& visVersions ) const
	{
		for ( const auto& version : visVersions )
		{
			auto versionStr = VisVersions::toString( version );
			if ( versionStr.empty() )
			{
				throw std::invalid_argument( "Invalid VIS version provided" );
			}
		}

		std::unordered_set<VisVersion> uniqueVersions( visVersions.begin(), visVersions.end() );

		std::unordered_map<VisVersion, const Gmod&> result;
		result.reserve( uniqueVersions.size() );

		for ( const auto& version : uniqueVersions )
		{
			result.emplace( version, gmod( version ) );
		}

		return result;
	}

	std::unordered_map<VisVersion, const Codebooks&> VIS::codebooksMap( const std::vector<VisVersion>& visVersions ) const
	{
		for ( const auto& version : visVersions )
		{
			auto versionStr = VisVersions::toString( version );
			if ( versionStr.empty() )
			{
				throw std::invalid_argument( "Invalid VIS version provided" );
			}
		}

		std::unordered_set<VisVersion> uniqueVersions( visVersions.begin(), visVersions.end() );

		std::unordered_map<VisVersion, const Codebooks&> result;
		result.reserve( uniqueVersions.size() );

		for ( const auto& version : uniqueVersions )
		{
			result.emplace( version, codebooks( version ) );
		}

		return result;
	}

	std::unordered_map<VisVersion, const Locations&> VIS::locationsMap( const std::vector<VisVersion>& visVersions ) const
	{
		for ( const auto& version : visVersions )
		{
			auto versionStr = VisVersions::toString( version );
			if ( versionStr.empty() )
			{
				throw std::invalid_argument( "Invalid VIS version provided" );
			}
		}

		std::unordered_set<VisVersion> uniqueVersions( visVersions.begin(), visVersions.end() );

		std::unordered_map<VisVersion, const Locations&> result;
		result.reserve( uniqueVersions.size() );

		for ( const auto& version : uniqueVersions )
		{
			result.emplace( version, locations( version ) );
		}

		return result;
	}
} // namespace dnv::vista::sdk
