#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

namespace ImBored::UI {

struct EmojiGlyph {
    uint32_t codepoint;
    float u0, v0, u1, v1;  // UV coordinates in atlas
    float width, height;    // Original size
    float advance;          // Horizontal advance
};

class EmojiManager {
public:
    EmojiManager();
    ~EmojiManager();
    
    // Initialize and load emojis from font file
    bool initialize(const char* fontPath, float fontSize);
    
    // Re-rasterize at new size
    void setFontSize(float newSize);
    
    // Get emoji glyph data
    const EmojiGlyph* getEmoji(uint32_t codepoint) const;
    
    // Get texture ID for ImGui
    void* getTextureID() const { return m_textureID; }
    
    // Get atlas dimensions
    int getAtlasWidth() const { return m_atlasWidth; }
    int getAtlasHeight() const { return m_atlasHeight; }
    
    // Check if codepoint is an emoji
    bool isEmoji(uint32_t codepoint) const;
    
private:
    void buildAtlas();
    void createTexture();
    bool extractSVG(uint32_t codepoint, std::string& svgData);
    
    std::unordered_map<uint32_t, EmojiGlyph> m_emojiGlyphs;
    std::string m_fontPath;
    float m_fontSize;
    
    // Texture data
    void* m_textureID;
    std::vector<uint8_t> m_atlasData;
    int m_atlasWidth;
    int m_atlasHeight;
    
    // FreeType font face
    void* m_ftFace;  // FT_Face
    void* m_ftLibrary; // FT_Library
};

} // namespace ImBored::UI
