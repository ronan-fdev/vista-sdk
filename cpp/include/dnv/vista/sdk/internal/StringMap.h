/**
 * @file StringMap.h
 * @brief Enhanced unordered_map with heterogeneous string lookup optimization
 * @details Provides zero-copy string_view lookups while maintaining std::string storage,
 *          eliminating temporary string allocations during key operations
 */

#pragma once

#include "dnv/vista/sdk/config/Platform.h"

namespace dnv::vista::sdk::internal
{
	//=====================================================================
	// Heterogeneous lookup functors for zero-copy string operations
	//=====================================================================

	/**
	 * @brief Hash functor supporting both std::string and std::string_view
	 * @details Enables heterogeneous lookup in unordered containers,
	 *          allowing direct string_view lookups without string construction.
	 */
	struct StringViewHash
	{
		/**
		 * @brief Enables heterogeneous lookup in unordered containers
		 * @details This type alias allows the hash functor to work with different
		 *          but compatible key types (std::string, std::string_view, const char*)
		 *          without requiring conversion to the container's key type.
		 */
		using is_transparent = void;

		static constexpr std::hash<std::string_view> s_hasher{};

		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE size_t operator()( std::string_view sv ) const noexcept
		{
			return s_hasher( sv );
		}

		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE size_t operator()( const std::string& s ) const noexcept
		{
			return s_hasher( std::string_view{ s.data(), s.size() } );
		}

		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE size_t operator()( const char* s ) const noexcept
		{
			return s_hasher( std::string_view{ s } );
		}
	};

	/**
	 * @brief Equality functor supporting both std::string and std::string_view
	 * @details Enables heterogeneous lookup in unordered containers,
	 *          providing all comparison overloads for string types.
	 */
	struct StringViewEqual
	{
		/**
		 * @brief Enables heterogeneous lookup in unordered containers
		 * @details This type alias allows the equality functor to work with different
		 *          but compatible key types (std::string, std::string_view, const char*)
		 *          without requiring conversion to the container's key type.
		 */
		using is_transparent = void;

		[[nodiscard]] inline VISTA_SDK_CPP_CONDITIONAL_CONSTEXPR bool operator()( const std::string& lhs, const std::string& rhs ) const noexcept
		{
			return lhs.size() == rhs.size() && lhs == rhs;
		}

		[[nodiscard]] inline VISTA_SDK_CPP_CONDITIONAL_CONSTEXPR bool operator()( const std::string& lhs, std::string_view rhs ) const noexcept
		{
			return lhs.size() == rhs.size() && lhs == rhs;
		}

		[[nodiscard]] inline VISTA_SDK_CPP_CONDITIONAL_CONSTEXPR bool operator()( std::string_view lhs, const std::string& rhs ) const noexcept
		{
			return lhs.size() == rhs.size() && lhs == rhs;
		}

		[[nodiscard]] constexpr inline bool operator()( std::string_view lhs, std::string_view rhs ) const noexcept
		{
			return lhs.size() == rhs.size() && lhs == rhs;
		}

		[[nodiscard]] inline VISTA_SDK_CPP_CONDITIONAL_CONSTEXPR bool operator()( const char* lhs, const std::string& rhs ) const noexcept
		{
			std::string_view lhs_view{ lhs };
			return lhs_view.size() == rhs.size() && lhs_view == rhs;
		}

		[[nodiscard]] inline VISTA_SDK_CPP_CONDITIONAL_CONSTEXPR bool operator()( const std::string& lhs, const char* rhs ) const noexcept
		{
			std::string_view rhs_view{ rhs };
			return lhs.size() == rhs_view.size() && lhs == rhs_view;
		}

		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE constexpr bool operator()( const char* lhs, std::string_view rhs ) const noexcept
		{
			std::string_view lhs_view{ lhs };
			return lhs_view.size() == rhs.size() && lhs_view == rhs;
		}

		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE constexpr bool operator()( std::string_view lhs, const char* rhs ) const noexcept
		{
			std::string_view rhs_view{ rhs };
			return lhs.size() == rhs_view.size() && lhs == rhs_view;
		}

		[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE constexpr bool operator()( const char* lhs, const char* rhs ) const noexcept
		{
			std::string_view lhs_view{ lhs };
			std::string_view rhs_view{ rhs };
			return lhs_view.size() == rhs_view.size() && lhs_view == rhs_view;
		}
	};

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
