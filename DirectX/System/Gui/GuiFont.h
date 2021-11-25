#pragma once

#include <vector>

struct GuiFont {
    std::vector<float> indexAdvanceX;
    float fallbackAdvanceX;
    float fontSize;

    std::vector<wchar_t> indexLookup;
};
