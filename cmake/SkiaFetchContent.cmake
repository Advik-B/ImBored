# SkiaFetchContent.cmake
# CMake module to download and set up prebuilt Skia using FetchContent
#
# This module automates Skia setup by downloading prebuilt binaries
# instead of building from source, which is much faster and simpler.

include(FetchContent)

# Check if we should use Skia
if(NOT USE_SKIA)
    message(STATUS "Skia disabled - skipping FetchContent setup")
    return()
endif()

message(STATUS "Setting up Skia with FetchContent (prebuilt binaries)...")

# Detect platform and architecture
# Platform names for JetBrains skia-pack: linux, macos, windows
# Architecture: x64, arm64
# Format: ${PLATFORM}-${BUILD_TYPE}-${ARCH}
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(SKIA_OS "linux")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64|AMD64")
        set(SKIA_ARCH "x64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
        set(SKIA_ARCH "arm64")
    else()
        message(WARNING "Unsupported Linux architecture: ${CMAKE_SYSTEM_PROCESSOR}")
        return()
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(SKIA_OS "macos")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        set(SKIA_ARCH "arm64")
    else()
        set(SKIA_ARCH "x64")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(SKIA_OS "windows")
    set(SKIA_ARCH "x64")
else()
    message(WARNING "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
    return()
endif()

message(STATUS "Detected platform: ${SKIA_OS}-${SKIA_ARCH}")

# Allow user to specify custom Skia URL
# This can be set via -DSKIA_PREBUILT_URL=<url> to use a custom prebuilt package
if(NOT DEFINED SKIA_PREBUILT_URL)
    # Use JetBrains Skia-pack (commonly used, well-maintained)
    # https://github.com/JetBrains/skia-pack
    
    # Version format: m<milestone>-<commit>-<build>
    # Using a stable m126 release
    set(SKIA_VERSION "m126-6fd3120c1b-1")
    set(SKIA_BUILD_TYPE "Release")  # or "Debug"
    
    # Construct the filename: Skia-{version}-{os}-{buildtype}-{arch}.zip
    set(SKIA_FILENAME "Skia-${SKIA_VERSION}-${SKIA_OS}-${SKIA_BUILD_TYPE}-${SKIA_ARCH}.zip")
    set(SKIA_URL "https://github.com/JetBrains/skia-pack/releases/download/${SKIA_VERSION}/${SKIA_FILENAME}")
else()
    set(SKIA_URL "${SKIA_PREBUILT_URL}")
    message(STATUS "Using custom Skia URL: ${SKIA_URL}")
endif()

# Try to download and populate Skia
FetchContent_Declare(
    skia
    URL ${SKIA_URL}
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_GetProperties(skia)
if(NOT skia_POPULATED)
    message(STATUS "Downloading prebuilt Skia from ${SKIA_URL}...")
    message(STATUS "This may take a few minutes depending on your connection...")
    
    # Use FetchContent_Populate with error handling
    FetchContent_Populate(skia)
    
    if(NOT skia_POPULATED)
        message(WARNING "Failed to download Skia prebuilt binaries")
        message(STATUS "You can:")
        message(STATUS "  1. Check your internet connection and try again")
        message(STATUS "  2. Provide a custom URL: cmake -DSKIA_PREBUILT_URL=<url>")
        message(STATUS "  3. Build without Skia: cmake -DUSE_SKIA=OFF")
        return()
    endif()
    
    message(STATUS "Skia downloaded to: ${skia_SOURCE_DIR}")
endif()

# Find the Skia library file
# Different prebuilt packages may have different directory structures
# We'll check common locations
set(SKIA_LIB_PATHS
    "${skia_SOURCE_DIR}/out/Release-${SKIA_OS}-${SKIA_ARCH}/libskia.a"
    "${skia_SOURCE_DIR}/out/Release-x64/libskia.a"
    "${skia_SOURCE_DIR}/out/Release/libskia.a"
    "${skia_SOURCE_DIR}/out/Static/libskia.a"
    "${skia_SOURCE_DIR}/lib/libskia.a"
    "${skia_SOURCE_DIR}/libskia.a"
    "${skia_SOURCE_DIR}/skia.lib"
    "${skia_SOURCE_DIR}/out/Release-x64/skia.lib"
    "${skia_SOURCE_DIR}/out/Release-${SKIA_OS}-${SKIA_ARCH}/skia.lib"
)

set(SKIA_LIB_FOUND FALSE)
foreach(LIB_PATH ${SKIA_LIB_PATHS})
    if(EXISTS "${LIB_PATH}")
        set(SKIA_LIB_PATH "${LIB_PATH}")
        set(SKIA_LIB_FOUND TRUE)
        break()
    endif()
endforeach()

if(NOT SKIA_LIB_FOUND)
    message(WARNING "Could not find Skia library in downloaded package")
    message(STATUS "Searched in:")
    foreach(LIB_PATH ${SKIA_LIB_PATHS})
        message(STATUS "  - ${LIB_PATH}")
    endforeach()
    message(STATUS "Package structure at ${skia_SOURCE_DIR}:")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E echo "Listing directory contents..."
        COMMAND find ${skia_SOURCE_DIR} -name "*.a" -o -name "*.lib"
        OUTPUT_VARIABLE SKIA_FILES
        ERROR_QUIET
    )
    message(STATUS "${SKIA_FILES}")
    message(STATUS "Falling back to building without Skia")
    return()
endif()

# Find include directory
if(EXISTS "${skia_SOURCE_DIR}/include")
    set(SKIA_INCLUDE_PATH "${skia_SOURCE_DIR}")
else()
    message(WARNING "Could not find Skia include directory in ${skia_SOURCE_DIR}")
    return()
endif()

# Create an imported library for Skia
add_library(skia STATIC IMPORTED GLOBAL)
set_target_properties(skia PROPERTIES
    IMPORTED_LOCATION ${SKIA_LIB_PATH}
)

# Set include directories for Skia
# Skia headers reference themselves with "include/..." paths
target_include_directories(skia INTERFACE
    ${SKIA_INCLUDE_PATH}
)

# Export Skia variables for use in the main project
set(SKIA_AVAILABLE TRUE)
set(SKIA_LIBRARY skia)
set(SKIA_INCLUDE_DIR ${SKIA_INCLUDE_PATH})

message(STATUS "Skia prebuilt setup complete")
message(STATUS "  Library: ${SKIA_LIB_PATH}")
message(STATUS "  Includes: ${SKIA_INCLUDE_PATH}")
