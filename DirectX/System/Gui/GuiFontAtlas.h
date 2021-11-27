#pragma once

#include "GuiFont.h"
#include <memory>

class GuiFontAtlas {
public:
    GuiFontAtlas();
    ~GuiFontAtlas();
    GuiFontAtlas(const GuiFontAtlas&) = delete;
    GuiFontAtlas& operator=(const GuiFontAtlas&) = delete;

private:
    int mTexID;
    std::unique_ptr<GuiFont> mFont;
};
