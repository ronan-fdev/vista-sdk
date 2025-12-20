#==============================================================================
# vista-sdk-cpp - Dependencies configuration
#==============================================================================

#----------------------------------------------
# Output configuration
#----------------------------------------------

set(_SAVED_CMAKE_REQUIRED_QUIET    ${CMAKE_REQUIRED_QUIET})
set(_SAVED_CMAKE_MESSAGE_LOG_LEVEL ${CMAKE_MESSAGE_LOG_LEVEL})
set(_SAVED_CMAKE_FIND_QUIETLY      ${CMAKE_FIND_QUIETLY})

set(CMAKE_REQUIRED_QUIET    ON     )
set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE) # [ERROR, WARNING, NOTICE, STATUS, VERBOSE, DEBUG]
set(CMAKE_FIND_QUIETLY      ON     )

#----------------------------------------------
# Dependency versions
#----------------------------------------------

set(VISTA_SDK_CPP_ZLIB_NG_VERSION                "2.3.1")
set(VISTA_SDK_CPP_DEPS_NFX_SERIALIZATION_VERSION "0.3.0")
set(VISTA_SDK_CPP_DEPS_NFX_STRINGBUILDER_VERSION "0.4.1")
set(VISTA_SDK_CPP_DEPS_NFX_STRINGUTILS_VERSION   "0.3.0")

#----------------------------------------------
# FetchContent dependencies
#----------------------------------------------

include(FetchContent)

if(DEFINED ENV{CI})
	set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
else()
	set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
endif()
set(FETCHCONTENT_QUIET OFF)

# --- zlib-ng ---
find_package(zlib-ng ${VISTA_SDK_CPP_ZLIB_NG_VERSION} EXACT QUIET)
if(NOT zlib-ng_FOUND)
	set(_SAVED_BUILD_SHARED_LIBS                ${BUILD_SHARED_LIBS})
	set(_SAVED_CMAKE_POSITION_INDEPENDENT_CODE  ${CMAKE_POSITION_INDEPENDENT_CODE})

	set(BUILD_SHARED_LIBS               OFF CACHE BOOL "Build shared libraries"               FORCE)
	set(CMAKE_POSITION_INDEPENDENT_CODE ON  CACHE BOOL "Build with -fPIC"                     FORCE)

	set(ZLIB_COMPAT                     OFF CACHE BOOL "Compile with zlib compatible API"     FORCE)
	set(BUILD_TESTING                   OFF CACHE BOOL "Build zlib-ng test binaries"          FORCE)
	set(ZLIBNG_ENABLE_TESTS             OFF CACHE BOOL "Build zlib-ng test binaries"          FORCE)
	set(WITH_GZFILEOP                   ON  CACHE BOOL "Support for gzFile related functions" FORCE)
	set(WITH_OPTIM                      ON  CACHE BOOL "Build with optimisations"             FORCE)
	set(WITH_NEW_STRATEGIES             ON  CACHE BOOL "Use new strategies"                   FORCE)
	set(WITH_NATIVE_INSTRUCTIONS        OFF CACHE BOOL "Use host-specific instructions"       FORCE)
	set(WITH_RUNTIME_CPU_DETECTION      OFF CACHE BOOL "Runtime CPU detection"                FORCE)
	set(WITH_SANITIZER                  OFF CACHE BOOL "Build with sanitizer"                 FORCE)
	set(WITH_GTEST                      OFF CACHE BOOL "Build gtest_zlib"                     FORCE)
	set(WITH_FUZZERS                    OFF CACHE BOOL "Build test/fuzz"                      FORCE)
	set(WITH_BENCHMARKS                 OFF CACHE BOOL "Build test/benchmarks"                FORCE)
	set(WITH_MAINTAINER_WARNINGS        OFF CACHE BOOL "Build with maintainer warnings"       FORCE)
	set(WITH_CODE_COVERAGE              OFF CACHE BOOL "Enable code coverage reporting"       FORCE)

	FetchContent_Declare(
		zlib-ng
		GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng.git
		GIT_TAG        ${VISTA_SDK_CPP_ZLIB_NG_VERSION}
		GIT_SHALLOW    TRUE
	)
