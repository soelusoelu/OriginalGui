#include "GuiWidgetSlider.h"
#include "GuiWidgetText.h"
#include "../GuiContext.h"
#include "../GuiDrawList.h"
#include "../GuiWindow.h"
#include "../../../Collision/Collision.h"
#include "../../../Device/DrawString.h"
#include "../../../Input/Input.h"
#include "../../../Utility/AsciiHelper.h"
#include "../../../Utility/StringUtil.h"
#include <cassert>

GuiWidgetSlider::GuiWidgetSlider(GuiWindow& window)
    : GuiWidgetFrameBase(window)
    , mText(std::make_unique<GuiWidgetText>(window))
{
}

GuiWidgetSlider::~GuiWidgetSlider() = default;

void GuiWidgetSlider::onUpdateFrame(const GuiFrameInfo& frame) {
    //事前計算
    const auto& mousePos = Input::mouse().getMousePosition();
    auto minX = calcGrabbingPosXMin(frame);
    auto maxX = calcGrabbingPosXMax(frame);
    auto clampMousePosX = Math::clamp(mousePos.x, minX, maxX);

    //フレーム矩形内におけるマウス位置の割合
    float t = (clampMousePosX - minX) / (maxX - minX);

    const auto& slider = mSliders[mFrameIndex];
    //タイプごとの数値処理
    updateNumber(slider, t);
    //文字列更新
    updateNumberText(slider);
    //グラブ移動処理
    updateGrabPosition(slider, frame, t);
}

void GuiWidgetSlider::sliderInt(const std::string& label, int& v, int min, int max) {
    sliderScalar(label, GuiDataType::INT, &v, min, max);

    //現在の値からグラブと変更する
    float t = static_cast<float>(v - min) / static_cast<float>(max - min);
    updateGrabPosition(mSliders.back(), mFrames.back(), t);
}

void GuiWidgetSlider::sliderFloat(const std::string& label, float& v, float min, float max) {
    sliderScalar(label, GuiDataType::FLOAT, &v, min, max);

    //現在の値からグラブと変更する
    float t = (v - min) / (max - min);
    updateGrabPosition(mSliders.back(), mFrames.back(), t);
}

void GuiWidgetSlider::sliderScalar(
    const std::string& label,
    GuiDataType type,
    void* v,
    const std::any& min,
    const std::any& max
) {
    auto& dl = mWindow.getDrawList();
    const auto& framePadding = mWindow.getContext().getFramePadding();

    //フレームの描画
    auto frameIdx = createSingleFrame();

    //グラブの描画
    auto grabStart = dl.getVertexCount();
    auto pos = getFramePosition(frameIdx) + Vector2::one * GRAB_PADDING;
    dl.addRectFilled(pos, pos + GRAB_SIZE, GRAB_COLOR);
    auto grabNumPoints = dl.getVertexCount() - grabStart;

    //ラベルの描画
    auto offsetX = GuiWidgetConstant::FRAME_WIDTH + framePadding.x;
    auto offsetY = GuiWidgetConstant::TEXT_HEIGHT_PADDING;
    mText->text(
        label,
        getFramePosition(frameIdx) + Vector2(offsetX, offsetY),
        GuiWidgetConstant::TEXT_HEIGHT
    );

    //値を文字列で描画
    auto textIdx = mText->text(
        numberToText(v, type),
        getFramePosition(frameIdx) + (GuiWidgetConstant::FRAME_SIZE / 2.f),
        GuiWidgetConstant::TEXT_HEIGHT - 2.f,
        GuiWidgetConstant::DIGITS,
        Vector4(ColorPalette::white, 1.f),
        Pivot::CENTER
    );

    //初期値が範囲を超えてる場合のためにクランプする
    clamp(v, min, max, type);

    //配列に追加
    mSliders.emplace_back(GuiSlider{
        label,
        type,
        v,
        min,
        max,
        grabStart,
        grabNumPoints,
        frameIdx,
        textIdx
    });
}

void GuiWidgetSlider::updateNumber(const GuiSlider& slider, float t) {
    if (slider.type == GuiDataType::INT) {
        auto& v = *static_cast<int*>(slider.data);
        auto min = std::any_cast<int>(slider.min);
        auto max = std::any_cast<int>(slider.max);

        v = Math::lerp(min, max, t);
    } else if (slider.type == GuiDataType::FLOAT) {
        auto& v = *static_cast<float*>(slider.data);
        auto min = std::any_cast<float>(slider.min);
        auto max = std::any_cast<float>(slider.max);

        v = Math::lerp(min, max, t);
    } else {
        assert(false);
    }
}

void GuiWidgetSlider::updateNumberText(const GuiSlider& slider) {
    auto str = numberToText(slider);
    mText->changeText(slider.valueTextIndex, str);
}

void GuiWidgetSlider::updateGrabPosition(const GuiSlider& slider, const GuiFrameInfo& frame, float t) {
    auto minX = calcGrabbingPosXMin(frame);
    auto maxX = calcGrabbingPosXMax(frame);
    float posX = Math::lerp(minX, maxX, t);

    auto& dl = mWindow.getDrawList();
    const auto& vb = dl.getVertexBuffer();
    const auto& prevPos = vb[slider.grabStartIndex].pos;
    //マウス位置から求めた位置と前回の位置の差分から、次のグラブの位置を求める
    //求めた位置から、マウス位置をグラブの中心にするために、グラブの半分戻す
    dl.updateVertexPosition(
        Vector2(posX - prevPos.x - GRAB_WIDTH_HALF, 0.f),
        slider.grabStartIndex,
        slider.grabNumPoints
    );
}

std::string GuiWidgetSlider::numberToText(const GuiSlider& slider) {
    return numberToText(slider.data, slider.type);
}

std::string GuiWidgetSlider::numberToText(const void* data, GuiDataType type) {
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

void GuiWidgetSlider::clamp(void* data, const std::any& min, const std::any& max, GuiDataType type) {
    if (type == GuiDataType::INT) {
        auto& v = *static_cast<int*>(data);
        v = Math::clamp(v, std::any_cast<int>(min), std::any_cast<int>(max));
    } else if (type == GuiDataType::FLOAT) {
        auto& v = *static_cast<float*>(data);
        v = Math::clamp(v, std::any_cast<float>(min), std::any_cast<float>(max));
    } else {
        assert(false);
    }
}

float GuiWidgetSlider::calcGrabbingPosXMin(const GuiFrameInfo& frame) const {
    return getFramePosition(frame).x + (GRAB_WIDTH_HALF + GRAB_PADDING);
}

float GuiWidgetSlider::calcGrabbingPosXMax(const GuiFrameInfo& frame) const {
    return getFramePosition(frame).x + GuiWidgetConstant::FRAME_WIDTH - (GRAB_WIDTH_HALF + GRAB_PADDING);
}
