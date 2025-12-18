#==============================================================================
# vista-sdk-cpp - CMake Sources
#==============================================================================

#----------------------------------------------
# Source files
#----------------------------------------------

set(private_sources)

list(APPEND private_sources
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/Codebook.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/Codebooks.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/MetadataTag.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/ParsingErrors.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/StringBuilder.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/SDK/VIS.cpp
)
