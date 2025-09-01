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
