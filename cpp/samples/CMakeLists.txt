# ==============================================================================
# VISTA-SDK-CPP - Samples
# ==============================================================================

# --- Test condition check ---
if(NOT VISTA_SDK_CPP_BUILD_SAMPLES)
	message(STATUS "Samples disabled, skipping...")
	return()
endif()

# --- Sample source files ---
set(SAMPLE_SOURCES
	Sample_BasicUsage.cpp
	Sample_Codebooks.cpp
	Sample_GMOD.cpp
	Sample_LocalId.cpp
	Sample_TimeSeriesDataPackage.cpp
)

# --- Configure sample executables ---
foreach(sample_source ${SAMPLE_SOURCES})
	get_filename_component(sample_target_name ${sample_source} NAME_WE)

	if(NOT TARGET ${sample_target_name})
		add_executable(${sample_target_name} ${sample_source})

		# --- Target-specific MSVC optimisation ---
		if(MSVC)
			target_compile_options(${sample_target_name} PRIVATE
				/std:c++20                             # C++20 standard
				/MP                                    # Multi-processor compilation
				/W4                                    # High warning level
				/Wall                                  # All warnings
				/WX-                                   # Warnings not as errors
				/permissive-                           # Strict conformance mode
				/fp:fast                               # Fast floating point
				/Zc:__cplusplus                        # __cplusplus macro
				/Zc:inline                             # Remove unreferenced COMDAT
				/Zc:preprocessor                       # Conforming preprocessor
				/external:anglebrackets                # Treat angle bracket includes as external
				/external:W0                           # No warnings for external headers
				/arch:AVX2                             # Enable AVX2 (covers AVX, SSE4.2, etc.)

				# Optimization settings
				$<$<CONFIG:Release>:/O2>               # Maximum speed optimization
				$<$<CONFIG:Release>:/Oi>               # Enable intrinsic functions
				$<$<CONFIG:Release>:/Ot>               # Favor fast code over small code
#				$<$<CONFIG:Release>:/Ob3>              # Aggressive inlining
				$<$<CONFIG:Release>:/Gy>               # Function-Level Linking
				$<$<CONFIG:Release>:/Qpar>             # Auto-parallelization
				$<$<CONFIG:Release>:/DNDEBUG>          # Disable debug assertions
				$<$<CONFIG:Release>:/GS->              # Disable buffer security checks
				$<$<CONFIG:Release>:/Gw>               # Optimize global data
				$<$<CONFIG:Release>:/fp:except->       # Disable floating point exceptions

				# Warning suppressions
				/wd4244                                # conversion from 'type1' to 'type2', possible loss of data
				/wd4266                                # no override available for virtual member function from base 'type'; function is hidden
				/wd4505                                # unreferenced local function has been removed
				/wd4514                                # unreferenced inline function has been removed
				/wd4625                                # copy constructor was implicitly defined as deleted
				/wd4626                                # assignment operator was implicitly defined as deleted
				/wd4868                                # compiler may not enforce left-to-right evaluation order in braced initializer list
				/wd4710                                # function not inlined
				/wd4711                                # function 'function' selected for inline expansion
				/wd4820                                # padding
				/wd4834                                # discarding return value of function with 'nodiscard' attribute
				/wd5026                                # move constructor was implicitly defined as deleted
				/wd5027                                # move assignment operator was implicitly defined as deleted
				/wd5045                                # Qspectre

				$<$<CXX_COMPILER_ID:Clang>:-Wno-pre-c++17-compat>          # Disable pre-C++17 warnings
				$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat>              # Disable C++98 compatibility warnings
				$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat-pedantic>     # Suppress C++98 pedantic compatibility warnings
				$<$<CXX_COMPILER_ID:Clang>:-Wno-global-constructors>       # Suppress global constructors warning (Clang/GCC)
				$<$<CXX_COMPILER_ID:Clang>:-Wno-covered-switch-default>    # Allow default in fully covered switch
				$<$<CXX_COMPILER_ID:Clang>:-Wno-switch-default>            # Allow switches without default when all enum values are covered
				$<$<CXX_COMPILER_ID:GNU>:-Wno-deprecated-declarations>     # Suppress POSIX deprecation warnings
				$<$<CXX_COMPILER_ID:Clang>:-Wno-deprecated-declarations>   # Suppress POSIX deprecation warnings
			)

			target_link_options(${sample_target_name} PRIVATE
				$<$<CONFIG:Release>:/OPT:REF>          # Remove unreferenced functions
				$<$<CONFIG:Release>:/OPT:ICF>          # Identical COMDAT folding
				$<$<CONFIG:Release>:/INCREMENTAL:NO>   # Disable incremental linking
			)
		else()
			# GCC/Clang settings for samples
			target_compile_options(${sample_target_name} PRIVATE
				-std=c++20                             # C++20 standard
				-Wall                                  # All warnings
				-Wextra                                # Extra warnings
				-msse4.2                               # SSE4.2 support (includes CRC32)
				$<$<CXX_COMPILER_ID:Clang>:-mcrc32>    # Explicit CRC32 support for Clang
				-mavx                                  # AVX support
				-mavx2                                 # AVX2 support
				-mfma                                  # Fused multiply-add

				# Optimization settings
				$<$<CONFIG:Release>:-O3>               # Maximum optimization
				$<$<CONFIG:Release>:-march=native>     # Use all available CPU features
				$<$<CONFIG:Release>:-mtune=native>     # Tune for current CPU
				$<$<CONFIG:Release>:-ffast-math>       # Fast math operations
				$<$<CONFIG:Release>:-funroll-loops>    # Unroll loops
				$<$<CONFIG:Release>:-DNDEBUG>          # Disable assertions

				# Debug settings
				$<$<CONFIG:Debug>:-O0>                 # No optimization
				$<$<CONFIG:Debug>:-g>                  # Debug information

				# Warning suppressions
				$<$<CXX_COMPILER_ID:Clang>:-Wno-system-headers>            # Suppress warnings from system headers
				$<$<CXX_COMPILER_ID:Clang>:-Wno-pre-c++17-compat>          # Disable pre-C++17 warnings
				$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat>              # Disable C++98 compatibility warnings
				$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat-pedantic>     # Suppress C++98 pedantic compatibility warnings
				$<$<CXX_COMPILER_ID:Clang>:-Wno-covered-switch-default>    # Allow default in fully covered switch
				$<$<CXX_COMPILER_ID:Clang>:-Wno-switch-default>            # Allow switches without default when all enum values are covered
				$<$<CXX_COMPILER_ID:GNU>:-Wno-deprecated-declarations>     # Suppress POSIX deprecation warnings
				$<$<CXX_COMPILER_ID:Clang>:-Wno-deprecated-declarations>   # Suppress POSIX deprecation warnings
			)
		endif()

		target_link_libraries(${sample_target_name} PRIVATE
			vista-sdk-cpp::static
			nfx-core::static
			nlohmann_json::nlohmann_json
			zlib
		)

		target_include_directories(${sample_target_name} PRIVATE
			${VISTA_SDK_CPP_INCLUDE_DIR}
		)

		# --- Properties ---
		set_target_properties(${sample_target_name} PROPERTIES
			CXX_STANDARD 20
			CXX_STANDARD_REQUIRED ON
			CXX_EXTENSIONS OFF
			POSITION_INDEPENDENT_CODE ON
			DEBUG_POSTFIX "-d"
		)

		# --- Add to sample target ---
		if(NOT TARGET samples)
			add_custom_target(samples)
		endif()
		add_dependencies(samples ${sample_target_name})
	endif()
endforeach()

