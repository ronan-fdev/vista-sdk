#==============================================================================
# vista-sdk-cpp - VISVersionsGenerator executable CMake configuration
#==============================================================================

cmake_minimum_required(VERSION 3.20)

#----------------------------------------------
# Target definition
#----------------------------------------------

add_executable(VISVersionsGenerator
	${VISTA_SDK_CPP_SOURCE_DIR}/SourceGenerator/VISVersionsGenerator.cpp
)

#----------------------------------------------
# Link libraries
#----------------------------------------------

target_link_libraries(VISVersionsGenerator PRIVATE
	EmbeddedResource
	nfx-stringbuilder::static
	nfx-stringutils::nfx-stringutils
)

#----------------------------------------------
# Target properties
#----------------------------------------------

set_target_properties(VISVersionsGenerator PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	POSITION_INDEPENDENT_CODE ON
	DEBUG_POSTFIX "-d"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)

#----------------------------------------------
# Code generation
#----------------------------------------------

set(VISTA_SDK_CPP_GENERATED_HEADER "${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/VISVersions.h")

add_custom_command(
	OUTPUT "${VISTA_SDK_CPP_GENERATED_HEADER}"
	COMMAND VISVersionsGenerator "${VISTA_SDK_CPP_GENERATED_HEADER}" "${VISTA_SDK_CPP_VERSION}"
	COMMAND ${CMAKE_COMMAND} -E env clang-format -i "${VISTA_SDK_CPP_GENERATED_HEADER}" || ${CMAKE_COMMAND} -E true
	DEPENDS VISVersionsGenerator
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
	COMMENT "Generating VISVersions.h from embedded resources..."
	VERBATIM
)

add_custom_target(generate_VISVersion ALL
	DEPENDS "${VISTA_SDK_CPP_GENERATED_HEADER}"
)
