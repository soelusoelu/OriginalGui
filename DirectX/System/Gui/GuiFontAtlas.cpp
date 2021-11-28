#include "GuiFontAtlas.h"
#include "GuiCursor.h"
#include "GuiFontCreater.h"
#include "GuiFontInitializer.h"

GuiFontAtlas::GuiFontAtlas()
    : mTexID(-1)
    , mTexGlyphPadding(1)
    , mFont(GuiFontCreater::createDefaultFont())
    , mCursor(std::make_unique<GuiCursor>())
{
    GuiFontInitializer::initialize(*this);
}

GuiFontAtlas::~GuiFontAtlas() = default;

int GuiFontAtlas::getTexGlyphPadding() const {
    return mTexGlyphPadding;
}

GuiFont& GuiFontAtlas::getFont() const {
    return *mFont;
}

const GuiCursor& GuiFontAtlas::getCursor() const {
    return *mCursor;
}
