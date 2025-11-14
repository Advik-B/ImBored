#include "ui/smart_text.hpp"
#include "ui/emoji_manager.hpp"
#include "imgui.h"
#include <string>
#include <cstring>

namespace ImBored::UI {

static EmojiManager* g_emojiManager = nullptr;

void SmartTextInit(EmojiManager* emojiManager) {
    g_emojiManager = emojiManager;
}

// Helper to decode UTF-8 codepoint
static uint32_t decodeUTF8(const char*& str) {
    uint32_t codepoint = 0;
    unsigned char c = *str++;
    
    if (c < 0x80) {
        codepoint = c;
    } else if ((c & 0xE0) == 0xC0) {
        codepoint = (c & 0x1F) << 6;
        codepoint |= (*str++ & 0x3F);
    } else if ((c & 0xF0) == 0xE0) {
        codepoint = (c & 0x0F) << 12;
        codepoint |= (*str++ & 0x3F) << 6;
        codepoint |= (*str++ & 0x3F);
    } else if ((c & 0xF8) == 0xF0) {
        codepoint = (c & 0x07) << 18;
        codepoint |= (*str++ & 0x3F) << 12;
        codepoint |= (*str++ & 0x3F) << 6;
        codepoint |= (*str++ & 0x3F);
    }
    
    return codepoint;
}

void SmartText(const char* text) {
    if (!g_emojiManager || !text) {
        ImGui::TextUnformatted(text);
        return;
    }
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImU32 color = ImGui::GetColorU32(ImGuiCol_Text);
    
    const char* textPtr = text;
    std::string buffer;
    float lineHeight = ImGui::GetTextLineHeight();
    float cursorX = pos.x;
    
    while (*textPtr) {
        uint32_t codepoint = decodeUTF8(textPtr);
        
        // Check if it's an emoji
        const EmojiGlyph* emoji = g_emojiManager->getEmoji(codepoint);
        
        if (emoji) {
            // Render any accumulated text first
            if (!buffer.empty()) {
                ImVec2 textPos(cursorX, pos.y);
                drawList->AddText(textPos, color, buffer.c_str());
                cursorX += ImGui::CalcTextSize(buffer.c_str()).x;
                buffer.clear();
            }
            
            // Render emoji as image
            ImVec2 emojiPos(cursorX, pos.y);
            ImVec2 emojiSize(emoji->width, emoji->height);
            
            // Center vertically if smaller than line height
            if (emoji->height < lineHeight) {
                emojiPos.y += (lineHeight - emoji->height) * 0.5f;
            }
            
            ImVec2 uv0(emoji->u0, emoji->v0);
            ImVec2 uv1(emoji->u1, emoji->v1);
            
            drawList->AddImage(
                g_emojiManager->getTextureID(),
                emojiPos,
                ImVec2(emojiPos.x + emojiSize.x, emojiPos.y + emojiSize.y),
                uv0,
                uv1
            );
            
            cursorX += emoji->advance;
        } else {
            // Regular character - add to buffer
            // Re-encode codepoint to UTF-8
            if (codepoint < 0x80) {
                buffer += static_cast<char>(codepoint);
            } else if (codepoint < 0x800) {
                buffer += static_cast<char>(0xC0 | (codepoint >> 6));
                buffer += static_cast<char>(0x80 | (codepoint & 0x3F));
            } else if (codepoint < 0x10000) {
                buffer += static_cast<char>(0xE0 | (codepoint >> 12));
                buffer += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                buffer += static_cast<char>(0x80 | (codepoint & 0x3F));
            } else {
                buffer += static_cast<char>(0xF0 | (codepoint >> 18));
                buffer += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
                buffer += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                buffer += static_cast<char>(0x80 | (codepoint & 0x3F));
            }
        }
    }
    
    // Render any remaining text
    if (!buffer.empty()) {
        ImVec2 textPos(cursorX, pos.y);
        drawList->AddText(textPos, color, buffer.c_str());
        cursorX += ImGui::CalcTextSize(buffer.c_str()).x;
    }
    
    // Advance cursor
    ImGui::Dummy(ImVec2(cursorX - pos.x, lineHeight));
}

void SmartText(const std::string& text) {
    SmartText(text.c_str());
}

void SmartTextWithEmoji(const char* text, const ImVec2& pos, ImU32 color, EmojiManager* emojiManager) {
    if (!emojiManager || !text) {
        ImGui::GetWindowDrawList()->AddText(pos, color, text);
        return;
    }
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const char* textPtr = text;
    std::string buffer;
    float cursorX = pos.x;
    
    while (*textPtr) {
        uint32_t codepoint = decodeUTF8(textPtr);
        const EmojiGlyph* emoji = emojiManager->getEmoji(codepoint);
        
        if (emoji) {
            if (!buffer.empty()) {
                ImVec2 textPos(cursorX, pos.y);
                drawList->AddText(textPos, color, buffer.c_str());
                cursorX += ImGui::CalcTextSize(buffer.c_str()).x;
                buffer.clear();
            }
            
            ImVec2 emojiPos(cursorX, pos.y);
            ImVec2 emojiSize(emoji->width, emoji->height);
            ImVec2 uv0(emoji->u0, emoji->v0);
            ImVec2 uv1(emoji->u1, emoji->v1);
            
            drawList->AddImage(
                emojiManager->getTextureID(),
                emojiPos,
                ImVec2(emojiPos.x + emojiSize.x, emojiPos.y + emojiSize.y),
                uv0,
                uv1
            );
            
            cursorX += emoji->advance;
        } else {
            // Re-encode to UTF-8
            if (codepoint < 0x80) {
                buffer += static_cast<char>(codepoint);
            } else if (codepoint < 0x800) {
                buffer += static_cast<char>(0xC0 | (codepoint >> 6));
                buffer += static_cast<char>(0x80 | (codepoint & 0x3F));
            } else if (codepoint < 0x10000) {
                buffer += static_cast<char>(0xE0 | (codepoint >> 12));
                buffer += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                buffer += static_cast<char>(0x80 | (codepoint & 0x3F));
            } else {
                buffer += static_cast<char>(0xF0 | (codepoint >> 18));
                buffer += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
                buffer += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                buffer += static_cast<char>(0x80 | (codepoint & 0x3F));
            }
        }
    }
    
    if (!buffer.empty()) {
        ImVec2 textPos(cursorX, pos.y);
        drawList->AddText(textPos, color, buffer.c_str());
    }
}

} // namespace ImBored::UI
