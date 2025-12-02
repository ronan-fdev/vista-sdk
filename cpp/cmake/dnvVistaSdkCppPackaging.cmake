#==============================================================================
# vista-sdk-cpp - Library packaging configuration (CPack)
#==============================================================================

#----------------------------------------------
# Packaging condition check
#----------------------------------------------

if(NOT VISTA_SDK_CPP_PACKAGE_SOURCE
		AND NOT VISTA_SDK_CPP_PACKAGE_ARCHIVE
		AND NOT VISTA_SDK_CPP_PACKAGE_DEB
		AND NOT VISTA_SDK_CPP_PACKAGE_RPM
		AND NOT VISTA_SDK_CPP_PACKAGE_WIX)
	return()
endif()

#----------------------------------------------
# CPack configuration
#----------------------------------------------

if(WIN32 AND VISTA_SDK_CPP_PACKAGE_WIX)
	set(CPACK_RESOURCE_FILE_LICENSE_TXT "${CMAKE_BINARY_DIR}/LICENSE.txt")
	configure_file(
		"${VISTA_SDK_LICENSE_FILE}"
		"${CPACK_RESOURCE_FILE_LICENSE_TXT}"
		COPYONLY
	)
	set(CPACK_RESOURCE_FILE_LICENSE "${CPACK_RESOURCE_FILE_LICENSE_TXT}")
else()
	set(CPACK_RESOURCE_FILE_LICENSE "${VISTA_SDK_LICENSE_FILE}")
endif()

# --- Detect processor count for parallel packaging ---
include(ProcessorCount)
ProcessorCount(processor_count)
if(NOT processor_count EQUAL 0)
	set(CPACK_THREADS ${processor_count})
else()
	set(CPACK_THREADS 1)
endif()

# --- Common settings ---
set(CPACK_PACKAGE_NAME                  ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR                "DNV")
set(CPACK_PACKAGE_DIRECTORY             "${CMAKE_BINARY_DIR}/packages")
set(CPACK_PACKAGE_VERSION_MAJOR         ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR         ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH         ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION               ${PROJECT_VERSION})
#set(CPACK_PACKAGE_DESCRIPTION           "")
#set(CPACK_PACKAGE_DESCRIPTION_FILE      "")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY   "C++ SDK for DNV Vessel Information Structure (VIS) and ISO19848/19847 standards")
set(CPACK_PACKAGE_HOMEPAGE_URL          ${CMAKE_PROJECT_HOMEPAGE_URL})
#set(CPACK_PACKAGE_FILE_NAME             "")
set(CPACK_PACKAGE_INSTALL_DIRECTORY     "dnv")
#set(CPACK_PACKAGE_ICON                  "")
set(CPACK_PACKAGE_CHECKSUM              "SHA256")
#set(CPACK_PROJECT_CONFIG_FILE           "")
#set(CPACK_RESOURCE_FILE_LICENSE         "") # Set above for WiX compatibility
#set(CPACK_RESOURCE_FILE_README          "")
#set(CPACK_RESOURCE_FILE_WELCOME         "")
set(CPACK_MONOLITHIC_INSTALL            TRUE)
#set(CPACK_GENERATOR                     "")
#set(CPACK_OUTPUT_CONFIG_FILE            "")
#set(CPACK_PACKAGE_EXECUTABLES           "")
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
	set(CPACK_STRIP_FILES               TRUE)
	message(STATUS "${CMAKE_BUILD_TYPE} build: Debug symbols will be stripped from packages")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CPACK_STRIP_FILES               FALSE)
	message(STATUS "${CMAKE_BUILD_TYPE} build: Debug symbols preserved in packages")
else()
	# Multi-config generator - strip by default
	set(CPACK_STRIP_FILES               TRUE)
	message(STATUS "Multi-config build: Debug symbols will be stripped from packages")
endif()
set(CPACK_VERBATIM_VARIABLES            TRUE)

# --- Source package settings ---
set(CPACK_SOURCE_PACKAGE_FILE_NAME      "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-Source")
#set(CPACK_SOURCE_STRIP_FILES            "")
if(VISTA_SDK_CPP_PACKAGE_SOURCE)
	set(CPACK_SOURCE_GENERATOR          "TGZ;ZIP")
else()
	set(CPACK_SOURCE_GENERATOR          "")
endif()
#set(CPACK_SOURCE_OUTPUT_CONFIG_FILE     "")
set(CPACK_SOURCE_IGNORE_FILES           ".git/;.github;.gitignore;build/;.deps/;Testing/;.vs/;.vscode/;.*~$")

# --- Advanced settings ---
#set(CPACK_INSTALL_COMMANDS              "")
#set(CPACK_INSTALL_SCRIPTS               "")
#set(CPACK_PRE_BUILD_SCRIPTS             "")
#set(CPACK_POST_BUILD_SCRIPTS            "")
#set(CPACK_PACKAGE_FILES                 "")
#set(CPACK_INSTALLED_DIRECTORIES         "")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY  "vista-sdk-cpp")
#set(CPACK_CREATE_DESKTOP_LINKS          "")

#----------------------------------------------
# Platform-specific generators
#----------------------------------------------

# --- Find packaging tools ---
if(UNIX AND NOT APPLE)
	find_program(DPKG_CMD dpkg)
	find_program(RPM_CMD rpm)
endif()

if(WIN32)
	if(NOT WIX_CANDLE OR NOT WIX_LIGHT)
		find_program(WIX_CANDLE NAMES candle.exe candle HINTS "$ENV{WIX}/bin")
		find_program(WIX_LIGHT NAMES light.exe light HINTS "$ENV{WIX}/bin")
	endif()
endif()

