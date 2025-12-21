#==============================================================================
# vista-sdk-cpp - Top-level CMake configuration
#==============================================================================

#----------------------------------------------
# Project definition
#----------------------------------------------

cmake_minimum_required(VERSION 3.20)

project(vista-sdk-cpp
	VERSION 0.0.0
	DESCRIPTION "C++ implementation of DNV Vista SDK for VIS and ISO 19847/19848 standards"
	HOMEPAGE_URL "https://github.com/dnv-opensource/vista-sdk"
	LANGUAGES CXX
)

#----------------------------------------------
# Version configuration
#----------------------------------------------

set(VISTA_SDK_CPP_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE INTERNAL "vista-sdk-cpp major version")
set(VISTA_SDK_CPP_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE INTERNAL "vista-sdk-cpp minor version")
set(VISTA_SDK_CPP_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE INTERNAL "vista-sdk-cpp patch version")
set(VISTA_SDK_CPP_VERSION       ${PROJECT_VERSION}       CACHE INTERNAL "vista-sdk-cpp version")

#----------------------------------------------
# Testing setup
#----------------------------------------------

enable_testing()

#----------------------------------------------
# Build options
#----------------------------------------------

# --- Library build types ---
option(VISTA_SDK_CPP_BUILD_STATIC        "Build static library"               OFF)
option(VISTA_SDK_CPP_BUILD_SHARED        "Build shared library"               OFF)

# --- Build components ---
option(VISTA_SDK_CPP_BUILD_TOOLS         "Build tools"                        OFF)
option(VISTA_SDK_CPP_BUILD_TESTS         "Build tests"                        OFF)
option(VISTA_SDK_CPP_BUILD_SAMPLES       "Build samples"                      OFF)
option(VISTA_SDK_CPP_BUILD_BENCHMARKS    "Build benchmarks"                   OFF)
option(VISTA_SDK_CPP_BUILD_DOCUMENTATION "Build Doxygen documentation"        OFF)

# --- Installation ---
option(VISTA_SDK_CPP_INSTALL_PROJECT     "Install project"                    OFF)

# --- Packaging ---
option(VISTA_SDK_CPP_PACKAGE_SOURCE      "Enable source package generation"   OFF)
option(VISTA_SDK_CPP_PACKAGE_ARCHIVE     "Enable TGZ/ZIP package generation"  OFF)
option(VISTA_SDK_CPP_PACKAGE_DEB         "Enable DEB package generation"      OFF)
option(VISTA_SDK_CPP_PACKAGE_RPM         "Enable RPM package generation"      OFF)
option(VISTA_SDK_CPP_PACKAGE_WIX         "Enable WiX Windows installer (MSI)" OFF)

#----------------------------------------------
# CMake modules configuration
#----------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#----------------------------------------------
# Main configuration
#----------------------------------------------

include(dnvVistaSdkCppBuildConfig)
include(dnvVistaSdkCppDependencies)
include(dnvVistaSdkCppSources)
include(dnvVistaSdkCppTargets)

#----------------------------------------------
# Installation and packaging
#----------------------------------------------

include(dnvVistaSdkCppInstall)
include(dnvVistaSdkCppPackaging)

#----------------------------------------------
# Code generation
#----------------------------------------------

include(dnvVistaSdkCppResourceGenerator)
include(dnvVistaSdkCppEmbeddedResource)
include(dnvVistaSdkCppEmbeddedTestData)
include(dnvVistaSdkCppVisVersionsGenerator)

#----------------------------------------------
# Targets
#----------------------------------------------

add_subdirectory(tools)
add_subdirectory(test)
add_subdirectory(samples)
add_subdirectory(benchmark)
add_subdirectory(doc)
