cmake_minimum_required(VERSION 3.20)
project(ImBored)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable LTO for release builds to minimize binary size
# This is especially important when linking with Skia (reduces size dramatically)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
    if(ipo_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        message(STATUS "LTO/IPO enabled for Release build")
    else()
        message(WARNING "LTO/IPO not supported: ${ipo_error}")
    endif()
endif()

# Use Ninja as the build system
set(CMAKE_MAKE_PROGRAM ninja CACHE FILEPATH "Ninja build system")
if(NOT CMAKE_GENERATOR MATCHES "Ninja")
    message(WARNING "It is recommended to use Ninja as the build system. Run: cmake -G Ninja -B build -S .")
endif()

# ============================================================================
# Dependency Management Strategy
# ============================================================================
# This project uses FetchContent to automatically download and build most
# dependencies, eliminating the need for manual installation.
#
# FetchContent-managed dependencies:
# - GLFW, FreeType, HarfBuzz, libpng, zlib, LunaSVG, glad
#
# System dependencies (cannot be fetched, OS-level graphics libraries):
# - X11/Wayland: Window management (Linux only)
# - OpenGL: Graphics API (provided by OS/drivers)
#
# See docs/BUILD.md for installation instructions.
# ============================================================================

include(FetchContent)

# ---- GLFW ----
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
set(GLFW_BUILD_WAYLAND OFF CACHE INTERNAL "Build support for Wayland")
FetchContent_MakeAvailable(glfw)

# ---- glad (manually included) ----
add_library(
        glad
        thirdparty/glad/src/gl.c
        thirdparty/glad/src/gles2.c
        thirdparty/glad/src/glsc2.c
)
target_include_directories(glad PUBLIC thirdparty/glad/include)

# ---- ZLIB ----
FetchContent_Declare(
        zlib
        GIT_REPOSITORY https://github.com/madler/zlib.git
        GIT_TAG v1.3.1
)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(zlib)
set(SKIP_INSTALL_ALL OFF CACHE BOOL "" FORCE)

# Set ZLIB variables for FreeType to find
set(ZLIB_FOUND TRUE CACHE BOOL "" FORCE)
set(ZLIB_LIBRARY zlibstatic CACHE STRING "" FORCE)
set(ZLIB_INCLUDE_DIR "${zlib_SOURCE_DIR};${zlib_BINARY_DIR}" CACHE STRING "" FORCE)
set(ZLIB_LIBRARIES zlibstatic CACHE STRING "" FORCE)

# Create ZLIB::ZLIB alias target if it doesn't exist
if(NOT TARGET ZLIB::ZLIB)
    add_library(ZLIB::ZLIB ALIAS zlibstatic)
endif()

# ---- PNG ----
FetchContent_Declare(
        libpng
        GIT_REPOSITORY https://github.com/glennrp/libpng.git
        GIT_TAG v1.6.43
)
set(PNG_SHARED OFF CACHE BOOL "" FORCE)
set(PNG_STATIC ON CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libpng)
set(SKIP_INSTALL_ALL OFF CACHE BOOL "" FORCE)

# Set PNG variables for FreeType to find
set(PNG_FOUND TRUE CACHE BOOL "" FORCE)
set(PNG_LIBRARY png_static CACHE STRING "" FORCE)
set(PNG_PNG_INCLUDE_DIR "${libpng_SOURCE_DIR};${libpng_BINARY_DIR}" CACHE STRING "" FORCE)
set(PNG_LIBRARIES png_static CACHE STRING "" FORCE)

# png_static already includes the necessary include directories, just create the alias
if(NOT TARGET PNG::PNG)
    add_library(PNG::PNG ALIAS png_static)
endif()

# Make sure png_static has the binary dir in its include directories for pnglibconf.h
target_include_directories(png_static PUBLIC 
    $<BUILD_INTERFACE:${libpng_SOURCE_DIR}>
    $<BUILD_INTERFACE:${libpng_BINARY_DIR}>
)

# ---- HarfBuzz ----
FetchContent_Declare(
        harfbuzz
        GIT_REPOSITORY https://github.com/harfbuzz/harfbuzz.git
        GIT_TAG 8.3.0
)
set(HB_BUILD_INTROSPECTION OFF CACHE BOOL "" FORCE)
set(HB_HAVE_GRAPHITE2 OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(harfbuzz)

# ---- FreeType ----
FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG VER-2-14-1
)
set(FT_DISABLE_HARFBUZZ OFF CACHE BOOL "" FORCE)
set(FT_REQUIRE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_REQUIRE_PNG ON CACHE BOOL "" FORCE)
# Enable COLR/CPAL support for color emoji rendering
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
# Disable installation to avoid export issues
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(freetype)
set(SKIP_INSTALL_ALL OFF CACHE BOOL "" FORCE)

# ---- LunaSVG (for SVG emoji support) ----
FetchContent_Declare(
        lunasvg
        GIT_REPOSITORY https://github.com/sammycage/lunasvg.git
        GIT_TAG master
)
FetchContent_MakeAvailable(lunasvg)

# ---- Skia (for COLRv1 emoji rendering) ----
# Skia is configured for minimal static build with LTO to reduce size
# Full Skia (~200MB) -> Minimal static + LTO (~5-20MB final binary increase)
option(USE_SKIA "Enable Skia for COLRv1 emoji rendering" ON)

if(USE_SKIA)
    # Check if Skia library exists in thirdparty/skia
    if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/skia/lib/libskia.a")
        set(SKIA_AVAILABLE TRUE)
        set(SKIA_LIBRARY "${CMAKE_SOURCE_DIR}/thirdparty/skia/lib/libskia.a")
        set(SKIA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/skia/include")
        message(STATUS "Found Skia library: ${SKIA_LIBRARY}")
        message(STATUS "Skia enabled - full COLRv1 emoji rendering available")
    else()
        set(SKIA_AVAILABLE FALSE)
        message(WARNING "Skia library not found at thirdparty/skia/lib/libskia.a")
        message(WARNING "Build Skia from source and place it there. See docs/SKIA_SETUP.md")
        message(WARNING "Falling back to PNG/CBDT + grayscale emoji rendering")
    endif()
else()
    set(SKIA_AVAILABLE FALSE)
    message(STATUS "Skia disabled - using PNG/CBDT + grayscale emoji rendering")
endif()

# ---- ImGUI ----
add_library(
    imgui
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_demo.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/imgui_widgets.cpp
    thirdparty/imgui/backends/imgui_impl_glfw.cpp
    thirdparty/imgui/backends/imgui_impl_opengl3.cpp
    thirdparty/imgui/misc/freetype/imgui_freetype.cpp
    thirdparty/imgui/misc/cpp/imgui_stdlib.cpp
)
target_include_directories(imgui PUBLIC thirdparty/imgui)
target_include_directories(imgui PUBLIC thirdparty/imgui/backends)
target_include_directories(imgui PUBLIC thirdparty/imgui/misc/freetype)
target_link_libraries(imgui PUBLIC freetype harfbuzz glfw lunasvg)

# ImGui's GLFW backend uses X11 functions directly on Linux, so we need to link X11
# X11 is a system library that cannot be fetched via FetchContent
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(imgui PRIVATE ${X11_LIBRARIES})
endif()

# ---- Modules ----
add_subdirectory(src)

# ---- ImBored Executable ----
add_executable(
    ImBored
    main.cpp
)
target_include_directories(ImBored PRIVATE include)

# Enable DPI awareness on Windows
if(MSVC)
    target_compile_definitions(ImBored PRIVATE _DPI_AWARENESS_CONTEXTS_)
    # Set manifest for DPI awareness
    if(WIN32)
        set_target_properties(ImBored PROPERTIES
            VS_WINDOWS_TARGET_PLATFORM_VERSION ${CMAKE_SYSTEM_VERSION}
        )
    endif()
endif()

# Tell MSVC to statically link the runtime
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

# ---- Link ----
# GLFW handles X11 dependencies internally on Linux
target_link_libraries(ImBored PRIVATE
        glfw
        glad
        imgui
        imbored_core
        imbored_rendering
        imbored_ui
)

# ---- Copy Resources ----
add_custom_command(TARGET ImBored POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    $<TARGET_FILE_DIR:ImBored>/resources
    COMMENT "Copying resources to output directory"
)