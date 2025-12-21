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
 * @file Gmod.inl
 * @brief Inline implementations for Gmod class
 * @details Provides inline implementations for simple accessor methods.
 */

namespace dnv::vista::sdk
{
	inline const GmodNode& Gmod::operator[]( std::string_view key ) const
	{
		auto* nodePtr = m_nodeMap.find( key );
		if ( nodePtr != nullptr )
		{
			return *nodePtr;
		}

		throw std::out_of_range{ "Key not found in Gmod node map: " + std::string{ key } };
	}

	inline VisVersion Gmod::version() const noexcept
	{
		return m_visVersion;
	}

	inline const GmodNode& Gmod::rootNode() const noexcept
	{
		return *m_rootNode;
	}

	inline std::optional<const GmodNode*> Gmod::node( std::string_view code ) const noexcept
	{
		auto* nodePtr = m_nodeMap.find( code );
		if ( nodePtr != nullptr )
		{
			return nodePtr;
		}
		return std::nullopt;
	}

	inline auto Gmod::begin() const noexcept
	{
		return m_nodeMap.begin();
	}

	inline auto Gmod::end() const noexcept
	{
		return m_nodeMap.end();
	}

	inline auto Gmod::cbegin() const noexcept
	{
		return m_nodeMap.cbegin();
	}

	inline auto Gmod::cend() const noexcept
	{
		return m_nodeMap.cend();
	}

	template <typename TState>
	inline bool Gmod::traverse( TState& state, TraverseHandlerWithState<TState> handler, TraversalOptions options ) const
	{
		return traverse( state, *m_rootNode, handler, options );
	}

	template <typename TState>
	inline bool Gmod::traverse( TState& state, const GmodNode& rootNode, TraverseHandlerWithState<TState> handler, TraversalOptions options ) const
	{
		TraversalParents parents;

		std::function<TraversalHandlerResult( const GmodNode& )> traverseNode;
		traverseNode = [&]( const GmodNode& node ) -> TraversalHandlerResult {
			auto result = handler( state, parents.asList(), node );
			if ( result == TraversalHandlerResult::Stop || result == TraversalHandlerResult::SkipSubtree )
			{
				return result;
			}

			const GmodNode* lastParent = parents.lastOrDefault();
			bool skipOccurrenceCheck = isProductSelectionAssignment( lastParent, &node );

			if ( !skipOccurrenceCheck )
			{
				int occ = parents.occurrences( node );

				if ( occ == options.maxTraversalOccurrence )
				{
					return TraversalHandlerResult::SkipSubtree;
				}
				if ( occ > options.maxTraversalOccurrence )
				{
					throw std::runtime_error( "Invalid state - node occurred more than expected" );
				}
			}

			parents.push( &node );

			for ( const auto* child : node.children() )
			{
				result = traverseNode( *child );
				if ( result == TraversalHandlerResult::Stop )
				{
					parents.pop();
					return result;
				}
			}

			parents.pop();

			return TraversalHandlerResult::Continue;
		};

		return traverseNode( rootNode ) == TraversalHandlerResult::Continue;
	}
} // namespace dnv::vista::sdk
