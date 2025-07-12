#include "pch.h"

#include "dnv/vista/sdk/Config.h"
#include "dnv/vista/sdk/GmodTraversal.h"
#include "dnv/vista/sdk/utils/StringUtils.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Traversal algorithms
	//=====================================================================

	namespace GmodTraversal
	{
		namespace detail
		{
			//----------------------------------------------
			// Path analysis context
			//----------------------------------------------

			struct PathExistsContext
			{
				const GmodNode& to;
				std::vector<const GmodNode*> remainingParents;
				std::vector<const GmodNode*> fromPath;

				PathExistsContext( const GmodNode& toNode, const std::vector<const GmodNode*>& fromPathList )
					: to{ toNode }, fromPath{ fromPathList }
				{
					remainingParents.reserve( 32 );
				}

				PathExistsContext() = delete;
				PathExistsContext( const PathExistsContext& ) = delete;
				PathExistsContext( PathExistsContext&& ) noexcept = delete;
				~PathExistsContext() = default;
				PathExistsContext& operator=( const PathExistsContext& ) = delete;
				PathExistsContext& operator=( PathExistsContext&& ) noexcept = delete;
			};
		}

		//=====================================================================
		// Public API
		//=====================================================================

		bool traverse( const Gmod& gmodInstance, TraverseHandler handler, const TraversalOptions& options )
		{
			TraverseHandler capturedHandler = handler;
			TraverseHandlerWithState<TraverseHandler> wrapperHandler =
				[]( TraverseHandler& h, const std::vector<const GmodNode*>& parents, const GmodNode& node )
				-> TraversalHandlerResult { return h( parents, node ); };

			return traverse( capturedHandler, gmodInstance, wrapperHandler, options );
		}

		bool traverse( const GmodNode& rootNode, TraverseHandler handler, const TraversalOptions& options )
		{
			TraverseHandler capturedHandler = handler;
			TraverseHandlerWithState<TraverseHandler> wrapperHandler =
				[]( TraverseHandler& h, const std::vector<const GmodNode*>& parents, const GmodNode& node )
				-> TraversalHandlerResult { return h( parents, node ); };

			return traverse( capturedHandler, rootNode, wrapperHandler, options );
		}
		bool pathExistsBetween(
			const Gmod& gmodInstance,
			const std::vector<const GmodNode*>& fromPath,
			const GmodNode& to,
			std::vector<const GmodNode*>& remainingParents )
		{
			remainingParents.clear();

			const GmodNode* lastAssetFunction = nullptr;
			for ( auto it = fromPath.rbegin(); it != fromPath.rend(); ++it )
			{
				if ( ( *it )->metadata().category() == GMODNODE_CATEGORY_ASSET_FUNCTION )
				{
					lastAssetFunction = *it;
					break;
				}
			}

			StringSet fromPathCodes;
			fromPathCodes.reserve( fromPath.size() );
			for ( const GmodNode* fromPathNode : fromPath )
			{
				fromPathCodes.emplace( fromPathNode->code() );
			}

			detail::PathExistsContext context( to, fromPath );
			TraverseHandlerWithState<detail::PathExistsContext> handler =
				[]( detail::PathExistsContext& state, const std::vector<const GmodNode*>& parents, const GmodNode& node ) -> TraversalHandlerResult {
				if ( std::string_view( node.code() ) != state.to.code() )
				{
					return TraversalHandlerResult::Continue;
				}

				static thread_local std::vector<const GmodNode*> actualParents;
				static thread_local std::vector<const GmodNode*> tempParents;

				actualParents.clear();
				actualParents.reserve( parents.size() + 16 );

				if ( !parents.empty() && !parents[0]->isRoot() )
				{
					tempParents.clear();
					tempParents.reserve( parents.size() + 16 );

					tempParents.assign( parents.begin(), parents.end() );

					while ( !tempParents.empty() && !tempParents[0]->isRoot() )
					{
						const GmodNode* parent = tempParents[0];
						const auto& parentOfCurrent = parent->parents();
						if ( parentOfCurrent.size() != 1 )
						{
							break;
						}
						tempParents.insert( tempParents.begin(), parentOfCurrent[0] );
					}

					actualParents = std::move( tempParents );
				}
				else
				{
					actualParents.assign( parents.begin(), parents.end() );
				}

				if ( actualParents.size() < state.fromPath.size() )
				{
					return TraversalHandlerResult::Continue;
				}

				bool match = true;
				for ( size_t i = 0; i < state.fromPath.size(); ++i )
				{
					if ( std::string_view( actualParents[i]->code() ) != state.fromPath[i]->code() )
					{
						match = false;
						break;
					}
				}

				if ( match )
				{
					state.remainingParents.clear();
					state.remainingParents.reserve( actualParents.size() );

					StringSet fromPathCodes;
					fromPathCodes.reserve( state.fromPath.size() );
					for ( const GmodNode* fromPathNode : state.fromPath )
					{
						fromPathCodes.emplace( fromPathNode->code() );
					}

					for ( const GmodNode* parent : actualParents )
					{
						if ( fromPathCodes.find( parent->code() ) == fromPathCodes.end() )
						{
							state.remainingParents.push_back( parent );
						}
					}
					return TraversalHandlerResult::Stop;
				}

				return TraversalHandlerResult::Continue;
			};

			bool reachedEnd = traverse(
				context,
				lastAssetFunction ? *lastAssetFunction : gmodInstance.rootNode(),
				handler );

			remainingParents = std::move( context.remainingParents );

			return !reachedEnd;
		}
	}
}
