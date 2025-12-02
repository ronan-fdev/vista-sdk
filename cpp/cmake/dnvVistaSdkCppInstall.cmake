#==============================================================================
# vista-sdk-cpp - Library installation
#==============================================================================

#----------------------------------------------
# Installation condition check
#----------------------------------------------

if(NOT VISTA_SDK_CPP_INSTALL_PROJECT)
	return()
endif()

#----------------------------------------------
# Install headers
#----------------------------------------------

install(
	DIRECTORY "${VISTA_SDK_CPP_INCLUDE_DIR}/"
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	COMPONENT Development
	FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inl"
)

#----------------------------------------------
# Install library targets
#----------------------------------------------

set(install_targets)

if(VISTA_SDK_CPP_BUILD_SHARED AND TARGET ${PROJECT_NAME})
	list(APPEND install_targets ${PROJECT_NAME})
endif()

if(VISTA_SDK_CPP_BUILD_STATIC AND TARGET ${PROJECT_NAME}-static)
	list(APPEND install_targets ${PROJECT_NAME}-static)
endif()

if(install_targets)
	install(
		TARGETS ${install_targets}
		EXPORT vista-sdk-cpp-targets
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
			COMPONENT Development
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
			COMPONENT Runtime
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
			COMPONENT Runtime
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)

	# Install PDB files for debug builds on Windows
	if(MSVC AND VISTA_SDK_CPP_BUILD_SHARED AND TARGET ${PROJECT_NAME})
		install(
			FILES $<TARGET_PDB_FILE:${PROJECT_NAME}>
			DESTINATION ${CMAKE_INSTALL_BINDIR}
			CONFIGURATIONS Debug RelWithDebInfo
			COMPONENT Development
			OPTIONAL
		)
	endif()
endif()

#----------------------------------------------
# Install CMake config files
#----------------------------------------------

if(install_targets)
	install(
		EXPORT vista-sdk-cpp-targets
		FILE vista-sdk-cpp-targets.cmake
		NAMESPACE vista-sdk-cpp::
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vista-sdk-cpp
		COMPONENT Development
	)

	# Install separate target files for each configuration (multi-config generators)
	if(CMAKE_CONFIGURATION_TYPES)
		foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
			install(
				EXPORT vista-sdk-cpp-targets
				FILE vista-sdk-cpp-targets-${CONFIG}.cmake
				NAMESPACE vista-sdk-cpp::
				DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vista-sdk-cpp
				CONFIGURATIONS ${CONFIG}
				COMPONENT Development
			)
		endforeach()
	endif()
endif()

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/vista-sdk-cpp-config-version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/dnv-vista-sdk-cpp-config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/vista-sdk-cpp-config.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vista-sdk-cpp
	PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)

install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/vista-sdk-cpp-config.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/vista-sdk-cpp-config-version.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vista-sdk-cpp
	COMPONENT Development
)

#----------------------------------------------
# Install license files
#----------------------------------------------

install(
	FILES "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE"
	DESTINATION "${CMAKE_INSTALL_DOCDIR}/licenses"
	RENAME "LICENSE-${PROJECT_NAME}.txt"
)

file(GLOB license_files "${CMAKE_CURRENT_SOURCE_DIR}/licenses/LICENSE-*")
foreach(license_file ${license_files})
	get_filename_component(license_name ${license_file} NAME)
	install(
		FILES ${license_file}
		DESTINATION "${CMAKE_INSTALL_DOCDIR}/licenses"
		RENAME "${license_name}.txt"
	)
endforeach()

#----------------------------------------------
# Install documentation
#----------------------------------------------

if(VISTA_SDK_CPP_BUILD_DOCUMENTATION)
	install(
		DIRECTORY "${CMAKE_BINARY_DIR}/doc/html"
		DESTINATION ${CMAKE_INSTALL_DOCDIR}
		OPTIONAL
		COMPONENT Documentation
	)

	if(WIN32)
		# Install Windows .cmd batch file
		install(
			FILES "${CMAKE_BINARY_DIR}/doc/index.html.cmd"
			DESTINATION ${CMAKE_INSTALL_DOCDIR}
			OPTIONAL
			COMPONENT Documentation
		)
	else()
		# Install Unix symlink
		install(
			FILES "${CMAKE_BINARY_DIR}/doc/index.html"
			DESTINATION ${CMAKE_INSTALL_DOCDIR}
			OPTIONAL
			COMPONENT Documentation
		)
	endif()
endif()

message(STATUS "Installation configured for targets: ${install_targets}")
