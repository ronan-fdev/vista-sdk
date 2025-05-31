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

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.15.2
  GIT_SHALLOW    TRUE
  CMAKE_ARGS
	-DSPDLOG_BUILD_SHARED=OFF
	-DSPDLOG_BUILD_EXAMPLE=OFF
	-DSPDLOG_BUILD_TESTING=OFF
	-DSPDLOG_FMT_EXTERNAL=OFF
	-DSPDLOG_BUILD_BENCH=OFF
)

FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG        v1.3.1
  GIT_SHALLOW    TRUE
  CMAKE_ARGS
	-DZLIB_BUILD_SHARED=OFF
	-DZLIB_BUILD_TESTING=OFF
	-DZLIB_BUILD_STATIC=ON
	-DZLIB_BUILD_MINIZIP=OFF
	-DZLIB_INSTALL=OFF
)

FetchContent_Declare(
  cpuid
  GIT_REPOSITORY https://github.com/anrieff/libcpuid.git
  GIT_TAG        v0.7.0
  GIT_SHALLOW    TRUE
  CMAKE_ARGS
	-DBUILD_SHARED_LIBS=OFF
	-DLIBCPUID_ENABLE_TESTS=OFF
	-DLIBCPUID_BUILD_DRIVERS=OFF
	-DLIBCPUID_ENABLE_DOCS=OFF
)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        11.0.2
  GIT_SHALLOW    TRUE
  CMAKE_ARGS
	-DBUILD_SHARED_LIBS=OFF
	-DFMT_TEST=OFF
	-DFMT_DOC=OFF
	-DFMT_INSTALL=OFF
)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.16.0
  GIT_SHALLOW    TRUE
  CMAKE_ARGS
	-Dgtest_build_shared_libs=OFF
	-DBUILD_SHARED_LIBS=OFF
	-DBUILD_GMOCK=OFF
	-DINSTALL_GTEST=OFF
	-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>DLL
)

FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.9.4
  GIT_SHALLOW    TRUE
  CMAKE_ARGS
	-DBENCHMARK_ENABLE_TESTING=OFF
	-DBENCHMARK_ENABLE_GTEST_TESTS=OFF
	-DBENCHMARK_ENABLE_INSTALL=OFF
	-DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON
	-DBUILD_SHARED_LIBS=OFF
	-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>DLL
)

FetchContent_MakeAvailable(fmt spdlog zlib cpuid googletest googlebenchmark)
