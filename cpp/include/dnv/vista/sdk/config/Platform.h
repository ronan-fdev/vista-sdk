/**
 * @file Platform.h
 * @brief Cross-platform compiler support and hardware optimization macros
 * @details Provides SIMD intrinsics, force inline directives, and compiler-specific
 *          feature detection for maximum performance across MSVC, GCC, and Clang
 */

#pragma once

//=====================================================================
// Cross-platform compiler support
//=====================================================================

//----------------------------------------------
// Compiler intrinsics headers
//----------------------------------------------

/** @brief Platform-specific intrinsics headers for SIMD and hardware optimization support */
#ifdef _MSC_VER
#	include <intrin.h>
#elif defined( __GNUC__ ) || defined( __clang__ )
#	include <immintrin.h>
#	include <x86intrin.h>
#endif

//----------------------------------------------
// Cross-compiler performance macros
//----------------------------------------------

/** @brief Cross-compiler force inline directive for performance-critical functions */
#if defined( _MSC_VER )
#	define VISTA_SDK_CPP_FORCE_INLINE __forceinline
#elif defined( __GNUC__ ) || defined( __clang__ )
#	define VISTA_SDK_CPP_FORCE_INLINE __attribute__( ( always_inline ) ) inline
#else
#	define VISTA_SDK_CPP_FORCE_INLINE inline
#endif

//----------------------------------------------
// Compiler-specific C++20 feature support
//----------------------------------------------

/** @brief Conditional constexpr support for GCC 11.x which has incomplete constexpr std::string support */
#if defined( __GNUC__ ) && __GNUC__ >= 11 && __GNUC__ < 12
#	define VISTA_SDK_CPP_CONDITIONAL_CONSTEXPR
#else
#	define VISTA_SDK_CPP_CONDITIONAL_CONSTEXPR constexpr
#endif

/** @brief No unique address attribute for zero-cost empty member optimization */
#if defined( __clang__ )
/* Clang or Clang-CL: Use feature detection regardless of _MSC_VER */
#	if __has_cpp_attribute( no_unique_address )
#		define VISTA_SDK_CPP_NO_UNIQUE_ADDRESS [[no_unique_address]]
#	else
#		define VISTA_SDK_CPP_NO_UNIQUE_ADDRESS
#	endif
#elif defined( _MSC_VER ) && _MSC_VER >= 1928
/* MSVC */
#	define VISTA_SDK_CPP_NO_UNIQUE_ADDRESS [[no_unique_address]]
#elif defined( __has_cpp_attribute ) && __has_cpp_attribute( no_unique_address ) >= 201803L
#	if defined( __cpp_lib_no_unique_address ) && __cpp_lib_no_unique_address >= 201803L
#		define VISTA_SDK_CPP_NO_UNIQUE_ADDRESS [[no_unique_address]]
#	else
#		define VISTA_SDK_CPP_NO_UNIQUE_ADDRESS
#	endif
#else
#	define VISTA_SDK_CPP_NO_UNIQUE_ADDRESS
#endif

//----------------------------------------------
// Cross-platform 128-bit integer support
//----------------------------------------------

/**
 * @brief Cross-platform 128-bit integer support detection
 * @details Detects native __int128 support for high-performance decimal arithmetic.
 *          - GCC/Clang: Native __int128 support since GCC 4.6+ and Clang 3.1+
 *          - MSVC: No native 128-bit support, requires manual implementation
 */
#if defined( __SIZEOF_INT128__ ) && !defined( _MSC_VER )
/* GCC and Clang have native __int128 support */
#	define VISTA_SDK_CPP_HAS_INT128 1
#	define VISTA_SDK_CPP_INT128 __int128
#	define VISTA_SDK_CPP_UINT128 unsigned __int128
#else
/* MSVC and other compilers without native 128-bit support */
#	define VISTA_SDK_CPP_HAS_INT128 0
/* For manual 128-bit implementation, we'll use our custom Int128 struct */
#endif

/** @brief Conditional compilation helper for 128-bit specific code paths */
#if VISTA_SDK_CPP_HAS_INT128
#	define VISTA_SDK_CPP_IF_INT128( code ) code
#	define VISTA_SDK_CPP_IF_NO_INT128( code )
#else
#	define VISTA_SDK_CPP_IF_INT128( code )
#	define VISTA_SDK_CPP_IF_NO_INT128( code ) code
#endif

//----------------------------------------------
// Cross-platform time functions
//----------------------------------------------

/** @brief Windows + GCC specific fixes */
// #if defined( _WIN32 ) && defined( __GNUC__ )
// /* Fix missing POSIX functions on Windows with GCC */
// #	ifndef _POSIX_THREAD_SAFE_FUNCTIONS
// #		define _POSIX_THREAD_SAFE_FUNCTIONS
// #	endif
// #endif

/** @brief Cross-platform thread-safe time conversion functions */
#if defined( _WIN32 )
/* Windows uses gmtime_s and localtime_s (different parameter order than POSIX) */
#	define VISTA_SDK_CPP_GMTIME_R( timer, result ) ( gmtime_s( ( result ), ( timer ) ) == 0 )
#	define VISTA_SDK_CPP_LOCALTIME_R( timer, result ) ( localtime_s( ( result ), ( timer ) ) == 0 )
#elif defined( __GNUC__ ) || defined( __clang__ )
/* POSIX systems use gmtime_r and localtime_r */
#	define VISTA_SDK_CPP_GMTIME_R( timer, result ) ( gmtime_r( ( timer ), ( result ) ) != nullptr )
#	define VISTA_SDK_CPP_LOCALTIME_R( timer, result ) ( localtime_r( ( timer ), ( result ) ) != nullptr )
#else
/* Fallback to non-thread-safe versions */
#	define VISTA_SDK_CPP_GMTIME_R( timer, result ) ( ( *( result ) = *gmtime( timer ) ), true )
#	define VISTA_SDK_CPP_LOCALTIME_R( timer, result ) ( ( *( result ) = *localtime( timer ) ), true )
#endif

// //----------------------------------------------
// // C++20 timezone support detection
// //----------------------------------------------

// /** @brief C++20 timezone support availability */
// #if defined( __cpp_lib_chrono ) && __cpp_lib_chrono >= 201907L
// #	define VISTA_SDK_CPP_HAS_TIMEZONE 1
// #else
// #	define VISTA_SDK_CPP_HAS_TIMEZONE 0
// #endif

// /** @brief Conditional compilation for timezone-aware code */
// #if VISTA_SDK_CPP_HAS_TIMEZONE
// #	define VISTA_SDK_CPP_IF_TIMEZONE( code ) code
// #	define VISTA_SDK_CPP_IF_NO_TIMEZONE( code )
// #else
// #	define VISTA_SDK_CPP_IF_TIMEZONE( code )
// #	define VISTA_SDK_CPP_IF_NO_TIMEZONE( code ) code
// #endif
