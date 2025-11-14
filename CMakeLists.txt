cmake_minimum_required(VERSION 3.20)
project(ImBored)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Use Ninja as the build system
set(CMAKE_MAKE_PROGRAM ninja CACHE FILEPATH "Ninja build system")
if(NOT CMAKE_GENERATOR MATCHES "Ninja")
    message(WARNING "It is recommended to use Ninja as the build system. Run: cmake -G Ninja -B build -S .")
endif()

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

# ---- HarfBuzz ----
find_package(harfbuzz CONFIG QUIET)
if(NOT harfbuzz_FOUND)
    FetchContent_Declare(
            harfbuzz
            GIT_REPOSITORY https://github.com/harfbuzz/harfbuzz.git
            GIT_TAG 8.3.0
    )
    set(HB_BUILD_INTROSPECTION OFF CACHE BOOL "" FORCE)
    set(HB_HAVE_GRAPHITE2 OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(harfbuzz)
endif()

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
FetchContent_MakeAvailable(freetype)

# ---- LunaSVG (for SVG emoji support) ----
FetchContent_Declare(
        lunasvg
        GIT_REPOSITORY https://github.com/sammycage/lunasvg.git
        GIT_TAG master
)
FetchContent_MakeAvailable(lunasvg)

# ---- Skia (for COLRv1 emoji rendering) ----
# Note: Skia is a large library. For now, we'll use system Skia if available,
# or download prebuilt binaries for the platform.
# TODO: Add proper Skia integration when stable binaries are available
# For now, we'll implement a Skia-based rasterizer that can be enabled when Skia is available

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
# Find X11 for Linux
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
endif()

target_link_libraries(ImBored PRIVATE
        glfw
        glad
        imgui
        imbored_core
        imbored_rendering
        imbored_ui
)

# Link X11 on Linux
if(UNIX AND NOT APPLE AND X11_FOUND)
    target_link_libraries(ImBored PRIVATE ${X11_LIBRARIES})
endif()

# ---- Copy Resources ----
add_custom_command(TARGET ImBored POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    $<TARGET_FILE_DIR:ImBored>/resources
    COMMENT "Copying resources to output directory"
)