# --- Generators ---
if(NOT DEFINED CPACK_GENERATOR)
	set(CPACK_GENERATOR "")

	# --- Archive packages ---
	if(VISTA_SDK_CPP_PACKAGE_ARCHIVE)
		if(UNIX AND NOT APPLE)
			set(CPACK_GENERATOR "TGZ")
		elseif(WIN32)
			set(CPACK_GENERATOR "ZIP")
		endif()
	endif()

	# --- DEB packages ---
	if(VISTA_SDK_CPP_PACKAGE_DEB AND UNIX AND NOT APPLE)
		if(DPKG_CMD)
			set(CPACK_GENERATOR "${CPACK_GENERATOR};DEB")
		else()
			message(STATUS "DEB packaging disabled - install dpkg-dev for .deb support")
		endif()
	endif()

	# --- RPM packages ---
	if(VISTA_SDK_CPP_PACKAGE_RPM AND UNIX AND NOT APPLE)
		if(RPM_CMD)
			set(CPACK_GENERATOR "${CPACK_GENERATOR};RPM")
		else()
			message(STATUS "RPM packaging disabled - install rpm for .rpm support")
		endif()
	endif()

	# --- WiX packages ---
	if(VISTA_SDK_CPP_PACKAGE_WIX AND WIN32)
		if(WIX_CANDLE AND WIX_LIGHT)
			set(CPACK_GENERATOR "${CPACK_GENERATOR};WIX")
			message(STATUS "WiX found: ${WIX_CANDLE} - Windows MSI installer generation enabled")
		else()
			message(STATUS "WiX not found - install WiX Toolset and ensure WIX environment variable is set")
		endif()
	endif()

	# Clean up leading semicolon
	string(REGEX REPLACE "^;" "" CPACK_GENERATOR "${CPACK_GENERATOR}")
endif()

message(STATUS "CPack configured:")
message(STATUS "  Generators: ${CPACK_GENERATOR}")
message(STATUS "  Output dir: ${CPACK_PACKAGE_DIRECTORY}")

#----------------------------------------------
# Architecture detection
#----------------------------------------------

if("DEB" IN_LIST CPACK_GENERATOR AND UNIX AND NOT APPLE)
	if(NOT DEFINED CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
		if(DPKG_CMD)
			execute_process(COMMAND ${DPKG_CMD} --print-architecture
				OUTPUT_VARIABLE detected_deb_arch
				OUTPUT_STRIP_TRAILING_WHITESPACE
				ERROR_QUIET)
			set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${detected_deb_arch}")
		endif()
	endif()

	message(STATUS "Debian package architecture: ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
endif()

if("RPM" IN_LIST CPACK_GENERATOR AND UNIX AND NOT APPLE)
	if(NOT DEFINED CPACK_RPM_PACKAGE_ARCHITECTURE)
			execute_process(COMMAND uname -m
				OUTPUT_VARIABLE detected_rpm_arch
				OUTPUT_STRIP_TRAILING_WHITESPACE
				ERROR_QUIET)
			set(CPACK_RPM_PACKAGE_ARCHITECTURE "${detected_rpm_arch}")
	endif()

	message(STATUS "RPM package architecture: ${CPACK_RPM_PACKAGE_ARCHITECTURE}")
endif()

#----------------------------------------------
# Package-specific settings
#----------------------------------------------

# --- DEB package settings ---
if("DEB" IN_LIST CPACK_GENERATOR AND UNIX AND NOT APPLE)
	set(CPACK_DEBIAN_PACKAGE_NAME         ${CPACK_PACKAGE_NAME})
	set(CPACK_DEBIAN_FILE_NAME            "DEB-DEFAULT")
	set(CPACK_DEBIAN_PACKAGE_VERSION      ${PROJECT_VERSION})
	set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER   ${CPACK_PACKAGE_VENDOR})
	set(CPACK_DEBIAN_PACKAGE_DESCRIPTION  ${PROJECT_DESCRIPTION})
	set(CPACK_DEBIAN_PACKAGE_SECTION      "libs")
	set(CPACK_DEBIAN_COMPRESSION_TYPE     "xz")
	set(CPACK_DEBIAN_PACKAGE_PRIORITY     "optional")
	set(CPACK_DEBIAN_PACKAGE_HOMEPAGE     ${CPACK_PACKAGE_HOMEPAGE_URL})
	set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS    ON)
endif()

# --- RPM package settings ---
if("RPM" IN_LIST CPACK_GENERATOR AND UNIX AND NOT APPLE)
	set(CPACK_RPM_PACKAGE_SUMMARY      ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
	set(CPACK_RPM_PACKAGE_NAME         ${CPACK_PACKAGE_NAME})
	set(CPACK_RPM_FILE_NAME            "RPM-DEFAULT")
	set(CPACK_RPM_PACKAGE_VERSION      ${PROJECT_VERSION})
	set(CPACK_RPM_PACKAGE_ARCHITECTURE ${CPACK_RPM_PACKAGE_ARCHITECTURE})
	set(CPACK_RPM_PACKAGE_RELEASE      "1")
	set(CPACK_RPM_PACKAGE_LICENSE      ${VISTA_SDK_LICENSE})
	set(CPACK_RPM_PACKAGE_GROUP        "Development/Libraries")
	set(CPACK_RPM_COMPRESSION_TYPE     "xz")
	set(CPACK_RPM_PACKAGE_VENDOR       ${CPACK_PACKAGE_VENDOR})
	set(CPACK_RPM_PACKAGE_DESCRIPTION  ${PROJECT_DESCRIPTION})
	set(CPACK_RPM_PACKAGE_URL          ${CPACK_PACKAGE_HOMEPAGE_URL})
	set(CPACK_RPM_PACKAGE_AUTOREQ      ON)
endif()

#----------------------------------------------
# Include CPack
#----------------------------------------------

include(CPack)
