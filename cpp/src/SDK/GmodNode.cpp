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
 * @file GmodNode.cpp
 * @brief Implementation of GmodNodeMetadata and GmodNode
 */

#include "dnv/vista/sdk/GmodNode.h"

#include "dto/GmodDto.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
	GmodNodeMetadata::GmodNodeMetadata(
		std::string_view category,
		std::string_view type,
		std::string_view name,
		std::optional<std::string> commonName,
		std::optional<std::string> definition,
		std::optional<std::string> commonDefinition,
		std::optional<bool> installSubstructure,
		std::unordered_map<std::string, std::string, StringHash, std::equal_to<>> normalAssignmentNames ) noexcept
		: m_category{ category },
		  m_type{ type },
		  m_name{ name },
		  m_commonName{ commonName },
		  m_definition{ definition },
		  m_commonDefinition{ commonDefinition },
		  m_installSubstructure{ installSubstructure },
		  m_normalAssignmentNames{ normalAssignmentNames },
		  m_fullType{ std::string{ category } + " " + std::string{ type } }
	{
	}

	GmodNode::GmodNode( VisVersion version, const GmodNodeDto& dto ) noexcept
		: m_visVersion{ version },
		  m_code{ dto.code },
		  m_location{ std::nullopt },
		  m_metadata{
			  dto.category,
			  dto.type,
			  dto.name.value_or( "" ),
			  dto.commonName,
			  dto.definition,
			  dto.commonDefinition,
			  dto.installSubstructure,
			  dto.normalAssignmentNames ? std::unordered_map<std::string, std::string, StringHash, std::equal_to<>>{ dto.normalAssignmentNames->begin(), dto.normalAssignmentNames->end() }
										: std::unordered_map<std::string, std::string, StringHash, std::equal_to<>>{} },
		  m_children{},
		  m_parents{},
		  m_childrenSet{ nullptr }
	{
	}

	std::optional<const GmodNode*> GmodNode::productType() const noexcept
	{
		if ( m_children.size() != 1 )
		{
			return std::nullopt;
		}
		if ( m_metadata.category().find( "FUNCTION" ) == std::string_view::npos )
		{
			return std::nullopt;
		}
		const GmodNode* child = m_children[0];
		if ( !child )
		{
			return std::nullopt;
		}
		if ( child->m_metadata.category() != "PRODUCT" )
		{
			return std::nullopt;
		}
		if ( child->m_metadata.type() != "TYPE" )
		{
			return std::nullopt;
		}

		return child;
	}

	std::optional<const GmodNode*> GmodNode::productSelection() const noexcept
	{
		if ( m_children.size() != 1 )
		{
			return std::nullopt;
		}
		if ( m_metadata.category().find( "FUNCTION" ) == std::string_view::npos )
		{
			return std::nullopt;
		}
		const GmodNode* child = m_children[0];
		if ( !child )
		{
			return std::nullopt;
		}
		if ( child->m_metadata.category().find( "PRODUCT" ) == std::string_view::npos )
		{
			return std::nullopt;
		}
		if ( child->m_metadata.type() != "SELECTION" )
		{
			return std::nullopt;
		}

		return child;
	}

	bool GmodNode::isIndividualizable( bool isTargetNode, bool isInSet ) const noexcept
	{
		if ( m_metadata.type() == "GROUP" )
		{
			return false;
		}
		if ( m_metadata.type() == "SELECTION" )
		{
			return false;
		}
		if ( isProductType() )
		{
			return false;
		}
		if ( m_metadata.category() == "ASSET" && m_metadata.type() == "TYPE" )
		{
			return false;
		}
		if ( isFunctionComposition() )
		{
			if ( m_code.empty() )
			{
				return false;
			}

			return m_code.back() == 'i' || isInSet || isTargetNode;
		}

		return true;
	}

	bool GmodNode::isFunctionComposition() const noexcept
	{
		return ( m_metadata.category() == "ASSET FUNCTION" || m_metadata.category() == "PRODUCT FUNCTION" ) &&
			   m_metadata.type() == "COMPOSITION";
	}

	bool GmodNode::isMappable() const noexcept
	{
		auto productTypeOpt = productType();
		if ( productTypeOpt.has_value() )
		{
			return false;
		}
		auto productSelectionOpt = productSelection();
		if ( productSelectionOpt.has_value() )
		{
			return false;
		}
		if ( m_metadata.category().find( "PRODUCT" ) != std::string_view::npos &&
			 m_metadata.type() == "SELECTION" )
		{
			return false;
		}
		if ( m_metadata.category() == "ASSET" )
		{
			return false;
		}
		if ( m_code.empty() )
		{
			return false;
		}

		char lastChar = m_code.back();
		return lastChar != 'a' && lastChar != 's';
	}

	bool GmodNode::isProductSelection() const
	{
		return m_metadata.category() == "PRODUCT" && m_metadata.type() == "SELECTION";
	}

	bool GmodNode::isProductType() const
	{
		return m_metadata.category() == "PRODUCT" && m_metadata.type() == "TYPE";
	}

	bool GmodNode::isAsset() const
	{
		return m_metadata.category() == "ASSET";
	}

	bool GmodNode::isLeafNode() const
	{
		return m_metadata.fullType() == "ASSET FUNCTION LEAF" || m_metadata.fullType() == "PRODUCT FUNCTION LEAF";
	}

	bool GmodNode::isFunctionNode() const
	{
		return m_metadata.category() != "PRODUCT" && m_metadata.category() != "ASSET";
	}

	bool GmodNode::isAssetFunctionNode() const
	{
		return m_metadata.category() == "ASSET FUNCTION";
	}

	std::string GmodNode::toString() const
	{
		if ( !m_location.has_value() )
		{
			return m_code;
		}
		return m_code + "-" + m_location->value();
	}

	void GmodNode::toString( StringBuilder& builder ) const noexcept
	{
		builder << m_code;

		if ( m_location.has_value() )
		{
			builder << '-' << m_location->value();
		}
	}

	GmodNode GmodNode::withLocation( const Location& location ) const
	{
		GmodNode result = *this;
		result.m_location = location;

		return result;
	}

	std::optional<GmodNode> GmodNode::withLocation( std::string_view locationStr ) const
	{
		const Locations& locations = VIS::instance().locations( m_visVersion );
		auto locationOpt = locations.fromString( locationStr );

		if ( !locationOpt.has_value() )
		{
			return std::nullopt;
		}

		return withLocation( locationOpt.value() );
	}

	std::optional<GmodNode> GmodNode::withLocation( std::string_view locationStr, ParsingErrors& errors ) const
	{
		const Locations& locations = VIS::instance().locations( m_visVersion );

		auto locationOpt = locations.fromString( locationStr, errors );
		if ( !locationOpt.has_value() )
		{
			return std::nullopt;
		}

		return withLocation( locationOpt.value() );
	}

	GmodNode GmodNode::withoutLocation() const
	{
		if ( !m_location.has_value() )
		{
			return *this;
		}

		GmodNode result = *this;
		result.m_location = std::nullopt;

		return result;
	}

	void GmodNode::addChild( GmodNode* child ) noexcept
	{
		if ( !child )
		{
			return;
		}

		m_children.push_back( child );
		if ( m_childrenSet )
		{
			m_childrenSet->emplace( child->code() );
		}
	}

	void GmodNode::addParent( GmodNode* parent ) noexcept
	{
		if ( !parent )
		{
			return;
		}

		m_parents.push_back( parent );
	}

	void GmodNode::trim() noexcept
	{
		m_children.shrink_to_fit();
		m_parents.shrink_to_fit();

		if ( !m_childrenSet )
		{
			m_childrenSet = std::make_shared<std::unordered_set<std::string, StringHash, std::equal_to<>>>();
		}

		m_childrenSet->clear();
		m_childrenSet->reserve( m_children.size() );

		for ( const auto* child : m_children )
		{
			if ( child )
			{
				m_childrenSet->emplace( child->code() );
			}
		}
	}
} // namespace dnv::vista::sdk
