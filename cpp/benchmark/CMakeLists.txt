# ==============================================================================
# VISTA-SDK-CPP - Benchmark suite CMake configuration
# ==============================================================================

# --- Benchmark condition check ---
if(NOT VISTA_SDK_CPP_BUILD_BENCHMARKS)
	message(STATUS "Benchmarks disabled, skipping...")
	return()
endif()

# --- Benchmarks source files ---
set(BENCHMARK_SOURCES
	codebooks/BM_CodebooksLookup.cpp
	datatypes/BM_Decimal128.cpp
	datatypes/BM_Int128.cpp
	gmod/BM_GmodLoad.cpp
	gmod/BM_GmodLookup.cpp
	gmod/BM_GmodPathParse.cpp
	gmod/BM_GmodTraversal.cpp
	gmod/BM_GmodVersioningConvertPath.cpp
	internal/BM_ShortStringHash.cpp
)

# --- Configure benchmark executables ---
foreach(benchmark_source ${BENCHMARK_SOURCES})
	get_filename_component(benchmark_target_name ${benchmark_source} NAME_WE)

	if(NOT TARGET ${benchmark_target_name})
		add_executable(${benchmark_target_name} ${benchmark_source})

		# --- Target-specific MSVC optimisation ---
		if(MSVC)
			target_compile_options(${benchmark_target_name} PRIVATE
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
				$<$<CONFIG:Release>:/GT>               # Support fiber-safe TLS
				$<$<CONFIG:Release>:/fp:except->       # Disable floating point exceptions

				# Warning suppressions
				/wd4244                                # conversion from 'type1' to 'type2', possible loss of data
				/wd4266                                # no override available for virtual member function from base 'type'; function is hidden
				/wd4505                                # unreferenced local function has been removed
				/wd4514                                # unreferenced inline function has been removed
				/wd4625                                # copy constructor was implicitly defined as deleted
				/wd4626                                # assignment operator was implicitly defined as deleted
				/wd4710                                # function not inlined
				/wd4711                                # function 'function' selected for inline expansion
				/wd4820                                # padding
				/wd4834                                # discarding return value of function with 'nodiscard' attribute
				/wd5026                                # move constructor was implicitly defined as deleted
				/wd5027                                # move assignment operator was implicitly defined as deleted
				/wd5045                                # Qspectre
			)

			target_link_options(${benchmark_target_name} PRIVATE
				$<$<CONFIG:Release>:/OPT:REF>          # Remove unreferenced functions
				$<$<CONFIG:Release>:/OPT:ICF>          # Identical COMDAT folding
				$<$<CONFIG:Release>:/INCREMENTAL:NO>   # Disable incremental linking
			)
		else()
			# GCC/Clang settings for benchmarks
			target_compile_options(${benchmark_target_name} PRIVATE
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
			)
		endif()

		target_precompile_headers(${benchmark_target_name} PRIVATE pch.h)

		target_link_libraries(${benchmark_target_name} PRIVATE
			vista-sdk-cpp
			benchmark::benchmark
			nlohmann_json::nlohmann_json
			zlib
			cpuid
			fmt::fmt-header-only
		)

		target_include_directories(${benchmark_target_name} PRIVATE
			${VISTA_SDK_CPP_INCLUDE_DIR}
		)

		# --- Properties ---
		set_target_properties(${benchmark_target_name} PROPERTIES
			CXX_STANDARD 20
			CXX_STANDARD_REQUIRED ON
			CXX_EXTENSIONS OFF
			POSITION_INDEPENDENT_CODE ON
			DEBUG_POSTFIX "-d"
		)

		# --- Add to benchmark target ---
		if(NOT TARGET benchmarks)
			add_custom_target(benchmarks)
		endif()
		add_dependencies(benchmarks ${benchmark_target_name})
	endif()
endforeach()

# --- Benchmark execution ---
if(TARGET benchmarks AND VISTA_SDK_CPP_RUN_BENCHMARKS)
	set(BENCHMARK_OUTPUT_DIR "${VISTA_SDK_CPP_BIN_DIR}/$<CONFIG>/benchmark_results")
	string(TIMESTAMP BENCHMARK_TIMESTAMP "%Y%m%d_%H%M%S")

	add_custom_target(run_benchmarks ALL
		COMMAND ${CMAKE_COMMAND} -E make_directory ${BENCHMARK_OUTPUT_DIR}
		COMMENT "Running Vista SDK C++ Benchmark Suite..."
		SOURCES ${BENCHMARK_SOURCES}
	)

	set_target_properties(run_benchmarks PROPERTIES
		FOLDER "Benchmarks"
		EXCLUDE_FROM_DEFAULT_BUILD FALSE
	)

	foreach(benchmark_source ${BENCHMARK_SOURCES})
		get_filename_component(benchmark_target_name ${benchmark_source} NAME_WE)
		set(OUTPUT_FILE "${BENCHMARK_OUTPUT_DIR}/${benchmark_target_name}_${BENCHMARK_TIMESTAMP}.txt")

		add_custom_command(TARGET run_benchmarks POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E echo ""
			COMMAND ${CMAKE_COMMAND} -E echo "================================================================================"
			COMMAND ${CMAKE_COMMAND} -E echo "Running: ${benchmark_target_name} -> ${OUTPUT_FILE}"
			COMMAND ${CMAKE_COMMAND} -E echo "================================================================================"
			COMMAND powershell -Command "$ProgressPreference='SilentlyContinue'; $output = & '${VISTA_SDK_CPP_BIN_DIR}/$<CONFIG>/${benchmark_target_name}.exe' --benchmark_format=console 2>&1; $output | ForEach-Object { Write-Host $_; Add-Content -Path '${OUTPUT_FILE}' -Value $_ -Force }"
			COMMAND ${CMAKE_COMMAND} -E echo ""
			COMMAND ${CMAKE_COMMAND} -E echo "Results saved to: ${OUTPUT_FILE}"
			WORKING_DIRECTORY "$<TARGET_FILE_DIR:${benchmark_target_name}>"
			VERBATIM
		)
	endforeach()

	add_dependencies(run_benchmarks benchmarks)
	message(STATUS "Benchmark execution enabled - results will be saved to ${BENCHMARK_OUTPUT_DIR}")
else()
	if(TARGET benchmarks)
		message(STATUS "Benchmark execution disabled - use -DVISTA_SDK_CPP_RUN_BENCHMARKS=ON to enable")
	endif()
endif()
