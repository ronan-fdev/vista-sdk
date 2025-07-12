/**
 * @file GmodTraversal.inl
 * @brief Inline implementations for performance-critical GmodTraversal operations
 */

namespace dnv::vista::sdk
{
	namespace GmodTraversal
	{
		namespace detail
		{
			inline Parents::Parents( size_t maxOccurrence )
				: m_maxTraversalOccurrence{ maxOccurrence }
			{
				m_parents.reserve( 64 );

				if ( m_maxTraversalOccurrence > 1 )
				{
					m_occurrences.reserve( 8 );
				}
			}

			inline void Parents::push( const GmodNode* parent )
			{
				m_parents.emplace_back( parent );

				std::string_view key = parent->code();
				if ( auto it = m_occurrences.find( key ); it != m_occurrences.end() )
				{
					++it->second;
				}
				else
				{
					m_occurrences[key] = 1;
				}
			}

			inline void Parents::pop()
			{
				if ( m_parents.empty() )
				{
					return;
				}

				const GmodNode* parent = m_parents.back();
				m_parents.pop_back();

				std::string_view key = parent->code();
				if ( auto it = m_occurrences.find( key ); it != m_occurrences.end() )
				{
					if ( it->second == 1 )
					{
						m_occurrences.erase( it );
					}
					else
					{
						--it->second;
					}
				}
			}

			inline size_t Parents::occurrences( const GmodNode& node ) const noexcept
			{
				std::string_view key = node.code();
				if ( auto it = m_occurrences.find( key ); it != m_occurrences.end() )
				{
					return it->second;
				}

				return 0;
			}

			inline const GmodNode* Parents::lastOrDefault() const noexcept
			{
				return m_parents.empty() ? nullptr : m_parents.back();
			}

			inline const std::vector<const GmodNode*>& Parents::asList() const noexcept
			{
				return m_parents;
			}

			template <typename TState>
			inline TraversalHandlerResult traverseNode( TraversalContext<TState>& context, const GmodNode& node )
			{
				if ( node.metadata().installSubstructure().has_value() && !node.metadata().installSubstructure().value() )
				{
					return TraversalHandlerResult::Continue;
				}

				TraversalHandlerResult result = context.handler( context.state, context.parents.asList(), node );
				if ( result == TraversalHandlerResult::Stop || result == TraversalHandlerResult::SkipSubtree )
				{
					return result;
				}

				bool skipOccurrenceCheck = Gmod::isProductSelectionAssignment( context.parents.lastOrDefault(), &node );
				if ( !skipOccurrenceCheck )
				{
					size_t occ = context.parents.occurrences( node );

					if ( occ == context.maxTraversalOccurrence )
					{
						return TraversalHandlerResult::SkipSubtree;
					}

					if ( occ > context.maxTraversalOccurrence )
					{
						throw std::runtime_error( "Invalid state - node occurred more than expected" );
					}
				}

				context.parents.push( &node );

				const auto& children = node.children();
				for ( const GmodNode* child : children )
				{
					result = traverseNode( context, *child );
					if ( result == TraversalHandlerResult::Stop )
					{
						context.parents.pop();
						return TraversalHandlerResult::Stop;
					}
					else if ( result == TraversalHandlerResult::SkipSubtree )
					{
						continue;
					}
				}

				context.parents.pop();

				return TraversalHandlerResult::Continue;
			}
		}

		template <typename TState>
		inline bool traverse( TState& state, const Gmod& gmodInstance, TraverseHandlerWithState<TState> handler, const TraversalOptions& options )
		{
			detail::Parents parentsStack( options.maxTraversalOccurrence );
			detail::TraversalContext<TState> context( parentsStack, handler, state, options.maxTraversalOccurrence );

			return detail::traverseNode( context, gmodInstance.rootNode() ) == TraversalHandlerResult::Continue;
		}

		template <typename TState>
		inline bool traverse( TState& state, const GmodNode& rootNode, TraverseHandlerWithState<TState> handler, const TraversalOptions& options )
		{
			detail::Parents parentsStack( options.maxTraversalOccurrence );
			detail::TraversalContext<TState> context( parentsStack, handler, state, options.maxTraversalOccurrence );

			return detail::traverseNode( context, rootNode ) == TraversalHandlerResult::Continue;
		}
	}
}
