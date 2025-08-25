# ==============================================================================
# VISTA-SDK-CPP - C++ library CMake configuration
# ==============================================================================

#----------------------------------------------
# Build validation
#----------------------------------------------

# --- Prevent in-source builds ---
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
	message(FATAL_ERROR "In-source builds are not allowed. Please use a separate build directory.")
endif()

# --- Ensure at least one library type is built ---
if(NOT VISTA_SDK_CPP_BUILD_STATIC AND NOT VISTA_SDK_CPP_BUILD_SHARED)
	message(WARNING "Neither VISTA_SDK_CPP_BUILD_STATIC nor VISTA_SDK_CPP_BUILD_SHARED is enabled.")
	message(STATUS "Applying fallback: Enabling shared library build (VISTA_SDK_CPP_BUILD_SHARED=ON)")
	set(VISTA_SDK_CPP_BUILD_SHARED ON CACHE BOOL "Build shared library (fallback)" FORCE)
endif()

# --- Enable test building if running tests is requested ---
if(VISTA_SDK_CPP_RUN_TESTS AND NOT VISTA_SDK_CPP_BUILD_TESTS)
	message(STATUS "VISTA_SDK_CPP_RUN_TESTS=ON requires building tests. Enabling VISTA_SDK_CPP_BUILD_TESTS.")
	set(VISTA_SDK_CPP_BUILD_TESTS ON CACHE BOOL "Build tests (required for running)" FORCE)
endif()

# --- Enable benchmark building if running benchmarks is requested ---
if(VISTA_SDK_CPP_RUN_BENCHMARKS AND NOT VISTA_SDK_CPP_BUILD_BENCHMARKS)
	message(STATUS "VISTA_SDK_CPP_RUN_BENCHMARKS=ON requires building benchmarks. Enabling VISTA_SDK_CPP_BUILD_BENCHMARKS.")
	set(VISTA_SDK_CPP_BUILD_BENCHMARKS ON CACHE BOOL "Build benchmarks (required for running)" FORCE)
endif()

# --- Validate CMake version ---
if(CMAKE_VERSION VERSION_LESS "3.20")
	message(FATAL_ERROR "CMake 3.20 or higher is required for reliable C++20 support")
endif()


#----------------------------------------------
# Multi-config generator setup
#----------------------------------------------

# --- For multi-config generators (Visual Studio, Xcode), set available configurations ---
if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "Available build configurations" FORCE)
	message(STATUS "Multi-config generator detected. Available configurations: ${CMAKE_CONFIGURATION_TYPES}")
