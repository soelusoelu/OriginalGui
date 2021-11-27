#include "GuiFontAtlas.h"
#include "GuiFontCreater.h"

GuiFontAtlas::GuiFontAtlas()
    : mTexID(-1)
    , mFont(GuiFontCreater::createDefaultFont())
{
}

GuiFontAtlas::~GuiFontAtlas() = default;
