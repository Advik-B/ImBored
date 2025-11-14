#include "ui/emoji_manager.hpp"
#include "ui/colrv1_renderer.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

// FreeType headers
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H
#include FT_TRUETYPE_TABLES_H
#include FT_SFNT_NAMES_H
#include FT_COLOR_H

// LunaSVG headers
#include <lunasvg.h>

// OpenGL for texture creation
#include <glad/gl.h>

namespace ImBored::UI {

// Emoji ranges we want to support
static const std::vector<std::pair<uint32_t, uint32_t>> EMOJI_RANGES = {
    {0x1F300, 0x1F5FF}, // Miscellaneous Symbols and Pictographs
    {0x1F600, 0x1F64F}, // Emoticons
    {0x1F680, 0x1F6FF}, // Transport and Map Symbols
    {0x1F900, 0x1F9FF}, // Supplemental Symbols and Pictographs
    {0x2600,  0x26FF},  // Miscellaneous Symbols
    {0x2700,  0x27BF},  // Dingbats
};

EmojiManager::EmojiManager()
    : m_fontSize(18.0f)
    , m_textureID(nullptr)
    , m_atlasWidth(0)
    , m_atlasHeight(0)
    , m_ftFace(nullptr)
    , m_ftLibrary(nullptr)
{
}

EmojiManager::~EmojiManager() {
    if (m_textureID) {
        GLuint texID = (GLuint)(uintptr_t)m_textureID;
        glDeleteTextures(1, &texID);
    }
    
    if (m_ftFace) {
        FT_Done_Face((FT_Face)m_ftFace);
    }
    
    if (m_ftLibrary) {
        FT_Done_FreeType((FT_Library)m_ftLibrary);
    }
}

bool EmojiManager::initialize(const char* fontPath, float fontSize) {
    m_fontPath = fontPath;
    m_fontSize = fontSize;
    
    // Initialize FreeType
    FT_Library library;
    if (FT_Init_FreeType(&library)) {
        std::cerr << "Failed to initialize FreeType\n";
        return false;
    }
    m_ftLibrary = library;
    
    // Load font face
    FT_Face face;
    if (FT_New_Face(library, fontPath, 0, &face)) {
        std::cerr << "Failed to load font: " << fontPath << "\n";
        return false;
    }
    m_ftFace = face;
    
    std::cout << "EmojiManager: Font info:\n";
    std::cout << "  Family: " << (face->family_name ? face->family_name : "N/A") << "\n";
    std::cout << "  Num fixed sizes: " << face->num_fixed_sizes << "\n";
    std::cout << "  Has color: " << ((face->face_flags & FT_FACE_FLAG_COLOR) ? "yes" : "no") << "\n";
    
    // Check for fixed sizes (color emoji fonts usually have these)
    if (face->num_fixed_sizes > 0) {
        std::cout << "EmojiManager: Available fixed sizes:\n";
        for (int i = 0; i < face->num_fixed_sizes; i++) {
            std::cout << "  [" << i << "] " << face->available_sizes[i].width 
                      << "x" << face->available_sizes[i].height << "px\n";
        }
        
        // Use the fixed size closest to requested fontSize
        int bestSize = 0;
        int minDiff = std::abs(face->available_sizes[0].height - static_cast<int>(fontSize));
        for (int i = 1; i < face->num_fixed_sizes; i++) {
            int diff = std::abs(face->available_sizes[i].height - static_cast<int>(fontSize));
            if (diff < minDiff) {
                minDiff = diff;
                bestSize = i;
            }
        }
        
        if (FT_Select_Size(face, bestSize) == 0) {
            std::cout << "EmojiManager: Selected fixed size " << bestSize << ": " 
                      << face->available_sizes[bestSize].height << "px\n";
            m_fontSize = face->available_sizes[bestSize].height; // Use actual size
        } else {
            std::cerr << "Failed to select fixed size\n";
            FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(fontSize));
        }
    } else {
        // Set font size
        FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(fontSize));
    }
    
    std::cout << "EmojiManager: Extracting emoji glyphs from font...\n";
    
    // Extract SVG glyphs for our emoji ranges
    int extractedCount = 0;
    for (const auto& range : EMOJI_RANGES) {
        for (uint32_t codepoint = range.first; codepoint <= range.second; ++codepoint) {
            // Check if this codepoint has a glyph in the font
            FT_UInt glyphIndex = FT_Get_Char_Index(face, codepoint);
            if (glyphIndex == 0) {
                continue;
            }
            
            // Try to extract SVG data
            std::string svgData;
            if (extractSVG(codepoint, svgData)) {
                EmojiGlyph emoji;
                emoji.codepoint = codepoint;
                m_emojiGlyphs[codepoint] = emoji;
                extractedCount++;
            }
        }
    }
    
    std::cout << "EmojiManager: Extracted " << extractedCount << " emoji glyphs\n";
    
    if (extractedCount > 0) {
        buildAtlas();
        return true;
    }
    
    return false;
}

bool EmojiManager::extractSVG(uint32_t codepoint, std::string& svgData) {
    FT_Face face = (FT_Face)m_ftFace;
    FT_UInt glyphIndex = FT_Get_Char_Index(face, codepoint);
    
    if (glyphIndex == 0) {
        return false;
    }
    
    // Load the glyph
    if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT)) {
        return false;
    }
    
    // Check if glyph has SVG document
    FT_GlyphSlot slot = face->glyph;
    
    // For NotoColorEmoji, we might need to use the CBDT/CBLC tables instead of SVG
    // For now, we'll use a fallback approach: render using FreeType's bitmap
    // This is a simplified implementation - full SVG extraction would require
    // parsing the SVG table directly
    
    // Fallback: just mark it as available, we'll rasterize using LunaSVG placeholder
    svgData = ""; // Placeholder
    return true;
}

