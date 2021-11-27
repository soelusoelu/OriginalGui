#pragma once

#include "GuiFont.h"
#include <memory>

class GuiCursor;

class GuiFontAtlas {
public:
    GuiFontAtlas();
    ~GuiFontAtlas();
    GuiFontAtlas(const GuiFontAtlas&) = delete;
    GuiFontAtlas& operator=(const GuiFontAtlas&) = delete;

    const GuiFont& getFont() const;
    const GuiCursor& getCursor() const;

private:
    int mTexID;
    std::unique_ptr<GuiFont> mFont;
    std::unique_ptr<GuiCursor> mCursor;
};
