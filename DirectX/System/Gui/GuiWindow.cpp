#include "GuiWindow.h"
#include "GuiContext.h"
#include "GuiWidgetSlider.h"

GuiWindow::GuiWindow(
    GuiContext& context,
    const std::string& name,
    bool endButton
)
    : mContext(context)
    , mWidgets(*this)
    , mName(name)
    , mDrawList(std::make_unique<GuiDrawList>(context))
    , mEndButton(endButton)
    , mPosition(Vector2(512.f, 256.f)) //デフォルト位置
    , mSize(Vector2(512.f, 512.f)) //デフォルトサイズ
    , mBackgroundColor(Vector4(0.05f, 0.05f, 0.05f, 0.9f)) //デフォルト背景色
    , mTitleBarColor(Vector4(0.15f, 0.3f, 0.5f, 1.f)) //デフォルトタイトルバー色
    , mNextWidgetPosition(Vector2::zero)
{
    auto titleBarSize = Vector2(mSize.x, 13.f + mContext.getFramePadding().y * 2);
    //背景
    mDrawList->addRectFilled(mPosition + Vector2(0.f, titleBarSize.y), mPosition + mSize, mBackgroundColor);
    //タイトルバー
    mDrawList->addRectFilled(mPosition, mPosition + titleBarSize, mTitleBarColor);

    mDrawList->setLayer(GuiDrawList::COMMAND_WIDGETS_INDEX);

    //ウィジェット表示位置の設定
    mNextWidgetPosition = mPosition + Vector2(0.f, titleBarSize.y) + mContext.getFramePadding();
}

GuiWindow::~GuiWindow() = default;

void GuiWindow::update() {
    mWidgets.update();
}

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

void GuiWindow::sliderInt(const std::string& label, int& v, int min, int max) {
    mWidgets.getSlider().sliderInt(label, v, min, max);
}

void GuiWindow::sliderFloat(const std::string& label, float& v, float min, float max) {
    mWidgets.getSlider().sliderFloat(label, v, min, max);
}

GuiContext& GuiWindow::getContext() const {
    return mContext;
}

GuiDrawList& GuiWindow::getDrawList() const {
    return *mDrawList;
}

void GuiWindow::setNextWidgetPosition(const Vector2& pos) {
    mNextWidgetPosition = pos;
}

const Vector2& GuiWindow::getNextWidgetPosition() const {
    return mNextWidgetPosition;
}
