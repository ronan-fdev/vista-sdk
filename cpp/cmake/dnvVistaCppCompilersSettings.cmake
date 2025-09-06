# ==============================================================================
# VISTA-SDK-CPP - C++ Library compilers settings
# ==============================================================================

include(CheckCXXSourceCompiles)

set(TARGETS_TO_CONFIGURE "")
if(VISTA_SDK_CPP_BUILD_SHARED AND TARGET ${PROJECT_NAME})
	list(APPEND TARGETS_TO_CONFIGURE ${PROJECT_NAME})
endif()
if(VISTA_SDK_CPP_BUILD_STATIC AND TARGET ${PROJECT_NAME}-static)
	list(APPEND TARGETS_TO_CONFIGURE ${PROJECT_NAME}-static)
endif()

foreach(target_name ${TARGETS_TO_CONFIGURE})
	if(MSVC)
		# ============================================================================
		# Basic compiler settings
		# ============================================================================
		target_compile_options(${target_name} PRIVATE
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
#			/analyze:WX-                                   # Static analysis warnings not as errors
			$<$<CXX_COMPILER_ID:MSVC>:/analyze:WX->
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
#			$<$<CONFIG:Release>:/Ob3>                      # Aggressive inlining
			$<$<CONFIG:Release>:/Gy>                       # Function-Level Linking
			$<$<CONFIG:Release>:/Qpar>                     # Auto-parallelization
			$<$<CONFIG:Release>:/DNDEBUG>                  # Disable debug assertions
			$<$<CONFIG:Release>:/GS->                      # Disable buffer security checks
			$<$<CONFIG:Release>:/Gw>                       # Optimize global data
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
#			/wd4061                                                   # switch not handled
#			/wd4100                                                   # unreferenced formal parameter
#			/wd4189                                                   # local variable is initialized but not referenced
#			/wd4244                                                   # conversion from 'type1' to 'type2', possible loss of data
#			/wd4267                                                   # conversion from 'size_t' to 'type', possible loss of data
#			/wd4365                                                   # conversion signed/unsigned mismatch
#			/wd4388                                                   # signed/unsigned mismatch
#			/wd4456                                                   # declaration hides previous local declaration
#			/wd4505                                                   # unreferenced local function has been removed
			/wd4514                                                   # unreferenced inline function has been removed
#			/wd4625                                                   # copy constructor was implicitly defined as deleted
			/wd4626                                                   # assignment operator was implicitly defined as deleted
			/wd4702                                                   # unreachable code
			/wd4710                                                   # function not inlined
			/wd4711                                                   # function 'function' selected for inline expansion
			/wd4820                                                   # padding
#			/wd4834                                                   # discarding return value of function with 'nodiscard' attribute
			/wd4866                                                   # compiler may not enforce left-to-right evaluation order for call to operator_name
			/wd4868                                                   # compiler may not enforce left-to-right evaluation order in braced initializer list
#			/wd5026                                                   # move constructor was implicitly defined as deleted
#			/wd5027                                                   # move assignment operator was implicitly defined as deleted
#			/wd5038                                                   # will be initialized after
			/wd5045                                                   # Qspectre
#			/wd5267                                                   # definition of implicit copy constructor/assignment operator is deprecated because it has a user-provided assignment operator/copy constructor
#			/wd6031                                                   # return value ignored: 'called-function' could return unexpected value
#			/wd6246                                                   # local declaration of 'variable' hides declaration of same name in outer scope
#			/wd6326                                                   # potential comparison of a constant with another constant
#			/wd6387                                                   # this does not adhere to the specification for the function
			$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat>             # Disable C++98 compatibility warnings
			$<$<CXX_COMPILER_ID:Clang>:-Wno-pre-c++17-compat>         # Disable pre-C++17 warnings
			$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat-pedantic>    # Suppress C++98 pedantic compatibility warnings
			$<$<CXX_COMPILER_ID:Clang>:-Wno-exit-time-destructors>    # Suppress exit-time destructor warnings
			$<$<CXX_COMPILER_ID:Clang>:-Wno-unsafe-buffer-usage>      # Suppress unsafe buffer usage warnings (Clang diagnostic)
			$<$<CXX_COMPILER_ID:Clang>:-Wno-covered-switch-default>   # Allow default in fully covered switch
			$<$<CXX_COMPILER_ID:Clang>:-Wno-switch-default>           # Allow switches without default when all enum values are covered
			$<$<CXX_COMPILER_ID:GNU>:-Wno-deprecated-declarations>    # Suppress POSIX deprecation warnings
			$<$<CXX_COMPILER_ID:Clang>:-Wno-deprecated-declarations>  # Suppress POSIX deprecation warnings
		)

		target_link_options(${target_name} PRIVATE
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
		target_compile_options(${target_name} PRIVATE
			-std=c++20                                                         # C++20 standard
			-Wall                                                              # All warnings
			-Wextra                                                            # Extra warnings
			-Wpedantic                                                         # Pedantic warnings
			$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-msse4.2>  # SSE4.2 support
			$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-mcrc32>   # CRC32 support
			-mavx                                                              # AVX support
			-mavx2                                                             # AVX2 support
			-mfma                                                              # Fused multiply-add

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
			$<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>                    # Colored diagnostics
			$<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>                         # Colored diagnostics

			$<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Windows>>:-fno-pie>      # Position independent code
			$<$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<PLATFORM_ID:Windows>>>:-fPIC>  # Position independent code
			$<$<AND:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Windows>>:-fno-pie>        # Position independent code
			$<$<AND:$<CXX_COMPILER_ID:GNU>,$<NOT:$<PLATFORM_ID:Windows>>>:-fPIC>    # Position independent code

			$<$<CXX_COMPILER_ID:Clang>:-Wno-pre-c++17-compat>                       # Disable pre-C++17 warnings
			$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat>                           # Disable C++98 compatibility warnings

		# ============================================================================
		# Clang-specific optimizations
		# ============================================================================
			$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Release>>:-fvectorize>        # Enable vectorization
			$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Release>>:-fslp-vectorize>    # SLP vectorization
			$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Release>>:-ffp-contract=fast> # Fast FP contraction

		# ============================================================================
		# Warning suppressions
		# ============================================================================
			$<$<CXX_COMPILER_ID:GNU>:-Wno-pedantic>                                   # Suppress pedantic warnings for all GCC (like __int128 warnings)
			$<$<CXX_COMPILER_ID:GNU>:-Wno-maybe-uninitialized>                        # Suppress false positive warnings
			$<$<CXX_COMPILER_ID:Clang>:-Wno-system-headers>                           # Suppress warnings from system headers
			$<$<CXX_COMPILER_ID:Clang>:-Wno-covered-switch-default>                   # Allow default in fully covered switch
			$<$<CXX_COMPILER_ID:Clang>:-Wno-switch-default>                           # Allow switches without default when all enum values are covered
			$<$<CXX_COMPILER_ID:GNU>:-Wno-deprecated-declarations>                    # Suppress POSIX deprecation warnings
			$<$<CXX_COMPILER_ID:Clang>:-Wno-deprecated-declarations>                  # Suppress POSIX deprecation warnings
		)

		target_link_options(${target_name} PRIVATE
			$<$<CONFIG:Release>:-s>                                                 # Strip symbols
			$<$<CONFIG:Release>:-Wl,--gc-sections>                                  # Remove unused sections
			$<$<CXX_COMPILER_ID:GNU>:-static-libgcc>                                # Static libgcc
			$<$<CXX_COMPILER_ID:GNU>:-static-libstdc++>                             # Static libstdc++
		)
	endif()

	# ============================================================================
	# Extract and format target properties for summary display
	# ============================================================================

	#----------------------------------------------
	# Compile flags
	#----------------------------------------------

	get_target_property(CUSTOM_COMPILE_FLAGS ${target_name} COMPILE_OPTIONS)

	if(CUSTOM_COMPILE_FLAGS)
		set(FORMATTED_COMPILE_FLAGS "")

		set(LABEL_TEXT "-- Custom compile flags            : ")
		string(LENGTH "${LABEL_TEXT}" LABEL_LENGTH)
		string(REPEAT " " ${LABEL_LENGTH} INDENT_SPACES)

		foreach(option ${CUSTOM_COMPILE_FLAGS})
			string(APPEND FORMATTED_COMPILE_FLAGS "${option}\n${INDENT_SPACES}")
		endforeach()

		string(REGEX REPLACE "\n[^\n]*$" "" FORMATTED_COMPILE_FLAGS "${FORMATTED_COMPILE_FLAGS}")
	else()
		set(FORMATTED_COMPILE_FLAGS "(none)")
	endif()

	#----------------------------------------------
	# Link flags
	#----------------------------------------------

	get_target_property(CUSTOM_LINK_OPTIONS ${target_name} LINK_OPTIONS)

	if(CUSTOM_LINK_OPTIONS)
		set(FORMATTED_LINK_OPTIONS "")

		set(LABEL_TEXT "-- Custom link options             : ")
		string(LENGTH "${LABEL_TEXT}" LABEL_LENGTH)
		string(REPEAT " " ${LABEL_LENGTH} INDENT_SPACES)

		foreach(option ${CUSTOM_LINK_OPTIONS})
			string(APPEND FORMATTED_LINK_OPTIONS "${option}\n${INDENT_SPACES}")
		endforeach()

		string(REGEX REPLACE "\n[^\n]*$" "" FORMATTED_LINK_OPTIONS "${FORMATTED_LINK_OPTIONS}")
	else()
		set(FORMATTED_LINK_OPTIONS "(none)")
	endif()

	#----------------------------------------------
	# Compile definitions
	#----------------------------------------------

	get_target_property(CUSTOM_COMPILE_DEFINITIONS ${target_name} COMPILE_DEFINITIONS)

	if(CUSTOM_COMPILE_DEFINITIONS)
		string(JOIN " " FORMATTED_COMPILE_DEFINITIONS ${CUSTOM_COMPILE_DEFINITIONS})
	else()
		set(FORMATTED_COMPILE_DEFINITIONS "(none)")
	endif()
endforeach()
