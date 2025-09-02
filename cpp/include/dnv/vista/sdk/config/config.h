/**
 * @file config.h
 * @brief Cross-platform compiler support and performance optimization macros
 * @details Provides SIMD intrinsics headers and force inline directives for
 *          maximum performance across MSVC, GCC, and Clang compilers
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
#	define VISTA_SDK_CPP_INLINE __forceinline
#elif defined( __GNUC__ ) || defined( __clang__ )
#	define VISTA_SDK_CPP_INLINE __attribute__( ( always_inline ) ) inline
#else
#	define VISTA_SDK_CPP_INLINE inline
#endif
