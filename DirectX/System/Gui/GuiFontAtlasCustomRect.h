#pragma once

#include "../../Math/Math.h"

struct GuiFont;

struct GuiFontAtlasCustomRect {
    unsigned short width = 0;
    unsigned short height = 0;
    unsigned short x = 0xFFFF;
    unsigned short y = 0xFFFF;
    unsigned glyphID = 0;
    float glyphAdvanceX = 0.f;
    Vector2 glyphOffset;
    GuiFont* font = nullptr;

    bool isPacked() const {
        return x != 0xFFFF;
    }
};
