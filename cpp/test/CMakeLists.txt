# ==============================================================================
# VISTA-SDK-CPP - Test suite CMake configuration
# ==============================================================================

cmake_minimum_required(VERSION 3.20)

project(vista-sdk-cpp-tests
	VERSION 0.1.0
	DESCRIPTION "DNV VISTA SDK C++ Tests"
	LANGUAGES CXX)

# --- Test condition check ---
if(NOT VISTA_SDK_CPP_BUILD_TESTS)
	return()
endif()

# --- GoogleTest integration ---
include(GoogleTest)

# --- Tests source files ---
set(TEST_SOURCES
	TESTS_Codebook.cpp
	TESTS_Codebooks.cpp
	TESTS_Gmod.cpp
	TESTS_GmodPath.cpp
	TESTS_GmodVersioning.cpp
	TESTS_ImoNumber.cpp
	TESTS_ISOString.cpp
	TESTS_LocalId.cpp
	TESTS_Locations.cpp
	TESTS_UniversalId.cpp
)

if(VISTA_SDK_CPP_BUILD_SMOKE_TESTS)
	list(APPEND TEST_SOURCES TESTS_GmodVersioningSmoke.cpp)
endif()

# --- Copy test data at build time ---
if(EXISTS ${VISTA_SDK_TEST_DATA_DIR})
	set(VISTA_SDK_CPP_TEST_DATA_OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testdata)

	if(NOT TARGET CopyTestData)
		add_custom_target(CopyTestData ALL
			COMMAND ${CMAKE_COMMAND} -E make_directory ${VISTA_SDK_CPP_TEST_DATA_OUTPUT_DIR}
			COMMAND ${CMAKE_COMMAND} -E copy_directory
				${VISTA_SDK_TEST_DATA_DIR} ${VISTA_SDK_CPP_TEST_DATA_OUTPUT_DIR}
			COMMENT "Copying VISTA SDK testdata to build directory ($<CONFIG>)"
			VERBATIM
		)
		message(STATUS "Test data will be copied from ${VISTA_SDK_TEST_DATA_DIR} to ${VISTA_SDK_CPP_TEST_DATA_OUTPUT_DIR} at build time")
	endif()
else()
	message(WARNING "Test data directory not found: ${VISTA_SDK_TEST_DATA_DIR}. Skipping test data copy.")
endif()

# --- Configure test executables ---
foreach(test_source ${TEST_SOURCES})
	get_filename_component(test_target_name ${test_source} NAME_WE)

	if(NOT TARGET ${test_target_name})
		add_executable(${test_target_name} TestDataLoader.h TestDataLoader.h TestDataLoader.cpp TestDataLoader.cpp ${test_source})

		# --- Target-specific MSVC optimisation ---
		if(MSVC)
			target_compile_options(${test_target_name} PRIVATE
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
				/wd4505                                # unreferenced local function has been removed
				/wd4514                                # unreferenced inline function has been removed
				/wd4625                                # copy constructor was implicitly defined as deleted
				/wd4626                                # assignment operator was implicitly defined as deleted
				/wd4710                                # function not inlined
				/wd4711                                # function 'function' selected for inline expansion
				/wd4820                                # padding
				/wd4834                                # discarding return value of function with 'nodiscard' attribute
				/wd4866                                # compiler may not enforce left-to-right evaluation order for call to operator_name
				/wd5026                                # move constructor was implicitly defined as deleted
				/wd5027                                # move assignment operator was implicitly defined as deleted
				/wd5045                                # Qspectre
				/wd6031                                # return value ignored: 'called-function' could return unexpected value
				/wd6326                                # potential comparison of a constant with another constant
			)

			target_link_options(${test_target_name} PRIVATE
				$<$<CONFIG:Release>:/OPT:REF>          # Remove unreferenced functions
				$<$<CONFIG:Release>:/OPT:ICF>          # Identical COMDAT folding
				$<$<CONFIG:Release>:/INCREMENTAL:NO>   # Disable incremental linking
			)
		else()
			# GCC/Clang settings for benchmarks
			target_compile_options(${test_target_name} PRIVATE
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

		target_precompile_headers(${test_target_name} PRIVATE pch.h)

		target_link_libraries(${test_target_name} PRIVATE
			vista-sdk-cpp
			GTest::gtest_main
			fmt::fmt
	        cpuid
			nlohmann_json::nlohmann_json
			zlibstatic
		)

		target_include_directories(${test_target_name} PRIVATE
			${VISTA_SDK_CPP_INCLUDE_DIR}
		)

		if(TARGET CopyTestData)
			add_dependencies(${test_target_name} CopyTestData)
		endif()

		gtest_discover_tests(${test_target_name}
			WORKING_DIRECTORY "$<TARGET_FILE_DIR:${test_target_name}>"
			DISCOVERY_VERBOSE_OUTPUT TRUE
			PROPERTIES
				TIMEOUT 120
				WILL_FAIL FALSE
				OUTPUT_ON_FAILURE TRUE
		)

		# --- Properties with LTO disabled ---
		set_target_properties(${test_target_name} PROPERTIES
			CXX_STANDARD 20
			CXX_STANDARD_REQUIRED ON
			RUNTIME_OUTPUT_DIRECTORY "${VISTA_SDK_CPP_BIN_DIR}/$<CONFIG>"
			EXCLUDE_FROM_ALL FALSE
			#INTERPROCEDURAL_OPTIMIZATION FALSE
		)
	endif()
endforeach()