endif()

if(NOT zlib-ng_FOUND)
	FetchContent_MakeAvailable(zlib-ng)
	set(BUILD_SHARED_LIBS ${_SAVED_BUILD_SHARED_LIBS}                             CACHE BOOL "Build shared libraries" FORCE)
	set(CMAKE_POSITION_INDEPENDENT_CODE ${_SAVED_CMAKE_POSITION_INDEPENDENT_CODE} CACHE BOOL "Build with -fPIC"       FORCE)
endif()

# --- nfx-serialization ---
find_package(nfx-serialization ${VISTA_SDK_CPP_DEPS_NFX_SERIALIZATION_VERSION} EXACT QUIET)
if(NOT nfx-serialization_FOUND)
	set(NFX_SERIALIZATION_WITH_JSON             ON  CACHE BOOL "Enable JSON support"         FORCE)
	set(NFX_SERIALIZATION_BUILD_STATIC          ON  CACHE BOOL "Build static library"        FORCE)
	set(NFX_SERIALIZATION_BUILD_SHARED          OFF CACHE BOOL "Build shared library"        FORCE)
	set(NFX_SERIALIZATION_BUILD_TESTS           OFF CACHE BOOL "Build tests"                 FORCE)
	set(NFX_SERIALIZATION_BUILD_EXTENSION_TESTS OFF CACHE BOOL "Build extension tests"       FORCE)
	set(NFX_SERIALIZATION_BUILD_SAMPLES         OFF CACHE BOOL "Build samples"               FORCE)
	set(NFX_SERIALIZATION_BUILD_BENCHMARKS      OFF CACHE BOOL "Build benchmarks"            FORCE)
	set(NFX_SERIALIZATION_BUILD_DOCUMENTATION   OFF CACHE BOOL "Build Doxygen documentation" FORCE)
	set(NFX_SERIALIZATION_INSTALL_PROJECT       OFF CACHE BOOL "Install project"             FORCE)
	set(NFX_SERIALIZATION_PACKAGE_SOURCE        OFF CACHE BOOL "Enable source package"       FORCE)
	set(NFX_SERIALIZATION_PACKAGE_ARCHIVE       OFF CACHE BOOL "Enable archive package"      FORCE)
	set(NFX_SERIALIZATION_PACKAGE_DEB           OFF CACHE BOOL "Enable DEB package"          FORCE)
	set(NFX_SERIALIZATION_PACKAGE_RPM           OFF CACHE BOOL "Enable RPM package"          FORCE)
	set(NFX_SERIALIZATION_PACKAGE_WIX           OFF CACHE BOOL "Enable WIX package"          FORCE)

	FetchContent_Declare(
		nfx-serialization
		GIT_REPOSITORY https://github.com/nfx-libs/nfx-serialization
		GIT_TAG        ${VISTA_SDK_CPP_DEPS_NFX_SERIALIZATION_VERSION}
		GIT_SHALLOW    TRUE
	)
endif()

if(NOT nfx-serialization_FOUND)
	FetchContent_MakeAvailable(nfx-serialization)
endif()