void EmojiManager::buildAtlas() {
    std::cout << "EmojiManager: Building texture atlas...\n";
    
    FT_Face face = (FT_Face)m_ftFace;
    
    // Set a larger pixel size for better quality
    int pixelSize = std::max(32, static_cast<int>(m_fontSize * 2));
    FT_Set_Pixel_Sizes(face, 0, pixelSize);
    std::cout << "EmojiManager: Using pixel size: " << pixelSize << "\n";
    
    // Check if face has COLR table
    FT_Color* palette = nullptr;
    FT_UShort num_palette_entries = 0;
    FT_Palette_Data palette_data;
    
    bool has_colr = FT_Palette_Select(face, 0, &palette) == 0;
    if (has_colr) {
        std::cout << "EmojiManager: Font has COLR/CPAL tables\n";
        FT_Palette_Data_Get(face, &palette_data);
        std::cout << "EmojiManager: Number of palettes: " << palette_data.num_palettes << "\n";
    }
    
    // Calculate required atlas size
    int emojiSize = pixelSize;
    int padding = 2;
    int glyphsPerRow = 16;
    int numGlyphs = m_emojiGlyphs.size();
    int numRows = (numGlyphs + glyphsPerRow - 1) / glyphsPerRow;
    
    m_atlasWidth = glyphsPerRow * (emojiSize + padding);
    m_atlasHeight = numRows * (emojiSize + padding);
    
    // Round up to power of 2 for better GPU compatibility
    auto nextPowerOf2 = [](int n) {
        int p = 1;
        while (p < n) p *= 2;
        return p;
    };
    m_atlasWidth = nextPowerOf2(m_atlasWidth);
    m_atlasHeight = nextPowerOf2(m_atlasHeight);
    
    std::cout << "EmojiManager: Atlas size: " << m_atlasWidth << "x" << m_atlasHeight << "\n";
    
    // Allocate RGBA atlas
    m_atlasData.resize(m_atlasWidth * m_atlasHeight * 4, 0);
    
    // Render each emoji into the atlas
    int x = 0, y = 0;
    int rendered = 0;
    int skipped = 0;
    
    // Create COLRv1 renderer for emoji rasterization
    COLRv1Renderer renderer(emojiSize, emojiSize);
    
    for (auto& pair : m_emojiGlyphs) {
        uint32_t codepoint = pair.first;
        EmojiGlyph& emoji = pair.second;
        
        FT_UInt glyphIndex = FT_Get_Char_Index(face, codepoint);
        if (glyphIndex == 0) {
            skipped++;
            continue;
        }
        
        // Use COLRv1 renderer to render the glyph
        if (!renderer.renderGlyph(m_ftFace, glyphIndex, codepoint)) {
            skipped++;
            continue;
        }
        
        // Copy rendered emoji to atlas
        const std::vector<uint8_t>& emoji_buffer = renderer.getBuffer();
        for (int row = 0; row < emojiSize && y + row < m_atlasHeight; ++row) {
            for (int col = 0; col < emojiSize && x + col < m_atlasWidth; ++col) {
                int atlasIdx = ((y + row) * m_atlasWidth + (x + col)) * 4;
                int bufferIdx = (row * emojiSize + col) * 4;
                
                m_atlasData[atlasIdx + 0] = emoji_buffer[bufferIdx + 0];
                m_atlasData[atlasIdx + 1] = emoji_buffer[bufferIdx + 1];
                m_atlasData[atlasIdx + 2] = emoji_buffer[bufferIdx + 2];
                m_atlasData[atlasIdx + 3] = emoji_buffer[bufferIdx + 3];
            }
        }
        
        // Calculate UV coordinates
        emoji.u0 = static_cast<float>(x) / m_atlasWidth;
        emoji.v0 = static_cast<float>(y) / m_atlasHeight;
        emoji.u1 = static_cast<float>(x + emojiSize) / m_atlasWidth;
        emoji.v1 = static_cast<float>(y + emojiSize) / m_atlasHeight;
        emoji.width = emojiSize;
        emoji.height = emojiSize;
        emoji.advance = emojiSize;
        
        // Move to next position
        x += emojiSize + padding;
        if (x + emojiSize >= m_atlasWidth) {
            x = 0;
            y += emojiSize + padding;
        }
        
        rendered++;
    }
    
    std::cout << "EmojiManager: Rendered " << rendered << " emojis to atlas (skipped: " << skipped << ")\n";
    
    // Create OpenGL texture
    createTexture();
}

void EmojiManager::createTexture() {
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_atlasWidth, m_atlasHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, m_atlasData.data());
    
    m_textureID = (void*)(uintptr_t)texID;
    
    std::cout << "EmojiManager: Created texture ID: " << texID << "\n";
}

void EmojiManager::setFontSize(float newSize) {
    if (std::abs(newSize - m_fontSize) < 0.1f) {
        return; // No significant change
    }
    
    m_fontSize = newSize;
    
    // Re-initialize with new size
    if (m_ftFace) {
        FT_Face face = (FT_Face)m_ftFace;
        FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(newSize));
        buildAtlas();
    }
}

const EmojiGlyph* EmojiManager::getEmoji(uint32_t codepoint) const {
    auto it = m_emojiGlyphs.find(codepoint);
    if (it != m_emojiGlyphs.end()) {
        return &it->second;
    }
    return nullptr;
}

bool EmojiManager::isEmoji(uint32_t codepoint) const {
    return m_emojiGlyphs.find(codepoint) != m_emojiGlyphs.end();
}

} // namespace ImBored::UI
