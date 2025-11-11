#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_freetype.h"

#include "include/core/window.hpp"
#include "include/rendering/renderer.hpp"

using namespace ImBored::Core;
using namespace ImBored::Rendering;

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
        
        // Load fonts with FreeType - Main font
        ImFontConfig config;
        config.FontDataOwnedByAtlas = false;
        config.FontLoaderFlags = ImGuiFreeTypeLoaderFlags_LoadColor;
        
        ImFont* mainFont = io.Fonts->AddFontFromFileTTF("resources/Quicksand-Regular.ttf", 18.0f, &config);
        
        // Load colored emoji font with comprehensive emoji ranges
        // Covers: emoticons, symbols, pictographs, miscellaneous, supplemental, extended
        static const ImWchar emoji_ranges[] = {
            0x1F300, 0x1F9FF, // Emoticons, symbols, pictographs, etc.
            0x2600,  0x27BF,  // Miscellaneous symbols
            0x2700,  0x27EF,  // Dingbats
            0xFE00,  0xFE0F,  // Variation selectors
            0,
        };
        
        ImFontConfig emojiConfig;
        emojiConfig.FontDataOwnedByAtlas = false;
        emojiConfig.FontLoaderFlags = ImGuiFreeTypeLoaderFlags_LoadColor;
        emojiConfig.MergeMode = true;
        emojiConfig.PixelSnapH = false; // Better for emoji rendering
        
        io.Fonts->AddFontFromFileTTF("resources/NotoColorEmoji-Regular.ttf", 18.0f, &emojiConfig, emoji_ranges);
        
        io.Fonts->Build();
        
        if (mainFont) {
            io.FontDefault = mainFont;
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
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 1.0f), "COLOUR EMOJI SUPPORT");
            ImGui::Spacing();
            ImGui::Text("Smileys & Emotion: 😀 😃 😄 😁 😆 😅 🤣 😂 😉 😊 😇 🙂 🙃 😌 😍 🥰");
            ImGui::Text("Hand Gestures: 👋 👏 🙌 👐 🤲 🤝 👂 👃 👀 👁 🧠 👅 👄");
            ImGui::Text("Animals: 🐶 🐱 🐭 🐹 🐰 🦊 🐻 🐼 🐨 🐯 🦁 🐮 🐷");
            ImGui::Text("Food: 🍕 🍔 🍟 🌭 🌮 🌯 🍿 🥓 🥞 🍗 🍖 🍝 🍜 🍲");
            ImGui::Text("Travel: 🚗 🚕 🚙 🚌 🚎 🏎 🚓 🚑 🚒 🚐 🛻 🚚 🚛 ✈️ 🚁 🚂 🚃 🚄");
            ImGui::Text("Sports: ⚽ 🏀 🏈 ⚾ 🎾 🏐 🏉 🥏 🎱 🎳 🏓 🏸 🥊 🥋");
            ImGui::Text("Symbols: ❤️ 💔 💕 💖 💗 💙 💚 💛 🖤 🤍 🤎 💝 💞");
            ImGui::Text("Nature: ☀️ 🌤️ ⛅ 🌥️ ☁️ 🌦️ 🌧️ ⛈️ 🌩️ 🌨️ ❄️ ☃️ ⚡ 🌈");
            ImGui::Spacing();
            ImGui::Text("Rendered using NotoColorEmoji-Regular.ttf with FreeType support");
            ImGui::End();
            
            ImGui::ShowDemoWindow();
            
            // Rendering
            renderer.setViewport(800, 600);
            renderer.clear();
            renderer.render();
            
            window.swapBuffers();
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
