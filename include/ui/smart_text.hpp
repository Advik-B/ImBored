#pragma once

#include "imgui.h"
#include <string>

namespace ImBored::UI {

class EmojiManager;

// Smart text rendering with inline emoji support
void SmartText(const char* text);
void SmartText(const std::string& text);

// Initialize the smart text system with an emoji manager
void SmartTextInit(EmojiManager* emojiManager);

// Render text with emoji inline
void SmartTextWithEmoji(const char* text, const ImVec2& pos, ImU32 color, EmojiManager* emojiManager);

} // namespace ImBored::UI
