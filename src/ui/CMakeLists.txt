# UI Module
add_library(
    imbored_ui
    font_manager.cpp
    emoji_manager.cpp
    smart_text.cpp
    ../../include/ui/font_manager.hpp
    ../../include/ui/emoji_manager.hpp
    ../../include/ui/smart_text.hpp
)

target_include_directories(imbored_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../../include)
target_link_libraries(imbored_ui PUBLIC imgui freetype harfbuzz lunasvg glad)
