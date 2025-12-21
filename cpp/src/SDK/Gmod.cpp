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
 * @file Gmod.cpp
 * @brief Implementation of Gmod class methods
 * @details Provides constructors and static helper methods for Gmod tree management
 */

#include "dnv/vista/sdk/Gmod.h"

#include "dto/GmodDto.h"

#include <stdexcept>

namespace dnv::vista::sdk
{
	Gmod::Gmod( VisVersion version, const GmodDto& dto )
		: m_visVersion{ version },
		  m_rootNode{ nullptr }
	{
		const auto& items = dto.items;
		std::vector<std::pair<std::string, GmodNode>> nodePairs;
		nodePairs.reserve( items.size() );

		for ( const auto& nodeDto : items )
		{
			nodePairs.emplace_back( std::string{ nodeDto.code }, GmodNode{ version, nodeDto } );
		}

		m_nodeMap = nfx::containers::PerfectHashMap<std::string, GmodNode, uint32_t>( std::move( nodePairs ) );

		for ( const auto& relation : dto.relations )
		{
			/*
			 * Each relation defines a parent-child relationship and must contain at least 2 elements:
			 * relation[0] = parent node code (e.g., "VE", "400", "410")
			 * relation[1] = child node code (e.g., "400", "410", "411")
			 * We need both parent and child codes to establish the bidirectional relationship.
			 * Relations with < 2 elements are malformed and would cause array access errors.
			 */
			const auto* parentNodePtr = m_nodeMap.find( relation[0] );
			if ( !parentNodePtr )
			{
				throw std::runtime_error( "Parent node not found in GMOD" );
			}
			const auto* childNodePtr = m_nodeMap.find( relation[1] );
			if ( !childNodePtr )
			{
				throw std::runtime_error( "Child node not found in GMOD" );
			}
			auto& parentNode = const_cast<GmodNode&>( *parentNodePtr );
			auto& childNode = const_cast<GmodNode&>( *childNodePtr );
			parentNode.addChild( &childNode );
			childNode.addParent( &parentNode );
		}

		for ( auto& [key, node] : m_nodeMap )
		{
			const_cast<GmodNode&>( node ).trim();
		}

		const auto* rootPtr = m_nodeMap.find( "VE" );
		if ( !rootPtr )
		{
			throw std::runtime_error( "Root node 'VE' not found in GMOD" );
		}
		m_rootNode = const_cast<GmodNode*>( rootPtr );
	}

	Gmod::Gmod( VisVersion version, const std::unordered_map<std::string, GmodNode, StringHash, std::equal_to<>>& nodeMap )
		: m_visVersion{ version }
	{
		std::vector<std::pair<std::string, GmodNode>> pairs;
		pairs.reserve( nodeMap.size() );
		for ( const auto& [code, node] : nodeMap )
		{
			pairs.emplace_back( code, node );
		}
		m_nodeMap = nfx::containers::PerfectHashMap<std::string, GmodNode, uint32_t>( std::move( pairs ) );

		for ( auto& [key, node] : m_nodeMap )
		{
			const_cast<GmodNode&>( node ).trim();
		}

		const auto* rootPtr = m_nodeMap.find( "VE" );
		if ( !rootPtr )
		{
			throw std::runtime_error( "Root node 'VE' not found in GMOD" );
		}
		m_rootNode = const_cast<GmodNode*>( rootPtr );
	}

	bool Gmod::isPotentialParent( std::string_view type ) noexcept
	{
		return type == "SELECTION" || type == "GROUP" || type == "LEAF";
	}

	bool Gmod::isProductTypeAssignment( const GmodNode* parent, const GmodNode* child ) noexcept
	{
		if ( !parent || !child )
		{
			return false;
		}
		if ( !parent->isFunctionNode() )
		{
			return false;
		}
		return child->isProductType();
	}

	bool Gmod::isProductSelectionAssignment( const GmodNode* parent, const GmodNode* child ) noexcept
	{
		if ( !parent || !child )
		{
			return false;
		}
		if ( !parent->isFunctionNode() )
		{
			return false;
		}
		return child->isProductSelection();
	}

	bool Gmod::traverse( TraverseHandler handler, TraversalOptions options ) const
	{
		return traverse( *m_rootNode, handler, options );
	}

	bool Gmod::traverse( const GmodNode& rootNode, TraverseHandler handler, TraversalOptions options ) const
	{
		struct DummyState
		{
		};

		DummyState state;

		TraverseHandlerWithState<DummyState> wrappedHandler =
			[handler]( DummyState&, const std::vector<const GmodNode*>& parents, const GmodNode& node )
			-> TraversalHandlerResult {
			return handler( parents, node );
		};

		return traverse( state, rootNode, wrappedHandler, options );
	}
} // namespace dnv::vista::sdk
