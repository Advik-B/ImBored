#pragma once

namespace ImBored::UI {

class FontManager {
public:
    FontManager();
    ~FontManager();
    
    void loadQuicksandFont(const char* fontPath, float fontSize);
    void setDefaultFont();
    
private:
    bool m_fontsLoaded;
};

} // namespace ImBored::UI
