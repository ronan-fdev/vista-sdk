# ==============================================================================
# VISTA-SDK-CPP - C++ library CMake configuration
# ==============================================================================

cmake_minimum_required(VERSION 3.20)

# --- Project setup ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

# --- Dependencies ---
include(Dependencies.cmake)

# --- Build options ---
option(VISTA_SDK_CPP_BUILD_STATIC          "Build static libraries instead of shared"                  OFF    )
option(VISTA_SDK_CPP_BUILD_TESTS           "Build tests"                                               ON     )
option(VISTA_SDK_CPP_BUILD_SMOKE_TESTS     "Build smoke tests"                                         OFF    )
option(VISTA_SDK_CPP_RUN_TESTS             "Run tests after build"                                     OFF    )
option(VISTA_SDK_CPP_BUILD_BENCHMARKS      "Build benchmarks"                                          ON     )
option(VISTA_SDK_CPP_RUN_BENCHMARKS        "Run benchmarks after build"                                OFF    )
option(VISTA_SDK_CPP_COPY_RESOURCES        "Copy resources to build directory"                         ON     )
option(VISTA_SDK_CPP_BUILD_SAMPLES         "Build vista-sdk-cpp samples"                               OFF    )
option(VISTA_SDK_CPP_BUILD_DOCUMENTATION   "Build vista-sdk-cpp doxygen documentation"                 OFF    )

# --- Platform specific settings ---
if(MSVC)
	set(WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
	set(BUILD_SHARED_LIBS NOT ${VISTA_SDK_CPP_BUILD_STATIC})
endif()

# --- Ccache setup ---
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
	message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
	set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
	set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
	set(ENV{CCACHE_SLOPPINESS} "pch_defines,time_macros")
	set(ENV{CCACHE_MAXSIZE} "5G")
	if(MSVC)
		set(ENV{CCACHE_CPP2} "yes")
		set(ENV{CCACHE_BASEDIR} "${CMAKE_SOURCE_DIR}")
	endif()
	message(STATUS "ccache enabled - compilation will be accelerated")
else()
	message(STATUS "ccache not found - normal compilation will be used")
endif()

# --- Output directories ---
set(VISTA_SDK_CPP_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set(VISTA_SDK_CPP_DOC_DIR ${CMAKE_BINARY_DIR}/doc)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VISTA_SDK_CPP_BIN_DIR}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${VISTA_SDK_CPP_BIN_DIR}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${VISTA_SDK_CPP_BIN_DIR}/$<CONFIG>)

# --- Copy resources at build time ---
if(VISTA_SDK_CPP_COPY_RESOURCES)
	if(EXISTS ${VISTA_SDK_RESOURCE_DIR})
		set(VISTA_SDK_CPP_RESOURCE_OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)
		add_custom_target(CopyResources ALL
			COMMAND ${CMAKE_COMMAND} -E make_directory ${VISTA_SDK_CPP_RESOURCE_OUTPUT_DIR}
			COMMAND ${CMAKE_COMMAND} -E copy_directory
				${VISTA_SDK_RESOURCE_DIR} ${VISTA_SDK_CPP_RESOURCE_OUTPUT_DIR}
			COMMENT "Copying VISTA SDK resources to build directory ($<CONFIG>)"
			VERBATIM
		)
		message(STATUS "Resources will be copied from ${VISTA_SDK_RESOURCE_DIR} to ${VISTA_SDK_CPP_RESOURCE_OUTPUT_DIR} at build time")
	else()
		message(WARNING "Resources directory not found: ${VISTA_SDK_RESOURCE_DIR}. Skipping resource copy.")
	endif()
else()
	message(STATUS "Skipping resource copy based on VISTA_SDK_CPP_COPY_RESOURCES option.")
endif()

# --- source files ---
include(Sources.cmake)

# --- Library definition ---
if(VISTA_SDK_CPP_BUILD_STATIC)
	add_library(${PROJECT_NAME} STATIC)
else()
	add_library(${PROJECT_NAME} SHARED)
endif()

target_sources(${PROJECT_NAME}
	PUBLIC
		${PUBLIC_HEADERS}
	PRIVATE
		${PRIVATE_HEADERS}
		${PRIVATE_SOURCES}
)

# --- Library versioning ---
configure_file(
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/internal/version.h.in
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/internal/version.h
	@ONLY
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}
)

if(WIN32 AND NOT VISTA_SDK_CPP_BUILD_STATIC)
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in
		${CMAKE_BINARY_DIR}/version.rc
		@ONLY
	)
	target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/version.rc)
endif()

# --- Compiler settings ---
include(CompilersSettings.cmake)

