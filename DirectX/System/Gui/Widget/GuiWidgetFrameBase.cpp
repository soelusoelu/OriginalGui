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
    , mText(std::make_unique<GuiWidgetText>(window))
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

    //毎フレーム更新
    updateNumberText();
    //子クラス更新
    update();
}

unsigned GuiWidgetFrameBase::createSingleFrame(const std::string& label) {
    const auto& nextPos = mWindow.getNextWidgetPosition();
    auto& dl = mWindow.getDrawList();

    //新規フレームを取得
    auto& frame = mFrames.emplace_back();
    frame.label = label;

    //フレームの情報を登録
    frame.startIndex = dl.getVertexCount();
    dl.addRectFilled(nextPos, nextPos + GuiWidgetConstant::FRAME_SIZE, GuiWidgetConstant::FRAME_COLOR);
    frame.numPoints = dl.getVertexCount() - frame.startIndex;

    //ラベルの情報を登録
    auto offsetX = GuiWidgetConstant::FRAME_WIDTH + mWindow.getContext().getFramePadding().x;
    auto offsetY = GuiWidgetConstant::TEXT_HEIGHT_PADDING;
    mText->text(
        label,
        getFramePosition(frame) + Vector2(offsetX, offsetY),
        GuiWidgetConstant::TEXT_HEIGHT
    );

    //次のウィジェットの描画位置を調整
    mWindow.setNextWidgetPosition(
        nextPos + Vector2(0.f, GuiWidgetConstant::FRAME_HEIGHT + mWindow.getContext().getFramePadding().y)
    );

    return (mFrames.size() - 1);
}

void GuiWidgetFrameBase::createFrameText(unsigned index, GuiDataType type, void* v) {
    auto& frame = mFrames[index];
    frame.type = type;
    frame.data = v;

    //値を文字列で描画
    frame.valueTextIndex = mText->text(
        numberToText(v, type),
        getFramePosition(index) + (GuiWidgetConstant::FRAME_SIZE / 2.f),
        GuiWidgetConstant::TEXT_HEIGHT - 2.f,
        GuiWidgetConstant::DIGITS,
        Vector4(ColorPalette::white, 1.f),
        Pivot::CENTER
    );
}

void GuiWidgetFrameBase::updateNumberText() {
    for (const auto& f : mFrames) {
        auto str = numberToText(f);
        mText->changeText(f.valueTextIndex, str);
    }
}

std::string GuiWidgetFrameBase::numberToText(const GuiFrameInfo& frame) {
    return numberToText(frame.data, frame.type);
}

std::string GuiWidgetFrameBase::numberToText(const void* data, GuiDataType type) {
    std::string str;
    if (type == GuiDataType::INT) {
        auto v = *static_cast<const int*>(data);
        str = StringUtil::intToString(v);
    } else if (type == GuiDataType::FLOAT) {
        auto v = *static_cast<const float*>(data);
        str = StringUtil::floatToString(v);
    } else {
        assert(false);
    }

    return str;
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
