#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_freetype.h"

#include "include/core/window.hpp"
#include "include/rendering/renderer.hpp"
#include "include/ui/emoji_manager.hpp"
#include "include/ui/smart_text.hpp"

using namespace ImBored::Core;
using namespace ImBored::Rendering;
using namespace ImBored::UI;

int main() {
    try {
        // Initialize window
        Window window(800, 600, "ImBored - Modular Architecture");
        
        // Load OpenGL functions
        if (!gladLoadGL(glfwGetProcAddress)) {
            std::cerr << "Failed to initialize glad (glad/gl.h)\n";
            return -1;
        }
        
        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        
        // Disable ini file
        io.IniFilename = nullptr;
        
        // Disable DPI scaling
        io.FontGlobalScale = 1.0f;
        
        // Setup font loader to use FreeType
        io.Fonts->SetFontLoader(ImGuiFreeType::GetFontLoader());
        
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window.getHandle(), true);
        std::cout << glGetString(GL_VERSION) << "\n";
        ImGui_ImplOpenGL3_Init("#version 330 core");
        
        // Load fonts with FreeType - Main font only (no emoji font merging)
        std::cout << "DEBUG: Loading main font...\n";
        auto font_start = std::chrono::high_resolution_clock::now();
        
        ImFontConfig config;
        config.FontDataOwnedByAtlas = false;
        
        ImFont* mainFont = io.Fonts->AddFontFromFileTTF("resources/Quicksand-Regular.ttf", 18.0f, &config);
        std::cout << "DEBUG: Main font loaded\n";
        
        // Build font atlas (no emoji glyphs)
        std::cout << "DEBUG: Building font atlas...\n";
        auto atlas_start = std::chrono::high_resolution_clock::now();
        io.Fonts->Build();
        auto atlas_end = std::chrono::high_resolution_clock::now();
        auto atlas_duration = std::chrono::duration_cast<std::chrono::milliseconds>(atlas_end - atlas_start);
        auto font_duration = std::chrono::duration_cast<std::chrono::milliseconds>(atlas_end - font_start);
        
        std::cout << "DEBUG: Font atlas built successfully in " << atlas_duration.count() << "ms\n";
        std::cout << "DEBUG: Total font loading time: " << font_duration.count() << "ms\n";
        
        if (mainFont) {
            io.FontDefault = mainFont;
        }
        
        // Initialize emoji manager for SVG-based emoji rendering
        std::cout << "DEBUG: Initializing emoji manager...\n";
        auto emoji_start = std::chrono::high_resolution_clock::now();
        EmojiManager emojiManager;
        bool emojiSuccess = emojiManager.initialize("resources/NotoColorEmoji-Regular.ttf", 18.0f);
        auto emoji_end = std::chrono::high_resolution_clock::now();
        auto emoji_duration = std::chrono::duration_cast<std::chrono::milliseconds>(emoji_end - emoji_start);
        
        if (emojiSuccess) {
            std::cout << "DEBUG: Emoji manager initialized in " << emoji_duration.count() << "ms\n";
            SmartTextInit(&emojiManager);
        } else {
            std::cerr << "WARNING: Failed to initialize emoji manager\n";
        }
        
        // Initialize renderer
        Renderer renderer;
        while (window.isOpen()) {
            window.pollEvents();
            
            // Start a new ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            // Example ImGui window
            ImGui::Begin("ImBored");
            ImGui::Text("Modular Architecture with Quicksand Font");
            ImGui::Text("FPS: %.1f", io.Framerate);
            ImGui::Separator();
            
            // Colour Emoji Support Section
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 1.0f), "COLOUR EMOJI SUPPORT (SVG-based)");
            ImGui::Spacing();
            
            if (emojiSuccess) {
                SmartText("Smileys & Emotion: 😀 😃 😄 😁 😆 😅 🤣 😂 😉 😊 😇 🙂 🙃 😌 😍 🥰");
                SmartText("Hand Gestures: 👋 👏 🙌 👐 🤲 🤝 👂 👃 👀 👁 🧠 👅 👄");
                SmartText("Animals: 🐶 🐱 🐭 🐹 🐰 🦊 🐻 🐼 🐨 🐯 🦁 🐮 🐷");
                SmartText("Food: 🍕 🍔 🍟 🌭 🌮 🌯 🍿 🥓 🥞 🍗 🍖 🍝 🍜 🍲");
                SmartText("Travel: 🚗 🚕 🚙 🚌 🚎 🏎 🚓 🚑 🚒 🚐 🛻 🚚 🚛 ✈️ 🚁 🚂 🚃 🚄");
                SmartText("Sports: ⚽ 🏀 🏈 ⚾ 🎾 🏐 🏉 🥏 🎱 🎳 🏓 🏸 🥊 🥋");
                SmartText("Symbols: ❤️ 💔 💕 💖 💗 💙 💚 💛 🖤 🤍 🤎 💝 💞");
                SmartText("Nature: ☀️ 🌤️ ⛅ 🌥️ ☁️ 🌦️ 🌧️ ⛈️ 🌩️ 🌨️ ❄️ ☃️ ⚡ 🌈");
            } else {
                ImGui::Text("Emoji manager failed to initialize");
            }
            
            ImGui::Spacing();
            ImGui::Text("Rendered using SVG extraction from NotoColorEmoji-Regular.ttf");
            ImGui::End();

            ImGui::ShowDemoWindow();
            
            // Rendering
            renderer.setViewport(800, 600);
            renderer.clear();
            renderer.render();
            
            window.swapBuffers();
            
            // Limit framerate to 60 FPS to reduce CPU load
            glfwWaitEventsTimeout(1.0 / 60.0);
        }
        
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return -1;
    }
}
