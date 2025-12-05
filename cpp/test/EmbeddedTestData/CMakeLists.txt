#==============================================================================
# vista-sdk-cpp - EmbeddedTestData test suite CMake configuration
#==============================================================================

#----------------------------------------------
# Target definition
#----------------------------------------------

add_executable(TESTS_EmbeddedTestData
	TESTS_EmbeddedTestData.cpp
)

#----------------------------------------------
# Link libraries
#----------------------------------------------

target_link_libraries(TESTS_EmbeddedTestData PRIVATE
	GTest::gtest_main
	EmbeddedTestData
	nfx-serialization::static
)

#----------------------------------------------
# Enable specific CPU features
#----------------------------------------------

target_compile_options(TESTS_EmbeddedTestData PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
	$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
)

#----------------------------------------------
# Target properties
#----------------------------------------------

set_target_properties(TESTS_EmbeddedTestData PROPERTIES
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

gtest_discover_tests(TESTS_EmbeddedTestData
	WORKING_DIRECTORY "$<TARGET_FILE_DIR:TESTS_EmbeddedTestData>"
	DISCOVERY_MODE POST_BUILD
	PROPERTIES
		TIMEOUT 120
		RUN_SERIAL OFF
)
