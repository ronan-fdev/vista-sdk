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
	 * @brief Arithmetic operators for VisVersion to optimize version arithmetic
	 */
	inline VisVersion operator++( VisVersion& version );
	inline VisVersion operator++( VisVersion& version, int );
	inline VisVersion operator+( VisVersion version, int increment );
	inline VisVersion operator-( VisVersion version, int decrement );
	inline bool operator<=( VisVersion lhs, VisVersion rhs );
	inline int operator-( VisVersion lhs, VisVersion rhs );

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
		 * @brief Get all valid VisVersion values (zero-copy)
		 * @return Const reference to vector containing all valid VisVersion values
		 */
		inline static const std::vector<VisVersion>& allVersions();

		/**
		 * @brief Get the latest available VIS version
		 * @return The latest VIS version
		 */
		inline static VisVersion latestVersion();

		/**
		 * @brief Convert a VisVersion to its string representation (zero-copy)
		 * @param version The version to convert
		 * @return String view representation of the version (no allocation)
		 */
		inline static std::string_view toVersionString( VisVersion version );

		/**
		 * @brief Try to parse a string_view to a VisVersion (zero-copy version)
		 * @param versionString The string_view to parse
		 * @param version Output parameter for the parsed version
		 * @return true if parsing succeeded, false otherwise
		 */
		inline static bool tryParse( std::string_view versionString, VisVersion& version );

		/**
		 * @brief Parse a string_view to a VisVersion (zero-copy version)
		 * @param versionString The string_view to parse
		 * @return The parsed VisVersion
		 * @throws std::invalid_argument if parsing fails
		 */
		inline static VisVersion parse( std::string_view versionString );
	};
}

#include "VISVersion.inl"
