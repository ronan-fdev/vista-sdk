# ==============================================================================
# VISTA-SDK-CPP - C++ Library dependencies configuration
# ==============================================================================

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Force static libraries for dependencies" FORCE)

#----------------------------------------------
# FetchContent dependencies
#----------------------------------------------

include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${VISTA_SDK_ROOT_DIR}/.deps")

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET OFF)

#----------------------------
# Dependency declarations
#----------------------------

# --- nlohmann/json ---
FetchContent_Declare(nlohmann_json
	URL https://github.com/nlohmann/json/releases/download/v3.12.0/include.zip
	URL_HASH SHA256=b8cb0ef2dd7f57f18933997c9934bb1fa962594f701cd5a8d3c2c80541559372
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# --- nfx-cpp-essentials ---
set(NFX_CPP_ESSENTIALS_BUILD_STATIC     ON   CACHE BOOL  ""  FORCE)
set(NFX_CPP_ESSENTIALS_BUILD_SHARED     OFF  CACHE BOOL  ""  FORCE)
set(NFX_CPP_ESSENTIALS_BUILD_TESTS      OFF  CACHE BOOL  ""  FORCE)
set(NFX_CPP_ESSENTIALS_BUILD_SAMPLES    OFF  CACHE BOOL  ""  FORCE)
set(NFX_CPP_ESSENTIALS_WITH_CONTAINERS  ON   CACHE BOOL  ""  FORCE)
set(NFX_CPP_ESSENTIALS_WITH_DATATYPES   ON   CACHE BOOL  ""  FORCE)
set(NFX_CPP_ESSENTIALS_WITH_MEMORY      ON   CACHE BOOL  ""  FORCE)
set(NFX_CPP_ESSENTIALS_WITH_STRING      ON   CACHE BOOL  ""  FORCE)
set(NFX_CPP_ESSENTIALS_WITH_TIME        ON   CACHE BOOL  ""  FORCE)

FetchContent_Declare(
	nfx-cpp-essentials
	GIT_REPOSITORY https://github.com/ronan-fdev/nfx-cpp-essentials.git
	GIT_TAG        0.8.2
	GIT_SHALLOW    TRUE
)


# --- zlib-ng ---
set(ZLIB_COMPAT                 ON   CACHE BOOL  "Compile with zlib compatible API"       FORCE)
set(ZLIB_ENABLE_TESTS           OFF  CACHE BOOL  "Build zlib-ng test binaries"            FORCE)
set(ZLIBNG_ENABLE_TESTS         OFF  CACHE BOOL  "Build zlib-ng test binaries"            FORCE)
set(WITH_GZFILEOP               ON   CACHE BOOL  "Support for gzFile related functions"   FORCE)
set(WITH_OPTIM                  ON   CACHE BOOL  "Build with optimisations"               FORCE)
set(WITH_NEW_STRATEGIES         ON   CACHE BOOL  "Use new strategies"                     FORCE)
set(WITH_NATIVE_INSTRUCTIONS    OFF  CACHE BOOL  "Use host-specific instructions"         FORCE)
set(WITH_RUNTIME_CPU_DETECTION  ON   CACHE BOOL  "Runtime CPU detection"                  FORCE)
set(WITH_SANITIZER              OFF  CACHE BOOL  "Build with sanitizer"                   FORCE)
set(WITH_GTEST                  OFF  CACHE BOOL  "Build gtest_zlib"                       FORCE)
set(WITH_FUZZERS                OFF  CACHE BOOL  "Build test/fuzz"                        FORCE)
set(WITH_BENCHMARKS             OFF  CACHE BOOL  "Build test/benchmarks"                  FORCE)
set(WITH_MAINTAINER_WARNINGS    OFF  CACHE BOOL  "Build with maintainer warnings"         FORCE)
set(WITH_CODE_COVERAGE          OFF  CACHE BOOL  "Enable code coverage reporting"         FORCE)

FetchContent_Declare(
	zlib-ng
	GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng.git
	GIT_TAG        2.2.5
	GIT_SHALLOW    TRUE
)

# --- {fmt} ---
set(FMT_FUZZ            OFF  CACHE BOOL  "Build fmt fuzzing tests"           FORCE)
set(FMT_TEST            OFF  CACHE BOOL  "Build fmt unit tests"              FORCE)
set(FMT_CUDA_TEST       OFF  CACHE BOOL  "Build fmt cuda tests"              FORCE)
set(FMT_DOC             OFF  CACHE BOOL  "Build fmt documentation"           FORCE)
set(FMT_INSTALL         OFF  CACHE BOOL  "Install fmt targets"               FORCE)
set(FMT_HEADER_ONLY     ON   CACHE BOOL  "Build fmt as header-only library"  FORCE)
set(FMT_MODULE          OFF  CACHE BOOL  "Enable fmt C++20 module support"   FORCE)
set(FMT_OS              OFF  CACHE BOOL  "Enable fmt OS-specific features"   FORCE)
set(FMT_SYSTEM_HEADERS  OFF  CACHE BOOL  "Use system headers for fmt"        FORCE)
set(FMT_UNICODE         ON   CACHE BOOL  "Enable Unicode support in fmt"     FORCE)

FetchContent_Declare(
	fmt
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
	GIT_TAG        11.2.0
	GIT_SHALLOW    TRUE
)

# --- Google test ---
if(VISTA_SDK_CPP_BUILD_TESTS)
	set(BUILD_GMOCK                     OFF  CACHE BOOL  "Build GoogleMock library"              FORCE)
	set(INSTALL_GTEST                   OFF  CACHE BOOL  "Install GoogleTest targets"            FORCE)
	set(GTEST_HAS_ABSL                  OFF  CACHE BOOL  "Disable Abseil support in GoogleTest"  FORCE)

	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
	    GIT_TAG        v1.17.0
		GIT_SHALLOW    TRUE
	)
