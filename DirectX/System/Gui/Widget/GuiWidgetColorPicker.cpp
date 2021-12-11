#include "GuiWidgetColorPicker.h"
#include "../GuiContext.h"
#include "../GuiWindow.h"
#include "../../../Collision/Collision.h"
#include "../../../Input/Input.h"
#include <cassert>

GuiWidgetColorPicker::GuiWidgetColorPicker(GuiWindow& window)
    : mWindow(window)
    , mColorPickerIndex(-1)
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
        mColorPickerIndex = -1;
        mHueBarIndex = -1;
    } else if (mouse.getMouseButtonDown(MouseCode::LeftButton)) {
        selectColorPicker();
        selectHueBar();
    }

    if (mouse.getMouseButton(MouseCode::LeftButton)) {
        updateColorPicker();
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
    auto nextPos = mWindow.getNextWidgetPosition();
    auto& drawList = mWindow.getDrawList();
    const auto& vb = drawList.getVertexBuffer();

    //カラーピッカーの描画
    auto colorPickerStart = static_cast<unsigned>(vb.size());
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

    //色相バー
    auto hueBarStart = static_cast<unsigned>(vb.size());
    const auto& framePadding = mWindow.getContext().getFramePadding();

    constexpr int HUE_BAR_COLORS_SIZE = HUE_BAR_COLORS.size();
    constexpr float SY = HUE_BAR_HEIGHT / HUE_BAR_COLORS_SIZE;
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

    //カーソル
    auto cursorStart = static_cast<unsigned>(vb.size());
    drawList.addCircleFilled(nextPos, CURSOR_RADIUS);
    drawList.addCircle(nextPos, CURSOR_RADIUS + 1.f, Vector4(ColorPalette::black, 1.f));
    auto cursorNumPoints = static_cast<unsigned>(vb.size()) - cursorStart;

    //描画位置をずらして設定
    nextPos.y += GuiWidgetConstant::FRAME_HEIGHT + framePadding.y;
    mWindow.setNextWidgetPosition(nextPos);

    //配列に追加
    mColorPickers.emplace_back(GuiColorPicker{
        label,
        color,
        isVec4,
        hueBarWidth,
        colorPickerStart,
        hueBarStart,
        cursorStart,
        cursorNumPoints
    });
}

void GuiWidgetColorPicker::selectColorPicker() {
    const auto& mousePos = Input::mouse().getMousePosition();
    for (int i = 0; i < mColorPickers.size(); ++i) {
        auto& cp = mColorPickers[i];
        const auto& colorPickerPos = getColorPickerPosition(cp);
        Square sq(colorPickerPos, colorPickerPos + COLOR_PICKER_SIZE);
        if (!sq.contains(mousePos)) {
            continue;
        }

        mColorPickerIndex = i;
        return;
    }
}

void GuiWidgetColorPicker::updateColorPicker() {
    if (!isSelectingColorPicker()) {
        return;
    }

    const auto& cp = getSelectingColorPicker();
    const auto& colorPickerPos = getColorPickerPosition(cp);
    const auto& mousePos = Input::mouse().getMousePosition();
    auto clampMousePos = Vector2::clamp(mousePos, colorPickerPos, colorPickerPos + COLOR_PICKER_SIZE);

    //カーソル位置更新
    auto& drawList = mWindow.getDrawList();
    const auto& vb = drawList.getVertexBuffer();
    auto velocity = clampMousePos - vb[cp.cursorVerticesStartIndex].pos + Vector2(CURSOR_RADIUS, 0.f);
    drawList.updateVertexPosition(velocity, cp.cursorVerticesStartIndex, cp.cursorVerticesNumPoints);

    //カラーピッカー内におけるマウス位置の割合
    float fx = (clampMousePos.x - colorPickerPos.x) / COLOR_PICKER_WIDTH;
    float fy = (clampMousePos.y - colorPickerPos.y) / COLOR_PICKER_HEIGHT;

    //線形補間で色を求める
    //頂点4つの矩形2つ想定
    auto idx = cp.colorPickerVerticesStartIndex;
    const auto& c1_0 = vb[idx].color;     //1つ目左上
    const auto& c1_1 = vb[idx + 1].color; //1つ目右上
    const auto& c1_2 = vb[idx + 2].color; //1つ目右下
    const auto& c1_3 = vb[idx + 3].color; //1つ目左下
    const auto& c2_0 = vb[idx + 4].color; //2つ目左上
    const auto& c2_1 = vb[idx + 5].color; //2つ目右上
    const auto& c2_2 = vb[idx + 6].color; //2つ目右下
    const auto& c2_3 = vb[idx + 7].color; //2つ目左下

    //2つの矩形から色を求める
    auto color = Vector4::lerp(
        Vector4::lerp(c1_0, c1_3, fy) * Vector4::lerp(c2_0, c2_3, fy),
        Vector4::lerp(c1_1, c1_2, fy) * Vector4::lerp(c2_1, c2_2, fy),
        fx
    );

    //値更新
    if (cp.isVec4) {
        auto& data = *static_cast<Vector4*>(cp.color);
        data = color;
    } else {
        auto& data = *static_cast<Vector3*>(cp.color);
        data = Vector3(color.x, color.y, color.z);
    }
}

void GuiWidgetColorPicker::selectHueBar() {
    const auto& mousePos = Input::mouse().getMousePosition();
    for (int i = 0; i < mColorPickers.size(); ++i) {
        auto& cp = mColorPickers[i];
        const auto& hueBarPos = getHueBarPosition(cp);
        Square sq(hueBarPos, hueBarPos + Vector2(cp.hueBarWidth, HUE_BAR_HEIGHT));
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
    auto clampMousePosY = Math::clamp(mousePosY, hueBarPosY, hueBarPosY + HUE_BAR_HEIGHT);

    //色相バー矩形内におけるマウス位置の割合
    float f = (clampMousePosY - hueBarPosY) / HUE_BAR_HEIGHT;

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
    return (isSelectingColorPicker()) ? mColorPickers[mColorPickerIndex] : mColorPickers[mHueBarIndex];
}

bool GuiWidgetColorPicker::isSelectingColorPicker() const {
    return (mColorPickerIndex != -1);
}

bool GuiWidgetColorPicker::isSelectingHueBar() const {
    return (mHueBarIndex != -1);
}
