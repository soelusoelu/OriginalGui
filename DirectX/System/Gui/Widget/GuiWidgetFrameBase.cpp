#include "GuiWidgetFrameBase.h"
#include "GuiWidgetConstant.h"
#include "GuiWidgetText.h"
#include "../GuiContext.h"
#include "../GuiWindow.h"
#include "../../../Collision/Collision.h"
#include "../../../Input/Input.h"
#include "../../../Utility/StringUtil.h"
#include <cassert>

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
        onUpdateSelectFrame(getSelectingFrame());
    }

    //毎フレーム更新
    updateNumberText();
    //子クラス更新
    update();
}

unsigned GuiWidgetFrameBase::createFrame(const std::string& label, int frameCount) {
    const auto& nextPos = mWindow.getNextWidgetPosition();
    auto& dl = mWindow.getDrawList();
    const auto& padding = mWindow.getContext().getFramePadding();

    //1本のフレームの長さ
    auto frameWidth = calcDivFrameLength(frameCount);
    auto frameSize = Vector2(frameWidth, GuiWidgetConstant::FRAME_HEIGHT);

    //フレームの情報を登録
    auto pos = nextPos;
    for (int i = 0; i < frameCount; ++i) {
        //新規フレームを取得
        auto& newFrame = mFrames.emplace_back();

        newFrame.startIndex = dl.getVertexCount();
        dl.addRectFilled(pos, pos + frameSize, GuiWidgetConstant::FRAME_COLOR);
        newFrame.numPoints = dl.getVertexCount() - newFrame.startIndex;

        pos.x += frameWidth;
        pos.x += padding.x;
    }

    //ラベルの情報を登録
    mWindow.getWidgets().getText().label(label, getFramePosition(mFrames[mFrames.size() - frameCount]));

    //次のウィジェットの描画位置を調整
    mWindow.setNextWidgetPosition(
        nextPos + Vector2(0.f, GuiWidgetConstant::FRAME_HEIGHT + mWindow.getContext().getFramePadding().y)
    );

    return (mFrames.size() - frameCount);
}

void GuiWidgetFrameBase::createFrameText(GuiFrameInfo& frame, GuiDataType type, void* v) {
    frame.type = type;
    frame.data = v;

    //値を文字列で描画
    frame.valueTextIndex = mWindow.getWidgets().getText().text(
        numberToText(frame),
        getFramePosition(frame) + (getFrameSize(frame) / 2.f),
        GuiWidgetConstant::TEXT_HEIGHT - 2.f,
        GuiWidgetConstant::DIGITS,
        Vector4(ColorPalette::white, 1.f),
        Pivot::CENTER
    );
}

void GuiWidgetFrameBase::updateNumberText() {
    for (const auto& f : mFrames) {
        auto str = numberToText(f);
        mWindow.getWidgets().getText().changeText(f.valueTextIndex, str);
    }
}

std::string GuiWidgetFrameBase::numberToText(const GuiFrameInfo& frame) {
    std::string str;
    if (frame.type == GuiDataType::INT) {
        auto v = *static_cast<const int*>(frame.data);
        str = StringUtil::intToString(v);
    } else if (frame.type == GuiDataType::FLOAT) {
        auto v = *static_cast<const float*>(frame.data);
        str = StringUtil::floatToString(v);
    } else {
        assert(false);
    }

    return str;
}

const Vector2& GuiWidgetFrameBase::getFramePosition(const GuiFrameInfo& frame) const {
    return mWindow.getDrawList().getVertexBuffer()[frame.startIndex].pos;
}

Vector2 GuiWidgetFrameBase::getFrameSize(const GuiFrameInfo& frame) const {
    const auto& vb = mWindow.getDrawList().getVertexBuffer();
    return (vb[frame.startIndex + 2].pos - vb[frame.startIndex].pos);
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
        Square sq(framePosition, framePosition + getFrameSize(f));
        if (!sq.contains(mousePos)) {
            continue;
        }

        mFrameIndex = i;
        onSelectFrame(f);
        return;
    }
}

float GuiWidgetFrameBase::calcDivFrameLength(int x) const {
    auto padding = mWindow.getContext().getFramePadding().x * (x - 1);
    auto w = GuiWidgetConstant::FRAME_WIDTH - padding;
    auto res = w / static_cast<float>(x);
    return res;
}
