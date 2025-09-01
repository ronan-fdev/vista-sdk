/**
 * @file Gmod.cpp
 * @brief Implementation of the Generic Product Model (GMOD) class.
 */

#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodPath.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Gmod class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Gmod::Gmod( VisVersion version, const GmodDto& dto )
		: m_visVersion{ version },
		  m_rootNode{ nullptr }
	{
		const auto& items = dto.items();
		std::vector<std::pair<std::string, GmodNode>> nodePairs;
		nodePairs.reserve( items.size() );

		for ( const auto& nodeDto : items )
		{
			nodePairs.emplace_back( std::string{ nodeDto.code() }, GmodNode{ version, nodeDto } );
		}

		m_nodeMap = internal::ChdDictionary<GmodNode>( std::move( nodePairs ) );

		for ( const auto& relation : dto.relations() )
		{
			if ( relation.size() >= 2 )
			{
				auto& parentNode = const_cast<GmodNode&>( m_nodeMap[relation[0]] );
				auto& childNode = const_cast<GmodNode&>( m_nodeMap[relation[1]] );

				parentNode.addChild( &childNode );
				childNode.addParent( &parentNode );
			}
		}

		for ( auto& [key, node] : m_nodeMap )
		{
			const_cast<GmodNode&>( node ).trim();
		}

		m_rootNode = const_cast<GmodNode*>( &m_nodeMap["VE"] );
	}

	Gmod::Gmod( VisVersion version, const std::unordered_map<std::string, GmodNode>& nodeMap )
		: m_visVersion{ version }
	{
		std::vector<std::pair<std::string, GmodNode>> pairs;
		pairs.reserve( nodeMap.size() );
		for ( const auto& [code, node] : nodeMap )
		{
			pairs.emplace_back( code, node );
		}
		m_nodeMap = internal::ChdDictionary<GmodNode>( std::move( pairs ) );

		for ( auto& [key, node] : m_nodeMap )
		{
			const_cast<GmodNode&>( node ).trim();
		}

		m_rootNode = const_cast<GmodNode*>( &m_nodeMap["VE"] );
	}

	Gmod::Gmod( const Gmod& other )
		: m_visVersion{ other.m_visVersion },
		  m_nodeMap{ other.m_nodeMap }
	{
		m_rootNode = const_cast<GmodNode*>( &m_nodeMap["VE"] );
	}

	//----------------------------------------------
	// Path parsing & navigation
	//----------------------------------------------

	GmodPath Gmod::parsePath( std::string_view item ) const
	{
		return GmodPath::parse( item, m_visVersion );
	}

	bool Gmod::tryParsePath( std::string_view item, std::optional<GmodPath>& path ) const noexcept
	{
		return GmodPath::tryParse( item, m_visVersion, path );
	}

	GmodPath Gmod::parseFromFullPath( std::string_view item ) const
	{
		return GmodPath::parseFullPath( item, m_visVersion );
	}

	bool Gmod::tryParseFromFullPath( std::string_view item, std::optional<GmodPath>& path ) const noexcept
	{
		return GmodPath::tryParseFullPath( item, m_visVersion, path );
	}

	//----------------------------------------------
	// Traversal methods
	//----------------------------------------------

	bool Gmod::pathExistsBetween( const std::vector<const GmodNode*>& fromPath, const GmodNode& to,
		std::vector<const GmodNode*>& remainingParents ) const
	{
		remainingParents.clear();

		const GmodNode* lastAssetFunction = nullptr;
		size_t assetFunctionIndex = SIZE_MAX;
		for ( size_t i = fromPath.size(); i > 0; --i )
		{
			size_t idx = i - 1;
			if ( fromPath[idx]->isAssetFunctionNode() )
			{
				lastAssetFunction = fromPath[idx];
				assetFunctionIndex = idx;
				break;
			}
		}

		const GmodNode& startNode = lastAssetFunction
										? *lastAssetFunction
										: rootNode();

		struct PathExistsState
		{
			const GmodNode& targetNode;
			const std::vector<const GmodNode*>& fromPath;
			std::vector<const GmodNode*>& remainingParents;
			size_t assetFunctionIndex;
			bool found = false;

			PathExistsState( const GmodNode& target, const std::vector<const GmodNode*>& from,
				std::vector<const GmodNode*>& remaining, size_t afIndex )
				: targetNode{ target },
				  fromPath{ from },
				  remainingParents{ remaining },
				  assetFunctionIndex{ afIndex }
			{
			}

			PathExistsState( const PathExistsState& ) = delete;
			PathExistsState( PathExistsState&& ) = delete;
			PathExistsState& operator=( const PathExistsState& ) = delete;
			PathExistsState& operator=( PathExistsState&& ) = delete;
		};

		PathExistsState state( to, fromPath, remainingParents, assetFunctionIndex );

		auto handler = +[]( PathExistsState& s, const std::vector<const GmodNode*>& parents,
							const GmodNode& node ) -> TraversalHandlerResult {
			if ( node.code() != s.targetNode.code() )
			{
				return TraversalHandlerResult::Continue;
			}

			std::vector<const GmodNode*> completePath;
			completePath.reserve( parents.size() );

			for ( const GmodNode* parent : parents )
			{
				if ( !parent->isRoot() )
				{
					completePath.push_back( parent );
				}
			}

			size_t startIndex = 0;

			if ( s.assetFunctionIndex != SIZE_MAX )
			{
				startIndex = s.assetFunctionIndex;
			}

			size_t requiredNodes = s.fromPath.size() - startIndex;
			if ( completePath.size() < requiredNodes )
			{
				return TraversalHandlerResult::Continue;
			}

			bool match = true;
			for ( size_t i = 0; i < requiredNodes; ++i )
			{
				size_t fromPathIdx = startIndex + i;
				if ( fromPathIdx >= s.fromPath.size() || i >= completePath.size() )
				{
					match = false;
					break;
				}
				if ( completePath[i]->code() != s.fromPath[fromPathIdx]->code() )
				{
					match = false;
					break;
				}
			}

			if ( match )
			{
				for ( size_t i = requiredNodes; i < completePath.size(); ++i )
				{
					s.remainingParents.push_back( completePath[i] );
				}
				s.found = true;
				return TraversalHandlerResult::Stop;
			}

			return TraversalHandlerResult::Continue;
		};

		traverse( state, startNode, handler );

		return state.found;
	}
}
