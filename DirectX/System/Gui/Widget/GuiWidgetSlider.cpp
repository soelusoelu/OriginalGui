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
    : mWindow(window)
    , mText(std::make_unique<GuiWidgetText>(window))
    , mGrabbingIndex(-1)
{
}

GuiWidgetSlider::~GuiWidgetSlider() = default;

void GuiWidgetSlider::update() {
    if (mSliders.empty()) {
        return;
    }

    const auto& mouse = Input::mouse();
    if (mouse.getMouseButtonUp(MouseCode::LeftButton)) {
        mGrabbingIndex = -1;
    } else if (mouse.getMouseButtonDown(MouseCode::LeftButton)) {
        selectSlider();
    }

    if (mouse.getMouseButton(MouseCode::LeftButton)) {
        updateSlider();
    }
}

void GuiWidgetSlider::sliderInt(const std::string& label, int& v, int min, int max) {
    sliderScalar(label, GuiDataType::INT, &v, min, max);

    //現在の値からグラブと変更する
    float t = static_cast<float>(v - min) / static_cast<float>(max - min);
    const auto& s = mSliders.back();
    updateGrabPosition(s, t);
}

void GuiWidgetSlider::sliderFloat(const std::string& label, float& v, float min, float max) {
    sliderScalar(label, GuiDataType::FLOAT, &v, min, max);

    //現在の値からグラブと変更する
    float t = (v - min) / (max - min);
    const auto& s = mSliders.back();
    updateGrabPosition(s, t);
}

void GuiWidgetSlider::sliderScalar(
    const std::string& label,
    GuiDataType type,
    void* v,
    const std::any& min,
    const std::any& max
) {
    auto nextPos = mWindow.getNextWidgetPosition();
    auto& dl = mWindow.getDrawList();
    const auto& framePadding = mWindow.getContext().getFramePadding();

    //フレームの描画
    auto frameStart = dl.getVertexCount();
    dl.addRectFilled(nextPos, nextPos + GuiWidgetConstant::FRAME_SIZE, GuiWidgetConstant::FRAME_COLOR);

    //グラブの描画
    auto grabStart = dl.getVertexCount();
    auto p = nextPos + Vector2::one * GRAB_PADDING;
    dl.addRectFilled(p, p + GRAB_SIZE, GRAB_COLOR);
    auto grabNumPoints = dl.getVertexCount() - grabStart;

    //ラベルの描画
    mText->text(
        label,
        nextPos + Vector2(GuiWidgetConstant::FRAME_WIDTH + framePadding.x, GuiWidgetConstant::TEXT_HEIGHT_PADDING),
        GuiWidgetConstant::TEXT_HEIGHT
    );

    //値を文字列で描画
    auto textIdx = mText->text(
        numberToText(v, type),
        nextPos + (GuiWidgetConstant::FRAME_SIZE / 2.f),
        GuiWidgetConstant::TEXT_HEIGHT - 2.f,
        GuiWidgetConstant::DIGITS,
        Vector4(ColorPalette::white, 1.f),
        Pivot::CENTER
    );

    //初期値が範囲を超えてる場合のためにクランプする
    clamp(v, min, max, type);

    //描画位置をずらして設定
    nextPos.y += GuiWidgetConstant::FRAME_HEIGHT + framePadding.y;
    mWindow.setNextWidgetPosition(nextPos);

    //配列に追加
    mSliders.emplace_back(GuiSlider{
        label,
        type,
        v,
        min,
        max,
        frameStart,
        grabStart,
        grabNumPoints,
        textIdx
    });
}

void GuiWidgetSlider::selectSlider() {
    const auto& mousePos = Input::mouse().getMousePosition();
    for (int i = 0; i < mSliders.size(); ++i) {
        //グラブの移動範囲(隙間なども考慮)
        auto& s = mSliders[i];
        const auto& framePosition = getFramePosition(s);
        auto sqMin = Vector2(calcGrabbingPosXMin(s), framePosition.y);
        auto sqMax = Vector2(calcGrabbingPosXMax(s), framePosition.y + GuiWidgetConstant::FRAME_HEIGHT);
        Square sq(sqMin, sqMax);
        if (!sq.contains(mousePos)) {
            continue;
        }

        mGrabbingIndex = i;
        return;
    }
}

void GuiWidgetSlider::updateSlider() {
    if (!isGrabbing()) {
        return;
    }

    //事前計算
    auto& s = getGrabbingSlider();
    auto minX = calcGrabbingPosXMin(s);
    auto maxX = calcGrabbingPosXMax(s);
    const auto& mousePos = Input::mouse().getMousePosition();
    auto clampMousePosX = Math::clamp(mousePos.x, minX, maxX);

    //フレーム矩形内におけるマウス位置の割合
    float t = (clampMousePosX - minX) / (maxX - minX);

    //タイプごとの数値処理
    updateNumber(s, t);
    //文字列更新
    updateNumberText(s);
    //グラブ移動処理
    updateGrabPosition(s, t);
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

void GuiWidgetSlider::updateGrabPosition(const GuiSlider& slider, float t) {
    auto minX = calcGrabbingPosXMin(slider);
    auto maxX = calcGrabbingPosXMax(slider);
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

const Vector2& GuiWidgetSlider::getFramePosition(const GuiSlider& slider) const {
    return mWindow.getDrawList().getVertexBuffer()[slider.frameStartIndex].pos;
}

const GuiSlider& GuiWidgetSlider::getGrabbingSlider() const {
    assert(isGrabbing());
    return mSliders[mGrabbingIndex];
}

bool GuiWidgetSlider::isGrabbing() const {
    return (mGrabbingIndex >= 0);
}

float GuiWidgetSlider::calcGrabbingPosXMin(const GuiSlider& slider) const {
    return getFramePosition(slider).x + (GRAB_WIDTH_HALF + GRAB_PADDING);
}

float GuiWidgetSlider::calcGrabbingPosXMax(const GuiSlider& slider) const {
    return getFramePosition(slider).x + GuiWidgetConstant::FRAME_WIDTH - (GRAB_WIDTH_HALF + GRAB_PADDING);
}
