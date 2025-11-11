#pragma once

struct GLFWwindow;

namespace ImBored::Core {

class Window {
public:
    Window(int width, int height, const char* title);
    ~Window();
    
    bool isOpen() const;
    void pollEvents();
    void swapBuffers();
    GLFWwindow* getHandle();
    
private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
};

} // namespace ImBored::Core
