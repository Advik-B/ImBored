#pragma once

namespace ImBored::Rendering {

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    void clear();
    void render();
    void setViewport(int width, int height);
    
private:
    void setupOpenGL();
};

} // namespace ImBored::Rendering
