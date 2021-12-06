#pragma once

#include "../../Math/Math.h"
#include <array>

struct GuiDrawListSharedData {
    Vector2 texUvWhitePixel = Vector2::zero;
    std::array<Vector2, 12> arcFastVertex;

    GuiDrawListSharedData() {
        int size = arcFastVertex.size();
        for (int i = 0; i < size; ++i) {
            float a = (i * Math::TwoPI) / static_cast<float>(size);
            arcFastVertex[i] = Vector2(cosf(a), sinf(a));
        }
    }
};
