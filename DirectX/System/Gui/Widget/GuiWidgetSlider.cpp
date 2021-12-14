#include "GuiWidgetSlider.h"
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

    //現在の値からグラブと数値を変更する
    v = Math::clamp(v, min, max);
    float t = static_cast<float>(v - min) / static_cast<float>(max - min);
    const auto& s = mSliders.back();
    updateGrabPosition(s, t);
    updateNumberText(s);
}

void GuiWidgetSlider::sliderFloat(const std::string& label, float& v, float min, float max) {
    sliderScalar(label, GuiDataType::FLOAT, &v, min, max);

    //現在の値からグラブと数値を変更する
    v = Math::clamp(v, min, max);
    float t = (v - min) / (max - min);
    const auto& s = mSliders.back();
    updateGrabPosition(s, t);
    updateNumberText(s);
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
    dl.addText(
        label,
        nextPos + Vector2(GuiWidgetConstant::FRAME_WIDTH + framePadding.x, GuiWidgetConstant::TEXT_HEIGHT_PADDING),
        GuiWidgetConstant::TEXT_HEIGHT
    );

    //値を文字列で描画
    auto textStart = dl.getVertexCount();
    dl.addText(
        "0",
        nextPos + (GuiWidgetConstant::FRAME_SIZE / 2.f),
        GuiWidgetConstant::TEXT_HEIGHT - 2.f,
        GuiWidgetConstant::DIGITS,
        Vector4(ColorPalette::white, 1.f),
        Pivot::CENTER
    );
    auto textNumPoints = dl.getVertexCount() - textStart;

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
        textStart,
        textNumPoints
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
    //一旦全文字空白にする
    clearTextNumber(slider);

    //タイプごとの処理
    auto str = numberToText(slider);

    auto& dl = mWindow.getDrawList();
    auto start = slider.valueTextStartIndex;
    const auto& frameCenter = getFramePosition(slider) + (GuiWidgetConstant::FRAME_SIZE / 2.f);
    auto size = getNumberTextSize(slider);
    auto pos = StringUtil::calcPositionFromPivot(frameCenter, str, size, Pivot::CENTER);

    int len = static_cast<int>(str.length());
    for (int i = 0; i < len; ++i) {
        auto leftTop = AsciiHelper::calcPositionRateToAscii(
            str[i],
            DrawString::WIDTH_CHAR_COUNT,
            DrawString::HEIGHT_CHAR_COUNT
        );

        //文字更新
        int idx = start + i * 4;
        dl.setVertexPosition(pos, idx);
        dl.setVertexPosition(pos + Vector2(size.x, 0.f), idx + 1);
        dl.setVertexPosition(pos + size, idx + 2);
        dl.setVertexPosition(pos + Vector2(0.f, size.y), idx + 3);

        dl.setVertexUV(leftTop, idx);
        dl.setVertexUV(leftTop + Vector2(DrawString::CHAR_WIDTH_RATE, 0.f), idx + 1);
        dl.setVertexUV(leftTop + DrawString::CHAR_RATE, idx + 2);
        dl.setVertexUV(leftTop + Vector2(0.f, DrawString::CHAR_HEIGHT_RATE), idx + 3);

        pos.x += size.x;
    }
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

void GuiWidgetSlider::clearTextNumber(const GuiSlider& slider) {
    mWindow.getDrawList().setVertexUVs(
        mWindow.getContext().getDrawListSharedData().texUvTransparentPixel,
        slider.valueTextStartIndex,
        slider.valueTextNumPoints
    );
}

std::string GuiWidgetSlider::numberToText(const GuiSlider& slider) {
    std::string str;
    if (slider.type == GuiDataType::INT) {
        auto v = *static_cast<int*>(slider.data);
        str = StringUtil::intToString(v);
    } else if (slider.type == GuiDataType::FLOAT) {
        auto v = *static_cast<float*>(slider.data);
        str = StringUtil::floatToString(v);
    } else {
        assert(false);
    }

    return str;
}

const Vector2& GuiWidgetSlider::getFramePosition(const GuiSlider& slider) const {
    return mWindow.getDrawList().getVertexBuffer()[slider.frameStartIndex].pos;
}

Vector2 GuiWidgetSlider::getNumberTextSize(const GuiSlider& slider) const {
    const auto& vb = mWindow.getDrawList().getVertexBuffer();
    return (vb[slider.valueTextStartIndex + 2].pos - vb[slider.valueTextStartIndex].pos);
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
