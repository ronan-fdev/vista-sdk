#==============================================================================
# vista-sdk-cpp - ResourceGenerator executable CMake configuration
#==============================================================================

#----------------------------------------------
# Target definition
#----------------------------------------------

add_executable(ResourceGenerator
	${VISTA_SDK_CPP_SOURCE_DIR}/SourceGenerator/ResourceGenerator.cpp
)

#----------------------------------------------
# Target properties
#----------------------------------------------

set_target_properties(ResourceGenerator PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	POSITION_INDEPENDENT_CODE ON
	DEBUG_POSTFIX "-d"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)

#----------------------------------------------
# Installation
#----------------------------------------------

if(VISTA_SDK_CPP_INSTALL_PROJECT)
	install(TARGETS ResourceGenerator
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/tools
		COMPONENT tools
	)
endif()
