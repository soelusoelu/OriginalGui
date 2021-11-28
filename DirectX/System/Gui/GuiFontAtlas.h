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

    int getTexGlyphPadding() const;
    GuiFont& getFont() const;
    const GuiCursor& getCursor() const;

private:
    int mTexID;
    //テクスチャ内のグリフ間のパディング(ピクセル単位)
    int mTexGlyphPadding;
    std::unique_ptr<GuiFont> mFont;
    std::unique_ptr<GuiCursor> mCursor;
};
