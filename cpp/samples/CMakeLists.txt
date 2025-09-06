# ==============================================================================
# VISTA-SDK-CPP - Samples
# ==============================================================================

#----------------------------------------------
# Sample condition check
#----------------------------------------------

if(NOT VISTA_SDK_CPP_BUILD_SAMPLES)
	message(STATUS "Samples disabled, skipping...")
	return()
endif()

#----------------------------------------------
# Samples source files
#----------------------------------------------

set(SAMPLE_SOURCES
	Sample_BasicUsage.cpp
	Sample_Codebooks.cpp
	Sample_GMOD.cpp
	Sample_LocalId.cpp
	Sample_TimeSeriesDataPackage.cpp
)

#----------------------------------------------
# Configure samples executables
#----------------------------------------------

foreach(sample_source ${SAMPLE_SOURCES})
	get_filename_component(sample_target_name ${sample_source} NAME_WE)

	if(NOT TARGET ${sample_target_name})
		add_executable(${sample_target_name} ${sample_source})

		#----------------------------------------------
		# Target-specific compiler optimisation
		#----------------------------------------------

		target_compile_options(${sample_target_name} PRIVATE

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

			# --- Optimization ---
			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/O2>            # Maximum speed optimization
			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Oi>            # Enable intrinsic functions
			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Ot>            # Favor fast code over small code
			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Gy>            # Function-Level Linking
			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Qpar>          # Auto-parallelization
			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/DNDEBUG>       # Disable debug assertions
			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/GS->           # Disable buffer security checks
			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Gw>            # Optimize global data
			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/fp:except->    # Disable floating point exceptions

			# --- Warning suppressions ---
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

			# --- Common settings ---
			$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-std=c++20>   # C++20 standard
			$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall>        # All warnings
			$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wextra>      # Extra warnings
			$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-msse4.2>     # SSE4.2 support (includes CRC32)
			$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-mavx>        # AVX support
			$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-mavx2>       # AVX2 support
			$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-mfma>        # Fused multiply-add

			# --- Clang-specific settings ---
			$<$<CXX_COMPILER_ID:Clang>:-mcrc32>                                   # Explicit CRC32 support for Clang

			# --- Optimization ---
			$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-O3>            # Maximum optimization
			$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-march=native>  # Use all available CPU features
			$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-mtune=native>  # Tune for current CPU
			$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-ffast-math>    # Fast math operations
			$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-funroll-loops> # Unroll loops
			$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Release>>:-DNDEBUG>       # Disable assertions

			# --- Debug ---
			$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Debug>>:-O0>              # No optimization
			$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:Debug>>:-g>               # Debug information

			# --- Common warning suppressions ---
			$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-std=c++20>

			# --- Clang-specific warning suppressions ---
			$<$<CXX_COMPILER_ID:Clang>:-Wno-deprecated-declarations>              # Suppress POSIX deprecation warnings
			$<$<CXX_COMPILER_ID:Clang>:-Wno-pre-c++17-compat>                     # Disable pre-C++17 warnings
			$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat>                         # Disable C++98 compatibility warnings
			$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat-pedantic>                # Suppress C++98 pedantic compatibility warnings
			$<$<CXX_COMPILER_ID:Clang>:-Wno-global-constructors>                  # Suppress global constructors warning
			$<$<CXX_COMPILER_ID:Clang>:-Wno-covered-switch-default>               # Allow default in fully covered switch
			$<$<CXX_COMPILER_ID:Clang>:-Wno-switch-default>                       # Allow switches without default when all enum values are covered
			$<$<CXX_COMPILER_ID:Clang>:-Wno-deprecated-declarations>              # Suppress POSIX deprecation warnings
		)

		#----------------------------------------------
		# Target-specific linker settings
		#----------------------------------------------

		target_link_options(${sample_target_name} PRIVATE

			#-----------------------------
			# MSVC
			#-----------------------------

			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/OPT:REF>          # Remove unreferenced functions
			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/OPT:ICF>          # Identical COMDAT folding
			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/INCREMENTAL:NO>   # Disable incremental linking
		)

		#----------------------------------------------
		# Target linking
		#----------------------------------------------

		target_link_libraries(${sample_target_name} PRIVATE
			vista-sdk-cpp::static
			nfx-core::static
		)

		#----------------------------------------------
		# Properties
		#----------------------------------------------

		set_target_properties(${sample_target_name} PROPERTIES
			CXX_STANDARD 20
			CXX_STANDARD_REQUIRED ON
			CXX_EXTENSIONS OFF
			POSITION_INDEPENDENT_CODE ON
			DEBUG_POSTFIX "-d"
		)

		#----------------------------------------------
		# Add to sample target
		#----------------------------------------------

		if(NOT TARGET samples)
			add_custom_target(samples)
		endif()
		add_dependencies(samples ${sample_target_name})
	endif()
endforeach()

