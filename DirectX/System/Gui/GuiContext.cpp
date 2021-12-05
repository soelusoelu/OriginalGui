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
    , mFramePadding(Vector2(8.f, 8.f))
{
    Gui::mContext = this;
}

GuiContext::~GuiContext() {
    Gui::mContext = nullptr;
}

void GuiContext::update() {
    for (auto&& w : mWindows) {
        w->update();
    }
}

void GuiContext::draw() {
    mGuiRenderer->draw(*this);
}

void GuiContext::addWindow(const std::shared_ptr<GuiWindow>& window) {
    mWindows.emplace_back(window);
}

const GuiWindow& GuiContext::getWindow(unsigned index) const {
    assert(index < getWindowCount());
    return *mWindows[index];
}

unsigned GuiContext::getWindowCount() const {
    return mWindows.size();
}

const GuiDrawListSharedData& GuiContext::getDrawListSharedData() const {
    return mDrawListSharedData;
}

void GuiContext::setFramePadding(const Vector2& padding) {
    mFramePadding = padding;
}

const Vector2& GuiContext::getFramePadding() const {
    return mFramePadding;
}
