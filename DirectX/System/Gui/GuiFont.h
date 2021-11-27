#pragma once

#include "GuiFontConfig.h"
#include <vector>

struct GuiFont {
    GuiFontConfig config;

    std::vector<float> indexAdvanceX;
    float fallbackAdvanceX = 0.f;
    float fontSize = 0.f;

    std::vector<wchar_t> indexLookup;
};
