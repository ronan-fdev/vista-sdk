/**
 * @file LocationSetsVisitor.h
 * @brief LocationSetsVisitor for GMOD path individualization processing
 * @details Visitor pattern implementation for traversing GMOD paths and managing individualization sets during version conversion
 */

#pragma once

namespace dnv::vista::sdk
{
	namespace internal
	{
		struct LocationSetsVisitor
		{
			size_t currentParentStart;

			LocationSetsVisitor() : currentParentStart{ std::numeric_limits<size_t>().max() } {}

			std::optional<std::tuple<size_t, size_t, std::optional<Location>>> visit(
				const GmodNode& node, size_t i, const std::vector<GmodNode*>& pathParents, const GmodNode& pathTargetNode )
			{
				bool isParent = Gmod::isPotentialParent( node.metadata().type() );
				bool isTargetNode = ( static_cast<size_t>( i ) == pathParents.size() );

				if ( currentParentStart == std::numeric_limits<size_t>().max() )
				{
					if ( isParent )
					{
						currentParentStart = i;
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

						if ( currentParentStart + 1 == i )
						{
							if ( node.isIndividualizable( isTargetNode ) )
								nodes = std::make_tuple( i, i, node.location() );
						}
						else
						{
							size_t skippedOne = std::numeric_limits<size_t>().max();
							bool hasComposition = false;

							for ( size_t j = currentParentStart + 1; j <= i; ++j )
							{
								const GmodNode* setNode = ( j < pathParents.size() )
															  ? pathParents[j]
															  : &pathTargetNode;

								if ( !setNode->isIndividualizable( j == pathParents.size(), true ) )
								{
									if ( nodes.has_value() )
									{
										skippedOne = j;
									}

									continue;
								}

								if ( nodes.has_value() && std::get<2>( nodes.value() ).has_value() && setNode->location().has_value() &&
									 std::get<2>( nodes.value() ) != setNode->location() )
								{
									throw std::runtime_error( "Mapping error: different locations in the same nodeset" );
								}

								if ( skippedOne != std::numeric_limits<size_t>().max() )
								{
									throw std::runtime_error( "Can't skip in the middle of individualizable set" );
								}

								if ( setNode->isFunctionComposition() )
								{
									hasComposition = true;
								}

								auto location =
									nodes.has_value() && std::get<2>( nodes.value() ).has_value()
										? std::get<2>( nodes.value() )
										: setNode->location();
								size_t start = nodes.has_value()
												   ? std::get<0>( nodes.value() )
												   : j;
								size_t end = j;
								nodes = std::make_tuple( start, end, location );
							}

							if ( nodes.has_value() && std::get<0>( nodes.value() ) == std::get<1>( nodes.value() ) && hasComposition )
							{
								nodes = std::nullopt;
							}
						}

						currentParentStart = i;
						if ( nodes.has_value() )
						{
							bool hasLeafNode = false;
							size_t startIdx = std::get<0>( nodes.value() );
							size_t endIdx = std::get<1>( nodes.value() );

							for ( size_t j = startIdx; j <= endIdx; ++j )
							{
								const GmodNode* setNode = ( j < pathParents.size() )
															  ? pathParents[j]
															  : &pathTargetNode;
								if ( setNode->isLeafNode() || j == pathParents.size() )
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
		};
	}
}
