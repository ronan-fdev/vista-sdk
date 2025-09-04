/**
 * @file GmodPath.inl
 * @brief Inline implementations for performance-critical GmodPath operations
 */

#include <variant>

#include <nfx/string/StringBuilderPool.h>

#include "config/config.h"
#include "Gmod.h"
#include "Locations.h"
#include "VIS.h"
#include "VISVersion.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// GmodPath class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline GmodPath::GmodPath()
		: m_visVersion{ VisVersion::Unknown },
		  m_gmod{ nullptr },
		  m_node{ std::nullopt },
		  m_parents{}
	{
	}

	inline GmodPath::GmodPath( const GmodPath& other )
		: m_visVersion{ other.m_visVersion },
		  m_gmod{ other.m_gmod },
		  m_node{ other.m_node },
		  m_parents{ other.m_parents }
	{
	}

	inline GmodPath::GmodPath( GmodPath&& other ) noexcept
		: m_visVersion{ other.m_visVersion },
		  m_gmod{ other.m_gmod },
		  m_node{ std::move( other.m_node ) },
		  m_parents{ std::move( other.m_parents ) }
	{
		other.m_gmod = nullptr;
		other.m_visVersion = VisVersion::Unknown;
	}

	//----------------------------------------------
	// Assignment operators
	//----------------------------------------------

	inline GmodPath& GmodPath::operator=( const GmodPath& other )
	{
		if ( this == &other )
		{
			return *this;
		}

		m_visVersion = other.m_visVersion;
		m_gmod = other.m_gmod;
		m_node = other.m_node;
		m_parents = other.m_parents;

		return *this;
	}

	inline GmodPath& GmodPath::operator=( GmodPath&& other ) noexcept
	{
		if ( this == &other )
		{
			return *this;
		}

		m_visVersion = other.m_visVersion;
		m_gmod = other.m_gmod;
		m_node = std::move( other.m_node );
		m_parents = std::move( other.m_parents );

		other.m_gmod = nullptr;
		other.m_visVersion = VisVersion::Unknown;

		return *this;
	}

	//----------------------------------------------
	// Equality operators
	//----------------------------------------------

	inline bool GmodPath::operator==( const GmodPath& other ) const noexcept
	{
		if ( this == &other )
		{
			return true;
		}

		const bool thisHasNode = m_node.has_value();
		const bool otherHasNode = other.m_node.has_value();
		if ( thisHasNode != otherHasNode )
		{
			return false;
		}
		if ( thisHasNode && m_node.value() != other.m_node.value() )
		{
			return false;
		}

		if ( m_parents.size() != other.m_parents.size() )
		{
			return false;
		}

		return std::equal( m_parents.begin(), m_parents.end(), other.m_parents.begin() );
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
				throw std::out_of_range{ "No target node in empty GmodPath" };
			}

			return m_node.value();
		}
		else
		{
			throw std::out_of_range{ "Index out of range" };
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
				throw std::out_of_range{ "No target node in empty GmodPath" };
			}
			return m_node.value();
		}
		else
		{
			throw std::out_of_range{ "Index out of range" };
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
			throw std::runtime_error{ "Accessing node on empty GmodPath" };
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

	//----------------------------------------------
	// State inspection methods
	//----------------------------------------------

	inline bool GmodPath::isValid( const std::vector<GmodNode*>& parents, const GmodNode& node )
	{
		int missingLinkAt;

		return isValid( parents, node, missingLinkAt );
	}

	inline bool GmodPath::isMappable() const
	{
		if ( !m_node.has_value() )
		{
			return false;
		}

		const GmodNode& node = m_node.value();
		return node.isMappable();
	}

	//----------------------------------------------
	// String conversion methods
	//----------------------------------------------

	inline std::string GmodPath::toString() const
	{
		auto lease = nfx::string::StringBuilderPool::lease();
		bool first = true;
		for ( const auto& parent : m_parents )
		{
			if ( !Gmod::isLeafNode( parent.metadata() ) )
			{
				continue;
			}
			if ( !first )
			{
				lease.builder().push_back( '/' );
			}
			lease.builder().append( parent.toString() );
			first = false;
		}

		if ( m_node.has_value() )
		{
			if ( !first )
			{
				lease.builder().push_back( '/' );
			}
			lease.builder().append( m_node->toString() );
		}

		return lease.toString();
	}

	inline std::string GmodPath::toFullPathString() const
	{
		auto lease = nfx::string::StringBuilderPool::lease();
		auto enumerator = this->fullPath();
		while ( enumerator.next() )
		{
			const auto& [depth, pathNode] = enumerator.current();
			lease.builder().append( pathNode->toString() );
			if ( depth != ( length() - 1 ) )
			{
				lease.builder().push_back( '/' );
			}
		}

		return lease.toString();
	}

	inline std::string GmodPath::toStringDump() const
	{
		auto lease = nfx::string::StringBuilderPool::lease();
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
				lease.builder().append( " | " );
			}

			lease.builder().append( pathNode->code() );

			const auto& name = pathNode->metadata().name();
			if ( !name.empty() )
			{
				lease.builder().append( "/N:" );
				lease.builder().append( name );
			}

			const auto& commonName = pathNode->metadata().commonName();
			if ( commonName.has_value() && !commonName->empty() )
			{
				lease.builder().append( "/CN:" );
				lease.builder().append( *commonName );
			}

			auto normalAssignment = normalAssignmentName( depth );
			if ( normalAssignment && !normalAssignment->empty() )
			{
				lease.builder().append( "/NAN:" );
				lease.builder().append( *normalAssignment );
			}
		}

		return lease.toString();
	}

	inline void GmodPath::toString( nfx::string::StringBuilder& builder, char separator ) const
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
				builder.push_back( separator );
			}
			parent.toString( builder );
			first = false;
		}

		if ( m_node.has_value() )
		{
			if ( !first )
			{
				builder.push_back( separator );
			}
			m_node->toString( builder );
		}
	}

	inline void GmodPath::toFullPathString( nfx::string::StringBuilder& builder ) const
	{
		auto enumerator = this->fullPath();
		while ( enumerator.next() )
		{
			const auto& [depth, pathNode] = enumerator.current();
			pathNode->toString( builder );
			if ( depth != ( length() - 1 ) )
			{
				builder.push_back( '/' );
			}
		}
	}

	inline void GmodPath::toStringDump( nfx::string::StringBuilder& builder ) const
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
				builder.append( " | " );
			}

			builder.append( pathNode->code() );

			const auto& name = pathNode->metadata().name();
			if ( !name.empty() )
			{
				builder.append( "/N:" );
				builder.append( name );
			}

			const auto& commonName = pathNode->metadata().commonName();
			if ( commonName.has_value() && !commonName->empty() )
			{
				builder.append( "/CN:" );
				builder.append( *commonName );
			}

			auto normalAssignment = normalAssignmentName( depth );
			if ( normalAssignment && !normalAssignment->empty() )
			{
				builder.append( "/NAN:" );
				builder.append( *normalAssignment );
			}
		}
	}

	//----------------------------------------------
	// Path enumeration methods
	//----------------------------------------------

	inline GmodPath::Enumerator GmodPath::fullPath() const
	{
		return fullPathFrom( 0 );
	}

	inline GmodPath::Enumerator GmodPath::fullPathFrom( size_t fromDepth ) const
	{
		return Enumerator( this, fromDepth );
	}

	//----------------------------
	// Enumeration
	//----------------------------

	inline GmodPath::Enumerator GmodPath::enumerator( size_t fromDepth ) const
	{
		return Enumerator( this, fromDepth );
	}

	//----------------------------------------------
	// Inline parsing methods
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE GmodPath GmodPath::parse( std::string_view item, VisVersion visVersion )
	{
		std::optional<GmodPath> path{ std::nullopt };

		if ( !tryParse( item, visVersion, path ) )
		{
			throw std::invalid_argument{ "Couldnt parse path" };
		}

		return std::move( path.value() );
	}

	VISTA_SDK_CPP_INLINE GmodPath GmodPath::parse( std::string_view item, const Gmod& gmod, const Locations& locations )
	{
		GmodParsePathResult result = parseInternal( item, gmod, locations );

		if ( result.isOk() )
		{
			return std::move( result.ok().path );
		}
		else
		{
			throw std::invalid_argument{ result.error().error };
		}
	}

	VISTA_SDK_CPP_INLINE GmodPath GmodPath::parseFullPath( std::string_view item, VisVersion visVersion )
	{
		VIS& vis = VIS::instance();
		const Gmod& gmod = vis.gmod( visVersion );
		const Locations& locations = vis.locations( visVersion );
		GmodParsePathResult result = parseFullPathInternal( item, gmod, locations );

		if ( result.isOk() )
		{
			return std::move( result.ok().path );
		}
		else
		{
			throw std::invalid_argument{ result.error().error };
		}
	}

	VISTA_SDK_CPP_INLINE bool GmodPath::tryParse(
		std::string_view item, VisVersion visVersion,
		std::optional<GmodPath>& outPath )
	{
		outPath.reset();

		VIS& vis = VIS::instance();
		const Gmod& gmod = vis.gmod( visVersion );
		const Locations& locations = vis.locations( visVersion );

		return tryParse( item, gmod, locations, outPath );
	}

	VISTA_SDK_CPP_INLINE bool GmodPath::tryParse(
		std::string_view item, const Gmod& gmod,
		const Locations& locations, std::optional<GmodPath>& outPath )
	{
		GmodParsePathResult result = parseInternal( item, gmod, locations );
		outPath.reset();

		if ( result.isOk() )
		{
			outPath.emplace( std::move( result.ok().path ) );
			return true;
		}

		return false;
	}

	VISTA_SDK_CPP_INLINE bool GmodPath::tryParseFullPath(
		std::string_view item, VisVersion visVersion,
		std::optional<GmodPath>& outPath )
	{
		outPath.reset();

		VIS& vis = VIS::instance();
		const Gmod& gmod = vis.gmod( visVersion );
		const Locations& locations = vis.locations( visVersion );

		return tryParseFullPath( item, gmod, locations, outPath );
	}

	VISTA_SDK_CPP_INLINE bool GmodPath::tryParseFullPath(
		std::string_view item, const Gmod& gmod, const Locations& locations, std::optional<GmodPath>& outPath )
	{
		GmodParsePathResult result = parseFullPathInternal( item, gmod, locations );

		if ( result.isOk() )
		{
			outPath.emplace( std::move( result.ok().path ) );
			return true;
		}

		outPath.reset();
		return false;
	}

	//----------------------------------------------
	// GmodPath::enumerator
	//----------------------------------------------

	//----------------------------
	// Enumeration methods
	//----------------------------

	inline bool GmodPath::Enumerator::next()
	{
		if ( !m_pathInstance || !m_pathInstance->m_node.has_value() )
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

		m_current.first = m_currentIndex;
		m_current.second = ( m_currentIndex < m_pathInstance->m_parents.size() ) ? &m_pathInstance->m_parents[m_currentIndex]
																				 : &m_pathInstance->m_node.value();

		return true;
	}

	inline const GmodPath::Enumerator::PathElement& GmodPath::Enumerator::current() const
	{
		return m_current;
	}

	inline void GmodPath::Enumerator::reset()
	{
		m_currentIndex = std::numeric_limits<size_t>::max();
		m_current.first = std::numeric_limits<size_t>::max();
		m_current.second = nullptr;
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

	//=====================================================================
	// GmodParsePathResult
	//=====================================================================

	inline GmodParsePathResult::Ok::Ok( GmodPath p )
		: path{ std::move( p ) }
	{
	}

	inline GmodParsePathResult::Error::Error( std::string e )
		: error{ std::move( e ) }
	{
	}

	inline GmodParsePathResult::GmodParsePathResult( Ok ok )
		: result{ std::move( ok ) }
	{
	}

	inline GmodParsePathResult::GmodParsePathResult( Error err )
		: result{ std::move( err ) }
	{
	}

	inline bool GmodParsePathResult::isOk() const noexcept
	{
		return std::holds_alternative<Ok>( result );
	}

	inline bool GmodParsePathResult::isError() const noexcept
	{
		return std::holds_alternative<Error>( result );
	}

	inline GmodParsePathResult::Ok& GmodParsePathResult::ok()
	{
		return std::get<Ok>( result );
	}

	inline const GmodParsePathResult::Ok& GmodParsePathResult::ok() const
	{
		return std::get<Ok>( result );
	}

	inline GmodParsePathResult::Error& GmodParsePathResult::error()
	{
		return std::get<Error>( result );
	}

	inline const GmodParsePathResult::Error& GmodParsePathResult::error() const
	{
		return std::get<Error>( result );
	}
}
