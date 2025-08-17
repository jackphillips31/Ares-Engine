#/******************************************************/
#/*                 Basic Configuration                */
#/******************************************************/

cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Configuring...")

# Define options
option(ARES_ENABLE_EXTRA_OPTIMIZATIONS "Enable extra optimizations for the release build." ON)
option(ARES_ENABLE_IDE_FOLDERS "Enable IDE folder grouping (e.g., Visual Studio filters)." ON)
option(ARES_ENABLE_STATIC_BUILD "Enable static build for release." OFF)
set(ARES_PROJECT_DIR "${CMAKE_SOURCE_DIR}/sample_project" CACHE STRING "Path to the project using Ares.")
set(ARES_PROJECT_NAME "Ares-Sample-Project" CACHE STRING "Name of the Ares project.")
set(ARES_PROJECT_NAME_UNDERSCORE "")

# Definitions
if(ARES_ENABLE_STATIC_BUILD)
	add_compile_definitions(ARES_STATIC_BUILD)
endif()
add_compile_definitions(
	$<$<CONFIG:Debug>:ARES_BUILD_DEBUG=1>
	$<$<CONFIG:Release>:ARES_BUILD_RELEASE=1>
	$<$<CONFIG:RelWithDebInfo>:ARES_BUILD_DEBUG=1>
	$<$<CONFIG:MinSizeRel>:ARES_BUILD_RELEASE=1>
)

if (WIN32)
	option(ARES_ENABLE_CONSOLE_APP "Build entry-point as a console application rather than windowed (Windows only)." ON)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin/release-debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}/bin/release-min)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin/release-debug/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}/bin/release-min/lib)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug/modules)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release/modules)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin/release-debug/modules)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}/bin/release-min/modules)

# Extra optimization tweaks for release builds (acts as distribution build)
if(ARES_ENABLE_EXTRA_OPTIMIZATIONS)
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
include(FetchContent)

message(STATUS "Fetching dependencies...")
message(STATUS "Fetching EABase...")
FetchContent_Declare(
	EABase
	GIT_REPOSITORY https://github.com/electronicarts/EABase.git
	GIT_TAG 123363eb82e132c0181ac53e43226d8ee76dea12
	GIT_SUBMODULES ""
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/libraries/EABase
)
FetchContent_MakeAvailable(EABase)

if(TARGET EABase)
	message(STATUS "Configuring EABase...")

	target_compile_definitions(EABase INTERFACE
		$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:
			EA_ASSERT_ENABLED=1
			EA_DEBUG=1
		>
		$<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:
			EA_ASSERT_ENABLED=0
			EA_DEBUG=0
		>
	)
endif()

message(STATUS "Fetching EASTL...")
FetchContent_Declare(
	EASTL
	GIT_REPOSITORY https://github.com/electronicarts/EASTL.git
	GIT_TAG 7fadbf0da01e6f6e0e7038b1b34343a069b8fc51
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/libraries/EASTL
)
FetchContent_MakeAvailable(EABase EASTL)

if(TARGET EASTL)
	message(STATUS "Configuring EASTL...")

	target_compile_definitions(EASTL PRIVATE
		$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:
			EASTL_ASSERT_ENABLED=1
			EASTL_DEBUG=1
		>
		$<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:
			EASTL_ASSERT_ENABLED=0
			EASTL_DEBUG=0
		>
		EASTL_USER_DEFINED_ALLOCATOR
	)
endif()

# Group Dependencies
set_property(TARGET EABase PROPERTY FOLDER "dependencies")
set_property(TARGET EASTL PROPERTY FOLDER "dependencies")

# Optional: allow folder grouping in IDEs like Visual Studio
if(ARES_ENABLE_IDE_FOLDERS)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Add subdirectories
add_subdirectory(modules/launcher)
add_subdirectory(modules/core)

#/******************************************************/
#/*               User Project Creation                */
#/******************************************************/

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

# Extra files
set (ARES_CORE_NATVIS_FILE ${CMAKE_SOURCE_DIR}/modules/core/doc/ares-core.natvis)

# Add sources
target_sources(${ARES_PROJECT_NAME_UNDERSCORE} PRIVATE
	${SAMPLE_PROJECT_SOURCE_FILES}
	${SAMPLE_PROJECT_HEADER_FILES}
)

# Include directory
target_include_directories(${ARES_PROJECT_NAME_UNDERSCORE}
	PRIVATE ${ARES_PROJECT_DIR}/src
)

# Link against launcher and modules
target_link_libraries(${ARES_PROJECT_NAME_UNDERSCORE} PRIVATE ares_launcher)
foreach(lib IN LISTS ARES_PROJECT_CONFIG_LIST)
	target_link_libraries(${ARES_PROJECT_NAME_UNDERSCORE} PRIVATE ares_${lib})
endforeach()

# Name the executable
set_target_properties(${ARES_PROJECT_NAME_UNDERSCORE} PROPERTIES
	OUTPUT_NAME ${ARES_PROJECT_NAME}
)

# Set subsystem depending on user option
if(WIN32)
	if(ARES_ENABLE_CONSOLE_APP)
		set_target_properties(${ARES_PROJECT_CONFIG_NAME_UNDERSCORE} PROPERTIES WIN32_EXECUTABLE OFF)
	else()
		set_target_properties(${ARES_PROJECT_CONFIG_NAME_UNDERSCORE} PROPERTIES WIN32_EXECUTABLE ON)
	endif()
endif()

# Set Solution
project (Ares_Engine VERSION 0.0.1 LANGUAGES CXX)