else()
	# --- For multi-config generators (Visual Studio, Xcode), set available configurations ---
	if(NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
		message(STATUS "Single-config generator detected. Default build type: ${CMAKE_BUILD_TYPE}")
	endif()
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

#----------------------------------------------
# Install prefix
#----------------------------------------------

include(GNUInstallDirs)

if(VISTA_SDK_CPP_INSTALL_DIR)
	set(CMAKE_INSTALL_PREFIX "${VISTA_SDK_CPP_INSTALL_DIR}" CACHE PATH "Install directory" FORCE)
	message(STATUS "Using custom install directory: ${VISTA_SDK_CPP_INSTALL_DIR}")
else()
	if(WIN32)
		if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
			if(CMAKE_SIZEOF_VOID_P EQUAL 8)
				set(CMAKE_INSTALL_PREFIX "C:/Program Files/${PROJECT_NAME}" CACHE PATH "Install directory" FORCE)
			else()
				set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/${PROJECT_NAME}" CACHE PATH "Install directory" FORCE)
			endif()
		endif()
	elseif(UNIX)
		if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
			set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Install directory" FORCE)
		endif()
	endif()
endif()

#----------------------------------------------
# Directory configuration
#----------------------------------------------

# --- Vista SDK root ---
set(VISTA_SDK_ROOT_DIR       "${VISTA_SDK_ROOT_DIR}"            CACHE PATH  "Vista SDK root directory"     )
set(VISTA_SDK_RESOURCES_DIR  "${VISTA_SDK_ROOT_DIR}/resources"  CACHE PATH  "Vista SDK resources directory")
set(VISTA_SDK_TESTDATA_DIR   "${VISTA_SDK_ROOT_DIR}/testdata"   CACHE PATH  "Vista SDK test data directory")

# --- Vista c++ SDK ---
set(VISTA_SDK_CPP_ROOT_DIR           "${VISTA_SDK_ROOT_DIR}/cpp"            CACHE PATH  "Vista SDK C++ SDK root directory"     )
set(VISTA_SDK_CPP_INCLUDE_DIR        "${VISTA_SDK_CPP_ROOT_DIR}/include"    CACHE PATH  "Vista SDK C++ SDK include directory"  )
set(VISTA_SDK_CPP_SOURCE_DIR         "${VISTA_SDK_CPP_ROOT_DIR}/src"        CACHE PATH  "Vista SDK C++ SDK source directory"   )
set(VISTA_SDK_CPP_INSTALL_DIR        "${CMAKE_INSTALL_PREFIX}"              CACHE PATH  "Vista SDK C++ SDK install directory"  )
set(VISTA_SDK_CPP_TEST_DIR           "${VISTA_SDK_CPP_ROOT_DIR}/test"       CACHE PATH  "Vista SDK C++ SDK test directory"     )
set(VISTA_SDK_CPP_BENCHMARK_DIR      "${VISTA_SDK_CPP_ROOT_DIR}/benchmark"  CACHE PATH  "Vista SDK C++ SDK benchmark directory")
set(VISTA_SDK_CPP_SAMPLES_DIR        "${VISTA_SDK_CPP_ROOT_DIR}/samples"    CACHE PATH  "Vista SDK C++ SDK samples directory"  )
set(VISTA_SDK_CPP_DOCUMENTATION_DIR  "${VISTA_SDK_CPP_ROOT_DIR}/samples"    CACHE PATH  "Vista SDK C++ SDK documentation"      )

#----------------------------------------------
# Compiler detection
#----------------------------------------------

set(COMPILER_NAME "Unknown")

if(WIN32)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set(COMPILER_NAME "MSVC")
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(COMPILER_NAME "GCC")
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		if(CMAKE_CXX_COMPILER MATCHES "clang-cl(\\.exe)?$")
			set(COMPILER_NAME "Clang-MSVC-CLI")
		else()
			set(COMPILER_NAME "Clang-GNU-CLI")
		endif()
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
		set(COMPILER_NAME "Intel")
	endif()
elseif(UNIX AND NOT APPLE)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(COMPILER_NAME "GCC")
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set(COMPILER_NAME "Clang")
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
		set(COMPILER_NAME "Intel")
	endif()
endif()

message(STATUS "Compiler resolved as: ${COMPILER_NAME}")

#----------------------------------------------
# Architecture detection
#----------------------------------------------

set(ARCH_NAME "Unknown")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(ARCH_NAME "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(ARCH_NAME "x86")
endif()

#----------------------------------------------
# Output directory configuration
#----------------------------------------------

set(COMPILER_DIR_NAME "${COMPILER_NAME}-${CMAKE_CXX_COMPILER_VERSION}-${ARCH_NAME}")
set(VISTA_SDK_CPP_BUILD_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}/${COMPILER_DIR_NAME}/$<CONFIG>)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VISTA_SDK_CPP_BUILD_DIR}/bin)

#----------------------------------------------
# Dependencies
#----------------------------------------------

include(Dependencies)

#----------------------------------------------
# Resource management (TODO: Remove later)
#----------------------------------------------

if(VISTA_SDK_CPP_COPY_RESOURCES)
	if(EXISTS ${VISTA_SDK_RESOURCES_DIR})
		set(VISTA_SDK_CPP_RESOURCES_OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)

		add_custom_target(CopyResources ALL
			COMMAND ${CMAKE_COMMAND} -E make_directory ${VISTA_SDK_CPP_RESOURCES_OUTPUT_DIR}
			COMMAND ${CMAKE_COMMAND} -E copy_directory
				${VISTA_SDK_RESOURCES_DIR}
				${VISTA_SDK_CPP_RESOURCES_OUTPUT_DIR}
			COMMENT "Copying VISTA SDK resources to build directory ($<CONFIG>)"
			VERBATIM
		)

		message(STATUS "Resources will be copied from ${VISTA_SDK_RESOURCES_DIR} to ${VISTA_SDK_CPP_RESOURCES_OUTPUT_DIR} at build time")
	else()
		message(WARNING "Resources directory not found: ${VISTA_SDK_RESOURCES_DIR}. Skipping resource copy.")
	endif()
else()
	message(STATUS "Skipping resource copy based on VISTA_SDK_CPP_COPY_RESOURCES option.")
endif()

#----------------------------------------------
# Library definition
#----------------------------------------------

# --- Source files ---
include(Sources)

