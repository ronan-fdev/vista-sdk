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

namespace dnv::vista::sdk
{
	namespace
	{
		struct QualifyingNodePair
		{
			const GmodNode* sourceNode;
			const GmodNode* targetNode;

			QualifyingNodePair( const GmodNode* source, const GmodNode* target )
				: sourceNode( source ), targetNode( target ) {}
		};
	}

	//=====================================================================
	// GmodVersioning class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	GmodVersioning::GmodVersioning( const std::unordered_map<std::string, GmodVersioningDto>& dto )
	{
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
		VisVersion currentVersion = sourceVersion;

		while ( currentVersion < targetVersion )
		{
			if ( !node.has_value() )
			{
				break;
			}
			VisVersion nextVersion = VisVersionExtensions::next( currentVersion );
			node = convertNodeInternal( currentVersion, *node, nextVersion );
			currentVersion = nextVersion;
		}

		return node;
	}

	//----------------------------
	// Path
	//----------------------------

	std::optional<GmodPath> GmodVersioning::convertPath(
		VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion ) const
	{
		std::optional<GmodNode> targetEndNodeOpt = convertNode( sourceVersion, sourcePath.node(), targetVersion );
		if ( !targetEndNodeOpt.has_value() )
		{
			return std::nullopt;
		}
		GmodNode targetEndNode = *targetEndNodeOpt;

		if ( targetEndNode.isRoot() )
		{
			const auto& targetGmod = VIS::instance().gmod( targetVersion );
			return GmodPath( targetGmod, targetEndNode, {} );
		}

		const auto& targetGmod = VIS::instance().gmod( targetVersion );

		std::vector<QualifyingNodePair> qualifyingNodes;
		std::deque<GmodNode> convertedNodes;
		auto enumerator = sourcePath.enumerator();
		while ( enumerator.next() )
		{
			const auto& [depth, sourceNode] = enumerator.current();
			std::optional<GmodNode> targetNode = convertNode( sourceVersion, *sourceNode, targetVersion );

			if ( !targetNode.has_value() )
			{
				throw std::runtime_error( "Could not convert node forward" );
			}

			convertedNodes.push_back( std::move( targetNode.value() ) );
			qualifyingNodes.emplace_back( sourceNode, &convertedNodes.back() );
		}

		std::vector<GmodNode> potentialParents;
		for ( size_t i = 0; i < qualifyingNodes.size() - 1; ++i )
		{
			potentialParents.push_back( *qualifyingNodes[i].targetNode );
		}

		std::vector<GmodNode*> potentialParentPtrs;
		for ( auto& parent : potentialParents )
		{
			potentialParentPtrs.push_back( &parent );
		}

		std::vector<GmodNode> simpleParents;
		for ( size_t i = 0; i < qualifyingNodes.size() - 1; ++i )
		{
			if ( simpleParents.empty() || simpleParents.back().code() != qualifyingNodes[i].targetNode->code() )
			{
				simpleParents.push_back( *qualifyingNodes[i].targetNode );
			}
		}

		std::vector<GmodNode*> simpleParentPtrs;
		for ( auto& parent : simpleParents )
		{
			simpleParentPtrs.push_back( &parent );
		}
		int missingLinkAt;
		if ( GmodPath::isValid( simpleParentPtrs, targetEndNode, missingLinkAt ) )
		{
			return GmodPath( targetGmod, targetEndNode, simpleParents );
		}

		auto addToPath = []( const Gmod& gmod, std::vector<GmodNode>& path, const GmodNode& node ) {
			if ( !path.empty() )
			{
				const auto& prev = path.back();
				if ( !prev.isChild( node ) )
				{
					for ( int j = static_cast<int>( path.size() ) - 1; j >= 0; --j )
					{
						const auto& parent = path[static_cast<size_t>( j )];
						std::vector<GmodNode> currentParents( path.begin(), path.begin() + j + 1 );
						std::vector<GmodNode> remaining;
						if ( !GmodTraversal::pathExistsBetween( gmod, currentParents, node, remaining ) )
						{
							bool hasOtherAssetFunction = false;
							for ( const auto& n : currentParents )
							{
								if ( n.isAssetFunctionNode() && n.code() != parent.code() )
								{
									hasOtherAssetFunction = true;
									break;
								}
							}
							if ( !hasOtherAssetFunction )
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
								for ( const auto& n : remaining )
								{
									nodes.push_back( n.isIndividualizable( false, true ) ? n.withLocation( *node.location() ) : n );
								}
							}
							else
							{
								nodes.insert( nodes.end(), remaining.begin(), remaining.end() );
							}
							path.insert( path.end(), nodes.begin(), nodes.end() );
							break;
						}
					}
				}
			}
			path.push_back( node );
		};