# --- Precompiled headers ---
target_precompile_headers(${PROJECT_NAME} PRIVATE ${VISTA_SDK_CPP_SOURCE_DIR}/dnv/vista/sdk/pch.h)
target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${VISTA_SDK_CPP_INCLUDE_DIR}>
		$<INSTALL_INTERFACE:include>
	PRIVATE
		${VISTA_SDK_CPP_SOURCE_DIR}
		${zlib_SOURCE_DIR}
)

# --- Linker ---
target_link_libraries(${PROJECT_NAME} PRIVATE
	nlohmann_json::nlohmann_json
	zlibstatic
	cpuid
	fmt::fmt
)

# --- Samples subdirectory ---
if(VISTA_SDK_CPP_BUILD_SAMPLES)
	#add_subdirectory(${VISTA_SDK_CPP_SAMPLES_DIR} samples)
endif()

# --- Tests subdirectory ---
add_subdirectory(${VISTA_SDK_CPP_TEST_DIR} test)

# --- Benchmark subdirectory ---
add_subdirectory(${VISTA_SDK_CPP_BENCHMARK_DIR} benchmark)

# --- Doxygen documentation subdirectory ---
if(VISTA_SDK_CPP_BUILD_DOCUMENTATION)
	add_subdirectory(${VISTA_SDK_CPP_DOCUMENTATION_DIR} doc)
endif()

# --- C++ library configuration summary ---
get_directory_property(COMPILE_OPTIONS COMPILE_OPTIONS)
get_directory_property(COMPILE_DEFINITIONS COMPILE_DEFINITIONS)
get_directory_property(LINK_OPTIONS LINK_OPTIONS)
string(REPLACE ";" " " FORMATTED_COMPILE_OPTIONS "${COMPILE_OPTIONS}")
string(REPLACE ";" " " FORMATTED_COMPILE_DEFINITIONS "${COMPILE_DEFINITIONS}")
string(REPLACE ";" " " FORMATTED_LINK_OPTIONS "${LINK_OPTIONS}")

message(STATUS "=========================================================")
message(STATUS "========== VISTA SDK C++ library configuration ==========")
message(STATUS "=========================================================")

