# ==============================================================================
# VISTA-SDK-CPP - C++ Library compilers settings
# ==============================================================================

include(CheckCXXSourceCompiles)

if(MSVC)
	# ============================================================================
	# Basic compiler settings
	# ============================================================================
	target_compile_options(${PROJECT_NAME} PRIVATE
		/std:c++20                                     # C++20 standard
		/MP                                            # Multi-processor compilation
		/W4                                            # High warning level
		/Wall                                          # All warnings
		/WX-                                           # Warnings not as errors
		/permissive-                                   # Strict conformance mode
		/fp:fast                                       # Fast floating point
		/Zc:__cplusplus                                # __cplusplus macro
		/Zc:inline                                     # Remove unreferenced COMDAT
		/Zc:preprocessor                               # Conforming preprocessor
		/external:anglebrackets                        # Treat angle bracket includes as external
		/external:W0                                   # No warnings for external headers
		/arch:AVX2                                     # Enable AVX2 (covers AVX, SSE4.2, etc.)

	# ============================================================================
	# Code analysis
	# ============================================================================
		/analyze:WX-                                   # Static analysis warnings not as errors
		/analyze:external-                             # Don't analyze external headers
		/analyze                                       # Enable static analysis

	# ============================================================================
	# Optimization settings by build configuration
	# ============================================================================

	#----------------------------------------------
	# Release builds
	#----------------------------------------------
		$<$<CONFIG:Release>:/O2>                       # Maximum speed optimization
		$<$<CONFIG:Release>:/Oi>                       # Enable intrinsic functions
		$<$<CONFIG:Release>:/Ot>                       # Favor fast code over small code
#		$<$<CONFIG:Release>:/Ob3>                      # Aggressive inlining
		$<$<CONFIG:Release>:/Gy>                       # Function-Level Linking
		$<$<CONFIG:Release>:/Qpar>                     # Auto-parallelization
		$<$<CONFIG:Release>:/DNDEBUG>                  # Disable debug assertions
		$<$<CONFIG:Release>:/GS->                      # Disable buffer security checks
		$<$<CONFIG:Release>:/Gw>                       # Optimize global data
		$<$<CONFIG:Release>:/GT>                       # Support fiber-safe TLS
		$<$<CONFIG:Release>:/fp:except->               # Disable floating point exceptions

	#----------------------------------------------
	# Debug builds
	#----------------------------------------------
		$<$<CONFIG:Debug>:/Od>                         # Disable optimizations
		$<$<CONFIG:Debug>:/MDd>                        # Debug runtime library
		$<$<CONFIG:Debug>:/Zi>                         # Debug information
		$<$<CONFIG:Debug>:/RTC1>                       # Runtime checks

	#----------------------------------------------
	# RelWithDebInfo builds
	#----------------------------------------------
		$<$<CONFIG:RelWithDebInfo>:/O2>                # Maximum speed optimization
		$<$<CONFIG:RelWithDebInfo>:/Zi>                # Debug information
		$<$<CONFIG:RelWithDebInfo>:/DNDEBUG>           # Disable debug assertions

	# ============================================================================
	# Warning suppressions
	# ============================================================================
#		/wd4061                                        # switch not handled
#		/wd4100                                        # unreferenced formal parameter
#		/wd4189                                        # local variable is initialized but not referenced
#		/wd4244                                        # conversion from 'type1' to 'type2', possible loss of data
#		/wd4267                                        # conversion from 'size_t' to 'type', possible loss of data
#		/wd4365                                        # conversion signed/unsigned mismatch
#		/wd4388                                        # signed/unsigned mismatch
#		/wd4456                                        # declaration hides previous local declaration
#		/wd4505                                        # unreferenced local function has been removed
		/wd4514                                        # unreferenced inline function has been removed
#		/wd4625                                        # copy constructor was implicitly defined as deleted
		/wd4626                                        # assignment operator was implicitly defined as deleted
		/wd4702                                        # unreachable code
		/wd4710                                        # function not inlined
		/wd4711                                        # function 'function' selected for inline expansion
		/wd4820                                        # padding
#		/wd4834                                        # discarding return value of function with 'nodiscard' attribute
		/wd4866                                        # compiler may not enforce left-to-right evaluation order for call to operator_name
#		/wd4868                                        # compiler may not enforce left-to-right evaluation order in braced initializer list
#		/wd5026                                        # move constructor was implicitly defined as deleted
#		/wd5027                                        # move assignment operator was implicitly defined as deleted
#		/wd5038                                        # will be initialized after
		/wd5045                                        # Qspectre
#		/wd5267                                        # definition of implicit copy constructor/assignment operator is deprecated because it has a user-provided assignment operator/copy constructor
#		/wd6031                                        # return value ignored: 'called-function' could return unexpected value
#		/wd6246                                        # local declaration of 'variable' hides declaration of same name in outer scope
#		/wd6326                                        # potential comparison of a constant with another constant
#		/wd6387                                        # this does not adhere to the specification for the function
	)

	target_link_options(${PROJECT_NAME} PRIVATE
		$<$<CONFIG:Release>:/OPT:REF>                  # Remove unreferenced functions
		$<$<CONFIG:Release>:/OPT:ICF>                  # Identical COMDAT folding
		$<$<CONFIG:Release>:/INCREMENTAL:NO>           # Disable incremental linking
	)
