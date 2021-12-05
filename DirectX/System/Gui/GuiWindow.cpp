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
    , mSize(Vector2(64.f, 64.f)) //デフォルトサイズ
    , mActive(true)
{
    mDrawList->addRectFilled(mPosition, mPosition + mSize);
}

GuiWindow::~GuiWindow() = default;

void GuiWindow::setPosition(const Vector2& pos) {
    //ウィンドウ位置を今との差分をもとに更新する
    mDrawList->updateWindowPosition(pos - mPosition);
    mPosition = pos;
}

const Vector2& GuiWindow::getPosition() const {
    return mPosition;
}

void GuiWindow::setSize(const Vector2& size) {
    mDrawList->updateWindowSize(size);
    mSize = size;
}

const Vector2& GuiWindow::getSize() const {
    return mSize;
}

GuiDrawList& GuiWindow::getDrawList() const {
    return *mDrawList;
}
