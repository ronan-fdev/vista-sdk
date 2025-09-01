/**
 * @file VISVersion.inl
 * @brief Inline implementations for performance-critical VISVersion operations
 */

#include <fmt/format.h>

#include <nfx/containers/StringMap.h>

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// Static lookup data
		//=====================================================================

		inline const std::vector<VisVersion>& allVersionsImpl()
		{
			static const std::vector<VisVersion> versions = []() {
				std::vector<VisVersion> result;
				for ( VisVersion v = VisVersion::v3_4a; v <= VisVersion::LATEST; v = v + 1 )
				{
					result.push_back( v );
				}
				return result;
			}();
			return versions;
		}

		inline const nfx::containers::StringMap<VisVersion>& versionMapImpl()
		{
			static const nfx::containers::StringMap<VisVersion> versionMap{
				{ "3.4a", VisVersion::v3_4a },
				{ "3.5a", VisVersion::v3_5a },
				{ "3.6a", VisVersion::v3_6a },
				{ "3.7a", VisVersion::v3_7a },
				{ "3.8a", VisVersion::v3_8a },
				{ "3.9a", VisVersion::v3_9a },

				{ "3-4a", VisVersion::v3_4a },
				{ "3-5a", VisVersion::v3_5a },
				{ "3-6a", VisVersion::v3_6a },
				{ "3-7a", VisVersion::v3_7a },
				{ "3-8a", VisVersion::v3_8a },
				{ "3-9a", VisVersion::v3_9a },

				{ "vis-3-4a", VisVersion::v3_4a },
				{ "vis-3-5a", VisVersion::v3_5a },
				{ "vis-3-6a", VisVersion::v3_6a },
				{ "vis-3-7a", VisVersion::v3_7a },
				{ "vis-3-8a", VisVersion::v3_8a },
				{ "vis-3-9a", VisVersion::v3_9a },

				{ "vis-3.4a", VisVersion::v3_4a },
				{ "vis-3.5a", VisVersion::v3_5a },
				{ "vis-3.6a", VisVersion::v3_6a },
				{ "vis-3.7a", VisVersion::v3_7a },
				{ "vis-3.8a", VisVersion::v3_8a },
				{ "vis-3.9a", VisVersion::v3_9a } };
			return versionMap;
		}
	}

	inline VisVersion operator++( VisVersion& version )
	{
		version = static_cast<VisVersion>( static_cast<int>( version ) + 100 );

		return version;
	}

	inline VisVersion operator++( VisVersion& version, int )
	{
		VisVersion old = version;
		++version;

		return old;
	}

	inline VisVersion operator+( VisVersion version, int increment )
	{
		if ( increment == 1 )
		{
			return static_cast<VisVersion>( static_cast<int>( version ) + 100 );
		}

		return static_cast<VisVersion>( static_cast<int>( version ) + increment * 100 );
	}

	inline VisVersion operator-( VisVersion version, int decrement )
	{
		if ( decrement == 1 )
		{
			return static_cast<VisVersion>( static_cast<int>( version ) - 100 );
		}

		return static_cast<VisVersion>( static_cast<int>( version ) - decrement * 100 );
	}

	inline bool operator<=( VisVersion lhs, VisVersion rhs )
	{
		return static_cast<int>( lhs ) <= static_cast<int>( rhs );
	}

	inline int operator-( VisVersion lhs, VisVersion rhs )
	{
		return ( static_cast<int>( lhs ) - static_cast<int>( rhs ) ) / 100;
	}

	inline bool VisVersionExtensions::isValid( VisVersion version )
	{
		return version >= VisVersion::v3_4a && version <= VisVersion::LATEST;
	}

	inline const std::vector<VisVersion>& VisVersionExtensions::allVersions()
	{
		return allVersionsImpl();
	}

	inline VisVersion VisVersionExtensions::latestVersion()
	{
		return VisVersion::LATEST;
	}

	inline std::string_view VisVersionExtensions::toVersionString( VisVersion version )
	{
		switch ( version )
		{
			case VisVersion::v3_4a:
			{
				return "vis-3-4a";
			}
			case VisVersion::v3_5a:
			{
				return "vis-3-5a";
			}
			case VisVersion::v3_6a:
			{
				return "vis-3-6a";
			}
			case VisVersion::v3_7a:
			{
				return "vis-3-7a";
			}
			case VisVersion::v3_8a:
			{
				return "vis-3-8a";
			}
			case VisVersion::v3_9a:
			{
				return "vis-3-9a";
			}
			case VisVersion::Unknown:
			{
				return "Unknown";
			}
			default:
			{
				throw std::invalid_argument{ "Invalid VIS version: " + std::to_string( static_cast<int>( version ) ) };
			}
		}
	}

	inline bool VisVersionExtensions::tryParse( std::string_view versionString, VisVersion& version )
	{
		const auto& versionMap = versionMapImpl();
		auto it = versionMap.find( versionString );
		if ( it != versionMap.end() )
		{
			version = it->second;

			return true;
		}
		return false;
	}

	inline VisVersion VisVersionExtensions::parse( std::string_view versionString )
	{
		VisVersion result;
		if ( !tryParse( versionString, result ) )
		{
			throw std::invalid_argument{ fmt::format( "Invalid VIS version string: ", versionString ) };
		}

		return result;
	}
}
