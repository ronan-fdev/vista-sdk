/**
 * @file GmodTraversal.inl
 * @brief Inline implementations for performance-critical GmodTraversal operations
 */

#include "Config.h"

namespace dnv::vista::sdk
{
	namespace GmodTraversal
	{
		namespace detail
		{
			//----------------------------------------------
			// Parents class
			//----------------------------------------------

			inline Parents::Parents()
			{
				m_parents.reserve( 64 );
				m_occurrences.reserve( 4 );
			}

			inline void Parents::push( const GmodNode& parent )
			{
				m_parents.push_back( parent );

				if ( auto it = m_occurrences.find( parent.code() ); it != m_occurrences.end() )
				{
					++it->second;
				}
				else
				{
					m_occurrences.emplace( std::string( parent.code() ), 1 );
				}
			}

			inline void Parents::pop()
			{
				if ( m_parents.empty() )
				{
					return;
				}

				const GmodNode& parent = m_parents.back();
				m_parents.pop_back();

				if ( auto it = m_occurrences.find( parent.code() ); it != m_occurrences.end() )
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

			inline int Parents::occurrences( const GmodNode& node ) const noexcept
			{
				if ( auto it = m_occurrences.find( node.code() ); it != m_occurrences.end() )
				{
					return it->second;
				}
				return 0;
			}

			inline const GmodNode* Parents::lastOrDefault() const noexcept
			{
				return m_parents.empty() ? nullptr : &m_parents.back();
			}

			inline const std::vector<GmodNode>& Parents::asList() const noexcept
			{
				return m_parents;
			}
		}
	}
}
