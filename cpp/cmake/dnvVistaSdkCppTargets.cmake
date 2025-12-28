#==============================================================================
# vista-sdk-cpp - CMake targets
#==============================================================================

#----------------------------------------------
# Check if we have sources to build
#----------------------------------------------

if(NOT private_sources)
	return()
endif()

#----------------------------------------------
# Targets definition
#----------------------------------------------

# --- Create shared library if requested ---
if(VISTA_SDK_CPP_BUILD_SHARED)
	add_library(${PROJECT_NAME} SHARED)
	target_sources(${PROJECT_NAME}
		PRIVATE
			${private_sources}
	)

	set_target_properties(${PROJECT_NAME} PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	)

	add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
endif()

# --- Create static library if requested ---
if(VISTA_SDK_CPP_BUILD_STATIC)
	add_library(${PROJECT_NAME}-static STATIC)
	target_sources(${PROJECT_NAME}-static
		PRIVATE
			${private_sources}
	)

	set_target_properties(${PROJECT_NAME}-static PROPERTIES
		OUTPUT_NAME ${PROJECT_NAME}-static
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	)

	add_library(${PROJECT_NAME}::static ALIAS ${PROJECT_NAME}-static)
endif()

#----------------------------------------------
# Targets properties
#----------------------------------------------

function(configure_target target_name)
	# --- Include directories ---
	target_include_directories(${target_name}
		PUBLIC
			$<BUILD_INTERFACE:${VISTA_SDK_CPP_INCLUDE_DIR}>
			$<INSTALL_INTERFACE:include>
		PRIVATE
			${VISTA_SDK_CPP_SOURCE_DIR}
	)

	target_sources(${target_name}
		PRIVATE
			$<TARGET_OBJECTS:EmbeddedResource>
	)

	target_link_libraries(${target_name}
		PUBLIC
			$<BUILD_INTERFACE:nfx-containers::nfx-containers>
		PRIVATE
			$<BUILD_INTERFACE:zlib-ng>
			$<BUILD_INTERFACE:nfx-serialization::static>
			$<BUILD_INTERFACE:nfx-stringbuilder::static>
			$<BUILD_INTERFACE:nfx-stringutils::nfx-stringutils>
	)

	# --- Properties ---
	set_target_properties(${target_name} PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		POSITION_INDEPENDENT_CODE ON
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		DEBUG_POSTFIX "-d"
	)

	# --- Enable specific CPU features ---
	target_compile_options(${target_name} PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
	)
endfunction()

# --- Apply configuration to both targets ---
if(VISTA_SDK_CPP_BUILD_SHARED)
	add_dependencies(${PROJECT_NAME} generate_VisVersions)
	configure_target(${PROJECT_NAME})
	if(WIN32)
		set_target_properties(${PROJECT_NAME} PROPERTIES
			WINDOWS_EXPORT_ALL_SYMBOLS TRUE
		)
		configure_file(
			${CMAKE_CURRENT_SOURCE_DIR}/cmake/dnvVistaSdkCppVersion.rc.in
			${CMAKE_BINARY_DIR}/dnvVistaSdkCppVersion.rc
			@ONLY
		)
		target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/dnvVistaSdkCppVersion.rc)
	endif()
endif()

if(VISTA_SDK_CPP_BUILD_STATIC)
	add_dependencies(${PROJECT_NAME}-static generate_VisVersions)
	configure_target(${PROJECT_NAME}-static)
endif()
