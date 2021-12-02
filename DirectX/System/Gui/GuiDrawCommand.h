#pragma once

#include "../../Math/Math.h"

struct GuiDrawCommand {
    //矩形のクリッピング
    Vector4 clipRect;
    void* textureID = nullptr;
    unsigned vertexOffset = 0;
    unsigned indexOffset = 0;
    unsigned elemCount = 0;
};
