# Rendering Module
add_library(
    imbored_rendering
    renderer.cpp
    ../../include/rendering/renderer.hpp
)

target_include_directories(imbored_rendering PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../../include)
target_link_libraries(imbored_rendering PUBLIC glad imgui)
