# ==============================================================================
# VISTA-SDK-CPP - Top-level CMake configuration
# ==============================================================================

#----------------------------------------------
# Project definition
#----------------------------------------------

cmake_minimum_required(VERSION 3.20)

project(vista-sdk-cpp
	VERSION 0.0.26
	DESCRIPTION "DNV VISTA SDK C++ port"
	LANGUAGES CXX
)

#----------------------------------------------
# Testing setup
#----------------------------------------------

enable_testing()

#----------------------------------------------
# Build options
#----------------------------------------------

option(VISTA_SDK_CPP_BUILD_STATIC          "Build static library"                       ON   )
option(VISTA_SDK_CPP_BUILD_SHARED          "Build shared library"                       ON   )
option(VISTA_SDK_CPP_BUILD_TESTS           "Build tests"                                ON   )
option(VISTA_SDK_CPP_BUILD_SMOKE_TESTS     "Build smoke tests"                          OFF  )
option(VISTA_SDK_CPP_RUN_TESTS             "Run tests after build"                      OFF  )
option(VISTA_SDK_CPP_BUILD_BENCHMARKS      "Build benchmarks"                           OFF  )
option(VISTA_SDK_CPP_RUN_BENCHMARKS        "Run benchmarks after build"                 OFF  )
option(VISTA_SDK_CPP_COPY_RESOURCES        "Copy resources to build directory"          ON   ) # TODO: Remove later
option(VISTA_SDK_CPP_BUILD_SAMPLES         "Build vista-sdk-cpp samples"                OFF  )
option(VISTA_SDK_CPP_BUILD_DOCUMENTATION   "Build vista-sdk-cpp doxygen documentation"  OFF  )

set(VISTA_SDK_CPP_INSTALL_DIR "" CACHE PATH "Install directory option (empty = use platform defaults)")

#----------------------------------------------
# Directory and CMake modules configuration
#----------------------------------------------

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH VISTA_SDK_ROOT_DIR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#----------------------------------------------
# Main configuration
#----------------------------------------------

include(dnvVistaCppConfig)
