#include "ui/colrv1_renderer.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>

// FreeType headers
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_COLOR_H
#include FT_GLYPH_H

namespace ImBored::UI {

COLRv1Renderer::COLRv1Renderer(int width, int height)
    : m_width(width)
    , m_height(height)
{
    m_buffer.resize(width * height * 4, 0);
}

COLRv1Renderer::~COLRv1Renderer() {
}

void COLRv1Renderer::clear() {
    std::fill(m_buffer.begin(), m_buffer.end(), 0);
}

void COLRv1Renderer::compositeLayer(const std::vector<uint8_t>& layer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    for (size_t i = 0; i < m_buffer.size(); i += 4) {
        // Get alpha from the layer (grayscale)
        uint8_t layer_alpha = layer[i + 3];
        if (layer_alpha == 0) continue;
        
        // Apply color and composite
        uint8_t final_alpha = (layer_alpha * a) / 255;
        uint8_t inv_alpha = 255 - final_alpha;
        
        m_buffer[i + 0] = (r * final_alpha + m_buffer[i + 0] * inv_alpha) / 255;
        m_buffer[i + 1] = (g * final_alpha + m_buffer[i + 1] * inv_alpha) / 255;
        m_buffer[i + 2] = (b * final_alpha + m_buffer[i + 2] * inv_alpha) / 255;
        m_buffer[i + 3] = std::max(final_alpha, m_buffer[i + 3]);
    }
}

bool COLRv1Renderer::renderPaintLayer(void* ftFace, uint32_t glyphIndex, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) {
    FT_Face face = (FT_Face)ftFace;
    
    // Load the glyph
    FT_Error err = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
    if (err != 0) {
        std::cerr << "COLRv1Renderer: Failed to load glyph " << glyphIndex << ", error: " << err << "\n";
        return false;
    }
    
    // Try to render the glyph
    err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    if (err != 0) {
        std::cerr << "COLRv1Renderer: Failed to render glyph " << glyphIndex << ", error: " << err << "\n";
        return false;
    }
    
    FT_Bitmap& bitmap = face->glyph->bitmap;
    if (bitmap.width == 0 || bitmap.rows == 0) {
        std::cerr << "COLRv1Renderer: Glyph " << glyphIndex << " has empty bitmap\n";
        return false;
    }
    
    // Create a layer buffer
    std::vector<uint8_t> layer_buffer(m_width * m_height * 4, 0);
    
    // Copy bitmap to layer buffer
    int bearingX = face->glyph->bitmap_left;
    int bearingY = m_height - face->glyph->bitmap_top;
    
    for (unsigned int row = 0; row < bitmap.rows; ++row) {
        int dest_y = bearingY + row;
        if (dest_y < 0 || dest_y >= m_height) continue;
        
        for (unsigned int col = 0; col < bitmap.width; ++col) {
            int dest_x = bearingX + col;
            if (dest_x < 0 || dest_x >= m_width) continue;
            
            int bufferIdx = (dest_y * m_width + dest_x) * 4;
            int bitmapIdx = row * bitmap.pitch + col;
            
            uint8_t gray = bitmap.buffer[bitmapIdx];
            layer_buffer[bufferIdx + 0] = gray;
            layer_buffer[bufferIdx + 1] = gray;
            layer_buffer[bufferIdx + 2] = gray;
            layer_buffer[bufferIdx + 3] = gray;
        }
    }
    
    // Composite the layer
    compositeLayer(layer_buffer, r, g, b, a);
    
    return true;
}

bool COLRv1Renderer::renderGlyph(void* ftFace, uint32_t glyphIndex, uint32_t codepoint) {
    FT_Face face = (FT_Face)ftFace;
    
    // Clear buffer
    clear();
    
    // Try COLR v0 first (layered rendering)
    FT_LayerIterator iterator;
    iterator.p = nullptr;
    FT_UInt layer_glyph_index;
    FT_UInt layer_color_index;
    
    // Get palette
    FT_Color* palette = nullptr;
    if (FT_Palette_Select(face, 0, &palette) != 0) {
        palette = nullptr;
    }
    
    bool has_layers = false;
    while (FT_Get_Color_Glyph_Layer(face, glyphIndex, &layer_glyph_index, &layer_color_index, &iterator)) {
        has_layers = true;
        
        // Get color for this layer
        uint8_t r = 0, g = 0, b = 0, a = 255;
        if (layer_color_index != 0xFFFF && palette != nullptr) {
            FT_Color color = palette[layer_color_index];
            r = color.red;
            g = color.green;
            b = color.blue;
            a = color.alpha;
        }
        
        // Render this layer
        renderPaintLayer(ftFace, layer_glyph_index, r, g, b, a);
    }
    
    if (has_layers) {
        return true;
    }
    
    // For COLRv1 or no color layers: render the base glyph in grayscale
    // COLRv1 requires complex paint rendering that's not yet fully supported in FreeType 2.14
    // For now, we render the outline to at least show something
    uint8_t r = 128, g = 128, b = 128, a = 255;
    bool rendered = renderPaintLayer(ftFace, glyphIndex, r, g, b, a);
    
    // If still nothing rendered, this glyph truly has no renderable data
    if (!rendered) {
        std::cerr << "COLRv1Renderer: Failed to render glyph " << glyphIndex << " (codepoint 0x" 
                  << std::hex << codepoint << std::dec << ")\n";
    }
    
    return rendered;
}

} // namespace ImBored::UI
