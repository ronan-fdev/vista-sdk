#==============================================================================
# vista-sdk-cpp - CMake Sources
#==============================================================================

#----------------------------------------------
# Source files
#----------------------------------------------

set(private_sources)

list(APPEND private_sources
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/parsing/LocationParsingErrorBuilder.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/Codebook.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/Codebooks.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/GmodNode.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/ImoNumber.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/LocationBuilder.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/Locations.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/MetadataTag.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/ParsingErrors.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/StringBuilder.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/VIS.cpp
)
