#pragma once

#include <vector>
#include <cstdint>

// Check if Skia is available
#ifdef USE_SKIA
#define SKIA_AVAILABLE
// Forward declarations for Skia types
class SkCanvas;
class SkSurface;
#endif

namespace ImBored::UI {

// COLRv1 paint renderer with optional Skia integration
// Falls back to basic rendering when Skia is not available
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
    
    // Check if Skia rendering is available
    static bool isSkiaAvailable();
    
private:
    int m_width;
    int m_height;
    std::vector<uint8_t> m_buffer; // RGBA buffer
    
#ifdef SKIA_AVAILABLE
    // Skia-specific members
    SkSurface* m_surface;
    SkCanvas* m_canvas;
    
    // Render using Skia
    bool renderWithSkia(void* ftFace, uint32_t glyphIndex, uint32_t codepoint);
#endif
    
    // Fallback rendering without Skia
    // Helper to composite layers
    void compositeLayer(const std::vector<uint8_t>& layer, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    
    // Render a single paint layer (fallback)
    bool renderPaintLayer(void* ftFace, uint32_t glyphIndex, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a);
};

} // namespace ImBored::UI