# --- Create shared library if requested ---
if(VISTA_SDK_CPP_BUILD_SHARED)
	add_library(${PROJECT_NAME} SHARED)
	target_sources(${PROJECT_NAME}
		PUBLIC
			${PUBLIC_HEADERS}
		PRIVATE
			${PRIVATE_HEADERS}
			${PRIVATE_SOURCES}
	)

	set_target_properties(${PROJECT_NAME} PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${VISTA_SDK_CPP_BUILD_DIR}/lib
		ARCHIVE_OUTPUT_DIRECTORY ${VISTA_SDK_CPP_BUILD_DIR}/lib
	)

	add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
endif()

# --- Create static library if requested ---
if(VISTA_SDK_CPP_BUILD_STATIC)
	add_library(${PROJECT_NAME}-static STATIC)
	target_sources(${PROJECT_NAME}-static
		PUBLIC
			${PUBLIC_HEADERS}
		PRIVATE
			${PRIVATE_HEADERS}
			${PRIVATE_SOURCES}
	)

	set_target_properties(${PROJECT_NAME}-static PROPERTIES
		OUTPUT_NAME ${PROJECT_NAME}-static
		ARCHIVE_OUTPUT_DIRECTORY ${VISTA_SDK_CPP_BUILD_DIR}/lib
	)

	add_library(${PROJECT_NAME}::static ALIAS ${PROJECT_NAME}-static)
endif()

#----------------------------------------------
# Target properties
#----------------------------------------------

function(configure_vista_target target_name)
	# --- Include directories---
	target_include_directories(${target_name}
		PUBLIC
			$<BUILD_INTERFACE:${VISTA_SDK_CPP_INCLUDE_DIR}>
			$<INSTALL_INTERFACE:include>
		PRIVATE
			${VISTA_SDK_CPP_SOURCE_DIR}
	)

	# --- Link libraries---
	target_link_libraries(${target_name} PRIVATE
		nlohmann_json::nlohmann_json
		zlib
		fmt::fmt-header-only
	)

	# --- Precompiled headers---
	target_precompile_headers(${target_name} PRIVATE ${VISTA_SDK_CPP_SOURCE_DIR}/dnv/vista/sdk/pch.h)

	# --- Properties ---
	set_target_properties(${target_name} PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		POSITION_INDEPENDENT_CODE ON
		DEBUG_POSTFIX "-d"
	)
endfunction()

# --- Apply configuration to both targets ---
if(VISTA_SDK_CPP_BUILD_SHARED)
	configure_vista_target(${PROJECT_NAME})
	if(WIN32)
		set_target_properties(${PROJECT_NAME} PROPERTIES
			WINDOWS_EXPORT_ALL_SYMBOLS TRUE
		)

		configure_file(
			${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
			${CMAKE_BINARY_DIR}/version.rc
			@ONLY
		)
		target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/version.rc)
	endif()
endif()

if(VISTA_SDK_CPP_BUILD_STATIC)
	configure_vista_target(${PROJECT_NAME}-static)
endif()

#----------------------------------------------
# Compiler and linker configuration
#----------------------------------------------

include(CompilersSettings)

#----------------------------------------------
# Install rules
#----------------------------------------------

