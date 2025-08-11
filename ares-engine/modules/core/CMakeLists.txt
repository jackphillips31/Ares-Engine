project(ares_core)

# CMAKE Policies
cmake_policy(SET CMP0156 NEW)
cmake_policy(SET CMP0179 NEW)

# Create the library
if(ARES_ENABLE_STATIC_BUILD)
	add_library(ares_core STATIC)
else()
	add_library(ares_core SHARED)
	set_target_properties(ares_core PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug/modules
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release/modules
		RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin/release-debug/modules
		RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}/bin/release-min/modules
	)
	target_compile_definitions(ares_core PRIVATE ARES_CORE_EXPORTS)
endif()

# Find all .cpp and .h files
file(GLOB_RECURSE CORE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE CORE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# Remove platform-specific files
list(FILTER CORE_SOURCE_FILES EXCLUDE REGEX ".*/src/core/internal/platform/.*")
list(FILTER CORE_HEADER_FILES EXCLUDE REGEX ".*/include/core/internal/platform/.*")

# Platform specific globs
if(WIN32)
	file(GLOB_RECURSE CORE_PLATFORM_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/core/internal/platform/windows/*.cpp)
	file(GLOB_RECURSE CORE_PLATFORM_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/core/internal/platform/windows/*.h)
elseif(UNIX)
	file(GLOB_RECURSE CORE_PLATFORM_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/core/internal/platform/unix/*.cpp)
	file(GLOB_RECURSE CORE_PLATFORM_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/core/internal/platform/unix/*.h)
endif()

# Combine all files
set(CORE_ALL_SOURCE_FILES ${CORE_SOURCE_FILES} ${CORE_PLATFORM_SOURCE_FILES})
set(CORE_ALL_HEADER_FILES ${CORE_HEADER_FILES} ${CORE_PLATFORM_HEADER_FILES})
set(CORE_PCH_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/ares_core_pch.h)

# Include source files and headers, maintaining folder structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${CORE_ALL_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files" FILES ${CORE_PCH_FILE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${CORE_ALL_HEADER_FILES})
source_group("Generated Files" REGULAR_EXPRESSION ".*cmake_pch\\.(cxx|hxx)$")

# Add sources
target_sources(ares_core PRIVATE
	${CORE_ALL_SOURCE_FILES}
	${CORE_ALL_HEADER_FILES}
	${CORE_PCH_FILE}
)

# Include directory
target_include_directories(ares_core
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Precompiled header
target_precompile_headers(ares_core
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ares_core_pch.h
)

# 3rd Party Libraries
target_link_libraries(ares_core
	PUBLIC
		EASTL
)

# Properties
set_target_properties(ares_core PROPERTIES FOLDER "modules")
set_target_properties(ares_core PROPERTIES OUTPUT_NAME core)