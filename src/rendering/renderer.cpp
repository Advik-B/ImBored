#include "../include/rendering/renderer.hpp"
#include <glad/gl.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>

namespace ImBored::Rendering {

Renderer::Renderer() {
    setupOpenGL();
}

Renderer::~Renderer() {
    ImGui_ImplOpenGL3_Shutdown();
}

void Renderer::setupOpenGL() {
    glClearColor(100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f);
}

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::setViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

} // namespace ImBored::Rendering
