/**
 * @file StringUtils.h
 * @brief String utilities and heterogeneous lookup support for Vista SDK
 * @details Contains performance-critical string utilities including
 *          heterogeneous lookup functors for zero-copy string_view operations.
 */

#pragma once

#include "dnv/vista/sdk/config/Platform.h"

namespace dnv::vista::sdk::utils
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
	// Performance-critical string utilities
	//=====================================================================

	/**
	 * @brief Fast check if string ends with suffix
	 * @param str String to check
	 * @param suffix Suffix to find
	 * @return True if str ends with suffix
	 */
	[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE constexpr bool endsWith( std::string_view str, std::string_view suffix ) noexcept
	{
		return str.size() >= suffix.size() && str.compare( str.size() - suffix.size(), suffix.size(), suffix ) == 0;
	}

	/**
	 * @brief Fast check if string starts with prefix
	 * @param str String to check
	 * @param prefix Prefix to find
	 * @return True if str starts with prefix
	 */
	[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE constexpr bool startsWith( std::string_view str, std::string_view prefix ) noexcept
	{
		return str.size() >= prefix.size() && str.compare( 0, prefix.size(), prefix ) == 0;
	}

	/**
	 * @brief Fast check if string contains substring
	 * @param str String to check
	 * @param substr Substring to find
	 * @return True if str contains substr
	 */
	[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE constexpr bool contains( std::string_view str, std::string_view substr ) noexcept
	{
		return str.find( substr ) != std::string_view::npos;
	}

	/**
	 * @brief Fast case-sensitive string comparison
	 * @param lhs First string
	 * @param rhs Second string
	 * @return True if strings are exactly equal
	 */
	[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE constexpr bool equals( std::string_view lhs, std::string_view rhs ) noexcept
	{
		return lhs == rhs;
	}

	/**
	 * @brief Fast case-insensitive string comparison
	 * @param lhs First string
	 * @param rhs Second string
	 * @return True if strings are equal (case-insensitive)
	 */
	[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE bool iequals( std::string_view lhs, std::string_view rhs ) noexcept
	{
		if ( lhs.size() != rhs.size() )
		{
			return false;
		}

		return std::equal( lhs.begin(), lhs.end(), rhs.begin(),
			[]( char a, char b ) noexcept { return std::tolower( static_cast<unsigned char>( a ) ) ==
												   std::tolower( static_cast<unsigned char>( b ) ); } );
	}

	//=====================================================================
	// StringViewSplitter class
	//=====================================================================

	/**
	 * @brief Zero-allocation string splitting iterator for performance-critical paths
	 * @details Provides efficient string_view-based splitting without heap allocations
	 */
	class StringViewSplitter
	{
	private:
		std::string_view m_str;
		char m_delimiter;

	public:
		VISTA_SDK_CPP_FORCE_INLINE explicit StringViewSplitter( std::string_view str, char delimiter ) noexcept
			: m_str{ str },
			  m_delimiter{ delimiter } {}

		/**
		 * @brief Forward iterator for string segments
		 */
		class Iterator
		{
		private:
			const StringViewSplitter* m_splitter;
			size_t m_currentPos;
			std::string_view m_currentSegment;
			bool m_isAtEnd;

		public:
			/**
			 * @brief Constructs iterator at beginning or end position
			 */
			VISTA_SDK_CPP_FORCE_INLINE explicit Iterator( const StringViewSplitter* splitter, bool at_end = false ) noexcept
				: m_splitter{ splitter },
				  m_currentPos{ 0 },
				  m_isAtEnd{ at_end }
			{
				if ( !m_isAtEnd )
				{
					advance();
				}
			}

			/**
			 * @brief Dereferences iterator to get current string segment
			 */
			VISTA_SDK_CPP_FORCE_INLINE std::string_view operator*() const noexcept
			{
				return m_currentSegment;
			}

			VISTA_SDK_CPP_FORCE_INLINE Iterator& operator++() noexcept
			{
				advance();
				return *this;
			}

			/**
			 * @brief Compares iterators for range-based loops
			 */
			VISTA_SDK_CPP_FORCE_INLINE bool operator!=( const Iterator& other ) const noexcept
			{
				return m_isAtEnd != other.m_isAtEnd;
			}

		private:
			/**
			 * @brief Advances to next segment using efficient string_view operations
			 */
			VISTA_SDK_CPP_FORCE_INLINE void advance() noexcept
			{
				if ( m_currentPos >= m_splitter->m_str.length() )
				{
					m_isAtEnd = true;
					return;
				}

				const size_t start = m_currentPos;
				const size_t delimiterPos = m_splitter->m_str.find( m_splitter->m_delimiter, start );

				if ( delimiterPos == std::string_view::npos )
				{
					m_currentSegment = m_splitter->m_str.substr( start );
					m_currentPos = m_splitter->m_str.length();
				}
				else
				{
					m_currentSegment = m_splitter->m_str.substr( start, delimiterPos - start );
					m_currentPos = delimiterPos + 1;
				}
			}
		};

		/**
		 * @brief Returns iterator to first segment
		 */
		VISTA_SDK_CPP_FORCE_INLINE Iterator begin() const noexcept
		{
			return Iterator( this );
		}

		/**
		 * @brief Returns end iterator for range-based loops
		 */
		VISTA_SDK_CPP_FORCE_INLINE Iterator end() const noexcept
		{
			return Iterator( this, true );
		}
	};

	/**
	 * @brief Factory function for zero-copy string splitting
	 */
	[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE StringViewSplitter splitView( std::string_view str, char delimiter ) noexcept
	{
		return StringViewSplitter( str, delimiter );
	}

	//=====================================================================
	// String validation utilities
	//=====================================================================

	/**
	 * @brief Fast check if string has exact length
	 * @param str String to check
	 * @param expectedLength Expected length
	 * @return True if string has exact expected length
	 */
	[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE constexpr bool hasExactLength( std::string_view str, std::size_t expectedLength ) noexcept
	{
		return str.size() == expectedLength;
	}

	/**
	 * @brief Fast check if string is empty
	 * @param str String to check
	 * @return True if string is empty
	 */
	[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE constexpr bool isEmpty( std::string_view str ) noexcept
	{
		return str.empty();
	}

	//=====================================================================
	// High-performance parsing utilities
	//=====================================================================

	/**
	 * @brief Fast boolean parsing with error handling
	 * @param str String to parse
	 * @param result Output boolean value
	 * @return True if parsing succeeded, false otherwise
	 */
	[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE bool tryParseBool( std::string_view str, bool& result ) noexcept
	{
		if ( str == "true" )
		{
			result = true;
			return true;
		}
		else if ( str == "false" )
		{
			result = false;
			return true;
		}
		return false;
	}

	/**
	 * @brief Fast integer parsing with error handling
	 * @param str String to parse
	 * @param result Output integer value
	 * @return True if parsing succeeded, false otherwise
	 */
	[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE bool tryParseInt( std::string_view str, int& result ) noexcept
	{
		if ( str.empty() )
		{
			return false;
		}

		const auto parseResult = std::from_chars( str.data(), str.data() + str.size(), result );
		return parseResult.ec == std::errc{} && parseResult.ptr == str.data() + str.size();
	}

	/**
	 * @brief Fast unsigned integer parsing with error handling
	 * @param str String to parse
	 * @param result Output unsigned integer value
	 * @return True if parsing succeeded, false otherwise
	 */
	[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE bool tryParseUInt( std::string_view str, std::uint32_t& result ) noexcept
	{
		if ( str.empty() )
		{
			return false;
		}

		const auto parseResult = std::from_chars( str.data(), str.data() + str.size(), result );
		return parseResult.ec == std::errc{} && parseResult.ptr == str.data() + str.size();
	}

	/**
	 * @brief Fast long integer parsing with error handling
	 * @param str String to parse
	 * @param result Output long integer value
	 * @return True if parsing succeeded, false otherwise
	 */
	[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE bool tryParseLong( std::string_view str, std::int64_t& result ) noexcept
	{
		if ( str.empty() )
		{
			return false;
		}

		const auto parseResult = std::from_chars( str.data(), str.data() + str.size(), result );
		return parseResult.ec == std::errc{} && parseResult.ptr == str.data() + str.size();
	}

	/**
	 * @brief Fast double parsing with error handling
	 * @param str String to parse
	 * @param result Output double value
	 * @return True if parsing succeeded, false otherwise
	 */
	[[nodiscard]] VISTA_SDK_CPP_FORCE_INLINE bool tryParseDouble( std::string_view str, double& result ) noexcept
	{
		if ( str.empty() )
		{
			return false;
		}

		const auto parseResult = std::from_chars( str.data(), str.data() + str.size(), result );
		return parseResult.ec == std::errc{} && parseResult.ptr == str.data() + str.size();
	}
}
