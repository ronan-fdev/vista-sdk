# ==============================================================================
# VISTA-SDK-CPP - Samples
# ==============================================================================

# --- Test condition check ---
if(NOT VISTA_SDK_CPP_BUILD_SAMPLES)
	message(STATUS "Samples disabled, skipping...")
	return()
endif()

# --- Samples source files ---
set(source_files
	simple/Simple.h
	simple/Simple.cpp
)

# --- Create individual sample executables ---
foreach(source_file ${source_files})
	if(source_file MATCHES "\\.cpp$")
		get_filename_component(sample_name ${source_file} NAME_WE)
		get_filename_component(sample_dir ${source_file} DIRECTORY)

		set(target_name "sample-${sample_dir}-${sample_name}")

		string(REPLACE ".cpp" ".h" header_file ${source_file})

		add_executable(${target_name}
			${source_file}
			${header_file}
		)

		set_target_properties(${target_name} PROPERTIES
			CXX_STANDARD 20
			CXX_STANDARD_REQUIRED ON
			OUTPUT_NAME "${sample_dir}-${sample_name}"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
		)

		target_link_libraries(${target_name} PRIVATE vista-sdk-cpp)
		target_include_directories(${target_name} PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}
			${VISTA_SDK_CPP_INCLUDE_DIR}
		)

		message(STATUS "Configured sample: ${target_name}")
	endif()
endforeach()

