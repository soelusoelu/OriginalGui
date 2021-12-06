#include "GuiWindow.h"
#include "DrawCornerFlags.h"
#include "GuiContext.h"
#include "GuiWidgetSlider.h"
#include "../../Collision/Collision.h"
#include "../../Input/Input.h"

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
    , mIsActive(true)
    , mPosition(Vector2(512.f, 256.f)) //デフォルト位置
    , mSize(Vector2(512.f, 512.f)) //デフォルトサイズ
    , mBackgroundColor(Vector4(0.05f, 0.05f, 0.05f, 0.9f)) //デフォルト背景色
    , mBackgroundVerticesStartIndex(0)
    , mBackgroundVerticesNumPoints(0)
    , mTitleBarSizeY(13.f + mContext.getFramePadding().y * 2.f)
    , mTitleBarColor(Vector4(0.15f, 0.3f, 0.5f, 1.f)) //アクティブ時のデフォルトタイトルバー色
    , mInactiveTitleBarColor(Vector4(0.15f, 0.15f, 0.15f, 1.f)) //非アクティブ時のデフォルトタイトルバー色
    , mTitleBarVerticesStartIndex(0)
    , mTitleBarVerticesNumPoints(0)
    , mNextWidgetPosition(Vector2::zero)
    , mIsWindowMoving(false)
{
    auto titleBarSize = Vector2(mSize.x, mTitleBarSizeY);
    //背景
    mBackgroundVerticesStartIndex = static_cast<unsigned>(mDrawList->getVertexBuffer().size());
    mDrawList->addRectFilled(
        mPosition + Vector2(0.f, titleBarSize.y),
        mPosition + mSize,
        mBackgroundColor,
        ROUNDING,
        DrawCornerFlags::BOT
    );
    mBackgroundVerticesNumPoints =
        static_cast<unsigned>(mDrawList->getVertexBuffer().size()) - mBackgroundVerticesStartIndex;

    //タイトルバー
    mTitleBarVerticesStartIndex = static_cast<unsigned>(mDrawList->getVertexBuffer().size());
    mDrawList->addRectFilled(
        mPosition,
        mPosition + titleBarSize,
        mTitleBarColor,
        ROUNDING,
        DrawCornerFlags::TOP
    );
    mTitleBarVerticesNumPoints =
        static_cast<unsigned>(mDrawList->getVertexBuffer().size()) - mTitleBarVerticesStartIndex;

    //ウィンドウの外枠線
    mDrawList->addRect(
        mPosition,
        mPosition + mSize,
        Vector4(1.f, 1.f, 1.f, 1.f),
        ROUNDING,
        DrawCornerFlags::ALL,
        OUTER_BORDRES_THICKNESS
    );

    mDrawList->setLayer(GuiDrawList::COMMAND_WIDGETS_INDEX);

    //ウィジェット表示位置の設定
    mNextWidgetPosition = mPosition + Vector2(0.f, titleBarSize.y) + mContext.getFramePadding();
}

GuiWindow::~GuiWindow() = default;

void GuiWindow::update() {
    const auto& mouse = Input::mouse();
    const auto& mousePos = mouse.getMousePosition();

    if (mouse.getMouseButtonDown(MouseCode::LeftButton)) {
        //クリックした位置によってウィンドウのアクティブ状態を変化させる
        Square sq(mPosition, mPosition + mSize);
        (sq.contains(mousePos)) ? setActive() : setInactive();

        //タイトルバーをクリックしていたら、ウィンドウを移動できる
        sq.max = mPosition + Vector2(mSize.x, mTitleBarSizeY);
        mIsWindowMoving = (sq.contains(mousePos));
    }

    //マウス長押しした状態で、フラグが立っていたら、ウィンドウを動かす
    if (mouse.getMouseButton(MouseCode::LeftButton)) {
        if (mIsWindowMoving) {
            setPosition(mPosition + mouse.getMouseVelocity());
        }
    }

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

void GuiWindow::setBackgroundColor(const Vector4& color) {
    mDrawList->setVertexColor(color, mBackgroundVerticesStartIndex, mBackgroundVerticesNumPoints);
    mBackgroundColor = color;
}

const Vector4& GuiWindow::getBackgroundColor() const {
    return mBackgroundColor;
}

void GuiWindow::setTitleBarColor(const Vector4& color) {
    setTitleBarVertexColor(color);
    mTitleBarColor = color;
}

const Vector4& GuiWindow::getTitleBarColor() const {
    return mTitleBarColor;
}

void GuiWindow::setActive() {
    if (!mIsActive) {
        mIsActive = true;
        setTitleBarVertexColor(mTitleBarColor);
    }
}

void GuiWindow::setInactive() {
    if (mIsActive) {
        mIsActive = false;
        setTitleBarVertexColor(mInactiveTitleBarColor);
    }
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

void GuiWindow::setTitleBarVertexColor(const Vector4& color) {
    mDrawList->setVertexColor(color, mTitleBarVerticesStartIndex, mTitleBarVerticesNumPoints);
}
