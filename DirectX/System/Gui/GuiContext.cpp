#include "GuiContext.h"
#include "GuiFontAtlas.h"
#include "GuiRenderer.h"
#include "GuiWindow.h"
#include <cassert>

GuiContext::GuiContext()
    : mFontAtlas(std::make_unique<GuiFontAtlas>())
    , mGuiRenderer(std::make_unique<GuiRenderer>())
    , mDrawListSharedData()
{
}

GuiContext::~GuiContext() = default;

std::shared_ptr<GuiWindow> GuiContext::createWindow(const std::string & name, bool endButton) {
    auto window = std::make_shared<GuiWindow>(*this, name, endButton);
    mWindows.emplace_back(window);
    return window;
}

void GuiContext::draw() {
    mGuiRenderer->draw(*this);
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
