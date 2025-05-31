#include "pch.h"
#include "dnv/vista/sdk/GmodTraversal.h"
#include "dnv/vista/sdk/Gmod.h"

namespace dnv::vista::sdk
{
	namespace GmodTraversal
	{
		namespace detail
		{
			Parents::Parents()
			{
				m_parents.reserve( 64 );
			}

			void Parents::push( const GmodNode* parent )
			{
				m_parents.push_back( parent );
				m_occurrences[parent->code()]++;
			}

			void Parents::pop()
			{
				if ( m_parents.empty() )
					return;

				const GmodNode* parent = m_parents.back();
				auto it = m_occurrences.find( parent->code() );
				if ( it != m_occurrences.end() )
				{
					it->second--;
					if ( it->second == 0 )
					{
						m_occurrences.erase( it );
					}
				}
				m_parents.pop_back();
			}

			int Parents::occurrences( const GmodNode& node ) const
			{
				auto it = m_occurrences.find( node.code() );
				return ( it != m_occurrences.end() ) ? it->second : 0;
			}

			const GmodNode* Parents::lastOrDefault() const
			{
				return m_parents.empty() ? nullptr : m_parents.back();
			}

			const std::vector<const GmodNode*>& Parents::asList() const
			{
				return m_parents;
			}
		}

		bool traverse( const Gmod& gmodInstance, TraverseHandler handler, const TraversalOptions& options )
		{
			bool dummyState = false;
			TraverseHandlerWithState<bool> wrappedHandler =
				[handler]( [[maybe_unused]] bool& s, const std::vector<const GmodNode*>& parents_list, const GmodNode& nodeRef ) {
					return handler( parents_list, nodeRef );
				};
			return GmodTraversal::traverse<bool>( dummyState, gmodInstance.rootNode(), wrappedHandler, options );
		}

		bool traverse( const GmodNode& rootNode, TraverseHandler handler, const TraversalOptions& options )
		{
			bool dummyState = false;
			TraverseHandlerWithState<bool> wrappedHandler =
				[handler]( [[maybe_unused]] bool& s, const std::vector<const GmodNode*>& parents_list, const GmodNode& nodeRef ) {
					return handler( parents_list, nodeRef );
				};
			return GmodTraversal::traverse<bool>( dummyState, rootNode, wrappedHandler, options );
		}

		namespace
		{
			struct PathExistsContext
			{
				const GmodNode& toNode;
				std::vector<const GmodNode*> remainingParents_list;
				const std::vector<const GmodNode*>& fromPathList;

				PathExistsContext( const GmodNode& to, const std::vector<const GmodNode*>& fromPath )
					: toNode{ to },
					  fromPathList{ fromPath }
				{
				}

				PathExistsContext( const PathExistsContext& ) = delete;
				PathExistsContext( PathExistsContext&& ) = delete;
				PathExistsContext& operator=( const PathExistsContext& ) = delete;
				PathExistsContext& operator=( PathExistsContext&& ) = delete;
			};
		}

