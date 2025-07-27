cmake_minimum_required(VERSION 3.20)
project (Ares-Engine VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Configuring...")

# Define options
option(ENABLE_MIN_SIZE_RELEASE "Enable a min-size release build." ON)
option(ENABLE_EXTRA_OPTIMIZATIONS "Enable extra optimizations for the release build." ON)
option(ENABLE_IDE_FOLDERS "Enable IDE folder grouping (e.g., Visual Studio filters)." ON)

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
add_subdirectory(entry-point)