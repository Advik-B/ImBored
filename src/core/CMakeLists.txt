# Core Module
add_library(
    imbored_core
    window.cpp
    ../../include/core/window.hpp
)

target_include_directories(imbored_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../../include)
target_link_libraries(imbored_core PUBLIC glfw)
