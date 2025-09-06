# ==============================================================================
# VISTA-SDK-CPP - Test suite CMake configuration
# ==============================================================================

#----------------------------------------------
# Test condition check
#----------------------------------------------

if(NOT VISTA_SDK_CPP_BUILD_TESTS)
	message(STATUS "Tests disabled, skipping...")
	return()
endif()

#----------------------------------------------
# GoogleTest integration
#----------------------------------------------

include(GoogleTest)

#----------------------------------------------
# Tests source files
#----------------------------------------------

set(TEST_SOURCES
	transport/TESTS_DataChannelId.cpp
	transport/TESTS_DataChannelISOmessages.cpp
	transport/TESTS_ISO19848.cpp
	transport/TESTS_ShipId.cpp
	transport/TESTS_TimeSeriesISOmessages.cpp
	TESTS_Codebook.cpp
	TESTS_Codebooks.cpp
	TESTS_Gmod.cpp
	TESTS_GmodPath.cpp
	TESTS_GmodPathQuery.cpp
	TESTS_GmodVersioning.cpp
	TESTS_HashCompatibility.cpp
	TESTS_ImoNumber.cpp
	TESTS_ISOString.cpp
	TESTS_LocalId.cpp
	TESTS_LocalIdQuery.cpp
	TESTS_Locations.cpp
	TESTS_UniversalId.cpp
)

if(VISTA_SDK_CPP_BUILD_SMOKE_TESTS)
	list(APPEND TEST_SOURCES TESTS_GmodVersioningSmoke.cpp)
endif()

#----------------------------------------------
# Copy test data at build time
#----------------------------------------------

if(EXISTS ${VISTA_SDK_TESTDATA_DIR})
	set(VISTA_SDK_CPP_TEST_DATA_OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testdata)

	if(NOT TARGET CopyTestData)
		add_custom_target(CopyTestData ALL
			COMMAND ${CMAKE_COMMAND} -E make_directory ${VISTA_SDK_CPP_TEST_DATA_OUTPUT_DIR}
			COMMAND ${CMAKE_COMMAND} -E copy_directory
				${VISTA_SDK_TESTDATA_DIR} ${VISTA_SDK_CPP_TEST_DATA_OUTPUT_DIR}
			COMMENT "Copying VISTA SDK testdata to build directory ($<CONFIG>)"
			VERBATIM
		)
		message(STATUS "Test data will be copied from ${VISTA_SDK_TESTDATA_DIR} to ${VISTA_SDK_CPP_TEST_DATA_OUTPUT_DIR} at build time")
	endif()
else()
	message(WARNING "Test data directory not found: ${VISTA_SDK_TESTDATA_DIR}. Skipping test data copy.")
endif()

#----------------------------------------------
# Configure test executables
#----------------------------------------------

foreach(test_source ${TEST_SOURCES})
	get_filename_component(test_target_name ${test_source} NAME_WE)

	if(NOT TARGET ${test_target_name})
		add_executable(${test_target_name} TestDataLoader.h TestDataLoader.cpp ${test_source})

		#----------------------------------------------
		# Target-specific compiler optimisation
		#----------------------------------------------

		target_compile_options(${test_target_name} PRIVATE

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
			$<$<CXX_COMPILER_ID:MSVC>:/wd4625>                                 # copy constructor was implicitly defined as deleted
			$<$<CXX_COMPILER_ID:MSVC>:/wd4626>                                 # assignment operator was implicitly defined as deleted
			$<$<CXX_COMPILER_ID:MSVC>:/wd4710>                                 # function not inlined
			$<$<CXX_COMPILER_ID:MSVC>:/wd4711>                                 # function selected for inline expansion
			$<$<CXX_COMPILER_ID:MSVC>:/wd4820>                                 # padding
			$<$<CXX_COMPILER_ID:MSVC>:/wd4866>                                 # compiler may not enforce left-to-right evaluation order for call to operator_name
			$<$<CXX_COMPILER_ID:MSVC>:/wd4868>                                 # compiler may not enforce left-to-right evaluation order in braced initializer list
			$<$<CXX_COMPILER_ID:MSVC>:/wd5026>                                 # move constructor was implicitly defined as deleted
			$<$<CXX_COMPILER_ID:MSVC>:/wd5027>                                 # move assignment operator was implicitly defined as deleted
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

		target_link_options(${test_target_name} PRIVATE

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

		target_link_libraries(${test_target_name} PRIVATE
			vista-sdk-cpp::static
			nfx-core::static
			GTest::gtest_main
		)

		#----------------------------------------------
		# Include directories
		#----------------------------------------------

		target_include_directories(${test_target_name} PRIVATE
			${VISTA_SDK_CPP_SOURCE_DIR} # For Vista SDG private source access
		)

		#----------------------------------------------
		# Test dependencies
		#----------------------------------------------

		if(TARGET CopyTestData)
			add_dependencies(${test_target_name} CopyTestData)
		endif()

		#----------------------------------------------
		# Test discovery and registration
		#----------------------------------------------

		gtest_discover_tests(${test_target_name}
			WORKING_DIRECTORY "$<TARGET_FILE_DIR:${test_target_name}>"
			DISCOVERY_MODE PRE_TEST
			DISCOVERY_VERBOSE_OUTPUT TRUE
			PROPERTIES
				TIMEOUT 120
				OUTPUT_ON_FAILURE TRUE
		)

		#----------------------------------------------
		# Properties
		#----------------------------------------------

		set_target_properties(${test_target_name} PROPERTIES
			CXX_STANDARD 20
			CXX_STANDARD_REQUIRED ON
			CXX_EXTENSIONS OFF
			POSITION_INDEPENDENT_CODE ON
			DEBUG_POSTFIX "-d"
		)
	endif()
endforeach()
