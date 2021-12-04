#include "GuiContext.h"
#include "Gui.h"
#include "GuiFontAtlas.h"
#include "GuiRenderer.h"
#include "GuiWindow.h"
#include <cassert>

GuiContext::GuiContext()
    //: mFontAtlas(std::make_unique<GuiFontAtlas>())
    : mFontAtlas(nullptr)
    , mGuiRenderer(std::make_unique<GuiRenderer>())
    , mDrawListSharedData()
{
    Gui::mContext = this;
}

GuiContext::~GuiContext() {
    Gui::mContext = nullptr;
}

void GuiContext::draw() {
    mGuiRenderer->draw(*this);
}

void GuiContext::addWindow(const std::shared_ptr<GuiWindow>& window) {
    mWindows.emplace_back(window);
}

const GuiDrawListSharedData& GuiContext::getDrawListSharedData() const {
    return mDrawListSharedData;
}

const GuiWindow& GuiContext::getWindow(unsigned index) const {
    assert(index < getWindowCount());
    return *mWindows[index];
}

unsigned GuiContext::getWindowCount() const {
    return mWindows.size();
}
