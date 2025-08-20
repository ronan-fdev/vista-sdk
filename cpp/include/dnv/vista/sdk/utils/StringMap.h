/**
 * @file StringMap.h
 * @brief Enhanced unordered_map with heterogeneous string lookup optimization
 * @details Provides zero-copy string_view lookups while maintaining std::string storage,
 *          eliminating temporary string allocations during key operations
 */

#pragma once

#include "dnv/vista/sdk/constants/CodebookConstants.h"
#include "dnv/vista/sdk/utils/StringUtils.h"

namespace dnv::vista::sdk::utils
{
	//=====================================================================
	// StringMap class
	//=====================================================================

	/**
	 * @brief Enhanced unordered map with full heterogeneous support
	 * @tparam T Value type
	 */
	template <typename T>
	class StringMap : public std::unordered_map<std::string, T, StringViewHash, StringViewEqual>
	{
		using Base = std::unordered_map<std::string, T, StringViewHash, StringViewEqual>;

	public:
		using Base::Base;
		using Base::operator[];
		using Base::try_emplace;

		/**
		 * @brief Heterogeneous operator[] for string_view
		 * @param key String view key
		 * @return Reference to the mapped value
		 */
		inline T& operator[]( std::string_view key )
		{
			return this->try_emplace( std::string{ key }, T{} ).first->second;
		}

		/**
		 * @brief Heterogeneous operator[] for const char*
		 * @param key C-string key
		 * @return Reference to the mapped value
		 */
		inline T& operator[]( const char* key )
		{
			return this->try_emplace( std::string{ key }, T{} ).first->second;
		}

		/**
		 * @brief Heterogeneous try_emplace for string_view
		 * @param key String view key
		 * @param args Arguments to construct the value
		 * @return Pair of iterator and bool indicating insertion
		 */
		template <typename... Args>
		inline std::pair<typename Base::iterator, bool> try_emplace( std::string_view key, Args&&... args )
		{
			return Base::try_emplace( std::string{ key }, std::forward<Args>( args )... );
		}

		/**
		 * @brief Heterogeneous try_emplace for const char*
		 * @param key C-string key
		 * @param args Arguments to construct the value
		 * @return Pair of iterator and bool indicating insertion
		 */
		template <typename... Args>
		inline std::pair<typename Base::iterator, bool> try_emplace( const char* key, Args&&... args )
		{
			return Base::try_emplace( std::string{ key }, std::forward<Args>( args )... );
		}
	};

	//=====================================================================
	// Type aliases for heterogeneous containers
	//=====================================================================

	/** @brief Unordered set with heterogeneous lookup support */
	using StringSet = std::unordered_set<std::string, StringViewHash, StringViewEqual>;
}
