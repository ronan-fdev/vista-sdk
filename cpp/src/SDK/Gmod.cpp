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
		for ( const auto& nodeDto : dto.items )
		{
			m_nodeMap.emplace( nodeDto.code, GmodNode{ version, nodeDto } );
		}

		for ( const auto& relation : dto.relations )
		{
			if ( relation.size() < 2 )
			{
				continue;
			}

			const auto& parentCode = relation[0];
			const auto& childCode = relation[1];

			auto parentIt = m_nodeMap.find( parentCode );
			auto childIt = m_nodeMap.find( childCode );

			if ( parentIt != m_nodeMap.end() && childIt != m_nodeMap.end() )
			{
				parentIt->second.addChild( &childIt->second );
				childIt->second.addParent( &parentIt->second );
			}
		}

		for ( auto& [code, node] : m_nodeMap )
		{
			node.trim();
		}

		auto rootIt = m_nodeMap.find( "VE" );
		if ( rootIt != m_nodeMap.end() )
		{
			m_rootNode = &rootIt->second;
		}
	}

	Gmod::Gmod( VisVersion version, const std::unordered_map<std::string, GmodNode, StringHash, std::equal_to<>>& nodeMap )
		: m_visVersion{ version },
		  m_nodeMap{ nodeMap }
	{
		for ( auto& [code, node] : m_nodeMap )
		{
			node.trim();
		}

		auto rootIt = m_nodeMap.find( "VE" );
		if ( rootIt != m_nodeMap.end() )
		{
			m_rootNode = &rootIt->second;
		}
		else
		{
			throw std::runtime_error( "Root node 'VE' not found in GMOD" );
		}
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

} // namespace dnv::vista::sdk
