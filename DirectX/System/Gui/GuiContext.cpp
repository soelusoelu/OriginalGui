#include "GuiContext.h"
#include "GuiFontAtlas.h"
#include "GuiWindow.h"

GuiContext::GuiContext()
    : mFontAtlas(std::make_unique<GuiFontAtlas>())
{
}

GuiContext::~GuiContext() = default;

std::shared_ptr<GuiWindow> GuiContext::createWindow(const std::string & name, bool endButton) {
    auto window = std::make_shared<GuiWindow>(name, endButton);
    mWindows.emplace_back(window);
    return window;
}

void GuiContext::render() {
    //GuiWindowのDrawDataをGuiContextのDrawListに追加していく

    //DrawListからDrawDataへの変換
}
