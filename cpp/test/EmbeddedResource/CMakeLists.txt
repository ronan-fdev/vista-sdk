#==============================================================================
# vista-sdk-cpp - EmbeddedResource test suite CMake configuration
#==============================================================================

#----------------------------------------------
# Target definition
#----------------------------------------------

add_executable(TESTS_EmbeddedResource
	TESTS_EmbeddedResource.cpp
)

#----------------------------------------------
# Include directories
#----------------------------------------------

target_include_directories(TESTS_EmbeddedResource
	PRIVATE
		${VISTA_SDK_CPP_SOURCE_DIR}/SDK/dto
)

#----------------------------------------------
# Link libraries
#----------------------------------------------

target_link_libraries(TESTS_EmbeddedResource PRIVATE
	GTest::gtest_main
	EmbeddedResource
	nfx-serialization::static
)

#----------------------------------------------
# Enable specific CPU features
#----------------------------------------------

target_compile_options(TESTS_EmbeddedResource PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
	$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
)

#----------------------------------------------
# Target properties
#----------------------------------------------

set_target_properties(TESTS_EmbeddedResource PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	POSITION_INDEPENDENT_CODE ON
	DEBUG_POSTFIX "-d"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests"
	FOLDER "Tests"
)

#----------------------------------------------
# Test discovery and registration
#----------------------------------------------

gtest_discover_tests(TESTS_EmbeddedResource
	WORKING_DIRECTORY "$<TARGET_FILE_DIR:TESTS_EmbeddedResource>"
	DISCOVERY_MODE POST_BUILD
	PROPERTIES
		TIMEOUT 120
		RUN_SERIAL OFF
)
