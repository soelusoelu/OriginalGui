#include "GuiWindow.h"

GuiWindow::GuiWindow(const std::string& name, bool endButton)
    : mName(name)
    , mDrawList(std::make_unique<GuiDrawList>())
    , mEndButton(endButton)
    , mPosition(Vector2(64.f, 64.f)) //デフォルト位置
    , mSize(Vector2(720.f, 1080.f)) //デフォルトサイズ
    , mActive(true)
{
}

GuiWindow::~GuiWindow() = default;

GuiDrawList& GuiWindow::getDrawList() const {
    return *mDrawList;
}
