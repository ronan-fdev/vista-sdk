/**
 * @file VISVersion.inl
 * @brief Inline implementations for performance-critical VISVersion operations
 */

namespace dnv::vista::sdk
{
	inline bool VisVersionExtensions::isValid( VisVersion version )
	{
		return version >= VisVersion::v3_4a && version <= VisVersion::LATEST;
	}

	inline VisVersion VisVersionExtensions::next( VisVersion version )
	{
		if ( version >= VisVersion::LATEST )
		{
			throw std::out_of_range( "Cannot get next version of the latest version" );
		}
		return static_cast<VisVersion>( static_cast<int>( version ) + 100 );
	}

	inline VisVersion VisVersionExtensions::latestVersion()
	{
		return VisVersion::LATEST;
	}

	inline std::string VisVersionExtensions::toVersionString( VisVersion version )
	{
		switch ( version )
		{
			case VisVersion::v3_4a:
			{
				return "3-4a";
			}
			case VisVersion::v3_5a:
			{
				return "3-5a";
			}
			case VisVersion::v3_6a:
			{
				return "3-6a";
			}
			case VisVersion::v3_7a:
			{
				return "3-7a";
			}
			case VisVersion::v3_8a:
			{
				return "3-8a";
			}
			case VisVersion::v3_9a:
			{
				return "3-9a";
			}
			case VisVersion::Unknown:
			{
				return "Unknown";
			}
			default:
			{
				throw std::invalid_argument( "Invalid VIS version: " + std::to_string( static_cast<int>( version ) ) );
			}
		}
	}
}
