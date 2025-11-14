# UI Module
add_library(
    imbored_ui
    font_manager.cpp
    emoji_manager.cpp
    smart_text.cpp
    colrv1_renderer.cpp
    ../../include/ui/font_manager.hpp
    ../../include/ui/emoji_manager.hpp
    ../../include/ui/smart_text.hpp
    ../../include/ui/colrv1_renderer.hpp
)

target_include_directories(imbored_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../../include)
target_link_libraries(imbored_ui PUBLIC imgui freetype harfbuzz lunasvg glad)

# Add Skia support if available
if(SKIA_AVAILABLE)
    target_compile_definitions(imbored_ui PRIVATE USE_SKIA)
    target_link_libraries(imbored_ui PUBLIC skia)
    message(STATUS "Skia integration enabled for imbored_ui")
endif()
