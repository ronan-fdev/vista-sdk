# ==============================================================================
# VISTA-SDK-CPP - C++ Library compilers settings
# ==============================================================================

#----------------------------------------------
# Target collection
#----------------------------------------------

set(TARGETS_TO_CONFIGURE "")
if(VISTA_SDK_CPP_BUILD_SHARED AND TARGET ${PROJECT_NAME})
	list(APPEND TARGETS_TO_CONFIGURE ${PROJECT_NAME})
endif()
if(VISTA_SDK_CPP_BUILD_STATIC AND TARGET ${PROJECT_NAME}-static)
	list(APPEND TARGETS_TO_CONFIGURE ${PROJECT_NAME}-static)
endif()

#----------------------------------------------
# Compiler configuration loop
#----------------------------------------------

foreach(target_name ${TARGETS_TO_CONFIGURE})

	#----------------------------------------------
	# Target-specific compiler optimisation
	#----------------------------------------------

	target_compile_options(${target_name} PRIVATE

		#-----------------------------
		# MSVC
		#-----------------------------

		# --- Settings ---
		$<$<CXX_COMPILER_ID:MSVC>:/std:c++20>                              # C++20 standard
		$<$<CXX_COMPILER_ID:MSVC>:/MP>                                     # Multi-processor compilation
		$<$<CXX_COMPILER_ID:MSVC>:/W4>                                     # High warning level
		$<$<CXX_COMPILER_ID:MSVC>:/Wall>                                   # All warnings
		$<$<CXX_COMPILER_ID:MSVC>:/WX->                                    # Warnings not as errors
		$<$<CXX_COMPILER_ID:MSVC>:/permissive->                            # Strict conformance mode
		$<$<CXX_COMPILER_ID:MSVC>:/fp:fast>                                # Fast floating point
		$<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>                         # __cplusplus macro
		$<$<CXX_COMPILER_ID:MSVC>:/Zc:inline>                              # Remove unreferenced COMDAT
		$<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>                        # Conforming preprocessor
		$<$<CXX_COMPILER_ID:MSVC>:/external:anglebrackets>                 # Treat angle bracket includes as external
		$<$<CXX_COMPILER_ID:MSVC>:/external:W0>                            # No warnings for external headers
		$<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>                              # Enable AVX2 (covers AVX, SSE4.2, etc.)

		# --- Code Analysis ---
		$<$<CXX_COMPILER_ID:MSVC>:/analyze:WX->                            # Static analysis warnings not as errors
		$<$<CXX_COMPILER_ID:MSVC>:/analyze:external->                      # Don't analyze external headers
		$<$<CXX_COMPILER_ID:MSVC>:/analyze>                                # Enable static analysis

		# --- Release Optimization ---
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/O2>            # Maximum speed optimization
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Oi>            # Enable intrinsic functions
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Ot>            # Favor fast code over small code
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Gy>            # Function-Level Linking
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Qpar>          # Auto-parallelization
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/DNDEBUG>       # Disable debug assertions
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/GS->           # Disable buffer security checks
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Gw>            # Optimize global data
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/fp:except->    # Disable floating point exceptions

		# --- Debug Settings ---
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Od>              # Disable optimizations
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/MDd>             # Debug runtime library
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>              # Debug information
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/RTC1>            # Runtime checks

		# --- RelWithDebInfo Settings ---
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RelWithDebInfo>>:/O2>      # Maximum speed optimization
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RelWithDebInfo>>:/Zi>      # Debug information
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RelWithDebInfo>>:/DNDEBUG> # Disable debug assertions

		# --- Warning Suppressions ---
		$<$<CXX_COMPILER_ID:MSVC>:/wd4514>                                 # unreferenced inline function has been removed
		$<$<CXX_COMPILER_ID:MSVC>:/wd4710>                                 # function not inlined
		$<$<CXX_COMPILER_ID:MSVC>:/wd4711>                                 # function selected for inline expansion
		$<$<CXX_COMPILER_ID:MSVC>:/wd4820>                                 # padding
		$<$<CXX_COMPILER_ID:MSVC>:/wd4866>                                 # compiler may not enforce left-to-right evaluation order for call to operator_name
		$<$<CXX_COMPILER_ID:MSVC>:/wd4868>                                 # compiler may not enforce left-to-right evaluation order in braced initializer list
		$<$<CXX_COMPILER_ID:MSVC>:/wd5045>                                 # Qspectre

		#-----------------------------
		# GCC/Clang
		#-----------------------------

		# --- Common Settings ---
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-std=c++20>   # C++20 standard
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall>        # All warnings
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wextra>      # Extra warnings
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wpedantic>   # Pedantic warnings
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-msse4.2>     # SSE4.2 support
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-mavx>        # AVX support
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-mavx2>       # AVX2 support
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-mfma>        # Fused multiply-add

		# --- GCC-specific Settings ---
		$<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>                  # Colored diagnostics

		# --- Clang-specific Settings ---
		$<$<CXX_COMPILER_ID:Clang>:-mcrc32>                                   # CRC32 support
		$<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>                       # Colored diagnostics

		# --- Release Optimization ---
		$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-O3>                   # Maximum optimization
		$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-march=native>         # Use all available CPU features
		$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-mtune=native>         # Tune for current CPU
		$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-ffast-math>           # Fast math operations
		$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-funroll-loops>        # Unroll loops
		$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-DNDEBUG>              # Disable assertions

		# --- Debug Settings ---
		$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Debug>>:-O0>                     # No optimization
		$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Debug>>:-g>                      # Debug information
		$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Debug>>:-fno-omit-frame-pointer> # Keep frame pointer for debugging

		# --- RelWithDebInfo Settings ---
		$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:RelWithDebInfo>>:-O2>            # Moderate optimization
		$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:RelWithDebInfo>>:-g>             # Debug information
		$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:RelWithDebInfo>>:-DNDEBUG>       # Disable assertions

		# --- Position Independent Code ---
		$<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Windows>>:-fno-pie>     # Position independent code (Windows)
		$<$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<PLATFORM_ID:Windows>>>:-fPIC> # Position independent code (Non-Windows)
		$<$<AND:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Windows>>:-fno-pie>       # Position independent code (Windows)
		$<$<AND:$<CXX_COMPILER_ID:GNU>,$<NOT:$<PLATFORM_ID:Windows>>>:-fPIC>   # Position independent code (Non-Windows)

		# --- Clang-specific Optimizations ---
		$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Release>>:-fvectorize>        # Enable vectorization
		$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Release>>:-fslp-vectorize>    # SLP vectorization
		$<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Release>>:-ffp-contract=fast> # Fast FP contraction

		# --- Warning Suppressions ---
		$<$<CXX_COMPILER_ID:GNU>:-Wno-pedantic>                               # Suppress pedantic warnings for all GCC
		$<$<CXX_COMPILER_ID:GNU>:-Wno-maybe-uninitialized>                    # Suppress false positive warnings
		$<$<CXX_COMPILER_ID:GNU>:-Wno-deprecated-declarations>                # Suppress POSIX deprecation warnings

		$<$<CXX_COMPILER_ID:Clang>:-Wno-system-headers>                       # Suppress warnings from system headers
		$<$<CXX_COMPILER_ID:Clang>:-Wno-pre-c++17-compat>                     # Disable pre-C++17 warnings
		$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat>                         # Disable C++98 compatibility warnings
		$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat-pedantic>                # Suppress C++98 pedantic compatibility warnings
		$<$<CXX_COMPILER_ID:Clang>:-Wno-exit-time-destructors>                # Suppress exit-time destructor warnings
		$<$<CXX_COMPILER_ID:Clang>:-Wno-unsafe-buffer-usage>                  # Suppress unsafe buffer usage warnings
		$<$<CXX_COMPILER_ID:Clang>:-Wno-covered-switch-default>               # Allow default in fully covered switch
		$<$<CXX_COMPILER_ID:Clang>:-Wno-switch-default>                       # Allow switches without default when all enum values are covered
		$<$<CXX_COMPILER_ID:Clang>:-Wno-deprecated-declarations>              # Suppress POSIX deprecation warnings
	)

	#----------------------------------------------
	# Target-specific linker settings
	#----------------------------------------------

	target_link_options(${target_name} PRIVATE

		#-----------------------------
		# MSVC
		#-----------------------------

		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/OPT:REF>          # Remove unreferenced functions
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/OPT:ICF>          # Identical COMDAT folding
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/INCREMENTAL:NO>   # Disable incremental linking

		#-----------------------------
		# GCC/Clang
		#-----------------------------

		$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-s>                     # Strip symbols
		$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-Wl,--gc-sections>      # Remove unused sections
		$<$<CXX_COMPILER_ID:GNU>:-static-libgcc>                              # Static libgcc
		$<$<CXX_COMPILER_ID:GNU>:-static-libstdc++>                           # Static libstdc++
	)

	#----------------------------------------------
	# Extract and format target properties
	#----------------------------------------------

	#-----------------------------
	# Compile flags
	#-----------------------------

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

	#-----------------------------
	# Link flags
	#-----------------------------

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

	#-----------------------------
	# Compile definitions
	#-----------------------------

	get_target_property(CUSTOM_COMPILE_DEFINITIONS ${target_name} COMPILE_DEFINITIONS)

	if(CUSTOM_COMPILE_DEFINITIONS)
		string(JOIN " " FORMATTED_COMPILE_DEFINITIONS ${CUSTOM_COMPILE_DEFINITIONS})
	else()
		set(FORMATTED_COMPILE_DEFINITIONS "(none)")
	endif()
endforeach()