# --- Install shared library if built ---
if(VISTA_SDK_CPP_BUILD_SHARED)
	install(TARGETS ${PROJECT_NAME}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
endif()

# --- Install static library if built ---
if(VISTA_SDK_CPP_BUILD_STATIC)
	install(TARGETS ${PROJECT_NAME}-static
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
endif()

# --- Install public headers ---
install(DIRECTORY ${VISTA_SDK_CPP_INCLUDE_DIR}/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	FILES_MATCHING PATTERN "*.h" PATTERN "*.inl"
)

#----------------------------------------------
# Subdirectories
#----------------------------------------------

add_subdirectory(${VISTA_SDK_CPP_DOCUMENTATION_DIR} doc)
add_subdirectory(${VISTA_SDK_CPP_SAMPLES_DIR} samples)
add_subdirectory(${VISTA_SDK_CPP_TEST_DIR} test)
add_subdirectory(${VISTA_SDK_CPP_BENCHMARK_DIR} benchmark)

#----------------------------------------------
# Configuration summary
#----------------------------------------------

message(STATUS "=========================================================")
message(STATUS "====== VISTA SDK C++ library configuration summary ======")
message(STATUS "=========================================================")
message(STATUS "")

#----------------------------
# Project information
#----------------------------

message(STATUS "--- Project information ---")
message(STATUS "Project                         : ${PROJECT_NAME}")
message(STATUS "Version                         : ${PROJECT_VERSION}")
message(STATUS "Description                     : ${PROJECT_DESCRIPTION}")
message(STATUS "")

#----------------------------
# Directory paths
#----------------------------

message(STATUS "--- Directory paths ---")
message(STATUS "Main Vista SDK root directory   : ${VISTA_SDK_ROOT_DIR}")
message(STATUS "Main Vista SDK resources dir    : ${VISTA_SDK_RESOURCES_DIR}")
message(STATUS "Main Vista SDK test data dir    : ${VISTA_SDK_TESTDATA_DIR}")
message(STATUS "Vista c++ SDK root directory    : ${VISTA_SDK_CPP_ROOT_DIR}")
message(STATUS "Vista c++ SDK include directory : ${VISTA_SDK_CPP_INCLUDE_DIR}")
message(STATUS "Vista c++ SDK source directory  : ${VISTA_SDK_CPP_SOURCE_DIR}")
message(STATUS "Vista c++ SDK build directory   : ${VISTA_SDK_CPP_BUILD_DIR}")
message(STATUS "Install prefix                  : ${VISTA_SDK_CPP_INSTALL_DIR}")
message(STATUS "")

#----------------------------
# Build options
#----------------------------

message(STATUS "--- Build options ---")
if(VISTA_SDK_CPP_BUILD_STATIC AND VISTA_SDK_CPP_BUILD_SHARED)
	message(STATUS "Library Types                   : Static + Shared")
elseif(VISTA_SDK_CPP_BUILD_STATIC)
	message(STATUS "Library Type                    : Static")
elseif(VISTA_SDK_CPP_BUILD_SHARED)
	message(STATUS "Library Type                    : Shared")
endif()
message(STATUS "Build tests                     : ${VISTA_SDK_CPP_BUILD_TESTS}")
message(STATUS "Build smoke tests               : ${VISTA_SDK_CPP_BUILD_SMOKE_TESTS}")
message(STATUS "Run tests                       : ${VISTA_SDK_CPP_RUN_TESTS}")
message(STATUS "Build benchmarks                : ${VISTA_SDK_CPP_BUILD_BENCHMARKS}")
message(STATUS "Run benchmarks                  : ${VISTA_SDK_CPP_RUN_BENCHMARKS}")
message(STATUS "Build samples                   : ${VISTA_SDK_CPP_BUILD_SAMPLES}")
message(STATUS "Build documentation             : ${VISTA_SDK_CPP_BUILD_DOCUMENTATION}")
message(STATUS "Copy resources                  : ${VISTA_SDK_CPP_COPY_RESOURCES}")
message(STATUS "")

#----------------------------
# Dependencies
#----------------------------

message(STATUS "--- Dependencies ---")
message(STATUS "nlohmann/json version           : ${NLOHMANN_VERSION}")
message(STATUS "{fmt} version                   : ${FMT_VERSION}")
message(STATUS "zlib-ng version                 : ${ZLIBNG_HEADER_VERSION}")
message(STATUS "GoogleTest version              : ${GTEST_VERSION}")
message(STATUS "Google Benchmark version        : ${BENCHMARK_VERSION}")
message(STATUS "")

#----------------------------
# CMake configuration
#----------------------------

message(STATUS "--- CMake configuration ---")
message(STATUS "CMake version                   : ${CMAKE_VERSION}")
message(STATUS "Generator                       : ${CMAKE_GENERATOR}")
message(STATUS "Build configurations            : ${CMAKE_CONFIGURATION_TYPES}")
message(STATUS "")

#----------------------------
# Compiler configuration
#----------------------------

message(STATUS "--- Compiler configuration ---")
message(STATUS "C++ compiler                    : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ compiler path               : ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ standard                    : ${CMAKE_CXX_STANDARD}")
message(STATUS "")

#----------------------------
# Compile and link flags
#----------------------------

message(STATUS "--- Compile and link flags ---")

# --- Compile flags ---
if(CMAKE_CXX_FLAGS)
	message(STATUS "Default compile flags           : ${CMAKE_CXX_FLAGS}")
else()
	message(STATUS "Default compile flags           : (none)")
endif()
message(STATUS "Custom compile flags            : ${FORMATTED_COMPILE_FLAGS}")

# --- Linker flags ---
if(CMAKE_CXX_LINK_FLAGS)
	message(STATUS "Default link flags              : ${CMAKE_CXX_LINK_FLAGS}")
else()
	message(STATUS "Default link flags              : (none)")
endif()
message(STATUS "Custom link options             : ${FORMATTED_LINK_OPTIONS}")

# --- Definitions ---
message(STATUS "Compile definitions             : ${FORMATTED_COMPILE_DEFINITIONS}")
message(STATUS "")

