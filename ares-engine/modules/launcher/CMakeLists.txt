project(ares_launcher)

# CMAKE Policies
cmake_policy(SET CMP0156 NEW)
cmake_policy(SET CMP0179 NEW)

# Create the executable
# add_executable(ares_launcher)
add_library(ares_launcher STATIC)

# Find all .cpp and .h files
file(GLOB_RECURSE ENTRY_POINT_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE ENTRY_POINT_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# Include source files and headers, maintaining folder structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${ENTRY_POINT_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${ENTRY_POINT_HEADER_FILES})

# Add sources
target_sources(ares_launcher PRIVATE
	${ENTRY_POINT_SOURCE_FILES}
	${ENTRY_POINT_HEADER_FILES}
)

# Include directory
target_include_directories(ares_launcher
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(ares_launcher
	PRIVATE ${CMAKE_SOURCE_DIR}/modules/core/include
)

# Properties
set_target_properties(ares_launcher PROPERTIES FOLDER "modules")
set_target_properties(ares_launcher PROPERTIES OUTPUT_NAME launcher)