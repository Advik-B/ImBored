#pragma once

#include <vector>
#include <cstdint>

namespace ImBored::UI {

// Simple COLRv1 paint renderer
// This implements basic COLRv1 paint operations without requiring full Skia
class COLRv1Renderer {
public:
    COLRv1Renderer(int width, int height);
    ~COLRv1Renderer();
    
    // Render a COLRv1 glyph to the internal buffer
    bool renderGlyph(void* ftFace, uint32_t glyphIndex, uint32_t codepoint);
    
    // Get the rendered buffer
    const std::vector<uint8_t>& getBuffer() const { return m_buffer; }
    
    // Clear the buffer
    void clear();
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
private:
    int m_width;
    int m_height;
    std::vector<uint8_t> m_buffer; // RGBA buffer
    
    // Helper to composite layers
    void compositeLayer(const std::vector<uint8_t>& layer, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    
    // Render a single paint layer
    bool renderPaintLayer(void* ftFace, uint32_t glyphIndex, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a);
};

} // namespace ImBored::UI
