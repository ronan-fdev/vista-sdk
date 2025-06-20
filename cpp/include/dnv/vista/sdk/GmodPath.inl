/**
 * @file GmodPath.inl
 * @brief Inline implementations for performance-critical GmodPath operations
 */

#include "Gmod.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// GmodPath class
	//=====================================================================

	//----------------------------------------------
	// Equality operators
	//----------------------------------------------

	inline bool GmodPath::operator==( const GmodPath& other ) const noexcept
	{
		if ( this == &other )
		{
			return true;
		}

		if ( m_parents.size() != other.m_parents.size() )
		{
			return false;
		}

		if ( m_node.has_value() != other.m_node.has_value() )
		{
			return false;
		}

		if ( m_node.has_value() && m_node.value() != other.m_node.value() )
		{
			return false;
		}

		for ( size_t i = 0; i < m_parents.size(); ++i )
		{
			if ( m_parents[i] != other.m_parents[i] )
			{
				return false;
			}
		}

		return true;
	}

	inline bool GmodPath::operator!=( const GmodPath& other ) const noexcept
	{
		return !( *this == other );
	}

	//----------------------------------------------
	// Lookup operators
	//----------------------------------------------

	inline const GmodNode& GmodPath::operator[]( size_t index ) const
	{
		if ( index < m_parents.size() )
		{
			return m_parents[index];
		}
		else if ( index == m_parents.size() )
		{
			if ( !m_node.has_value() )
			{
				throw std::out_of_range( "No target node in empty GmodPath" );
			}

			return m_node.value();
		}
		else
		{
			throw std::out_of_range( "Index out of range" );
		}
	}

	inline GmodNode& GmodPath::operator[]( size_t index )
	{
		if ( index < m_parents.size() )
		{
			return m_parents[index];
		}
		else if ( index == m_parents.size() )
		{
			if ( !m_node.has_value() )
			{
				throw std::out_of_range( "No target node in empty GmodPath" );
			}
			return m_node.value();
		}
		else
		{
			throw std::out_of_range( "Index out of range" );
		}
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline VisVersion GmodPath::visVersion() const noexcept
	{
		return m_visVersion;
	}

	inline const Gmod* GmodPath::gmod() const noexcept
	{
		return m_gmod;
	}

	inline const GmodNode& GmodPath::node() const
	{
		if ( !m_node.has_value() )
		{
			throw std::runtime_error( "Accessing node on empty GmodPath" );
		}

		return m_node.value();
	}

	inline const std::vector<GmodNode>& GmodPath::parents() const noexcept
	{
		return m_parents;
	}

	inline size_t GmodPath::length() const noexcept
	{
		return m_parents.size() + ( m_node.has_value() ? 1 : 0 );
	}

	inline size_t GmodPath::hashCode() const noexcept
	{
		if ( !m_node.has_value() )
		{
			return 0;
		}

		size_t hashCode = 0;

		for ( const auto& parent : m_parents )
		{
			size_t nodeHash = parent.hashCode();
			hashCode ^= nodeHash + 0x9e3779b9 + ( hashCode << 6 ) + ( hashCode >> 2 );
		}

		size_t nodeHash = m_node->hashCode();
		hashCode ^= nodeHash + 0x9e3779b9 + ( hashCode << 6 ) + ( hashCode >> 2 );

		return hashCode;
	}

	//----------------------------------------------
	// String conversion methods
	//----------------------------------------------

	inline std::string GmodPath::toString() const
	{
		fmt::memory_buffer builder;
		toString( std::back_inserter( builder ) );

		return fmt::to_string( builder );
	}

	inline std::string GmodPath::toFullPathString() const
	{
		fmt::memory_buffer builder;
		toFullPathString( std::back_inserter( builder ) );
		return fmt::to_string( builder );
	}

	inline std::string GmodPath::toStringDump() const
	{
		fmt::memory_buffer builder;
		toStringDump( std::back_inserter( builder ) );
		return fmt::to_string( builder );
	}

	template <typename OutputIt>
	inline OutputIt GmodPath::toString( OutputIt out, char separator ) const
	{
		bool first = true;
		for ( const auto& parent : m_parents )
		{
			if ( !Gmod::isLeafNode( parent.metadata() ) )
			{
				continue;
			}
			if ( !first )
			{
				*out++ = separator;
			}
			out = parent.toString( out );
			first = false;
		}

		if ( m_node.has_value() )
		{
			if ( !first )
			{
				*out++ = separator;
			}
			out = m_node->toString( out );
		}
		return out;
	}

	template <typename OutputIt>
	inline OutputIt GmodPath::toFullPathString( OutputIt out ) const
	{
		auto enumerator = this->fullPath();
		while ( enumerator.next() )
		{
			const auto& [depth, pathNode] = enumerator.current();
			out = pathNode->toString( out );
			if ( depth != ( length() - 1 ) )
			{
				*out++ = '/';
			}
		}
		return out;
	}

	template <typename OutputIt>
	inline OutputIt GmodPath::toStringDump( OutputIt out ) const
	{
		auto enumerator = this->fullPath();
		while ( enumerator.next() )
		{
			const auto& [depth, pathNode] = enumerator.current();

			if ( depth == 0 )
			{
				continue;
			}

			if ( depth != 1 )
			{
				out = fmt::format_to( out, " | " );
			}

			out = fmt::format_to( out, "{}", pathNode->code() );

			const auto& name = pathNode->metadata().name();
			if ( !name.empty() )
			{
				out = fmt::format_to( out, "/N:{}", name );
			}

			const auto& commonName = pathNode->metadata().commonName();
			if ( commonName.has_value() && !commonName->empty() )
			{
				out = fmt::format_to( out, "/CN:{}", *commonName );
			}

			auto normalAssignment = normalAssignmentName( depth );
			if ( normalAssignment && !normalAssignment->empty() )
			{
				out = fmt::format_to( out, "/NAN:{}", *normalAssignment );
			}
		}
		return out;
	}

	//----------------------------------------------
	// GmodPath enumerator
	//----------------------------------------------

	//----------------------------
	// Enumeration methods
	//----------------------------

	bool GmodPath::Enumerator::next()
	{
		if ( !m_pathInstance || !m_pathInstance->m_gmod || !m_pathInstance->m_node.has_value() )
		{
			return false;
		}

		if ( m_currentIndex == std::numeric_limits<size_t>::max() )
		{
			m_currentIndex = 0;
		}
		else
		{
			++m_currentIndex;
		}

		if ( m_currentIndex >= m_pathInstance->length() )
		{
			return false;
		}

		if ( m_currentIndex < m_pathInstance->m_parents.size() )
		{
			m_current = std::make_pair( m_currentIndex, &m_pathInstance->m_parents[m_currentIndex] );
		}
		else
		{
			m_current = std::make_pair( m_currentIndex, &m_pathInstance->m_node.value() );
		}

		return true;
	}

	inline const GmodPath::Enumerator::PathElement& GmodPath::Enumerator::current() const
	{
		if ( !m_pathInstance || m_currentIndex >= m_pathInstance->length() )
		{
			static PathElement invalidElement{ std::numeric_limits<size_t>::max(), nullptr };
			return invalidElement;
		}

		m_current.first = m_currentIndex;

		if ( m_currentIndex < m_pathInstance->m_parents.size() )
		{
			m_current.second = &( m_pathInstance->m_parents[m_currentIndex] );
		}
		else if ( m_currentIndex == m_pathInstance->m_parents.size() && m_pathInstance->m_node.has_value() )
		{
			m_current.second = &( m_pathInstance->m_node.value() );
		}
		else
		{
			static PathElement invalidElement{ std::numeric_limits<size_t>::max(), nullptr };

			return invalidElement;
		}

		return m_current;
	}

	//=====================================================================
	// GmodIndividualizableSet class
	//=====================================================================

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::vector<int>& GmodIndividualizableSet::nodeIndices() const noexcept
	{
		return m_nodeIndices;
	}

	inline std::optional<Location> GmodIndividualizableSet::location() const
	{
		if ( m_nodeIndices.empty() )
		{
			return std::nullopt;
		}

		int firstNodeIdx = m_nodeIndices[0];
		if ( firstNodeIdx < 0 || static_cast<size_t>( firstNodeIdx ) >= m_path.length() )
		{
			return std::nullopt;
		}

		const GmodNode& firstNode = m_path[static_cast<size_t>( firstNodeIdx )];

		return firstNode.location();
	}
}