endif()


# --- Google benchmark ---
if(VISTA_SDK_CPP_BUILD_BENCHMARKS)
	set(BENCHMARK_ENABLE_TESTING         OFF  CACHE BOOL  "Build benchmark tests"                          FORCE)
	set(BENCHMARK_ENABLE_EXCEPTIONS      ON   CACHE BOOL  "Enable exceptions in benchmark library"         FORCE)
	set(BENCHMARK_ENABLE_LTO             OFF  CACHE BOOL  "Enable link time optimization"                  FORCE)
	set(BENCHMARK_USE_LIBCXX             OFF  CACHE BOOL  "Use libc++ as the standard library"             FORCE)
	set(BENCHMARK_ENABLE_WERROR          OFF  CACHE BOOL  "Treat warnings as errors"                       FORCE)
	set(BENCHMARK_FORCE_WERROR           OFF  CACHE BOOL  "Force warnings as errors regardless of issues"  FORCE)
	set(BENCHMARK_BUILD_32_BITS          OFF  CACHE BOOL  "Build a 32-bit version of the library"          FORCE)
	set(BENCHMARK_ENABLE_INSTALL         OFF  CACHE BOOL  "Install benchmark targets"                      FORCE)
	set(BENCHMARK_ENABLE_DOXYGEN         OFF  CACHE BOOL  "Build documentation with Doxygen"               FORCE)
	set(BENCHMARK_INSTALL_DOCS           OFF  CACHE BOOL  "Install documentation"                          FORCE)
	set(BENCHMARK_DOWNLOAD_DEPENDENCIES  OFF  CACHE BOOL  "Download and build unmet dependencies"          FORCE)
	set(BENCHMARK_ENABLE_GTEST_TESTS     OFF  CACHE BOOL  "Build benchmark GTest-based tests"              FORCE)
	set(BENCHMARK_USE_BUNDLED_GTEST      OFF  CACHE BOOL  "Use bundled GoogleTest for benchmark"           FORCE)
	set(BENCHMARK_ENABLE_LIBPFM          OFF  CACHE BOOL  "Enable performance counters via libpfm"         FORCE)
	set(ENABLE_ASSEMBLY_TESTS_DEFAULT    OFF  CACHE BOOL  "Enable assembly tests by default"               FORCE)

	FetchContent_Declare(
		googlebenchmark
		GIT_REPOSITORY https://github.com/google/benchmark.git
		GIT_TAG        v1.9.4
		GIT_SHALLOW    TRUE
	)
