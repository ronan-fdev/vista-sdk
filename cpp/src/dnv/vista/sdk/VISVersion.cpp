#include "pch.h"

#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk
{
	bool VisVersionExtensions::tryParse( const std::string& versionString, VisVersion& version )
	{
		static const std::unordered_map<std::string_view, VisVersion> versionMap = {
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

		auto it = versionMap.find( versionString );
		if ( it != versionMap.end() )
		{
			version = it->second;
			return true;
		}

		return false;
	}

	VisVersion VisVersionExtensions::parse( const std::string& versionString )
	{
		VisVersion result;
		if ( !tryParse( versionString, result ) )
		{
			throw std::invalid_argument( "Invalid VIS version string: " + versionString );
		}
		return result;
	}

	std::vector<VisVersion> VisVersionExtensions::allVersions()
	{
		static constexpr std::array<VisVersion, 6> versions = { { VisVersion::v3_4a,
			VisVersion::v3_5a,
			VisVersion::v3_6a,
			VisVersion::v3_7a,
			VisVersion::v3_8a,
			VisVersion::v3_9a } };

		return std::vector<VisVersion>( versions.begin(), versions.end() );
	}
}