message(STATUS "Project             : ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "Description         : ${PROJECT_DESCRIPTION}")
message(STATUS "Top-Level Source Dir: ${CMAKE_SOURCE_DIR}")
message(STATUS "Build Directory     : ${CMAKE_BINARY_DIR}")
message(STATUS "Install Prefix      : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Generator           : ${CMAKE_GENERATOR}")
message(STATUS "Platform            : ${CMAKE_SYSTEM_NAME} (${CMAKE_SYSTEM_PROCESSOR})")
if(CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Build Type          : Multi-config (${CMAKE_CONFIGURATION_TYPES})")
else()
	message(STATUS "Build Type          : ${CMAKE_BUILD_TYPE}")
endif()
message(STATUS "Build Shared Libs   : ${BUILD_SHARED_LIBS}")
message(STATUS "C++ Compiler        : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Compiler Path   : ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Standard        : ${CMAKE_CXX_STANDARD}")

message(STATUS "--------- Build Options ---------")
message(STATUS "Build Tests         : ${VISTA_SDK_CPP_BUILD_TESTS}")
message(STATUS "Build Smoke Tests   : ${VISTA_SDK_CPP_BUILD_SMOKE_TESTS}")
message(STATUS "Run Tests           : ${VISTA_SDK_CPP_RUN_TESTS}")
message(STATUS "Build Benchmarks    : ${VISTA_SDK_CPP_BUILD_BENCHMARKS}")
message(STATUS "Run Benchmarks      : ${VISTA_SDK_CPP_RUN_BENCHMARKS}")
message(STATUS "Build Samples       : ${VISTA_SDK_CPP_BUILD_SAMPLES}")
message(STATUS "Build Documentation : ${VISTA_SDK_CPP_BUILD_DOCUMENTATION}")
message(STATUS "Copy Resources      : ${VISTA_SDK_CPP_COPY_RESOURCES}")

message(STATUS "--------- Paths ---------")
if(VISTA_SDK_CPP_COPY_RESOURCES)
	message(STATUS "Resources Source     : ${VISTA_SDK_RESOURCE_DIR}")
	message(STATUS "Resources Target     : ${VISTA_SDK_CPP_BIN_DIR}/<CONFIG>/resources")
endif()
message(STATUS "Library Include Dir  : ${VISTA_SDK_CPP_INCLUDE_DIR}")
message(STATUS "Library Source Dir   : ${VISTA_SDK_CPP_SOURCE_DIR}")
message(STATUS "Output Base Directory: ${VISTA_SDK_CPP_BIN_DIR}")
if(CCACHE_PROGRAM)
	message(STATUS "ccache Enabled: ${CCACHE_PROGRAM}")
else()
	message(STATUS "ccache Enabled: No")
endif()
message(STATUS "--------- Compiler Options ---------")
message(STATUS "Compile Options    : ${FORMATTED_COMPILE_OPTIONS}")
message(STATUS "Compile Definitions: ${FORMATTED_COMPILE_DEFINITIONS}")
message(STATUS "Link Options       : ${FORMATTED_LINK_OPTIONS}")

message(STATUS "--------- System Information ---------")
cmake_host_system_information(RESULT TOTAL_MEMORY QUERY TOTAL_PHYSICAL_MEMORY)
cmake_host_system_information(RESULT AVAILABLE_MEMORY QUERY AVAILABLE_PHYSICAL_MEMORY)
math(EXPR MEMORY_GB "${TOTAL_MEMORY} / 1024")
message(STATUS "System Memory      : ${MEMORY_GB} GB total")
message(STATUS "Processor Count    : ${CMAKE_HOST_SYSTEM_PROCESSOR}")

message(STATUS "--------- Dependencies ---------")

if(TARGET nlohmann_json::nlohmann_json)
	string(REPLACE "v" "" NLOHMANN_VERSION "${NLOHMANN_JSON_VERSION}")
endif()

if(TARGET fmt::fmt)
	get_target_property(FMT_VERSION fmt::fmt VERSION)
endif()

if(TARGET zlibstatic)
	get_target_property(ZLIB_VERSION zlibstatic VERSION)
	if(NOT ZLIB_VERSION)
		if(zlib_SOURCE_DIR AND EXISTS "${zlib_SOURCE_DIR}/zlib.h")
			file(STRINGS "${zlib_SOURCE_DIR}/zlib.h" ZLIB_VER_LINES
				REGEX "^#define[ \t]+ZLIB_VERSION[ \t]+\"[^\"]*\"")
			if(ZLIB_VER_LINES)
				string(REGEX REPLACE "^#define[ \t]+ZLIB_VERSION[ \t]+\"([^\"]*)\".*" "\\1"
					ZLIB_VERSION "${ZLIB_VER_LINES}")
			endif()
		endif()
	endif()
endif()

if(TARGET cpuid)
	set(CPUID_VERSION "")
	if(cpuid_SOURCE_DIR AND EXISTS "${cpuid_SOURCE_DIR}/CMakeLists.txt")
		file(STRINGS "${cpuid_SOURCE_DIR}/CMakeLists.txt" CPUID_VER_LINES
			REGEX "^set\\(VERSION \"[^\"]*\"\\)$")
		if(CPUID_VER_LINES)
			string(REGEX REPLACE "^set\\(VERSION \"([^\"]*)\"\\)$" "\\1"
				CPUID_VERSION "${CPUID_VER_LINES}")
		endif()
	endif()
	if(NOT CPUID_VERSION)
		get_target_property(CPUID_VERSION cpuid VERSION)
	endif()
endif()

if(TARGET GTest::gtest)
	get_target_property(GTEST_VERSION GTest::gtest VERSION)
endif()

if(TARGET benchmark::benchmark)
	get_target_property(BENCHMARK_VERSION benchmark::benchmark VERSION)
endif()

message(STATUS "nlohmann/json version   : ${NLOHMANN_VERSION}")
message(STATUS "{fmt} version           : ${FMT_VERSION}")
message(STATUS "zlib version            : ${ZLIB_VERSION}")
message(STATUS "libcpuid version        : ${CPUID_VERSION}")
message(STATUS "GoogleTest version      : ${GTEST_VERSION}")
message(STATUS "Google Benchmark version: ${BENCHMARK_VERSION}")

if(CCACHE_PROGRAM)
	message(STATUS "--------- ccache Statistics ---------")
	execute_process(
		COMMAND ${CCACHE_PROGRAM} -s
		OUTPUT_VARIABLE CCACHE_STATS
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	string(REPLACE "\n" ";" CCACHE_STATS_LINES "${CCACHE_STATS}")
	foreach(LINE ${CCACHE_STATS_LINES})
		message(STATUS "${LINE}")
	endforeach()

	execute_process(
		COMMAND ${CCACHE_PROGRAM} -s
		OUTPUT_VARIABLE CCACHE_STATS_RAW
	)

	if(CCACHE_STATS_RAW MATCHES "cache hit rate ([0-9.]+)%")
		message(STATUS "Cache Efficiency    : ${CMAKE_MATCH_1}% hit rate")
	endif()
endif()
