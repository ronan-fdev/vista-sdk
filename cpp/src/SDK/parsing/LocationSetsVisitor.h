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
 * @file LocationSetsVisitor.h
 * @brief LocationSetsVisitor for GMOD path individualization processing
 * @details Visitor pattern implementation for traversing GMOD paths and managing individualization sets during version conversion
 */

#pragma once

#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodNode.h"
#include "dnv/vista/sdk/Locations.h"

#include <limits>
#include <optional>
#include <stdexcept>
#include <tuple>

namespace dnv::vista::sdk::internal
{
	/**
	 * @brief Visitor for traversing GMOD paths and managing individualization sets during version conversion.
	 *
	 * @details
	 * The LocationSetsVisitor class implements the visitor pattern for traversing nodes in a GMOD path,
	 * identifying and managing sets of nodes that can be individualized (assigned unique locations) during
	 * version conversion or path processing. It provides logic to detect contiguous and valid individualizable
	 * node sets, handles error conditions for invalid groupings, and returns tuple information for each set found.
	 * This class is used internally by the VISTA SDK for advanced path and location management workflows.
	 */
	class LocationSetsVisitor
	{
	public:
		LocationSetsVisitor()
			: m_currentParentStart{ std::numeric_limits<size_t>().max() }
		{
		}

		std::optional<std::tuple<size_t, size_t, std::optional<Location>>> visit(
			const GmodNode& node, size_t i, const std::vector<GmodNode>& pathParents, const GmodNode& pathTargetNode )
		{
			bool isParent = Gmod::isPotentialParent( node.metadata().type() );
			bool isTargetNode = ( static_cast<size_t>( i ) == pathParents.size() );

			if ( m_currentParentStart == std::numeric_limits<size_t>().max() )
			{
				if ( isParent )
				{
					m_currentParentStart = i;
				}
				if ( node.isIndividualizable( isTargetNode ) )
				{
					return std::make_tuple( i, i, node.location() );
				}
			}
			else
			{
				if ( isParent || isTargetNode )
				{
					std::optional<std::tuple<size_t, size_t, std::optional<Location>>> nodes = std::nullopt;

					if ( m_currentParentStart + 1 == i )
					{
						if ( node.isIndividualizable( isTargetNode ) )
							nodes = std::make_tuple( i, i, node.location() );
					}
					else
					{
						size_t skippedOne = std::numeric_limits<size_t>().max();
						bool hasComposition = false;

						for ( size_t j = m_currentParentStart + 1; j <= i; ++j )
						{
							const GmodNode& setNode = ( j < pathParents.size() )
														  ? pathParents[j]
														  : pathTargetNode;

							if ( !setNode.isIndividualizable( j == pathParents.size(), true ) )
							{
								if ( nodes.has_value() )
								{
									skippedOne = j;
								}

								continue;
							}

							if ( nodes.has_value() &&
								 std::get<2>( nodes.value() ).has_value() &&
								 setNode.location().has_value() &&
								 std::get<2>( nodes.value() ) != setNode.location() )
							{
								throw std::runtime_error{ "Mapping error: different locations in the same nodeset" };
							}

							if ( skippedOne != std::numeric_limits<size_t>().max() )
							{
								throw std::runtime_error{ "Can't skip in the middle of individualizable set" };
							}

							if ( setNode.isFunctionComposition() )
							{
								hasComposition = true;
							}

							auto location =
								nodes.has_value() && std::get<2>( nodes.value() ).has_value() ? std::get<2>( nodes.value() )
																							  : setNode.location();
							size_t start = nodes.has_value() ? std::get<0>( nodes.value() )
															 : j;
							size_t end = j;
							nodes = std::make_tuple( start, end, location );
						}

						if ( nodes.has_value() && std::get<0>( nodes.value() ) == std::get<1>( nodes.value() ) && hasComposition )
						{
							nodes = std::nullopt;
						}
					}

					m_currentParentStart = i;
					if ( nodes.has_value() )
					{
						bool hasLeafNode = false;
						size_t startIdx = std::get<0>( nodes.value() );
						size_t endIdx = std::get<1>( nodes.value() );

						for ( size_t j = startIdx; j <= endIdx; ++j )
						{
							const GmodNode& setNode = ( j < pathParents.size() )
														  ? pathParents[j]
														  : pathTargetNode;

							if ( setNode.isLeafNode() || j == pathParents.size() )
							{
								hasLeafNode = true;

								break;
							}
						}
						if ( hasLeafNode )
						{
							return nodes;
						}
					}
				}

				if ( isTargetNode && node.isIndividualizable( isTargetNode ) )
				{
					return std::make_tuple( i, i, node.location() );
				}
			}

			return std::nullopt;
		}

	private:
		size_t m_currentParentStart;
	};
} // namespace dnv::vista::sdk::internal
