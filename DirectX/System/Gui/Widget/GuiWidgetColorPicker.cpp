#include "GuiWidgetColorPicker.h"
#include "../GuiContext.h"
#include "../GuiWindow.h"
#include "../../../Collision/Collision.h"
#include "../../../Input/Input.h"
#include <cassert>

GuiWidgetColorPicker::GuiWidgetColorPicker(GuiWindow& window)
    : mWindow(window)
    , mHueBarIndex(-1)
{
}

GuiWidgetColorPicker::~GuiWidgetColorPicker() = default;

void GuiWidgetColorPicker::update() {
    if (mColorPickers.empty()) {
        return;
    }

    const auto& mouse = Input::mouse();
    if (mouse.getMouseButtonUp(MouseCode::LeftButton)) {
        mHueBarIndex = -1;
    } else if (mouse.getMouseButtonDown(MouseCode::LeftButton)) {
        selectHueBar();
    }

    if (mouse.getMouseButton(MouseCode::LeftButton)) {
        updateHueBar();
    }
}

void GuiWidgetColorPicker::colorPicker3(const std::string& label, Vector3& color) {
    colorPicker(label, &color, false);
}

void GuiWidgetColorPicker::colorPicker4(const std::string& label, Vector4& color) {
    colorPicker(label, &color, true);
}

void GuiWidgetColorPicker::colorPicker(const std::string& label, void* color, bool isVec4) {
    //カラーピッカーの描画
    auto nextPos = mWindow.getNextWidgetPosition();
    auto& drawList = mWindow.getDrawList();
    auto colorPickerStart = static_cast<unsigned>(drawList.getVertexBuffer().size());
    //最初は赤
    drawList.addRectFilledMultiColor(
        nextPos,
        nextPos + COLOR_PICKER_SIZE,
        Vector4(ColorPalette::white, 1.f),
        Vector4(ColorPalette::red, 1.f),
        Vector4(ColorPalette::red, 1.f),
        Vector4(ColorPalette::white, 1.f)
    );
    //ベース色の上に白から黒への半透明グラデーションをかける
    drawList.addRectFilledMultiColor(
        nextPos,
        nextPos + COLOR_PICKER_SIZE,
        Vector4(ColorPalette::white, 0.f),
        Vector4(ColorPalette::white, 0.f),
        Vector4(ColorPalette::black, 1.f),
        Vector4(ColorPalette::black, 1.f)
    );


    //色彩バー
    auto hueBarStart = static_cast<unsigned>(drawList.getVertexBuffer().size());
    const auto& framePadding = mWindow.getContext().getFramePadding();

    constexpr int HUE_BAR_COLORS_SIZE = HUE_BAR_COLORS.size();
    constexpr float SY = COLOR_PICKER_HEIGHT / HUE_BAR_COLORS_SIZE;
    auto hueBarWidth = GuiWidgetConstant::FRAME_WIDTH - COLOR_PICKER_WIDTH - framePadding.x;
    auto huePos = nextPos;
    huePos.x += COLOR_PICKER_WIDTH + framePadding.x;
    //全色グラデーションするために6回に分ける
    for (int i = 0; i < HUE_BAR_COLORS_SIZE; ++i) {
        drawList.addRectFilledMultiColor(
            huePos,
            huePos + Vector2(hueBarWidth, SY),
            Vector4(HUE_BAR_COLORS[i], 1.f),
            Vector4(HUE_BAR_COLORS[i], 1.f),
            Vector4(HUE_BAR_COLORS[(i + 1) % HUE_BAR_COLORS_SIZE], 1.f),
            Vector4(HUE_BAR_COLORS[(i + 1) % HUE_BAR_COLORS_SIZE], 1.f)
        );

        huePos.y += SY;
    }

    //描画位置をずらして設定
    nextPos.y += GuiWidgetConstant::FRAME_HEIGHT + framePadding.y;
    mWindow.setNextWidgetPosition(nextPos);

    //配列に追加
    mColorPickers.emplace_back(GuiColorPicker{ label, color, isVec4, hueBarWidth, colorPickerStart, hueBarStart });
}

void GuiWidgetColorPicker::selectHueBar() {
    const auto& mousePos = Input::mouse().getMousePosition();
    for (int i = 0; i < mColorPickers.size(); ++i) {
        auto& cp = mColorPickers[i];
        const auto& hueBarPos = getHueBarPosition(cp);
        Square sq(hueBarPos, hueBarPos + Vector2(cp.hueBarWidth, COLOR_PICKER_HEIGHT));
        if (!sq.contains(mousePos)) {
            continue;
        }

        mHueBarIndex = i;
        return;
    }
}

void GuiWidgetColorPicker::updateHueBar() {
    if (!isSelectingHueBar()) {
        return;
    }

    const auto& cp = getSelectingColorPicker();
    const auto& hueBarPosY = getHueBarPosition(cp).y;
    auto mousePosY = Input::mouse().getMousePosition().y;
    auto clampMousePosY = Math::clamp(mousePosY, hueBarPosY, hueBarPosY + COLOR_PICKER_HEIGHT);

    //色彩バー矩形内におけるマウス位置の割合
    float f = (clampMousePosY - hueBarPosY) / COLOR_PICKER_HEIGHT;

    //マウス位置の割合を6つの色相(矩形6つで構成されているから)に対応できるように変更
    constexpr float RATIO = 1.f / HUE_BAR_COLORS.size();
    int idx = static_cast<int>(f / RATIO);
    f -= RATIO * idx;
    f *= HUE_BAR_COLORS.size();

    //線形補間で色を求める
    auto selectColor = Vector3::lerp(
        HUE_BAR_COLORS[idx % HUE_BAR_COLORS.size()],
        HUE_BAR_COLORS[(idx + 1) % HUE_BAR_COLORS.size()],
        f
    );

    //色変更
    //頂点4つの矩形想定
    auto& drawList = mWindow.getDrawList();
    drawList.setVertexColor(Vector4(selectColor, 1.f), cp.colorPickerVerticesStartIndex + 1);
    drawList.setVertexColor(Vector4(selectColor, 1.f), cp.colorPickerVerticesStartIndex + 2);
}

const Vector2& GuiWidgetColorPicker::getColorPickerPosition(const GuiColorPicker& colorPicker) const {
    return mWindow.getDrawList().getVertexBuffer()[colorPicker.colorPickerVerticesStartIndex].pos;
}

const Vector2& GuiWidgetColorPicker::getHueBarPosition(const GuiColorPicker& colorPicker) const {
    return mWindow.getDrawList().getVertexBuffer()[colorPicker.hueBarVerticesStartIndex].pos;
}

const GuiColorPicker& GuiWidgetColorPicker::getSelectingColorPicker() const {
    assert(isSelectingHueBar());
    return mColorPickers[mHueBarIndex];
}

bool GuiWidgetColorPicker::isSelectingHueBar() const {
    return (mHueBarIndex != -1);
}
