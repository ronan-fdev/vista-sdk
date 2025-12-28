#==============================================================================
# vista-sdk-cpp - EmbeddedSchemas library CMake configuration
#==============================================================================

#----------------------------------------------
# Target definition
#----------------------------------------------

add_library(EmbeddedSchemas OBJECT
	${VISTA_SDK_CPP_SOURCE_DIR}/EmbeddedSchemas/EmbeddedSchemas.cpp
)

#----------------------------------------------
# Include directories
#----------------------------------------------

target_include_directories(EmbeddedSchemas
	PUBLIC
		${VISTA_SDK_CPP_SOURCE_DIR}/EmbeddedSchemas
)

#----------------------------------------------
# Link libraries
#----------------------------------------------

target_link_libraries(EmbeddedSchemas PRIVATE
	nfx-serialization::static
	nfx-stringutils::nfx-stringutils
)

#----------------------------------------------
# Target properties
#----------------------------------------------

set_target_properties(EmbeddedSchemas PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	POSITION_INDEPENDENT_CODE ON
	DEBUG_POSTFIX "-d"
)

#----------------------------------------------
# Embed schemas at build time
#----------------------------------------------

set(EMBEDDED_SCHEMAS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated/schemas_registry")

# Embedding function: converts schema files to C++ byte arrays
function(embed_all_schemas_to_target TARGET_NAME SCHEMAS_DIR OUTPUT_DIR)
	file(GLOB_RECURSE SCHEMA_FILES
		"${SCHEMAS_DIR}/*.json"
		"${SCHEMAS_DIR}/*.xsd"
	)

	if(NOT SCHEMA_FILES)
		message(WARNING "No schema files found in ${SCHEMAS_DIR}")
		return()
	endif()

	list(SORT SCHEMA_FILES COMPARE NATURAL)

	set(ALL_FILES "")
	set(ALL_SCHEMA_IDS "")

	foreach(SCHEMA_FILE ${SCHEMA_FILES})
		# Get relative path from SCHEMAS_DIR for better naming
		file(RELATIVE_PATH REL_PATH "${SCHEMAS_DIR}" "${SCHEMA_FILE}")
		string(REPLACE "/" "_" SCHEMA_NAME "${REL_PATH}")
		string(MAKE_C_IDENTIFIER "${SCHEMA_NAME}" SCHEMA_ID)
		set(OUTPUT_CPP "${OUTPUT_DIR}/schema_${SCHEMA_ID}.cpp")

		add_custom_command(
			OUTPUT "${OUTPUT_CPP}"
			COMMAND $<TARGET_FILE:ResourceGenerator> "${SCHEMA_FILE}" "${OUTPUT_CPP}" "${SCHEMA_ID}" "${REL_PATH}"
			DEPENDS "${SCHEMA_FILE}" ResourceGenerator
			COMMENT "Embedding schema: ${REL_PATH}"
			VERBATIM
		)

		list(APPEND ALL_FILES "${OUTPUT_CPP}")
		list(APPEND ALL_SCHEMA_IDS "${SCHEMA_ID}")
	endforeach()

	# Generate schema registry implementation
	set(REGISTRY_CPP "${OUTPUT_DIR}/schemas_registry.cpp")
	set(REGISTRY_INCLUDES "")
	set(REGISTRY_ENTRIES "")

	foreach(SCHEMA_ID ${ALL_SCHEMA_IDS})
		string(APPEND REGISTRY_INCLUDES "    extern const uint8_t ${SCHEMA_ID}_data[];\n")
		string(APPEND REGISTRY_INCLUDES "    extern const size_t ${SCHEMA_ID}_size;\n")
		string(APPEND REGISTRY_INCLUDES "    extern const char ${SCHEMA_ID}_name[];\n")
		string(APPEND REGISTRY_ENTRIES "        { registry::${SCHEMA_ID}_name, registry::${SCHEMA_ID}_data, registry::${SCHEMA_ID}_size },\n")
	endforeach()

	file(WRITE "${REGISTRY_CPP}"
"#include <string_view>
#include <vector>
#include <cstdint>
#include <cstddef>

namespace dnv::vista::sdk::schemas
{
	struct SchemaResource
	{
		std::string_view name;
		const uint8_t* data;
		size_t size;
	};

	// Forward declare registry namespace symbols
}

namespace dnv::vista::sdk::registry
{
${REGISTRY_INCLUDES}
}

namespace dnv::vista::sdk::schemas
{
	const std::vector<SchemaResource>& all()
	{
		static const std::vector<SchemaResource> resources = {
${REGISTRY_ENTRIES}
		};
		return resources;
	}
}
")

	# Add all files to target
	list(APPEND ALL_FILES "${REGISTRY_CPP}")
	set_source_files_properties(${ALL_FILES} PROPERTIES GENERATED TRUE)
	target_sources(${TARGET_NAME} PRIVATE ${ALL_FILES})

	list(LENGTH ALL_SCHEMA_IDS NUM_SCHEMAS)
	message(STATUS "Configured ${NUM_SCHEMAS} embedded schema files for ${TARGET_NAME}")
endfunction()

# Generate embedded C++ files for all schema files
embed_all_schemas_to_target(
	EmbeddedSchemas
	"${PROJECT_SOURCE_DIR}/../schemas"
	"${EMBEDDED_SCHEMAS_OUTPUT_DIR}"
)

# Add generated files directory to include path
target_include_directories(EmbeddedSchemas
	PRIVATE
		"${EMBEDDED_SCHEMAS_OUTPUT_DIR}"
)
