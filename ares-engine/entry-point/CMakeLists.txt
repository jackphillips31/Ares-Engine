project(ares-entry-point)

# CMAKE Policies
cmake_policy(SET CMP0156 NEW)
cmake_policy(SET CMP0179 NEW)

# Create the executable
add_executable(ares-entry-point)

# Find all .cpp and .h files
file(GLOB_RECURSE ENTRY_POINT_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
file(GLOB_RECURSE ENTRY_POINT_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

# Include source files and headers, maintaining folder structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Project Files" FILES ${ENTRY_POINT_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Project Files" FILES ${ENTRY_POINT_HEADER_FILES})

# Add sources
target_sources(ares-entry-point PRIVATE
	${ENTRY_POINT_SOURCE_FILES}
	${ENTRY_POINT_HEADER_FILES}
)

# Include directory
target_include_directories(ares-entry-point
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Name the executable
set_target_properties(ares-entry-point PROPERTIES
	OUTPUT_NAME "Ares-Application"
)

# Set subsystem depending on user option
if(WIN32)
	if(ENABLE_CONSOLE_APP)
		set_target_properties(ares-entry-point PROPERTIES WIN32_EXECUTABLE OFF)
	else()
		set_target_properties(ares-entry-point PROPERTIES WIN32_EXECUTABLE ON)
	endif()
endif()