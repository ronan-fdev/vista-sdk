/**
 * @file GmodVersioning.cpp
 * @brief Implementation of the GmodVersioning class for converting GMOD objects between VIS versions.
 */

#include "pch.h"

#include "dnv/vista/sdk/GmodVersioning.h"

#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodNode.h"
#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/GmodTraversal.h"
#include "dnv/vista/sdk/LocalId.h"
#include "dnv/vista/sdk/LocalIdBuilder.h"
#include "dnv/vista/sdk/MetadataTag.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISVersion.h"
#include "dnv/vista/sdk/utils/PathConversionCache.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Static helper functions
	//=====================================================================

	static void addToPath( const Gmod& gmod, std::vector<GmodNode>& path, const GmodNode& node )
	{
		if ( !path.empty() )
		{
			const GmodNode& prev = path.back();
			if ( !prev.isChild( node ) )
			{
				for ( int j = static_cast<int>( path.size() ) - 1; j >= 0; --j )
				{
					const GmodNode& parent = path[static_cast<size_t>( j )];

					std::vector<const GmodNode*> currentParents;
					currentParents.reserve( static_cast<size_t>( j + 1 ) );
					for ( size_t k = 0; k <= static_cast<size_t>( j ); ++k )
					{
						currentParents.push_back( &path[k] );
					}

					std::vector<const GmodNode*> remaining;
					if ( !GmodTraversal::pathExistsBetween( gmod, currentParents, node, remaining ) )
					{
						bool hasOtherAssetFunction = false;
						const std::string_view parentCode = parent.code();
						for ( const GmodNode* pathNode : currentParents )
						{
							if ( pathNode->isAssetFunctionNode() && pathNode->code() != parentCode )
							{
								hasOtherAssetFunction = true;
								break;
							}
						}

						if ( !hasOtherAssetFunction )
						{
							throw std::runtime_error( "Tried to remove last asset function node" );
						}
						path.erase( path.begin() + static_cast<std::ptrdiff_t>( j ) );
					}
					else
					{
						const auto nodeLocation = node.location();
						if ( nodeLocation.has_value() )
						{
							for ( const GmodNode* n : remaining )
							{
								if ( n->isIndividualizable( false, true ) )
								{
									path.emplace_back( n->tryWithLocation( nodeLocation.value() ) );
								}
								else
								{
									path.emplace_back( *n );
								}
							}
						}
						else
						{
							path.reserve( path.size() + remaining.size() );
							for ( const GmodNode* n : remaining )
							{
								path.emplace_back( *n );
							}
						}
						break;
					}
				}
			}
		}

		path.emplace_back( node );
	}

	//=====================================================================
	// GmodVersioning class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	GmodVersioning::GmodVersioning( const StringMap<GmodVersioningDto>& dto )
	{
		m_versioningsMap.reserve( dto.size() );

		for ( const auto& [versionStr, versioningDto] : dto )
		{
			VisVersion version = VisVersionExtensions::parse( versionStr );

			m_versioningsMap.emplace( version, GmodVersioningNode( version, versioningDto.items() ) );
		}
	}

	//----------------------------------------------
	// Conversion
	//----------------------------------------------

	//----------------------------
	// Node
	//----------------------------

	std::optional<GmodNode> GmodVersioning::convertNode(
		VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion ) const
	{
		if ( sourceNode.code().empty() )
		{
			return std::nullopt;
		}

		validateSourceAndTargetVersions( sourceVersion, targetVersion );

		std::optional<GmodNode> node = sourceNode;
		VisVersion source = sourceVersion;

		while ( source <= targetVersion - 1 )
		{
			if ( !node.has_value() )
			{
				break;
			}

			const VisVersion target = source + 1;

			node = convertNodeInternal( source, *node, target );

			++source;
		}

		return node;
	}

	std::optional<GmodNode> GmodVersioning::convertNode(
		VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion, const Gmod& targetGmod ) const
	{
		if ( sourceNode.code().empty() )
		{
			return std::nullopt;
		}

		validateSourceAndTargetVersions( sourceVersion, targetVersion );

		std::optional<GmodNode> node = sourceNode;
		VisVersion source = sourceVersion;

		while ( source <= targetVersion - 1 )
		{
			if ( !node.has_value() )
			{
				break;
			}

			const VisVersion target = source + 1;

			if ( target == targetVersion )
			{
				node = convertNodeInternal( source, *node, target, targetGmod );
			}
			else
			{
				node = convertNodeInternal( source, *node, target );
			}

			++source;
		}

		return node;
	}

	//----------------------------
	// Path
	//----------------------------

	std::optional<GmodPath> GmodVersioning::convertPath( VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion ) const
	{
		auto& pathCache = PathConversionCache::instance();
		const std::string pathString = sourcePath.toString();

		std::optional<GmodPath> cachedResult;
		if ( pathCache.tryGetCachedConversion( sourceVersion, pathString, targetVersion, cachedResult ) )
		{
			return cachedResult;
		}

		auto result = convertPathInternal( sourceVersion, sourcePath, targetVersion );

		pathCache.cacheConversion( sourceVersion, pathString, targetVersion, result );

		return result;
	}

	std::optional<GmodPath> GmodVersioning::convertPathInternal( VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion ) const
	{
		std::optional<GmodNode> targetEndNode = convertNode( sourceVersion, sourcePath.node(), targetVersion );
		if ( !targetEndNode.has_value() )
		{
			return std::nullopt;
		}

		auto& vis = VIS::instance();
		const auto& targetGmod = vis.gmod( targetVersion );

		if ( targetEndNode->isRoot() )
		{
			const GmodNode* rootNodeInGmodPtr = nullptr;
			if ( !targetGmod.tryGetNode( targetEndNode->code(), rootNodeInGmodPtr ) || !rootNodeInGmodPtr )
			{
				throw std::runtime_error( "Failed to get root node from targetGmod during root path conversion" );
			}

			return GmodPath( targetGmod, *rootNodeInGmodPtr, {} );
		}

		std::vector<std::pair<const GmodNode*, GmodNode>> qualifyingNodes;
		qualifyingNodes.reserve( sourcePath.length() );

		for ( size_t i = 0; i < sourcePath.length(); ++i )
		{
			const GmodNode& originalNodeInPath = sourcePath[i];

			std::optional<GmodNode> convertedNodeOpt = convertNode( sourceVersion, originalNodeInPath, targetVersion, targetGmod );
			if ( !convertedNodeOpt.has_value() )
			{
				throw std::runtime_error( "Could not convert node forward" );
			}

			qualifyingNodes.emplace_back( &originalNodeInPath, std::move( *convertedNodeOpt ) );
		}

		std::vector<GmodNode> potentialParents;
		potentialParents.reserve( qualifyingNodes.size() );

		for ( size_t i = 0; i < qualifyingNodes.size() - 1; ++i )
		{
			potentialParents.emplace_back( qualifyingNodes[i].second );
		}

		std::vector<GmodNode*> potentialParentPtrs;
		potentialParentPtrs.reserve( potentialParents.size() );
		std::transform( potentialParents.begin(), potentialParents.end(), std::back_inserter( potentialParentPtrs ),
			[]( GmodNode& parent ) noexcept { return &parent; } );

		if ( GmodPath::isValid( potentialParentPtrs, *targetEndNode ) )
		{
			return GmodPath( targetGmod, *targetEndNode, std::move( potentialParents ) );
		}

		std::vector<GmodNode> path;
		path.reserve( 64 );

		for ( size_t i = 0; i < qualifyingNodes.size(); ++i )
		{
			const auto& qualifyingNode = qualifyingNodes[i];

			const std::string_view currentCode = qualifyingNode.second.code();
			if ( i > 0 )
			{
				const std::string_view prevCode = qualifyingNodes[i - 1].second.code();
				if ( currentCode == prevCode )
				{
					continue;
				}
			}

			const std::string_view sourceCode = qualifyingNode.first->code();
			const bool codeChanged = sourceCode != currentCode;

			const std::optional<GmodNode> sourceNormalAssignment = qualifyingNode.first->productType();
			const std::optional<GmodNode> targetNormalAssignment = qualifyingNode.second.productType();

			const bool normalAssignmentChanged =
				sourceNormalAssignment.has_value() != targetNormalAssignment.has_value() ||
				( sourceNormalAssignment.has_value() && targetNormalAssignment.has_value() &&
					sourceNormalAssignment->code() != targetNormalAssignment->code() );

			bool selectionChanged = false;

			if ( codeChanged )
			{
				addToPath( targetGmod, path, qualifyingNode.second );
			}
			else if ( normalAssignmentChanged )
			{
				bool wasDeleted = sourceNormalAssignment.has_value() && !targetNormalAssignment.has_value();

				if ( !codeChanged )
				{
					addToPath( targetGmod, path, qualifyingNode.second );
				}

				if ( wasDeleted )
				{
					if ( qualifyingNode.second.code() == targetEndNode->code() )
					{
						if ( i + 1 < qualifyingNodes.size() )
						{
							const auto& next = qualifyingNodes[i + 1];
							if ( next.second.code() != qualifyingNode.second.code() )
							{
								throw std::runtime_error( "Normal assignment end node was deleted" );
							}
						}
					}
					continue;
				}
				else if ( currentCode != targetEndNode->code() )
				{
					if ( targetNormalAssignment.has_value() )
					{
						GmodNode targetNormalAssignmentVal = *targetNormalAssignment;
						if ( qualifyingNode.second.location().has_value() && targetNormalAssignmentVal.isIndividualizable( false, true ) )
						{
							targetNormalAssignmentVal = targetNormalAssignmentVal.tryWithLocation( *qualifyingNode.second.location() );
						}
						addToPath( targetGmod, path, targetNormalAssignmentVal );
						++i;
					}
				}
			}

			if ( selectionChanged )
			{
				/* SC || SN || SD */
			}

			if ( !codeChanged && !normalAssignmentChanged )
			{
				addToPath( targetGmod, path, qualifyingNode.second );
			}

			if ( !path.empty() && std::string_view( path.back().code() ) == targetEndNode->code() )
			{
				break;
			}
		}

		if ( path.empty() || path.size() < 1 )
		{
			throw std::runtime_error( "Path reconstruction resulted in an empty path" );
		}

		std::vector<GmodNode> potentialParentsFromPath;
		potentialParentsFromPath.reserve( path.size() > 0 ? path.size() - 1 : 0 );

		if ( path.size() > 1 )
		{
			potentialParentsFromPath.insert( potentialParentsFromPath.end(),
				std::make_move_iterator( path.begin() ),
				std::make_move_iterator( path.end() - 1 ) );
		}

		GmodNode targetEndNodeFromPath = std::move( path.back() );

		std::vector<GmodNode*> potentialParentPtrsFromPath;
		potentialParentPtrsFromPath.reserve( potentialParentsFromPath.size() );
		for ( auto& parent : potentialParentsFromPath )
		{
			potentialParentPtrsFromPath.push_back( &parent );
		}

		int missingLinkAt;
		if ( !GmodPath::isValid( potentialParentPtrsFromPath, targetEndNodeFromPath, missingLinkAt ) )
		{
			throw std::runtime_error( "Did not end up with a valid path" );
		}

		return GmodPath( targetGmod, std::move( targetEndNodeFromPath ), std::move( potentialParentsFromPath ) );
	}

	//----------------------------
	// Local Id
	//----------------------------

	std::optional<LocalIdBuilder> GmodVersioning::convertLocalId(
		const LocalIdBuilder& sourceLocalId, VisVersion targetVersion ) const
	{
		if ( !sourceLocalId.visVersion().has_value() )
		{
			throw std::invalid_argument( "Cannot convert local ID without a specific VIS version" );
		}

		LocalIdBuilder targetLocalId = LocalIdBuilder::create( targetVersion );

		if ( sourceLocalId.primaryItem().has_value() )
		{
			auto targetPrimaryItem = convertPath(
				*sourceLocalId.visVersion(),
				sourceLocalId.primaryItem().value(),
				targetVersion );

			if ( !targetPrimaryItem.has_value() )
			{
				return std::nullopt;
			}

			targetLocalId = targetLocalId.withPrimaryItem( std::move( *targetPrimaryItem ) );
		}

		if ( sourceLocalId.secondaryItem().has_value() )
		{
			auto targetSecondaryItem = convertPath(
				*sourceLocalId.visVersion(),
				sourceLocalId.secondaryItem().value(),
				targetVersion );

			if ( !targetSecondaryItem.has_value() )
			{
				return std::nullopt;
			}

			targetLocalId = targetLocalId.withSecondaryItem( std::move( *targetSecondaryItem ) );
		}

		return targetLocalId
			.withVerboseMode( sourceLocalId.isVerboseMode() )
			.tryWithMetadataTag( sourceLocalId.quantity() )
			.tryWithMetadataTag( sourceLocalId.content() )
			.tryWithMetadataTag( sourceLocalId.calculation() )
			.tryWithMetadataTag( sourceLocalId.state() )
			.tryWithMetadataTag( sourceLocalId.command() )
			.tryWithMetadataTag( sourceLocalId.type() )
			.tryWithMetadataTag( sourceLocalId.position() )
			.tryWithMetadataTag( sourceLocalId.detail() );
	}

	std::optional<LocalId> GmodVersioning::convertLocalId(
		const LocalId& sourceLocalId, VisVersion targetVersion ) const
	{
		auto builder = convertLocalId( sourceLocalId.builder(), targetVersion );

		return builder.has_value()
				   ? std::make_optional( builder->build() )
				   : std::nullopt;
	}

	//----------------------------------------------
	// GmodVersioning::GmodVersioningNode class
	//----------------------------------------------

	//----------------------------
	// Construction
	//----------------------------

	GmodVersioning::GmodVersioningNode::GmodVersioningNode(
		VisVersion visVersion,
		const StringMap<GmodNodeConversionDto>& dto )
		: m_visVersion{ visVersion }
	{
		for ( const auto& [code, dtoNode] : dto )
		{
			GmodNodeConversion conversion;
			conversion.source = dtoNode.source();
			if ( dtoNode.target().empty() )
			{
				conversion.target = std::nullopt;
			}
			else
			{
				conversion.target = dtoNode.target();
			}
			conversion.oldAssignment = dtoNode.oldAssignment();
			conversion.newAssignment = dtoNode.newAssignment();
			conversion.deleteAssignment = dtoNode.deleteAssignment();

			if ( !dtoNode.operations().empty() )
			{
				for ( const auto& type : dtoNode.operations() )
				{
					conversion.operations.insert( parseConversionType( type ) );
				}
			}

			m_versioningNodeChanges.emplace( code, conversion );
		}
	}

	//----------------------------
	// Private helper methods
	//----------------------------

	std::optional<GmodNode> GmodVersioning::convertNodeInternal(
		[[maybe_unused]] VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion ) const
	{
		validateSourceAndTargetVersionPair( sourceNode.visVersion(), targetVersion );

		std::string_view nextCodeView = sourceNode.code();

		const GmodVersioningNode* versioningNode = nullptr;
		if ( tryGetVersioningNode( targetVersion, versioningNode ) )
		{
			const GmodNodeConversion* change = nullptr;
			if ( versioningNode->tryGetCodeChanges( nextCodeView, change ) && change && change->target.has_value() )
			{
				nextCodeView = change->target.value();
			}
		}

		const auto& targetGmod = VIS::instance().gmod( targetVersion );

		const GmodNode* targetNodePtr = nullptr;
		if ( !targetGmod.tryGetNode( nextCodeView, targetNodePtr ) )
		{
			return std::nullopt;
		}

		if ( sourceNode.location().has_value() )
		{
			return targetNodePtr->tryWithLocation( sourceNode.location().value() );
		}

		return *targetNodePtr;
	}

	std::optional<GmodNode> GmodVersioning::convertNodeInternal(
		[[maybe_unused]] VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion, const Gmod& targetGmod ) const
	{
		validateSourceAndTargetVersionPair( sourceNode.visVersion(), targetVersion );

		std::string_view nextCodeView = sourceNode.code();

		const GmodVersioningNode* versioningNode = nullptr;
		if ( tryGetVersioningNode( targetVersion, versioningNode ) )
		{
			const GmodNodeConversion* change = nullptr;
			if ( versioningNode->tryGetCodeChanges( nextCodeView, change ) && change && change->target.has_value() )
			{
				nextCodeView = change->target.value();
			}
		}

		const GmodNode* targetNodePtr = nullptr;
		if ( !targetGmod.tryGetNode( nextCodeView, targetNodePtr ) )
		{
			return std::nullopt;
		}

		if ( sourceNode.location().has_value() )
		{
			return targetNodePtr->tryWithLocation( sourceNode.location().value() );
		}

		return *targetNodePtr;
	}
}
