/**
 * @file Locations.inl
 * @brief Inline implementations for performance-critical Locations operations
 */

#include "config/Platform.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Location Class
	//=====================================================================

	//----------------------------------------------
	// Comparison Operators
	//----------------------------------------------

	inline bool Location::operator==( const Location& other ) const
	{
		return m_value == other.m_value;
	}

	inline bool Location::operator!=( const Location& other ) const
	{
		return !( m_value == other.m_value );
	}

	//----------------------------------------------
	// Conversion Operators
	//----------------------------------------------

	inline Location::operator std::string() const noexcept
	{
		return m_value;
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	const inline std::string& Location::value() const noexcept
	{
		return m_value;
	}

	const inline std::string& Location::toString() const noexcept
	{
		return m_value;
	}

	//----------------------------------------------
	// Public static helper methods
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE bool Locations::tryParseInt( std::string_view span, int start, int length, int& number )
	{
		if ( start < 0 || length <= 0 || static_cast<size_t>( start + length ) > span.length() )
		{
			return false;
		}

		const char* begin = span.data() + start;
		const char* end = begin + length;
		auto result = std::from_chars( begin, end, number );
		if ( result.ec == std::errc() && result.ptr == end )
		{
			return true;
		}

		return false;
	}

	//=====================================================================
	// RelativeLocation Class
	//=====================================================================

	//----------------------------------------------
	// Comparison Operators
	//----------------------------------------------

	inline bool RelativeLocation::operator==( const RelativeLocation& other ) const
	{
		return m_code == other.m_code;
	}

	inline bool RelativeLocation::operator!=( const RelativeLocation& other ) const
	{
		return !( m_code == other.m_code );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline char RelativeLocation::code() const noexcept
	{
		return m_code;
	}

	inline const std::string& RelativeLocation::name() const noexcept
	{
		return m_name;
	}

	inline const std::optional<std::string>& RelativeLocation::definition() const noexcept
	{
		return m_definition;
	}

	inline const Location& RelativeLocation::location() const noexcept
	{
		return m_location;
	}
}
