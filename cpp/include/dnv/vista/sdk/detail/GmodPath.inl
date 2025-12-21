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
 * @file GmodPath.inl
 * @brief Inline implementations for GmodPath class
 * @details Provides inline implementations for simple accessor methods.
 */

#include <stdexcept>

namespace dnv::vista::sdk
{
	inline bool GmodPath::operator==( const GmodPath& other ) const noexcept
	{
		if ( m_parents.size() != other.m_parents.size() )
			return false;

		for ( size_t i = 0; i < m_parents.size(); ++i )
		{
			if ( m_parents[i] != other.m_parents[i] )
				return false;
		}

		return m_node == other.m_node;
	}

	inline bool GmodPath::operator!=( const GmodPath& other ) const noexcept
	{
		return !( *this == other );
	}

	inline const GmodNode& GmodPath::operator[]( size_t index ) const
	{
		if ( index > m_parents.size() )
		{
			throw std::out_of_range( "Index out of range for GmodPath indexer" );
		}

		return index < m_parents.size() ? m_parents[index] : m_node;
	}

	inline GmodNode& GmodPath::operator[]( size_t index )
	{
		if ( index > m_parents.size() )
		{
			throw std::out_of_range( "Index out of range for GmodPath indexer" );
		}

		return index < m_parents.size() ? m_parents[index] : m_node;
	}

	inline VisVersion GmodPath::version() const noexcept
	{
		return m_visVersion;
	}

	inline const GmodNode& GmodPath::node() const
	{
		return m_node;
	}

	inline const std::vector<GmodNode>& GmodPath::parents() const noexcept
	{
		return m_parents;
	}

	inline size_t GmodPath::length() const noexcept
	{
		return m_parents.size() + 1;
	}

	inline bool GmodPath::isMappable() const
	{
		return m_node.isMappable();
	}

	inline std::string GmodPath::toString() const
	{
		StringBuilder builder;
		toString( builder );
		return builder.toString();
	}

	inline void GmodPath::toString( StringBuilder& builder, char separator ) const
	{
		for ( const auto& parent : m_parents )
		{
			if ( !parent.isLeafNode() )
				continue;

			parent.toString( builder );
			builder.append( separator );
		}

		m_node.toString( builder );
	}

	inline void GmodPath::toFullPathString( StringBuilder& builder ) const
	{
		for ( const auto& [depth, pathNode] : fullPath() )
		{
			pathNode.toString( builder );

			if ( depth != ( length() - 1 ) )
				builder.append( '/' );
		}
	}

	inline std::string GmodPath::toStringDump() const
	{
		StringBuilder builder;
		toStringDump( builder );
		return builder.toString();
	}

	inline void GmodPath::toStringDump( StringBuilder& builder ) const
	{
		for ( const auto& [depth, pathNode] : fullPath() )
		{
			if ( depth == 0 )
				continue;

			if ( depth != 1 )
				builder.append( " | " );

			builder.append( pathNode.code() );

			const auto& name = pathNode.metadata().name();
			if ( !name.empty() )
			{
				builder.append( "/N:" );
				builder.append( name );
			}

			const auto& commonName = pathNode.metadata().commonName();
			if ( commonName.has_value() && !commonName->empty() )
			{
				builder.append( "/CN:" );
				builder.append( *commonName );
			}

			const auto normalAssignmentName = this->normalAssignmentName( depth );
			if ( normalAssignmentName.has_value() && !normalAssignmentName->empty() )
			{
				builder.append( "/NAN:" );
				builder.append( *normalAssignmentName );
			}
		}
	}

	inline GmodPath::FullPathRange GmodPath::fullPath() const
	{
		return FullPathRange( this, 0 );
	}

	inline GmodPath::FullPathRange GmodPath::fullPathFrom( size_t fromDepth ) const
	{
		if ( fromDepth > m_parents.size() )
		{
			throw std::out_of_range( "fromDepth out of range" );
		}
		return FullPathRange( this, fromDepth );
	}

	inline const std::vector<int>& GmodIndividualizableSet::nodeIndices() const noexcept
	{
		return m_nodeIndices;
	}

	inline std::optional<Location> GmodIndividualizableSet::location() const
	{
		if ( !m_path.has_value() )
		{
			throw std::runtime_error( "GmodIndividualizableSet has already been built" );
		}

		if ( m_nodeIndices.empty() )
		{
			return std::nullopt;
		}

		return ( *m_path )[m_nodeIndices[0]].location();
	}
} // namespace dnv::vista::sdk