# --- nfx-stringbuilder ---
find_package(nfx-stringbuilder ${VISTA_SDK_CPP_DEPS_NFX_STRINGBUILDER_VERSION} EXACT QUIET)
if(NOT nfx-stringbuilder_FOUND)
	set(NFX_STRINGBUILDER_BUILD_STATIC        ON  CACHE BOOL "Build static library"        FORCE)
	set(NFX_STRINGBUILDER_BUILD_SHARED        OFF CACHE BOOL "Build shared library"        FORCE)
	set(NFX_STRINGBUILDER_BUILD_TESTS         OFF CACHE BOOL "Build tests"                 FORCE)
	set(NFX_STRINGBUILDER_BUILD_SAMPLES       OFF CACHE BOOL "Build samples"               FORCE)
	set(NFX_STRINGBUILDER_BUILD_BENCHMARKS    OFF CACHE BOOL "Build benchmarks"            FORCE)
	set(NFX_STRINGBUILDER_BUILD_DOCUMENTATION OFF CACHE BOOL "Build Doxygen documentation" FORCE)
	set(NFX_STRINGBUILDER_INSTALL_PROJECT     OFF CACHE BOOL "Install project"             FORCE)
	set(NFX_STRINGBUILDER_PACKAGE_SOURCE      OFF CACHE BOOL "Enable source package"       FORCE)
	set(NFX_STRINGBUILDER_PACKAGE_ARCHIVE     OFF CACHE BOOL "Enable archive package"      FORCE)
	set(NFX_STRINGBUILDER_PACKAGE_DEB         OFF CACHE BOOL "Enable DEB package"          FORCE)
	set(NFX_STRINGBUILDER_PACKAGE_RPM         OFF CACHE BOOL "Enable RPM package"          FORCE)
	set(NFX_STRINGBUILDER_PACKAGE_WIX         OFF CACHE BOOL "Enable WIX package"          FORCE)

	FetchContent_Declare(
		nfx-stringbuilder
		GIT_REPOSITORY https://github.com/nfx-libs/nfx-stringbuilder
		GIT_TAG        ${VISTA_SDK_CPP_DEPS_NFX_STRINGBUILDER_VERSION}
		GIT_SHALLOW    TRUE
	)
endif()

if(NOT nfx-stringbuilder_FOUND)
	FetchContent_MakeAvailable(nfx-stringbuilder)
endif()

# --- nfx-stringutils ---
find_package(nfx-stringutils ${VISTA_SDK_CPP_DEPS_NFX_STRINGUTILS_VERSION} EXACT QUIET)
if(NOT nfx-stringutils_FOUND)
	set(NFX_STRINGUTILS_BUILD_TESTS         OFF CACHE BOOL "Build tests"                 FORCE)
	set(NFX_STRINGUTILS_BUILD_SAMPLES       OFF CACHE BOOL "Build samples"               FORCE)
	set(NFX_STRINGUTILS_BUILD_BENCHMARKS    OFF CACHE BOOL "Build benchmarks"            FORCE)
	set(NFX_STRINGUTILS_BUILD_DOCUMENTATION OFF CACHE BOOL "Build Doxygen documentation" FORCE)
	set(NFX_STRINGUTILS_INSTALL_PROJECT     OFF CACHE BOOL "Install project"             FORCE)
	set(NFX_STRINGUTILS_PACKAGE_SOURCE      OFF CACHE BOOL "Enable source package"       FORCE)
	set(NFX_STRINGUTILS_PACKAGE_ARCHIVE     OFF CACHE BOOL "Enable archive package"      FORCE)
	set(NFX_STRINGUTILS_PACKAGE_DEB         OFF CACHE BOOL "Enable DEB package"          FORCE)
	set(NFX_STRINGUTILS_PACKAGE_RPM         OFF CACHE BOOL "Enable RPM package"          FORCE)
	set(NFX_STRINGUTILS_PACKAGE_WIX         OFF CACHE BOOL "Enable WIX package"          FORCE)

	FetchContent_Declare(
		nfx-stringutils
		GIT_REPOSITORY https://github.com/nfx-libs/nfx-stringutils
		GIT_TAG        ${VISTA_SDK_CPP_DEPS_NFX_STRINGUTILS_VERSION}
		GIT_SHALLOW    TRUE
	)
endif()

if(NOT nfx-stringutils_FOUND)
	FetchContent_MakeAvailable(nfx-stringutils)
endif()

#----------------------------------------------
# Cleanup
#----------------------------------------------

set(CMAKE_REQUIRED_QUIET    ${_SAVED_CMAKE_REQUIRED_QUIET})
set(CMAKE_MESSAGE_LOG_LEVEL ${_SAVED_CMAKE_MESSAGE_LOG_LEVEL})
set(CMAKE_FIND_QUIETLY      ${_SAVED_CMAKE_FIND_QUIETLY})
