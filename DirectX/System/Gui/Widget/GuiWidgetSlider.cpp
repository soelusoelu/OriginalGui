#include "GuiWidgetSlider.h"
#include "../GuiContext.h"
#include "../GuiDrawList.h"
#include "../GuiWindow.h"
#include "../../../Collision/Collision.h"
#include "../../../Input/Input.h"
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
        updateNumber();
    }
}

void GuiWidgetSlider::sliderInt(const std::string& label, int& v, int min, int max) {
    sliderScalar(label, GuiDataType::INT, &v, min, max);
}

void GuiWidgetSlider::sliderFloat(const std::string& label, float& v, float min, float max) {
    sliderScalar(label, GuiDataType::FLOAT, &v, min, max);
}

void GuiWidgetSlider::sliderScalar(
    const std::string& label,
    GuiDataType type,
    void* v,
    const std::any& min,
    const std::any& max
) {
    //フレームの描画
    auto nextPos = mWindow.getNextWidgetPosition();
    auto& drawList = mWindow.getDrawList();
    auto frameStart = static_cast<unsigned>(drawList.getVertexBuffer().size());
    drawList.addRectFilled(nextPos, nextPos + GuiWidgetConstant::FRAME_SIZE, Vector4(0.15f, 0.25f, 0.35f, 0.8f));

    //グラブ描画前の頂点数を取得しておく
    auto beforeSize = static_cast<unsigned>(drawList.getVertexBuffer().size());

    //グラブの描画
    auto p = nextPos + Vector2::one * GRAB_PADDING;
    drawList.addRectFilled(p, p + GRAB_SIZE, Vector4(0.15f, 0.3f, 0.75f, 0.9f));

    //グラブ描画後の頂点数を取得する
    auto afterSize = static_cast<unsigned>(drawList.getVertexBuffer().size());

    //描画位置をずらして設定
    nextPos.y += GuiWidgetConstant::FRAME_HEIGHT + mWindow.getContext().getFramePadding().y;
    mWindow.setNextWidgetPosition(nextPos);

    //配列に追加
    mSliders.emplace_back(GuiSlider{ label, type, v, min, max, frameStart, beforeSize, afterSize - beforeSize });
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

void GuiWidgetSlider::updateNumber() {
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
    float f = (clampMousePosX - minX) / (maxX - minX);

    //タイプごとの数値処理
    if (s.type == GuiDataType::INT) {
        auto& v = *static_cast<int*>(s.data);
        auto min = std::any_cast<int>(s.min);
        auto max = std::any_cast<int>(s.max);

        v = Math::lerp(min, max, f);
    } else if (s.type == GuiDataType::FLOAT) {
        auto& v = *static_cast<float*>(s.data);
        auto min = std::any_cast<float>(s.min);
        auto max = std::any_cast<float>(s.max);

        v = Math::lerp(min, max, f);
    } else {
        assert(false);
    }

    //グラブ移動処理
    updateGrabPosition(f);
}

void GuiWidgetSlider::updateGrabPosition(float f) {
    if (!isGrabbing()) {
        return;
    }

    auto& s = getGrabbingSlider();

    auto minX = calcGrabbingPosXMin(s);
    auto maxX = calcGrabbingPosXMax(s);
    float posX = Math::lerp(minX, maxX, f);

    auto& drawList = mWindow.getDrawList();
    const auto& vb = drawList.getVertexBuffer();
    const auto& prevPos = vb[s.grabVerticesStartIndex].pos;
    //マウス位置から求めた位置と前回の位置の差分から、次のグラブの位置を求める
    //求めた位置から、マウス位置をグラブの中心にするために、グラブの半分戻す
    drawList.updateVertexPosition(
        Vector2(posX - prevPos.x - GRAB_WIDTH_HALF, 0.f),
        s.grabVerticesStartIndex,
        s.grabVerticesNumPoints
    );
}

const Vector2& GuiWidgetSlider::getFramePosition(const GuiSlider& slider) const {
    return mWindow.getDrawList().getVertexBuffer()[slider.frameVerticesStartIndex].pos;
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