else()
	# ============================================================================
	# GCC/Clang settings
	# ============================================================================

	# ============================================================================
	# Basic compiler settings
	# ============================================================================
	target_compile_options(${PROJECT_NAME} PRIVATE
		-std=c++20                                     # C++20 standard
		-Wall                                          # All warnings
		-Wextra                                        # Extra warnings
		-Wpedantic                                     # Pedantic warnings
		-msse4.2                                       # SSE4.2 support
		-mavx                                          # AVX support
		-mavx2                                         # AVX2 support
		-mfma                                          # Fused multiply-add

	# ============================================================================
	# Optimization settings by build configuration
	# ============================================================================

	#----------------------------------------------
	# Release builds
	#----------------------------------------------
		$<$<CONFIG:Release>:-O3>                       # Maximum optimization
		$<$<CONFIG:Release>:-march=native>             # Use all available CPU features
		$<$<CONFIG:Release>:-mtune=native>             # Tune for current CPU
		$<$<CONFIG:Release>:-ffast-math>               # Fast math operations
		$<$<CONFIG:Release>:-funroll-loops>            # Unroll loops
		$<$<CONFIG:Release>:-DNDEBUG>                  # Disable assertions

	#----------------------------------------------
	# Debug builds
	#----------------------------------------------
		$<$<CONFIG:Debug>:-O0>                         # No optimization
		$<$<CONFIG:Debug>:-g>                          # Debug information
		$<$<CONFIG:Debug>:-fno-omit-frame-pointer>     # Keep frame pointer for debugging

	#----------------------------------------------
	# RelWithDebInfo builds
	#----------------------------------------------
		$<$<CONFIG:RelWithDebInfo>:-O2>                # Moderate optimization
		$<$<CONFIG:RelWithDebInfo>:-g>                 # Debug information
		$<$<CONFIG:RelWithDebInfo>:-DNDEBUG>           # Disable assertions

	# ============================================================================
	# Compiler-specific settings
	# ============================================================================
		$<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>          # Colored diagnostics
		$<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>               # Colored diagnostics
		$<$<CXX_COMPILER_ID:Clang>:-fno-pie>                          # Position independent code
		$<$<CXX_COMPILER_ID:Clang>:-fPIC>                             # Position independent code
		$<$<CXX_COMPILER_ID:Clang>:-Wno-pre-c++17-compat>             # Disable pre-C++17 warnings
		$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat>                 # Disable C++98 compatibility warnings

	# ============================================================================
	# Clang-specific optimizations
	# ============================================================================
		$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Release>>:-fvectorize>        # Enable vectorization
		$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Release>>:-fslp-vectorize>    # SLP vectorization
		$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Release>>:-ffp-contract=fast> # Fast FP contraction
	)

	target_link_options(${PROJECT_NAME} PRIVATE
		$<$<CONFIG:Release>:-s>                                                 # Strip symbols
		$<$<CONFIG:Release>:-Wl,--gc-sections>                                  # Remove unused sections
		$<$<CXX_COMPILER_ID:GNU>:-static-libgcc>                                # Static libgcc
		$<$<CXX_COMPILER_ID:GNU>:-static-libstdc++>                             # Static libstdc++
	)
endif()
