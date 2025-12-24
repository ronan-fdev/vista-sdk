#==============================================================================
# vista-sdk-cpp - Samples
#==============================================================================

#----------------------------------------------
# Sample condition check
#----------------------------------------------

if(NOT VISTA_SDK_CPP_BUILD_SAMPLES)
	return()
endif()

#----------------------------------------------
# Samples source files
#----------------------------------------------

set(sample_sources)

list(APPEND sample_sources
	simple/Sample_Codebooks.cpp
	simple/Sample_Gmod.cpp
	simple/Sample_GmodPath.cpp
	simple/Sample_ImoNumber.cpp
	simple/Sample_LocalId.cpp
	simple/Sample_Locations.cpp
)

if(NOT sample_sources)
	return()
endif()

#----------------------------------------------
# Configure samples executables
#----------------------------------------------

foreach(sample_source ${sample_sources})
	get_filename_component(sample_target_name ${sample_source} NAME_WE)

	if(NOT TARGET ${sample_target_name})
		add_executable(${sample_target_name} ${sample_source})

		#----------------------------------------------
		# Target linking
		#----------------------------------------------

		target_link_libraries(${sample_target_name} PRIVATE
			vista-sdk-cpp::static
		)

		#----------------------------------------------
		# Enable specific CPU features
		#----------------------------------------------

		target_compile_options(${sample_target_name} PRIVATE
			$<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
			$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
		)

		#----------------------------------------------
		# Properties
		#----------------------------------------------

		set_target_properties(${sample_target_name} PROPERTIES
			CXX_STANDARD 20
			CXX_STANDARD_REQUIRED ON
			CXX_EXTENSIONS OFF
			POSITION_INDEPENDENT_CODE ON
			DEBUG_POSTFIX "-d"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/samples"
			RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/samples"
			RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/samples"
		)
	endif()
endforeach()