		bool pathExistsBetween(
			const Gmod& gmodInstance,
			const std::vector<const GmodNode*>& fromPath,
			const GmodNode& to,
			std::vector<const GmodNode*>& remainingParents )
		{
			// Clear output parameter - mirror C# behavior
			remainingParents.clear();

			// Find lastAssetFunction - EXACT C# MIRROR
			const GmodNode* lastAssetFunction = nullptr;
			for ( auto it = fromPath.rbegin(); it != fromPath.rend(); ++it )
			{
				if ( ( *it )->isAssetFunctionNode() )
				{
					lastAssetFunction = *it;
					break;
				}
			}

			// Create PathExistsContext - MIRROR C# state
			struct PathExistsContext
			{
				const GmodNode& to;
				std::vector<const GmodNode*> remainingParents_result;
				std::vector<const GmodNode*> fromPath_copy;

				PathExistsContext( const GmodNode& toNode, const std::vector<const GmodNode*>& fromPathInput )
					: to( toNode ), fromPath_copy( fromPathInput ) {}
			};

			PathExistsContext context( to, fromPath );

			// EXACT C# LAMBDA MIRROR - NO O(n²) DISASTERS!
			TraverseHandlerWithState<PathExistsContext> handler =
				[]( PathExistsContext& state, const std::vector<const GmodNode*>& parents, const GmodNode& node ) -> TraversalHandlerResult {
				// EXACT C# CHECK: if (node.Code != state.To.Code)
				if ( node.code() != state.to.code() )
					return TraversalHandlerResult::Continue;

				// MIRROR C# LOGIC: List<GmodNode>? actualParents = null;
				std::vector<const GmodNode*> actualParents;
				bool needsActualParents = false;

				// Create working copy of parents for modification
				std::vector<const GmodNode*> workingParents = parents;

				// EXACT C# WHILE LOOP: while (!parents[0].IsRoot)
				while ( !workingParents.empty() && !workingParents[0]->isRoot() )
				{
					// MIRROR C#: if (actualParents is null)
					if ( !needsActualParents )
					{
						// MIRROR C#: actualParents = [.. parents];
						actualParents = workingParents;
						workingParents = actualParents; // Work on the copy
						needsActualParents = true;
					}

					// MIRROR C#: var parent = parents[0];
					const GmodNode* parent = workingParents[0];

					// MIRROR C#: if (parent.Parents.Count != 1)
					if ( parent->parents().size() != 1 )
						throw std::runtime_error( "Invalid state - expected one parent" );

					// MIRROR C#: actualParents.Insert(0, parent.Parents[0]);
					const GmodNode* grandParent = parent->parents()[0];
					workingParents.insert( workingParents.begin(), grandParent );
				}

				// EXACT C# VALIDATION: if (parents.Count < state.FromPath.Count)
				if ( workingParents.size() < state.fromPath_copy.size() )
					return TraversalHandlerResult::Continue;

				// EXACT C# MATCH LOGIC: Must have same start order
				bool match = true;
				for ( size_t i = 0; i < state.fromPath_copy.size(); i++ )
				{
					// MIRROR C#: if (parents[i].Code != state.FromPath[i].Code)
					if ( workingParents[i]->code() != state.fromPath_copy[i]->code() )
					{
						match = false;
						break;
					}
				}

				if ( match )
				{
					// MIRROR C#: state.RemainingParents = parents.Where(p => !state.FromPath.Any(pp => pp.Code == p.Code)).ToArray();
					state.remainingParents_result.clear();

					for ( size_t i = 0; i < workingParents.size(); i++ )
					{
						const GmodNode* parent = workingParents[i];

						// Check if this parent is NOT in fromPath
						bool foundInFromPath = false;
						for ( const GmodNode* fromPathNode : state.fromPath_copy )
						{
							if ( parent->code() == fromPathNode->code() )
							{
								foundInFromPath = true;
								break;
							}
						}

						if ( !foundInFromPath )
						{
							state.remainingParents_result.push_back( parent );
						}
					}

					return TraversalHandlerResult::Stop;
				}

				return TraversalHandlerResult::Continue;
			};

			// EXACT C# TRAVERSE CALL MIRROR

			// FIXED traverse call (Lines 215-222):

			const GmodNode* startNode = lastAssetFunction ? lastAssetFunction : &gmodInstance.rootNode();

			bool reachedEnd = GmodTraversal::traverse<PathExistsContext>(
				context,	// ← Argument 1: PathExistsContext& state
				*startNode, // ← Argument 2: const GmodNode& rootNode
				handler );	// ← Argument 3: TraverseHandlerWithState<PathExistsContext>
			// ← Argument 4 (optional): TraversalOptions defaults to {}

			// MIRROR C#: remainingParents = state.RemainingParents;
			remainingParents = std::move( context.remainingParents_result );

			// EXACT C# RETURN: return !reachedEnd;
			return !reachedEnd;
		}
	}
}
