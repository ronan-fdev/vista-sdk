# ==============================================================================
# VISTA-SDK-CPP - C++ Library Dependencies
# ==============================================================================

# --- Header-only dependencies ---
set(NLOHMANN_JSON_VERSION "v3.12.0")
set(NLOHMANN_JSON_URL "https://github.com/nlohmann/json/releases/download/${NLOHMANN_JSON_VERSION}/json.hpp")
set(NLOHMANN_JSON_INCLUDE_DIR "${CMAKE_BINARY_DIR}/_deps/nlohmann_json-include")
set(NLOHMANN_JSON_HEADER "${NLOHMANN_JSON_INCLUDE_DIR}/nlohmann/json.hpp")

if(NOT EXISTS ${NLOHMANN_JSON_HEADER})
	message(STATUS "Downloading nlohmann/json header (${NLOHMANN_JSON_VERSION})...")
	file(DOWNLOAD ${NLOHMANN_JSON_URL} ${NLOHMANN_JSON_HEADER}
		 SHOW_PROGRESS
		 STATUS download_status
		 LOG download_log)
	list(GET download_status 0 download_code)
	if(NOT download_code EQUAL 0)
		message(FATAL_ERROR "Failed to download nlohmann/json header. Error: ${download_log}")
	else()
		message(STATUS "Downloaded nlohmann/json header to ${NLOHMANN_JSON_HEADER}")
	endif()
else()
	message(STATUS "Found existing nlohmann/json header: ${NLOHMANN_JSON_HEADER}")
endif()

add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${NLOHMANN_JSON_INCLUDE_DIR})
add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)

# --- FetchContent dependencies ---
include(FetchContent)

# --- spdlog ---
set(SPDLOG_ENABLE_PCH        ON  CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE     OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE_HO  OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS       OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH       OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_WARNINGS    OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL      ON  CACHE BOOL "" FORCE)
set(SPDLOG_NO_EXCEPTIONS     ON  CACHE BOOL "" FORCE)
set(SPDLOG_PREVENT_CHILD_FD  OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
	spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog.git
	GIT_TAG        v1.15.3
	GIT_SHALLOW    TRUE
)

# --- zlib ---
set(ZLIB_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ZLIB_BUILD_STATIC  ON  CACHE BOOL "" FORCE)
set(ZLIB_BUILD_MINIZIP OFF CACHE BOOL "" FORCE)
set(ZLIB_PREFIX        OFF CACHE BOOL "" FORCE)
set(ZLIB_INSTALL       OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
	zlib
	GIT_REPOSITORY https://github.com/madler/zlib.git
	GIT_TAG        v1.3.1
	GIT_SHALLOW    TRUE
)

# --- {cpuid} ---
set(LIBCPUID_ENABLE_TESTS   OFF CACHE BOOL "" FORCE)
set(LIBCPUID_BUILD_DRIVERS  OFF CACHE BOOL "" FORCE)
set(LIBCPUID_ENABLE_DOCS    OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
	cpuid
	GIT_REPOSITORY https://github.com/anrieff/libcpuid.git
	GIT_TAG        v0.8.0
	GIT_SHALLOW    TRUE
)

# --- {fmt} ---
set(FMT_FUZZ         OFF CACHE BOOL "" FORCE)
set(FMT_TEST         OFF CACHE BOOL "" FORCE)
set(FMT_DOC          OFF CACHE BOOL "" FORCE)
set(FMT_INSTALL      OFF CACHE BOOL "" FORCE)
set(FMT_HEADER_ONLY  ON  CACHE BOOL "" FORCE)
set(FMT_OS           OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
	fmt
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
	GIT_TAG        11.2.0
	GIT_SHALLOW    TRUE
)

# --- Google test ---
set(BUILD_GMOCK   OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK   OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG        v1.17.0
	GIT_SHALLOW    TRUE
)

# --- Google benchmark ---
set(BENCHMARK_ENABLE_INSTALL         OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_TESTING         OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS     OFF CACHE BOOL "" FORCE)
set(BENCHMARK_USE_BUNDLED_GTEST      OFF CACHE BOOL "" FORCE)
set(BENCHMARK_DOWNLOAD_DEPENDENCIES  OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
	googlebenchmark
	GIT_REPOSITORY https://github.com/google/benchmark.git
	GIT_TAG        v1.9.4
	GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(fmt spdlog zlib cpuid googletest googlebenchmark)
