/**
 * @file GmodVersioning.cpp
 * @brief Implementation of the GmodVersioning class for converting GMOD objects between VIS versions.
 */

#include "pch.h"

#include "dnv/vista/sdk/GmodVersioning.h"

#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodTraversal.h"
#include "dnv/vista/sdk/GmodNode.h"
#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/LocalIdBuilder.h"
#include "dnv/vista/sdk/VIS.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// GmodVersioning Class
	//=====================================================================

	//----------------------------------------------
	// Construction / destruction
	//----------------------------------------------

	GmodVersioning::GmodVersioning( const std::unordered_map<std::string, GmodVersioningDto>& dto )
	{
		SPDLOG_INFO( "Creating GmodVersioning with {} version entries", dto.size() );

		for ( const auto& [versionStr, versioningDto] : dto )
		{
			VisVersion version = VisVersionExtensions::parse( versionStr );

			m_versioningsMap.emplace( version, GmodVersioningNode( version, versioningDto.items() ) );
		}
	}

	//----------------------------------------------
	// Conversion
	//----------------------------------------------

	std::optional<GmodNode> GmodVersioning::convertNode(
		VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion ) const
	{
		if ( sourceNode.code().empty() )
		{
			return std::nullopt;
		}

		if ( !VisVersionExtensions::isValid( sourceVersion ) ||
			 !VisVersionExtensions::isValid( targetVersion ) ||
			 sourceVersion >= targetVersion )
		{
			return std::nullopt;
		}

		const auto& allVersions = VisVersionExtensions::allVersions();
		auto it = std::find( allVersions.begin(), allVersions.end(), sourceVersion );
		if ( it == allVersions.end() )
		{
			return std::nullopt;
		}

		std::optional<GmodNode> currentNodeOpt = sourceNode;
		VisVersion currentVersion = sourceVersion;

		while ( it != allVersions.end() && currentVersion != targetVersion )
		{
			++it;
			if ( it == allVersions.end() )
			{
				return std::nullopt;
			}

			VisVersion nextVersion = *it;
			currentNodeOpt = convertNodeInternal( currentVersion, *currentNodeOpt, nextVersion );
			if ( !currentNodeOpt.has_value() )
			{
				return std::nullopt;
			}

			currentVersion = nextVersion;
		}

		return currentNodeOpt;
	}

	std::optional<GmodPath> GmodVersioning::convertPath(
		VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion ) const
	{
		if ( !VisVersionExtensions::isValid( sourceVersion ) ||
			 !VisVersionExtensions::isValid( targetVersion ) ||
			 sourceVersion >= targetVersion )
		{
			return std::nullopt;
		}

		std::optional<GmodNode> targetEndNodeOpt = convertNode( sourceVersion, *sourcePath.node(), targetVersion );
		if ( !targetEndNodeOpt.has_value() )
		{
			return std::nullopt;
		}
		GmodNode targetEndNode = *targetEndNodeOpt;

		if ( targetEndNode.isRoot() )
		{
			auto& vis = VIS::instance();
			const auto& targetGmod = vis.gmod( targetVersion );
			const GmodNode* rootNodePtr = nullptr;
			if ( !targetGmod.tryGetNode( targetEndNode.code(), rootNodePtr ) || !rootNodePtr )
			{
				return std::nullopt;
			}
			return GmodPath( targetGmod, const_cast<GmodNode*>( rootNodePtr ), {} );
		}

		auto& vis = VIS::instance();
		const auto& targetGmod = vis.gmod( targetVersion );
		const auto& sourceGmod = vis.gmod( sourceVersion );

		std::vector<std::pair<const GmodNode*, GmodNode>> qualifyingNodes;
		auto enumerator = sourcePath.enumerator();
		while ( enumerator.next() )
		{
			const auto& [depth, originalNode] = enumerator.current();
			std::optional<GmodNode> convertedNodeOpt = convertNode( sourceVersion, *originalNode, targetVersion );
			if ( !convertedNodeOpt.has_value() )
			{
				return std::nullopt;
			}
			qualifyingNodes.emplace_back( originalNode, std::move( *convertedNodeOpt ) );
		}

		if ( qualifyingNodes.empty() )
		{
			return std::nullopt;
		}

		std::vector<GmodNode> potentialParentsList;
		for ( size_t i = 0; i < qualifyingNodes.size() - 1; ++i )
		{
			potentialParentsList.push_back( qualifyingNodes[i].second );
		}

		std::vector<GmodNode*> potentialParents;
		std::vector<GmodNode> parentStorage;
		for ( const auto& node : potentialParentsList )
		{
			const GmodNode* basePtr = nullptr;
			if ( !targetGmod.tryGetNode( node.code(), basePtr ) || !basePtr )
			{
				return std::nullopt;
			}

			if ( node.location().has_value() )
			{
				parentStorage.push_back( basePtr->tryWithLocation( *node.location() ) );
				potentialParents.push_back( &parentStorage.back() );
			}
			else
			{
				potentialParents.push_back( const_cast<GmodNode*>( basePtr ) );
			}
		}

		const GmodNode* targetEndNodePtr = nullptr;
		if ( !targetGmod.tryGetNode( targetEndNode.code(), targetEndNodePtr ) || !targetEndNodePtr )
		{
			return std::nullopt;
		}

		GmodNode* finalTargetEndNode;
		if ( targetEndNode.location().has_value() )
		{
			parentStorage.push_back( targetEndNodePtr->tryWithLocation( *targetEndNode.location() ) );
			finalTargetEndNode = &parentStorage.back();
		}
		else
		{
			finalTargetEndNode = const_cast<GmodNode*>( targetEndNodePtr );
		}

		int missingLinkAt;
		if ( GmodPath::isValid( potentialParents, *finalTargetEndNode, missingLinkAt ) )
		{
			return GmodPath( targetGmod, finalTargetEndNode, potentialParents );
		}

		std::vector<GmodNode> path;
		std::vector<GmodNode> pathStorage;

		auto addToPath = [&]( const GmodNode& node ) -> void {
			if ( path.size() > 0 )
			{
				const GmodNode& prev = path.back();
				if ( !prev.isChild( node ) )
				{
					for ( int j = static_cast<int>( path.size() ) - 1; j >= 0; j-- )
					{
						const GmodNode& parent = path[j];

						std::vector<const GmodNode*> currentParents;
						std::vector<GmodNode> currentParentStorage;
						for ( int k = 0; k <= j; k++ )
						{
							const GmodNode* basePtr = nullptr;
							if ( !targetGmod.tryGetNode( path[k].code(), basePtr ) || !basePtr )
							{
								continue;
							}

							if ( path[k].location().has_value() )
							{
								currentParentStorage.push_back( basePtr->tryWithLocation( *path[k].location() ) );
								currentParents.push_back( &currentParentStorage.back() );
							}
							else
							{
								currentParents.push_back( basePtr );
							}
						}

						std::vector<const GmodNode*> remaining;
						bool pathExists = GmodTraversal::pathExistsBetween( targetGmod, currentParents, node, remaining );

						if ( !pathExists )
						{
							bool hasNonAssetFunction = false;
							for ( const auto* currentParent : currentParents )
							{
								if ( currentParent->isAssetFunctionNode() && currentParent->code() != parent.code() )
								{
									hasNonAssetFunction = true;
									break;
								}
							}

							if ( !hasNonAssetFunction )
							{
								throw std::runtime_error( "Tried to remove last asset function node" );
							}

							path.erase( path.begin() + j );
						}
						else
						{
							std::vector<GmodNode> nodes;

							if ( node.location().has_value() )
							{
								for ( const auto* n : remaining )
								{
									if ( !n->isIndividualizable( false, true ) )
									{
										nodes.push_back( *n );
									}
									else
									{
										nodes.push_back( n->tryWithLocation( *node.location() ) );
									}
								}
							}
							else
							{
								for ( const auto* n : remaining )
								{
									nodes.push_back( *n );
								}
							}

							path.insert( path.end(), nodes.begin(), nodes.end() );
							break;
						}
					}
				}
			}

			path.push_back( node );
		};

		for ( size_t i = 0; i < qualifyingNodes.size(); ++i )
		{
			const auto& qualifyingNode = qualifyingNodes[i];
			const GmodNode* sourceNode = qualifyingNode.first;
			const GmodNode& targetNode = qualifyingNode.second;

			if ( i > 0 && targetNode.code() == qualifyingNodes[i - 1].second.code() )
			{
				continue;
			}

			bool codeChanged = sourceNode->code() != targetNode.code();

			const GmodNode* sourceNormalAssignment = sourceNode->productType();
			const GmodNode* targetNormalAssignment = targetNode.productType();

			bool normalAssignmentChanged = false;
			if ( sourceNormalAssignment != nullptr && targetNormalAssignment != nullptr )
			{
				normalAssignmentChanged = sourceNormalAssignment->code() != targetNormalAssignment->code();
			}
			else if ( ( sourceNormalAssignment != nullptr ) != ( targetNormalAssignment != nullptr ) )
			{
				normalAssignmentChanged = true;
			}

			bool selectionChanged = false;

			if ( codeChanged )
			{
				addToPath( targetNode );
			}
			else if ( normalAssignmentChanged ) /* AC || AN || AD */
			{
				bool wasDeleted = sourceNormalAssignment != nullptr && targetNormalAssignment == nullptr;

				if ( !codeChanged )
				{
					addToPath( targetNode );
				}

				if ( wasDeleted )
				{
					if ( targetNode.code() == targetEndNode.code() )
					{
						if ( i + 1 < qualifyingNodes.size() )
						{
							const auto& next = qualifyingNodes[i + 1];
							if ( next.second.code() != targetNode.code() )
							{
								throw std::runtime_error( "Normal assignment end node was deleted" );
							}
						}
					}
					continue;
				}
				else if ( targetNode.code() != targetEndNode.code() )
				{
					if ( targetNormalAssignment != nullptr )
					{
						addToPath( *targetNormalAssignment );
						i++;
					}
				}
			}

			if ( selectionChanged ) /* SC || SN || SD */
			{
			}

			if ( !codeChanged && !normalAssignmentChanged )
			{
				addToPath( targetNode );
			}

			if ( !path.empty() && path.back().code() == targetEndNode.code() )
			{
				break;
			}
		}

		if ( path.empty() )
		{
			return std::nullopt;
		}

		std::vector<GmodNode> finalPotentialParents( path.begin(), path.end() - 1 );
		GmodNode finalTargetEndNodeResult = path.back();

		std::vector<GmodNode*> finalParents;
		std::vector<GmodNode> finalStorage;

		for ( const auto& pathNode : finalPotentialParents )
		{
			const GmodNode* basePtr = nullptr;
			if ( !targetGmod.tryGetNode( pathNode.code(), basePtr ) || !basePtr )
			{
				return std::nullopt;
			}

			if ( pathNode.location().has_value() )
			{
				finalStorage.push_back( basePtr->tryWithLocation( *pathNode.location() ) );
				finalParents.push_back( &finalStorage.back() );
			}
			else
			{
				finalParents.push_back( const_cast<GmodNode*>( basePtr ) );
			}
		}

		const GmodNode* finalEndPtr = nullptr;
		if ( !targetGmod.tryGetNode( finalTargetEndNodeResult.code(), finalEndPtr ) || !finalEndPtr )
		{
			return std::nullopt;
		}

		GmodNode* finalEndNodeForPath;
		if ( finalTargetEndNodeResult.location().has_value() )
		{
			finalStorage.push_back( finalEndPtr->tryWithLocation( *finalTargetEndNodeResult.location() ) );
			finalEndNodeForPath = &finalStorage.back();
		}
		else
		{
			finalEndNodeForPath = const_cast<GmodNode*>( finalEndPtr );
		}

		int missingLinkAtFinal;
		if ( !GmodPath::isValid( finalParents, *finalEndNodeForPath, missingLinkAtFinal ) )
		{
			throw std::runtime_error( "Didn't end up with valid path for " + sourcePath.toString() );
		}

		return GmodPath( targetGmod, finalEndNodeForPath, finalParents );
	}

	std::optional<LocalIdBuilder> GmodVersioning::convertLocalId(
		const LocalIdBuilder& sourceLocalId, VisVersion targetVersion ) const
	{
		if ( !sourceLocalId.visVersion().has_value() )
		{
			throw std::invalid_argument( "Cannot convert local ID without a specific VIS version" );
		}

		std::optional<GmodPath> primaryItemPathOpt;
		if ( sourceLocalId.primaryItem().has_value() )
		{
			primaryItemPathOpt = convertPath(
				*sourceLocalId.visVersion(),
				sourceLocalId.primaryItem().value(),
				targetVersion );
			if ( !primaryItemPathOpt.has_value() )
			{
				return std::nullopt;
			}
		}

		std::optional<GmodPath> secondaryItemPathOpt;
		if ( sourceLocalId.secondaryItem().has_value() )
		{
			secondaryItemPathOpt = convertPath(
				*sourceLocalId.visVersion(),
				sourceLocalId.secondaryItem().value(),
				targetVersion );
			if ( !secondaryItemPathOpt.has_value() )
			{
				return std::nullopt;
			}
		}

		return LocalIdBuilder::create( targetVersion )
			.tryWithPrimaryItem( std::move( primaryItemPathOpt ) )
			.tryWithSecondaryItem( std::move( secondaryItemPathOpt ) )
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
		if ( !builder.has_value() )
		{
			return std::nullopt;
		}

		return builder->build();
	}

	//----------------------------------------------
	// GmodVersioningNode Class
	//----------------------------------------------

	//----------------------------------------------
	// Construction / destruction
	//----------------------------------------------

	GmodVersioning::GmodVersioningNode::GmodVersioningNode(
		VisVersion visVersion,
		const std::unordered_map<std::string, GmodNodeConversionDto>& dto )
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

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	VisVersion GmodVersioning::GmodVersioningNode::visVersion() const
	{
		return m_visVersion;
	}

	bool GmodVersioning::GmodVersioningNode::tryGetCodeChanges(
		const std::string& code, GmodNodeConversion& nodeChanges ) const
	{
		auto it = m_versioningNodeChanges.find( code );
		if ( it != m_versioningNodeChanges.end() )
		{
			nodeChanges = it->second;
			return true;
		}
		return false;
	}

	//----------------------------------------------
	// Private helper methods
	//----------------------------------------------

	std::optional<GmodNode> GmodVersioning::convertNodeInternal(
		[[maybe_unused]] VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion ) const
	{
		if ( sourceNode.visVersion() >= targetVersion )
		{
			return std::nullopt;
		}

		const std::string& sourceCode = sourceNode.code();
		std::string targetCode = sourceCode;

		GmodVersioningNode versioningNode{};
		if ( tryGetVersioningNode( targetVersion, versioningNode ) )
		{
			GmodNodeConversion change{};
			if ( versioningNode.tryGetCodeChanges( sourceCode, change ) )
			{
				if ( change.target.has_value() )
				{
					targetCode = change.target.value();
				}
			}
		}

		static thread_local auto* vis = &VIS::instance();
		const auto& targetGmod = vis->gmod( targetVersion );

		const GmodNode* targetNodePtr = nullptr;
		if ( !targetGmod.tryGetNode( targetCode, targetNodePtr ) || !targetNodePtr )
		{
			return std::nullopt;
		}

		const auto& sourceLocation = sourceNode.location();
		if ( !sourceLocation.has_value() )
		{
			return *targetNodePtr;
		}

		GmodNode resultNode = targetNodePtr->tryWithLocation( sourceLocation );

		return resultNode;
	}

	bool GmodVersioning::tryGetVersioningNode(
		VisVersion visVersion, GmodVersioningNode& versioningNode ) const
	{
		auto it = m_versioningsMap.find( visVersion );
		if ( it != m_versioningsMap.end() )
		{
			versioningNode = it->second;
			return true;
		}

		return false;
	}

	const GmodVersioning::GmodVersioningNode* GmodVersioning::tryGetVersioningNode( VisVersion visVersion ) const noexcept
	{
		auto it = m_versioningsMap.find( visVersion );
		if ( it != m_versioningsMap.end() )
		{
			return &it->second;
		}

		return nullptr;
	}

	//----------------------------------------------
	// Private Validation Methods
	//----------------------------------------------

	void GmodVersioning::validateSourceAndTargetVersions( VisVersion sourceVersion, VisVersion targetVersion ) const
	{
		if ( !VisVersionExtensions::isValid( sourceVersion ) ||
			 !VisVersionExtensions::isValid( targetVersion ) )
		{
			throw std::invalid_argument( "Invalid VIS version" );
		}

		if ( sourceVersion >= targetVersion )
		{
			throw std::invalid_argument( "Source version must be earlier than target version" );
		}
	}

	void GmodVersioning::validateSourceAndTargetVersionPair(
		VisVersion sourceVersion, VisVersion targetVersion ) const
	{
		if ( sourceVersion >= targetVersion )
		{
			throw std::invalid_argument( "Source version must be less than target version" );
		}

		const auto& allVersions = VisVersionExtensions::allVersions();
		auto itSource = std::find( allVersions.begin(), allVersions.end(), sourceVersion );

		bool isTargetTheExactNextVersion = false;
		if ( itSource != allVersions.end() )
		{
			auto itNext = std::next( itSource );
			if ( itNext != allVersions.end() )
			{
				if ( *itNext == targetVersion )
				{
					isTargetTheExactNextVersion = true;
				}
			}
		}

		if ( !isTargetTheExactNextVersion )
		{
			throw std::invalid_argument( "Target version must be exactly one version higher than source version" );
		}
	}

	//----------------------------------------------
	// Private Static Utility Methods
	//----------------------------------------------

	GmodVersioning::ConversionType GmodVersioning::parseConversionType( const std::string& type )
	{
		static const std::unordered_map<std::string, ConversionType> typeMap = {
			{ "changeCode", ConversionType::ChangeCode },
			{ "merge", ConversionType::Merge },
			{ "move", ConversionType::Move },
			{ "assignmentChange", ConversionType::AssignmentChange },
			{ "assignmentDelete", ConversionType::AssignmentDelete } };

		auto it = typeMap.find( type );
		if ( it != typeMap.end() )
		{
			return it->second;
		}
		else
		{
			std::string errorMsg = "Invalid conversion type: " + type;
			throw std::invalid_argument( errorMsg );
		}
	}
}
