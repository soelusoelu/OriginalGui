#include "GuiWidgetFrameBase.h"
#include "GuiWidgetConstant.h"
#include "../GuiContext.h"
#include "../GuiWindow.h"
#include "../../../Collision/Collision.h"
#include "../../../Input/Input.h"

GuiWidgetFrameBase::GuiWidgetFrameBase(GuiWindow& window)
    : mWindow(window)
    , mFrameIndex(-1)
{
}

GuiWidgetFrameBase::~GuiWidgetFrameBase() = default;

void GuiWidgetFrameBase::baseUpdate() {
    if (mFrames.empty()) {
        return;
    }

    const auto& mouse = Input::mouse();
    if (mouse.getMouseButtonUp(MouseCode::LeftButton)) {
        mFrameIndex = -1;
    } else if (mouse.getMouseButtonDown(MouseCode::LeftButton)) {
        selectFrame();
    }

    if (isSelectingFrame() && mouse.getMouseButton(MouseCode::LeftButton)) {
        onUpdateFrame(getSelectingFrame());
    }

    update();
}

unsigned GuiWidgetFrameBase::createSingleFrame() {
    const auto& nextPos = mWindow.getNextWidgetPosition();
    auto& dl = mWindow.getDrawList();

    //フレーム情報を登録
    auto frameStart = dl.getVertexCount();
    dl.addRectFilled(nextPos, nextPos + GuiWidgetConstant::FRAME_SIZE, GuiWidgetConstant::FRAME_COLOR);
    auto numPoints = dl.getVertexCount() - frameStart;

    //フレームを登録
    mFrames.emplace_back(GuiFrameInfo{ frameStart, numPoints });

    //次のウィジェットの描画位置を調整
    mWindow.setNextWidgetPosition(
        nextPos + Vector2(0.f, GuiWidgetConstant::FRAME_HEIGHT + mWindow.getContext().getFramePadding().y)
    );

    return (mFrames.size() - 1);
}

const Vector2& GuiWidgetFrameBase::getFramePosition(unsigned index) const {
    return getFramePosition(mFrames[index]);
}

const Vector2& GuiWidgetFrameBase::getFramePosition(const GuiFrameInfo& frame) const {
    return mWindow.getDrawList().getVertexBuffer()[frame.startIndex].pos;
}

const GuiFrameInfo& GuiWidgetFrameBase::getSelectingFrame() const {
    return mFrames[mFrameIndex];
}

bool GuiWidgetFrameBase::isSelectingFrame() const {
    return (mFrameIndex >= 0);
}

void GuiWidgetFrameBase::selectFrame() {
    const auto& mousePos = Input::mouse().getMousePosition();
    int size = static_cast<int>(mFrames.size());
    for (int i = 0; i < size; ++i) {
        //グラブの移動範囲(隙間なども考慮)
        const auto& f = mFrames[i];
        const auto& framePosition = getFramePosition(f);
        Square sq(framePosition, framePosition + GuiWidgetConstant::FRAME_SIZE);
        if (!sq.contains(mousePos)) {
            continue;
        }

        mFrameIndex = i;
        onSelectFrame(f);
        return;
    }
}
