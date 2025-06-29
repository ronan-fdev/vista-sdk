#include "pch.h"

#include "dnv/vista/sdk/Config.h"
#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/GmodTraversal.h"
#include "dnv/vista/sdk/utils/StringUtils.h"

namespace dnv::vista::sdk
{
	namespace GmodTraversal
	{
		//=====================================================================
		// Public API
		//=====================================================================

		bool traverse( const Gmod& gmodInstance, TraverseHandler handler, const TraversalOptions& options )
		{
			TraverseHandler capturedHandler = handler;
			TraverseHandlerWithState<TraverseHandler> wrapperHandler =
				[]( TraverseHandler& h, const std::vector<GmodNode>& parents, const GmodNode& node )
				-> TraversalHandlerResult {
				return h( parents, node );
			};

			return traverse( capturedHandler, gmodInstance, wrapperHandler, options );
		}

		bool traverse( const GmodNode& rootNode, TraverseHandler handler, const TraversalOptions& options )
		{
			TraverseHandler capturedHandler = handler;
			TraverseHandlerWithState<TraverseHandler> wrapperHandler =
				[]( TraverseHandler& h, const std::vector<GmodNode>& parents, const GmodNode& node )
				-> TraversalHandlerResult {
				return h( parents, node );
			};

			return traverse( capturedHandler, rootNode, wrapperHandler, options );
		}

		bool pathExistsBetween(
			const Gmod& gmodInstance,
			const std::vector<GmodNode>& fromPath,
			const GmodNode& to,
			std::vector<GmodNode>& remainingParents )
		{
			remainingParents.clear();

			auto it = std::find_if( fromPath.rbegin(), fromPath.rend(), []( const GmodNode& n ) {
				return n.isAssetFunctionNode();
			} );

			const GmodNode* lastAssetFunction = ( it != fromPath.rend() ) ? &( *it ) : nullptr;

			detail::PathExistsContext context( to, fromPath );

			TraverseHandlerWithState<detail::PathExistsContext> handler =
				[]( detail::PathExistsContext& state, const std::vector<GmodNode>& parents, const GmodNode& node ) -> TraversalHandlerResult {
				if ( !equals( node.code(), state.to.code() ) )
				{
					return TraversalHandlerResult::Continue;
				}

				std::vector<GmodNode> actualParents;
				const std::vector<GmodNode>* parentsToUse = &parents;

				if ( !parents.empty() && !parents[0].isRoot() )
				{
					actualParents = parents;
					parentsToUse = &actualParents;

					while ( !( *parentsToUse )[0].isRoot() )
					{
						auto parent = ( *parentsToUse )[0];
						auto parentOfCurrent = parent.parents();
						if ( parentOfCurrent.size() != 1 )
						{
							throw std::runtime_error( "Invalid state - expected one parent" );
						}
						actualParents.insert( actualParents.begin(), parentOfCurrent[0] );
					}
				}

				if ( parentsToUse->size() < state.fromPath.size() )
				{
					return TraversalHandlerResult::Continue;
				}

				bool match = true;
				for ( size_t i = 0; i < state.fromPath.size(); i++ )
				{
					if ( !equals( ( *parentsToUse )[i].code(), state.fromPath[i].code() ) )
					{
						match = false;
						break;
					}
				}

				if ( match )
				{
					state.remainingParents.clear();

					for ( const auto& parent : *parentsToUse )
					{
						bool existsInFromPath = false;
						for ( const auto& fromPathNode : state.fromPath )
						{
							if ( equals( parent.code(), fromPathNode.code() ) )
							{
								existsInFromPath = true;
								break;
							}
						}

						if ( !existsInFromPath )
						{
							state.remainingParents.push_back( parent );
						}
					}

					return TraversalHandlerResult::Stop;
				}

				return TraversalHandlerResult::Continue;
			};

			bool reachedEnd = traverse( context, lastAssetFunction ? *lastAssetFunction : gmodInstance.rootNode(), handler );

			remainingParents = std::move( context.remainingParents );

			return !reachedEnd;
		}
	}
}