endif()

#----------------------------
# Dependency fetching
#----------------------------

message(STATUS "Fetching dependencies...")

FetchContent_MakeAvailable(
	nfx-cpp-essentials
	nlohmann_json
	zlib-ng
	fmt
)

if(VISTA_SDK_CPP_BUILD_TESTS)
	FetchContent_MakeAvailable(
		googletest
	)
endif()

if(VISTA_SDK_CPP_BUILD_BENCHMARKS)
	FetchContent_MakeAvailable(
		googlebenchmark
	)
endif()

#----------------------------------------------
# Target configuration and version extraction
#----------------------------------------------

#----------------------------
# nlohmann_json configuration
#----------------------------

if(NOT TARGET nlohmann_json)
    add_library(nlohmann_json INTERFACE)
    target_include_directories(nlohmann_json INTERFACE ${nlohmann_json_SOURCE_DIR}/include)
    add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
endif()

if(nlohmann_json_SOURCE_DIR AND EXISTS "${nlohmann_json_SOURCE_DIR}/single_include/nlohmann/json.hpp")
	file(STRINGS "${nlohmann_json_SOURCE_DIR}/single_include/nlohmann/json.hpp" NLOHMANN_JSON_VER_LINES
		REGEX "^#define[ \t]+NLOHMANN_JSON_VERSION_")
	if(NLOHMANN_JSON_VER_LINES)
		string(REGEX MATCHALL "[0-9]+" NLOHMANN_JSON_VER_NUMBERS "${NLOHMANN_JSON_VER_LINES}")
		list(GET NLOHMANN_JSON_VER_NUMBERS 0 NLOHMANN_JSON_VERSION_MAJOR)
		list(GET NLOHMANN_JSON_VER_NUMBERS 1 NLOHMANN_JSON_VERSION_MINOR)
		list(GET NLOHMANN_JSON_VER_NUMBERS 2 NLOHMANN_JSON_VERSION_PATCH)
		set(NLOHMANN_VERSION "${NLOHMANN_JSON_VERSION_MAJOR}.${NLOHMANN_JSON_VERSION_MINOR}.${NLOHMANN_JSON_VERSION_PATCH}")
	endif()
endif()

#----------------------------
# zlib-ng configuration
#----------------------------

if(TARGET zlib)
	set_target_properties(zlib PROPERTIES
		EXCLUDE_FROM_ALL TRUE
		EXCLUDE_FROM_DEFAULT_BUILD TRUE
		POSITION_INDEPENDENT_CODE TRUE
	)
endif()

if(zlib-ng_BINARY_DIR AND EXISTS "${zlib-ng_BINARY_DIR}/zlib.h")
	file(STRINGS "${zlib-ng_BINARY_DIR}/zlib.h" ZLIBNG_VER_LINES
		REGEX "^#define[ \t]+ZLIBNG_VERSION[ \t]+\"[^\"]*\"")
	if(ZLIBNG_VER_LINES)
		string(REGEX REPLACE "^#define[ \t]+ZLIBNG_VERSION[ \t]+\"([^\"]*)\".*" "\\1"
			ZLIBNG_HEADER_VERSION "${ZLIBNG_VER_LINES}")
	endif()
endif()

#----------------------------
# {fmt} configuration
#----------------------------

if(TARGET fmt::fmt)
	get_target_property(FMT_VERSION fmt::fmt VERSION)
endif()

#----------------------------
# googletest configuration
#----------------------------

if(TARGET GTest::gtest)
	get_target_property(GTEST_VERSION GTest::gtest VERSION)
endif()

#----------------------------
# googlebenchmark configuration
#----------------------------

if(TARGET benchmark::benchmark)
	get_target_property(BENCHMARK_VERSION benchmark::benchmark VERSION)
endif()
