#==============================================================================
# vista-sdk-cpp - Tools
#==============================================================================

#----------------------------------------------
# Tolls condition check
#----------------------------------------------

if(NOT VISTA_SDK_CPP_BUILD_TOOLS)
	return()
endif()

#----------------------------------------------
# Tools source files
#----------------------------------------------

set(tools_sources)

list(APPEND tools_sources
	codebooks-cli.cpp
	gmodexplorer-cli.cpp
	locations-cli.cpp
)

if(NOT tools_sources)
	return()
endif()

#----------------------------------------------
# Configure tools executables
#----------------------------------------------

foreach(tools_source ${tools_sources})
	get_filename_component(tool_target_name ${tools_source} NAME_WE)

	if(NOT TARGET ${tool_target_name})
		add_executable(${tool_target_name} ${tools_source})

		#----------------------------------------------
		# Target linking
		#----------------------------------------------

		target_link_libraries(${tool_target_name} PRIVATE
			vista-sdk-cpp::static
		)

		#----------------------------------------------
		# Enable specific CPU features
		#----------------------------------------------

		target_compile_options(${tool_target_name} PRIVATE
			$<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
			$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
		)

		#----------------------------------------------
		# Properties
		#----------------------------------------------

		set_target_properties(${tool_target_name} PROPERTIES
			CXX_STANDARD 20
			CXX_STANDARD_REQUIRED ON
			CXX_EXTENSIONS OFF
			POSITION_INDEPENDENT_CODE ON
			DEBUG_POSTFIX "-d"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tools"
			RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tools"
			RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tools"
		)
	endif()
endforeach()
