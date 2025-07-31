project(ares_entry_point)

# CMAKE Policies
cmake_policy(SET CMP0156 NEW)
cmake_policy(SET CMP0179 NEW)

# Create the executable
# add_executable(ares_entry_point)
add_library(ares_entry_point STATIC)

# Find all .cpp and .h files
file(GLOB_RECURSE ENTRY_POINT_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
file(GLOB_RECURSE ENTRY_POINT_PRIVATE_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)
file(GLOB_RECURSE ENTRY_POINT_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

# Include source files and headers, maintaining folder structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Project Files" FILES ${ENTRY_POINT_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Project Files" FILES ${ENTRY_POINT_PRIVATE_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Project Files" FILES ${ENTRY_POINT_HEADER_FILES})

# Add sources
target_sources(ares_entry_point PRIVATE
	${ENTRY_POINT_SOURCE_FILES}
	${ENTRY_POINT_PRIVATE_HEADER_FILES}
	${ENTRY_POINT_HEADER_FILES}
)

# Include directory
target_include_directories(ares_entry_point
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(ares_entry_point
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Properties
set_target_properties(ares_entry_point PROPERTIES FOLDER "modules")
set_target_properties(ares_entry_point PROPERTIES
	OUTPUT_NAME entry_point
)