# UI Module
add_library(
    imbored_ui
    font_manager.cpp
    ../../include/ui/font_manager.hpp
)

target_include_directories(imbored_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../../include)
target_link_libraries(imbored_ui PUBLIC imgui freetype harfbuzz)
