#==============================================================================
# vista-sdk-cpp - VisVersionsGenerator executable CMake configuration
#==============================================================================

#----------------------------------------------
# Target definition
#----------------------------------------------

add_executable(VisVersionsGenerator
	${VISTA_SDK_CPP_SOURCE_DIR}/SourceGenerator/VisVersionsGenerator.cpp
)

#----------------------------------------------
# Link libraries
#----------------------------------------------

target_link_libraries(VisVersionsGenerator PRIVATE
	EmbeddedResource
	nfx-stringbuilder::static
)

#----------------------------------------------
# Target properties
#----------------------------------------------

set_target_properties(VisVersionsGenerator PROPERTIES
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

set(VISTA_SDK_CPP_GENERATED_HEADER      "${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/VisVersions.h")
set(VISTA_SDK_CPP_GENERATED_EXTENSIONS  "${VISTA_SDK_CPP_SOURCE_DIR}/SDK/VisVersionsExtensions.h")

add_custom_command(
	OUTPUT "${VISTA_SDK_CPP_GENERATED_HEADER}" "${VISTA_SDK_CPP_GENERATED_EXTENSIONS}"
	COMMAND VisVersionsGenerator "${VISTA_SDK_CPP_GENERATED_HEADER}" "${VISTA_SDK_CPP_GENERATED_EXTENSIONS}" "${VISTA_SDK_CPP_VERSION}"
	COMMAND clang-format -i "${VISTA_SDK_CPP_GENERATED_HEADER}" || ${CMAKE_COMMAND} -E true
	COMMAND clang-format -i "${VISTA_SDK_CPP_GENERATED_EXTENSIONS}" || ${CMAKE_COMMAND} -E true
	DEPENDS VisVersionsGenerator
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
	COMMENT "Generating VisVersions.h and VisVersionsExtensions.h from embedded resources..."
	VERBATIM
)

add_custom_target(generate_VisVersions ALL
	DEPENDS "${VISTA_SDK_CPP_GENERATED_HEADER}" "${VISTA_SDK_CPP_GENERATED_EXTENSIONS}"
)
