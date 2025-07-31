cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Configuring...")

# Define options
option(ENABLE_MIN_SIZE_RELEASE "Enable a min-size release build." ON)
option(ENABLE_EXTRA_OPTIMIZATIONS "Enable extra optimizations for the release build." ON)
option(ENABLE_IDE_FOLDERS "Enable IDE folder grouping (e.g., Visual Studio filters)." ON)
option(ENABLE_STATIC_BUILD "Enable static build for release." OFF)
set(ARES_PROJECT_DIR "${CMAKE_SOURCE_DIR}/sample_project" CACHE STRING "Path to the project using Ares.")
set(ARES_PROJECT_NAME "Ares-Sample-Project" CACHE STRING "Name of the Ares project.")
set(ARES_PROJECT_NAME_UNDERSCORE "")

# Definitions
if(ENABLE_STATIC_BUILD)
	add_compile_definitions(ARES_STATIC_BUILD)
endif()

if (WIN32)
	option(ENABLE_CONSOLE_APP "Build entry-point as a console application rather than windowed (Windows only)." ON)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin/debug-release)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin/debug-release/lib)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug/modules)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release/modules)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin/debug-release/modules)

if(ENABLE_MIN_SIZE_RELEASE)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}/bin/min-release)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}/bin/min-release/lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}/bin/min-release/modules)
endif()

# Extra optimization tweaks for release builds (acts as distribution build)
if(ENABLE_EXTRA_OPTIMIZATIONS)
	message(STATUS "Configuring extra release optimizations...")
	if(MSVC)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /02 /GL")
		set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
	else()
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -03 -march=native -ffast-math")
		set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
	endif()
endif()

# Fetch dependencies
# include(FetchContent)

# Optional: allow folder grouping in IDEs like Visual Studio
if(ENABLE_IDE_FOLDERS)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Add subdirectories
add_subdirectory(modules/entry_point)
add_subdirectory(modules/core)

# Add project
if(NOT EXISTS "${ARES_PROJECT_DIR}")
	message(FATAL_ERROR "ARES_PROJECT_DIR '${ARES_PROJECT_DIR}' does not exist!")
endif()
if(NOT EXISTS "${ARES_PROJECT_DIR}/modules.ini")
	message(FATAL_ERROR "modules.ini '${ARES_PROJECT_DIR}/modules.ini' does not exist!")
endif()

# Parse project modules config
message(STATUS "Parsing '${ARES_PROJECT_DIR}/modules.ini'")
file(READ "${ARES_PROJECT_DIR}/modules.ini" ARES_PROJECT_CONFIG)

# Remove newlines to simplify regex
string(REPLACE "\r\n" "\n" ARES_PROJECT_CONFIG "${ARES_PROJECT_CONFIG}")
string(REPLACE "\n" ";" ARES_PROJECT_CONFIG "${ARES_PROJECT_CONFIG}")

# Variables to store results
set(ARES_PROJECT_CONFIG_NAME "")
set(ARES_PROJECT_CONFIG_LIST "")

foreach(line IN LISTS ARES_PROJECT_CONFIG)
	# Skip empty lines and comments
	if(line MATCHES "^[ \t]*$" OR line MATCHES "^[ \t]*#")
		continue()
	endif()

	# Match name=...
	if(line MATCHES "name=(.*)")
		string(REGEX REPLACE "name=(.*)" "\\1" ARES_PROJECT_CONFIG_NAME "${line}")
		string(STRIP "${ARES_PROJECT_CONFIG_NAME}" ARES_PROJECT_CONFIG_NAME)
		message(STATUS "Found project name: ${ARES_PROJECT_CONFIG_NAME}")
		# Convert spaces to underscores
		string(REPLACE " " "_" ARES_PROJECT_NAME_UNDERSCORE "${ARES_PROJECT_CONFIG_NAME}")
		message(STATUS "'${ARES_PROJECT_NAME_UNDERSCORE}' will be used rather than '${ARES_PROJECT_NAME}'")
		set(ARES_PROJECT_NAME ${ARES_PROJECT_CONFIG_NAME})
	endif()

	# Match modules=...
	if(line MATCHES "modules=(.*)")
		string(REGEX REPLACE "modules=(.*)" "\\1" MODULES_RAW "${line}")
		string(STRIP "${MODULES_RAW}" MODULES_RAW)
		# Convert comma seperated list to CMake list
		string(REPLACE "," ";" ARES_PROJECT_CONFIG_LIST "${MODULES_RAW}")
		message(STATUS "Found project module(s): ${ARES_PROJECT_CONFIG_LIST}")
	endif()
endforeach()

project(${ARES_PROJECT_NAME_UNDERSCORE})

# CMAKE Policies
cmake_policy(SET CMP0156 NEW)
cmake_policy(SET CMP0179 NEW)

# Create the executable
add_executable(${ARES_PROJECT_NAME_UNDERSCORE})

# Find all .cpp and .h files
file(GLOB_RECURSE SAMPLE_PROJECT_SOURCE_FILES
	${ARES_PROJECT_DIR}/src/*.cpp
)
file(GLOB_RECURSE SAMPLE_PROJECT_HEADER_FILES
	${ARES_PROJECT_DIR}/src/*.h
)

# Include source files and headers, maintaining folder structure
source_group(TREE ${ARES_PROJECT_DIR}/src PREFIX "Project Files" FILES ${SAMPLE_PROJECT_SOURCE_FILES})
source_group(TREE ${ARES_PROJECT_DIR}/include PREFIX "Project Files" FILES ${SAMPLE_PROJECT_HEADER_FILES})

# Add sources
target_sources(${ARES_PROJECT_NAME_UNDERSCORE} PRIVATE
	${SAMPLE_PROJECT_SOURCE_FILES}
	${SAMPLE_PROJECT_HEADER_FILES}
)

# Include directory
target_include_directories(${ARES_PROJECT_NAME_UNDERSCORE}
	PRIVATE ${ARES_PROJECT_DIR}/src
)

# Link against entry-point
target_link_libraries(${ARES_PROJECT_NAME_UNDERSCORE} PRIVATE ares_entry_point)
foreach(lib IN LISTS ARES_PROJECT_CONFIG_LIST)
	target_link_libraries(${ARES_PROJECT_NAME_UNDERSCORE} PRIVATE ares_${lib})
endforeach()

# Name the executable
set_target_properties(${ARES_PROJECT_NAME_UNDERSCORE} PROPERTIES
	OUTPUT_NAME ${ARES_PROJECT_NAME}
)

# Set subsystem depending on user option
if(WIN32)
	if(ENABLE_CONSOLE_APP)
		set_target_properties(${ARES_PROJECT_CONFIG_NAME_UNDERSCORE} PROPERTIES WIN32_EXECUTABLE OFF)
	else()
		set_target_properties(${ARES_PROJECT_CONFIG_NAME_UNDERSCORE} PROPERTIES WIN32_EXECUTABLE ON)
	endif()
endif()

# Set Solution
project (Ares_Engine VERSION 0.0.1 LANGUAGES CXX)