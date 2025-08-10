project(ares_launcher)

# CMAKE Policies
cmake_policy(SET CMP0156 NEW)
cmake_policy(SET CMP0179 NEW)

# Create the executable
# add_executable(ares_launcher)
add_library(ares_launcher STATIC)

# Find all .cpp and .h files
file(GLOB_RECURSE LAUNCHER_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE LAUNCHER_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# Remove platform-specific files
list(FILTER LAUNCHER_SOURCE_FILES EXCLUDE REGEX ".*/src/launcher/platform/.*")
list(FILTER LAUNCHER_HEADER_FILES EXCLUDE REGEX ".*/include/launcher/platform/.*")

# Platform specific globs
if(WIN32)
	file(GLOB_RECURSE LAUNCHER_PLATFORM_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/launcher/platform/windows/*.cpp)
	file(GLOB_RECURSE LAUNCHER_PLATFORM_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/launcher/platform/windows/*.h)
elseif(UNIX)
	file(GLOB_RECURSE LAUNCHER_PLATFORM_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/launcher/platform/unix/*.cpp)
	file(GLOB_RECURSE LAUNCHER_PLATFORM_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/launcher/platform/unix/*.h)
endif()

# Combine all files
set (LAUNCHER_ALL_SOURCE_FILES ${LAUNCHER_SOURCE_FILES} ${LAUNCHER_PLATFORM_SOURCE_FILES})
set (LAUNCHER_ALL_HEADER_FILES ${LAUNCHER_HEADER_FILES} ${LAUNCHER_PLATFORM_HEADER_FILES})
set (LAUNCHER_PCH_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/ares_launcher_pch.h)

# Include source files and headers, maintaining folder structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${LAUNCHER_ALL_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files" FILES ${LAUNCHER_PCH_FILE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${LAUNCHER_ALL_HEADER_FILES})
source_group("Generated Files" REGULAR_EXPRESSION ".*cmake_pch\\.(cxx||hxx)$")

# Add sources
target_sources(ares_launcher PRIVATE
	${LAUNCHER_ALL_SOURCE_FILES}
	${LAUNCHER_ALL_HEADER_FILES}
	${LAUNCHER_PCH_FILE}
)

# Include directory
target_include_directories(ares_launcher
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_SOURCE_DIR}/modules/core/include	
)

# Precompiled header
target_precompile_headers(ares_launcher
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ares_launcher_pch.h
)

# Properties
set_target_properties(ares_launcher PROPERTIES FOLDER "modules")
set_target_properties(ares_launcher PROPERTIES OUTPUT_NAME launcher)