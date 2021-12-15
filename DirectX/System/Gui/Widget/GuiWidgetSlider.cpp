#include "GuiWidgetSlider.h"
#include "../GuiContext.h"
#include "../GuiDrawList.h"
#include "../GuiWindow.h"
#include "../../../Input/Input.h"
#include <cassert>

GuiWidgetSlider::GuiWidgetSlider(GuiWindow& window)
    : GuiWidgetFrameBase(window)
{
}

GuiWidgetSlider::~GuiWidgetSlider() = default;

void GuiWidgetSlider::update() {
    assert(mSliders.size() == mFrames.size());
    for (int i = 0; i < mSliders.size(); ++i) {
        auto& s = mSliders[i];
        clamp(s.data, s.min, s.max, s.type);
        updateGrabPosition(s, mFrames[i]);
    }
}

void GuiWidgetSlider::onUpdateSelectFrame(const GuiFrameInfo& frame) {
    //事前計算
    const auto& mousePos = Input::mouse().getMousePosition();
    auto minX = calcGrabbingPosXMin(frame);
    auto maxX = calcGrabbingPosXMax(frame);
    auto clampMousePosX = Math::clamp(mousePos.x, minX, maxX);

    //フレーム矩形内におけるマウス位置の割合
    float t = (clampMousePosX - minX) / (maxX - minX);

    const auto& s = mSliders[mFrameIndex];
    //タイプごとの数値処理
    updateNumber(s, t);
    //グラブ移動処理
    updateGrabPosition(s, frame, t);
}

void GuiWidgetSlider::sliderInt(
    const std::string& label,
    int& v,
    int min,
    int max
) {
    //フレームの描画
    unsigned frameIdx = createFrame(label);
    sliderScalar(mFrames[frameIdx], GuiDataType::INT, &v, min, max);
}

void GuiWidgetSlider::sliderFloat(
    const std::string& label,
    float& v,
    float min,
    float max
) {
    //フレームの描画
    unsigned frameIdx = createFrame(label);
    sliderScalar(mFrames[frameIdx], GuiDataType::FLOAT, &v, min, max);
}

void GuiWidgetSlider::sliderVector2(
    const std::string& label,
    Vector2& v,
    const Vector2& min,
    const Vector2& max
) {
    //フレームの描画
    unsigned frameIdx = createFrame(label, 2);
    //Vector2はfloat2回分で登録する
    sliderScalar(mFrames[frameIdx], GuiDataType::FLOAT, &v.x, min.x, max.x);
    sliderScalar(mFrames[frameIdx + 1], GuiDataType::FLOAT, &v.y, min.y, max.y);
}

void GuiWidgetSlider::sliderVector3(
    const std::string& label,
    Vector3& v,
    const Vector3& min,
    const Vector3& max
) {
    //フレームの描画
    unsigned frameIdx = createFrame(label, 3);
    //Vector3はfloat3回分で登録する
    sliderScalar(mFrames[frameIdx], GuiDataType::FLOAT, &v.x, min.x, max.x);
    sliderScalar(mFrames[frameIdx + 1], GuiDataType::FLOAT, &v.y, min.y, max.y);
    sliderScalar(mFrames[frameIdx + 2], GuiDataType::FLOAT, &v.z, min.z, max.z);
}

void GuiWidgetSlider::sliderVector4(
    const std::string& label,
    Vector4& v,
    const Vector4& min,
    const Vector4& max
) {
    //フレームの描画
    unsigned frameIdx = createFrame(label, 4);
    //Vector4はfloat4回分で登録する
    sliderScalar(mFrames[frameIdx], GuiDataType::FLOAT, &v.x, min.x, max.x);
    sliderScalar(mFrames[frameIdx + 1], GuiDataType::FLOAT, &v.y, min.y, max.y);
    sliderScalar(mFrames[frameIdx + 2], GuiDataType::FLOAT, &v.z, min.z, max.z);
    sliderScalar(mFrames[frameIdx + 3], GuiDataType::FLOAT, &v.w, min.w, max.w);
}

void GuiWidgetSlider::sliderScalar(
    GuiFrameInfo& frame,
    GuiDataType type,
    void* v,
    const std::any& min,
    const std::any& max
) {
    auto& dl = mWindow.getDrawList();
    const auto& framePadding = mWindow.getContext().getFramePadding();

    //グラブの描画
    auto grabStart = dl.getVertexCount();
    auto pos = getFramePosition(frame) + Vector2::one * GRAB_PADDING;
    dl.addRectFilled(pos, pos + GRAB_SIZE, GRAB_COLOR);
    auto grabNumPoints = dl.getVertexCount() - grabStart;

    //値テキストの描画
    createFrameText(frame, type, v);

    //配列に追加
    mSliders.emplace_back(GuiSlider{ type, v, min, max, grabStart, grabNumPoints });

    //初期値が範囲を超えてる場合のためにクランプする
    clamp(v, min, max, type);
    //初期値をグラブに反映する
    updateGrabPosition(mSliders.back(), frame);
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

void GuiWidgetSlider::updateGrabPosition(const GuiSlider& slider, const GuiFrameInfo& frame) {
    float t = 0.f;
    if (slider.type == GuiDataType::INT) {
        auto& v = *static_cast<int*>(slider.data);
        auto min = std::any_cast<int>(slider.min);
        auto max = std::any_cast<int>(slider.max);
        t = static_cast<float>(v - min) / static_cast<float>(max - min);
    } else if (slider.type == GuiDataType::FLOAT) {
        auto& v = *static_cast<float*>(slider.data);
        auto min = std::any_cast<float>(slider.min);
        auto max = std::any_cast<float>(slider.max);
        t = (v - min) / (max - min);
    } else {
        assert(false);
    }

    updateGrabPosition(slider, frame, t);
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
    return getFramePosition(frame).x + getFrameSize(frame).x - (GRAB_WIDTH_HALF + GRAB_PADDING);
}
