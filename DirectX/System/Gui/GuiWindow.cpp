#include "GuiWindow.h"

GuiWindow::GuiWindow(
    GuiContext& context,
    const std::string& name,
    bool endButton
)
    : mContext(context)
    , mName(name)
    , mDrawList(std::make_unique<GuiDrawList>(context))
    , mEndButton(endButton)
    , mPosition(Vector2(64.f, 64.f)) //デフォルト位置
    , mSize(Vector2(256.f, 512.f)) //デフォルトサイズ
    , mActive(true)
{
    mDrawList->addRectFilled(mPosition, mPosition + mSize);
}

GuiWindow::~GuiWindow() = default;

GuiDrawList& GuiWindow::getDrawList() const {
    return *mDrawList;
}
