/**
 * @file VIS.inl
 * @brief Inline implementations for performance-critical VIS operations
 */

namespace dnv::vista::sdk
{
	//----------------------------------------------
	// ISO string validation methods
	//----------------------------------------------

	template <typename StringLike>
	inline bool VIS::matchISOLocalIdString( const StringLike& value ) noexcept
	{
		for ( const auto c : value )
		{
			if ( c == '/' )
			{
				continue;
			}
			if ( !isISOString( c ) )
			{
				return false;
			}
		}

		return true;
	}

	template <typename StringLike>
	inline bool VIS::isISOString( const StringLike& value ) noexcept
	{
		for ( const auto c : value )
		{
			if ( !isISOString( c ) )
			{
				return false;
			}
		}

		return true;
	}

	template <typename StringLike>
	inline bool VIS::isISOLocalIdString( const StringLike& value ) noexcept
	{
		return matchISOLocalIdString( value );
	}

	inline bool VIS::isISOString( char c ) noexcept
	{
		return matchAsciiDecimal( static_cast<int>( c ) );
	}

	inline bool VIS::matchAsciiDecimal( int code ) noexcept
	{
		/* Numbers (48-57) */
		if ( code >= 48 && code <= 57 )
		{
			return true;
		}

		/* Uppercase A-Z (65-90) */
		if ( code >= 65 && code <= 90 )
		{
			return true;
		}

		/* Lowercase a-z (97-122) */
		if ( code >= 97 && code <= 122 )
		{
			return true;
		}

		/* Special chars: "-", ".", "_", "~" */
		return ( code == 45 || code == 46 || code == 95 || code == 126 );
	}
}
