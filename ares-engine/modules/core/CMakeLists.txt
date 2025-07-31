project(ares_core)

# CMAKE Policies
cmake_policy(SET CMP0156 NEW)
cmake_policy(SET CMP0179 NEW)

# Create the library
if(ENABLE_STATIC_BUILD)
	add_library(ares_core STATIC)
else()
	add_library(ares_core SHARED)
	set_target_properties(ares_core PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug/modules
	)
	target_compile_definitions(ares_core PRIVATE ARES_CORE_EXPORTS)
endif()

# Find all .cpp and .h files
file(GLOB_RECURSE CORE_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
file(GLOB_RECURSE CORE_PRIVATE_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)
file(GLOB_RECURSE CORE_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

# Include source files and headers, maintaining folder structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Project Files" FILES ${CORE_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Project Files" FILES ${CORE_PRIVATE_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Project Files" FILES ${CORE_HEADER_FILES})

# Add sources
target_sources(ares_core PRIVATE
	${CORE_SOURCE_FILES}
	${CORE_PRIVATE_HEADER_FILES}
	${CORE_HEADER_FILES}
)

# Include directory
target_include_directories(ares_core
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(ares_core
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)



# Properties
set_target_properties(ares_core PROPERTIES FOLDER "modules")
set_target_properties(ares_core PROPERTIES
	OUTPUT_NAME core
)