#==============================================================================
# vista-sdk-cpp - EmbeddedSchemas test suite CMake configuration
#==============================================================================

#----------------------------------------------
# Target definition
#----------------------------------------------

add_executable(TESTS_EmbeddedSchemas
	TESTS_EmbeddedSchemas.cpp
)

#----------------------------------------------
# Link libraries
#----------------------------------------------

target_link_libraries(TESTS_EmbeddedSchemas PRIVATE
	GTest::gtest_main
	EmbeddedSchemas
	nfx-serialization::static
)

#----------------------------------------------
# Include directories
#----------------------------------------------

target_include_directories(TESTS_EmbeddedSchemas PRIVATE
	${VISTA_SDK_CPP_SOURCE_DIR}
)

#----------------------------------------------
# Enable specific CPU features
#----------------------------------------------

target_compile_options(TESTS_EmbeddedSchemas PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
	$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
)

#----------------------------------------------
# Target properties
#----------------------------------------------

set_target_properties(TESTS_EmbeddedSchemas PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	POSITION_INDEPENDENT_CODE ON
	DEBUG_POSTFIX "-d"
)

#----------------------------------------------
# Register with CTest
#----------------------------------------------

add_test(
	NAME EmbeddedSchemas
	COMMAND TESTS_EmbeddedSchemas
)