		bool selectionChanged = false;
		std::vector<GmodNode> path;
		for ( size_t i = 0; i < qualifyingNodes.size(); ++i )
		{
			const auto& qn = qualifyingNodes[i];

			if ( i > 0 && qn.targetNode->code() == qualifyingNodes[i - 1].targetNode->code() )
			{
				continue;
			}

			bool codeChanged = qn.sourceNode->code() != qn.targetNode->code();

			const GmodNode* sourceNormalAssignment = qn.sourceNode->productType().value_or( nullptr );
			const GmodNode* targetNormalAssignment = qn.targetNode->productType().value_or( nullptr );

			std::string_view sourceAssignmentCode = sourceNormalAssignment ? sourceNormalAssignment->code() : "";
			std::string_view targetAssignmentCode = targetNormalAssignment ? targetNormalAssignment->code() : "";
			bool normalAssignmentChanged = sourceAssignmentCode != targetAssignmentCode;

			if ( codeChanged )
			{
				addToPath( targetGmod, path, *qn.targetNode );
			}

			else if ( normalAssignmentChanged )
			{
				bool wasDeleted = sourceNormalAssignment != nullptr && targetNormalAssignment == nullptr;

				if ( !codeChanged )
				{
					addToPath( targetGmod, path, *qn.targetNode );
				}

				if ( wasDeleted )
				{
					if ( qn.targetNode->code() == targetEndNode.code() )
					{
						if ( i + 1 < qualifyingNodes.size() )
						{
							const auto& next = qualifyingNodes[i + 1];
							if ( next.targetNode->code() != qn.targetNode->code() )
							{
								throw std::runtime_error( "Normal assignment end node was deleted" );
							}
						}
					}
					continue;
				}
				else if ( qn.targetNode->code() != targetEndNode.code() )
				{
					if ( targetNormalAssignment != nullptr )
					{
						addToPath( targetGmod, path, *targetNormalAssignment );
						i++;
					}
				}
			}

			if ( selectionChanged )
			{
				/* SC || SN || SD */
			}

			if ( !codeChanged && !normalAssignmentChanged )
			{
				addToPath( targetGmod, path, *qn.targetNode );
			}

			if ( !path.empty() && path.back().code() == targetEndNode.code() )
			{
				break;
			}
		}

		potentialParents = std::vector<GmodNode>( path.begin(), path.end() - 1 );
		targetEndNode = path.back();

		potentialParentPtrs.clear();
		for ( auto& parent : potentialParents )
		{
			potentialParentPtrs.push_back( &parent );
		}

		if ( !GmodPath::isValid( potentialParentPtrs, targetEndNode, missingLinkAt ) )
		{
			throw std::runtime_error( "Didnt end up with valid path for " + sourcePath.toString() );
		}

		return GmodPath( targetGmod, targetEndNode, potentialParents );
	}

	//----------------------------
	// Local Id
	//----------------------------

	std::optional<LocalIdBuilder>
	GmodVersioning::convertLocalId( const LocalIdBuilder& sourceLocalId, VisVersion targetVersion ) const
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
	// GmodVersioningNode class
	//----------------------------------------------

	//----------------------------------------------
	// Construction
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

	bool GmodVersioning::GmodVersioningNode::tryGetCodeChanges(
		std::string_view code, GmodNodeConversion& nodeChanges ) const
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
		validateSourceAndTargetVersionPair( sourceNode.visVersion(), targetVersion );

		std::string nextCode{ sourceNode.code() };

		auto versioningIt = m_versioningsMap.find( targetVersion );
		if ( versioningIt != m_versioningsMap.end() )
		{
			const auto& versioningNode = versioningIt->second;

			GmodNodeConversion change{};
			if ( versioningNode.tryGetCodeChanges( nextCode, change ) && change.target.has_value() )
			{
				nextCode = change.target.value();
			}
		}

		const auto& targetGmod = VIS::instance().gmod( targetVersion );

		const GmodNode* targetNodePtr = nullptr;
		if ( !targetGmod.tryGetNode( nextCode, targetNodePtr ) )
		{
			return std::nullopt;
		}

		auto resultNode = targetNodePtr->tryWithLocation( sourceNode.location() );
		if ( sourceNode.location().has_value() && resultNode.location() != sourceNode.location() )
		{
			throw std::runtime_error( "Failed to set location" );
		}

		return resultNode;
	}

	bool GmodVersioning::tryGetVersioningNode(
		VisVersion visVersion,
		GmodVersioningNode& versioningNode ) const
	{
		auto it = m_versioningsMap.find( visVersion );
		if ( it != m_versioningsMap.end() )
		{
			versioningNode = it->second;

			return true;
		}

		return false;
	}

	//----------------------------------------------
	// Private validation methods
	//----------------------------------------------

	void GmodVersioning::validateSourceAndTargetVersions(
		VisVersion sourceVersion, VisVersion targetVersion ) const
	{
		if ( sourceVersion == VisVersion::Unknown )
		{
			throw std::invalid_argument( "Invalid source VIS Version: Unknown" );
		}

		if ( targetVersion == VisVersion::Unknown )
		{
			throw std::invalid_argument( "Invalid target VIS Version: Unknown" );
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

		if ( static_cast<int>( targetVersion ) - static_cast<int>( sourceVersion ) != 100 )
		{
			throw std::invalid_argument( "Target version must be exactly one version higher than source version" );
		}
	}

	//----------------------------------------------
	// Private static utility methods
	//----------------------------------------------

	GmodVersioning::ConversionType GmodVersioning::parseConversionType( std::string_view type )
	{
		if ( type == "changeCode" )
		{
			return ConversionType::ChangeCode;
		}
		if ( type == "merge" )
		{
			return ConversionType::Merge;
		}
		if ( type == "move" )
		{
			return ConversionType::Move;
		}
		if ( type == "assignmentChange" )
		{
			return ConversionType::AssignmentChange;
		}
		if ( type == "assignmentDelete" )
		{
			return ConversionType::AssignmentDelete;
		}

		throw std::invalid_argument( "Invalid conversion type: " + std::string{ type } );
	}
}
