#include "GuiContext.h"
#include "GuiFontAtlas.h"

GuiContext::GuiContext()
    : mFontAtlas(std::make_unique<GuiFontAtlas>())
{
}

GuiContext::~GuiContext() = default;
