#include "../include/ui/font_manager.hpp"
#include <imgui.h>
#include <imgui_freetype.h>
#include <iostream>

namespace ImBored::UI {

FontManager::FontManager() : m_fontsLoaded(false) {
}

FontManager::~FontManager() {
}

void FontManager::loadQuicksandFont(const char* fontPath, float fontSize) {
    ImGuiIO& io = ImGui::GetIO();
    
    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    config.FontLoaderFlags = ImGuiFreeTypeLoaderFlags_LoadColor;
    
    io.Fonts->AddFontFromFileTTF(fontPath, fontSize, &config);
    
    io.Fonts->Build();
    m_fontsLoaded = true;
}

void FontManager::setDefaultFont() {
    ImGuiIO& io = ImGui::GetIO();
    if (!io.Fonts->Fonts.empty()) {
        ImGui::GetIO().FontDefault = io.Fonts->Fonts[0];
    }
}

} // namespace ImBored::UI
