#pragma once

namespace dnv::vista::sdk
{
	/**
	 * @brief Enumeration of VIS versions
	 */
	enum class VisVersion
	{
		Unknown = 0,

		v3_4a = 3400,
		v3_5a = 3500,
		v3_6a = 3600,
		v3_7a = 3700,
		v3_8a = 3800,
		v3_9a = 3900,

		LATEST = v3_9a,
	};

	/**
	 * @brief Static helper class for VisVersion operations
	 */
	class VisVersionExtensions final
	{
	public:
		/**
		 * @brief Check if a VisVersion is valid
		 * @param version The version to check
		 * @return true if valid, false otherwise
		 */
		inline static bool isValid( VisVersion version );

		/**
		 * @brief Get the next VIS version in sequence
		 * @param version The current version
		 * @return The next VisVersion
		 * @throws std::out_of_range if the current version is the latest
		 */
		inline static VisVersion next( VisVersion version );

		/**
		 * @brief Get the latest available VIS version
		 * @return The latest VIS version
		 */
		inline static VisVersion latestVersion();

		/**
		 * @brief Convert a VisVersion to its string representation
		 * @param version The version to convert
		 * @return String representation of the version
		 */
		inline static std::string toVersionString( VisVersion version );

		/**
		 * @brief Try to parse a string to a VisVersion
		 * @param versionString The string to parse
		 * @param version Output parameter for the parsed version
		 * @return true if parsing succeeded, false otherwise
		 */
		static bool tryParse( const std::string& versionString, VisVersion& version );

		/**
		 * @brief Parse a string to a VisVersion
		 * @param versionString The string to parse
		 * @return The parsed VisVersion
		 * @throws std::invalid_argument if parsing fails
		 */
		static VisVersion parse( const std::string& versionString );

		/**
		 * @brief Get all valid VisVersion values
		 * @return Vector containing all valid VisVersion values
		 */
		static std::vector<VisVersion> allVersions();
	};
}

#include "VISVersion.inl"
