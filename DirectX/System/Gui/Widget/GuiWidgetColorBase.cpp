#include "GuiWidgetColorBase.h"
#include "../GuiContext.h"
#include "../GuiWindow.h"
#include "../../../Collision/Collision.h"
#include "../../../Input/Input.h"
#include <cassert>

GuiWidgetColorBase::GuiWidgetColorBase(GuiWindow& window)
    : mWindow(window)
    , mColorPickerIndex(-1)
    , mHueBarIndex(-1)
{
}

GuiWidgetColorBase::~GuiWidgetColorBase() = default;

void GuiWidgetColorBase::baseUpdate() {
    if (mColorsVertexInfo.empty()) {
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

    //子クラスを更新
    update();
}

void GuiWidgetColorBase::createColorPicker(const Vector2& pos) {
    auto& info = mColorsVertexInfo.emplace_back();
    calcStartIndex(info.colorPickerStartIndex);

    //最初はとりあえず赤
    auto& dl = mWindow.getDrawList();
    dl.addRectFilledMultiColor(
        pos,
        pos + COLOR_PICKER_SIZE,
        Vector4(ColorPalette::white, 1.f),
        Vector4(ColorPalette::red, 1.f),
        Vector4(ColorPalette::red, 1.f),
        Vector4(ColorPalette::white, 1.f)
    );
    //ベース色の上に白から黒への半透明グラデーションをかける
    dl.addRectFilledMultiColor(
        pos,
        pos + COLOR_PICKER_SIZE,
        Vector4(ColorPalette::white, 0.f),
        Vector4(ColorPalette::white, 0.f),
        Vector4(ColorPalette::black, 1.f),
        Vector4(ColorPalette::black, 1.f)
    );
}

void GuiWidgetColorBase::createColorPickerCursor() {
    auto& info = mColorsVertexInfo.back();
    calcStartIndex(info.colorPickerCursorStartIndex);

    auto& dl = mWindow.getDrawList();

    //add...関数を呼ぶたびに頂点バッファのポインタがずれるからgetColorPickerPositionは毎回呼ぶ
    //円のカーソル
    dl.addCircleFilled(getColorPickerPosition(info), COLOR_PICKER_CURSOR_RADIUS);
    //カーソルの枠線
    dl.addCircle(getColorPickerPosition(info), COLOR_PICKER_CURSOR_RADIUS + 1.f, Vector4(ColorPalette::black, 1.f));

    calcNumPoints(info.colorPickerCursorNumPoints, info.colorPickerCursorStartIndex);
}

void GuiWidgetColorBase::createHueBar() {
    auto& info = mColorsVertexInfo.back();
    calcStartIndex(info.hueBarStartIndex);

    constexpr float ONE_SIZE_Y = HUE_BAR_HEIGHT / HUE_BAR_COLORS_SIZE;
    auto framePaddingX = mWindow.getContext().getFramePadding().x;
    auto& dl = mWindow.getDrawList();

    auto width = GuiWidgetConstant::FRAME_WIDTH - COLOR_PICKER_WIDTH - framePaddingX;
    auto pos = getColorPickerPosition(info) + Vector2(COLOR_PICKER_WIDTH + framePaddingX, 0.f);
    //全色グラデーションするために6回に分ける
    for (int i = 0; i < HUE_BAR_COLORS_SIZE; ++i) {
        dl.addRectFilledMultiColor(
            pos,
            pos + Vector2(width, ONE_SIZE_Y),
            Vector4(HUE_BAR_COLORS[i], 1.f),
            Vector4(HUE_BAR_COLORS[i], 1.f),
            Vector4(HUE_BAR_COLORS[(i + 1) % HUE_BAR_COLORS_SIZE], 1.f),
            Vector4(HUE_BAR_COLORS[(i + 1) % HUE_BAR_COLORS_SIZE], 1.f)
        );

        //描画位置をずらす
        pos.y += ONE_SIZE_Y;
    }
}

void GuiWidgetColorBase::createHueBarCursor() {
    auto& info = mColorsVertexInfo.back();
    calcStartIndex(info.hueBarCursorStartIndex);

    auto& dl = mWindow.getDrawList();
    //左カーソル
    auto pos = getHueBarPosition(info);
    auto p1 = pos + Vector2(HUE_BAR_CURSOR_HALF_SIZE, 0.f);
    auto p2 = pos + Vector2(-HUE_BAR_CURSOR_HALF_SIZE, HUE_BAR_CURSOR_HALF_SIZE);
    auto p3 = pos + Vector2(-HUE_BAR_CURSOR_HALF_SIZE, -HUE_BAR_CURSOR_HALF_SIZE);
    dl.addTriangleFilled(p1, p2, p3);
    dl.addTriangle(p1, p2, p3, Vector4(ColorPalette::black, 1.f));

    //右カーソル
    pos.x += getHueBarWidth(info);
    p1 = pos + Vector2(-HUE_BAR_CURSOR_HALF_SIZE, 0.f);
    p2 = pos + Vector2(HUE_BAR_CURSOR_HALF_SIZE, -HUE_BAR_CURSOR_HALF_SIZE);
    p3 = pos + Vector2(HUE_BAR_CURSOR_HALF_SIZE, HUE_BAR_CURSOR_HALF_SIZE);
    dl.addTriangleFilled(p1, p2, p3);
    dl.addTriangle(p1, p2, p3, Vector4(ColorPalette::black, 1.f));

    calcNumPoints(info.hueBarCursorNumPoints, info.hueBarCursorStartIndex);
}

const GuiColorBaseVertexInfo& GuiWidgetColorBase::getSelectingVertexInfo() const {
    return mColorsVertexInfo[getSelectingIndex()];
}

int GuiWidgetColorBase::getSelectingIndex() const {
    return isSelectingColorPicker() ? mColorPickerIndex : mHueBarIndex;
}

bool GuiWidgetColorBase::isSelectingColorPicker() const {
    return (mColorPickerIndex != -1);
}

bool GuiWidgetColorBase::isSelectingHueBar() const {
    return (mHueBarIndex != -1);
}

const GuiColorBaseVertexInfo& GuiWidgetColorBase::getVertexInfo(unsigned index) const {
    assert(index < mColorsVertexInfo.size());
    return mColorsVertexInfo[index];
}

const Vector2& GuiWidgetColorBase::getColorPickerPosition(const GuiColorBaseVertexInfo& info) const {
    return mWindow.getDrawList().getVertexBuffer()[info.colorPickerStartIndex].pos;
}

const Vector2& GuiWidgetColorBase::getColorPickerCursorPosition(const GuiColorBaseVertexInfo& info) const {
    return mWindow.getDrawList().getVertexBuffer()[info.colorPickerCursorStartIndex].pos;
}

const Vector2& GuiWidgetColorBase::getHueBarPosition(const GuiColorBaseVertexInfo& info) const {
    return mWindow.getDrawList().getVertexBuffer()[info.hueBarStartIndex].pos;
}

float GuiWidgetColorBase::getHueBarWidth(const GuiColorBaseVertexInfo& info) const {
    const auto& vb = mWindow.getDrawList().getVertexBuffer();
    auto idx = info.hueBarStartIndex;
    return (vb[idx + 1].pos.x - vb[idx].pos.x);
}

void GuiWidgetColorBase::selectColorPicker() {
    const auto& mousePos = Input::mouse().getMousePosition();
    for (int i = 0; i < mColorsVertexInfo.size(); ++i) {
        auto& info = mColorsVertexInfo[i];
        const auto& pos = getColorPickerPosition(info);
        Square sq(pos, pos + COLOR_PICKER_SIZE);
        if (!sq.contains(mousePos)) {
            continue;
        }

        mColorPickerIndex = i;

        //子呼び出し
        onSelectColorPicker();

        return;
    }
}

void GuiWidgetColorBase::updateColorPicker() {
    if (!isSelectingColorPicker()) {
        return;
    }

    const auto& info = getSelectingVertexInfo();
    const auto& colorPickerPos = getColorPickerPosition(info);
    const auto& mousePos = Input::mouse().getMousePosition();
    auto clampMousePos = Vector2::clamp(mousePos, colorPickerPos, colorPickerPos + COLOR_PICKER_SIZE);

    //カーソル位置更新
    auto& dl = mWindow.getDrawList();
    auto velocity = clampMousePos - getColorPickerCursorPosition(info) + Vector2(COLOR_PICKER_CURSOR_RADIUS, 0.f);
    dl.updateVertexPosition(velocity, info.colorPickerCursorStartIndex, info.colorPickerCursorNumPoints);

    //カラーピッカー内におけるマウス位置の割合
    float fx = (clampMousePos.x - colorPickerPos.x) / COLOR_PICKER_WIDTH;
    float fy = (clampMousePos.y - colorPickerPos.y) / COLOR_PICKER_HEIGHT;

    //線形補間で色を求める
    //頂点4つの矩形2つ想定
    const auto& vb = dl.getVertexBuffer();
    auto idx = info.colorPickerStartIndex;
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

    //子呼び出し
    onUpdateColorPicker(color);
}

void GuiWidgetColorBase::selectHueBar() {
    const auto& mousePos = Input::mouse().getMousePosition();
    for (int i = 0; i < mColorsVertexInfo.size(); ++i) {
        auto& info = mColorsVertexInfo[i];
        const auto& pos = getHueBarPosition(info);
        Square sq(pos, pos + Vector2(getHueBarWidth(info), HUE_BAR_HEIGHT));
        if (!sq.contains(mousePos)) {
            continue;
        }

        mHueBarIndex = i;

        //子呼び出し
        onSelectHueBar();

        return;
    }
}

void GuiWidgetColorBase::updateHueBar() {
    if (!isSelectingHueBar()) {
        return;
    }

    const auto& info = getSelectingVertexInfo();
    const auto& hueBarPosY = getHueBarPosition(info).y;
    auto mousePosY = Input::mouse().getMousePosition().y;
    auto clampMousePosY = Math::clamp(mousePosY, hueBarPosY, hueBarPosY + HUE_BAR_HEIGHT);

    //色相バー矩形内におけるマウス位置の割合
    float f = (clampMousePosY - hueBarPosY) / HUE_BAR_HEIGHT;

    //マウス位置の割合を6つの色相(矩形6つで構成されているから)に対応できるように変更
    constexpr float RATIO = 1.f / HUE_BAR_COLORS_SIZE;
    int idx = static_cast<int>(f / RATIO);
    f -= RATIO * idx;
    f *= HUE_BAR_COLORS_SIZE;

    //線形補間で色を求める
    auto color = Vector3::lerp(
        HUE_BAR_COLORS[idx % HUE_BAR_COLORS_SIZE],
        HUE_BAR_COLORS[(idx + 1) % HUE_BAR_COLORS_SIZE],
        f
    );

    //カラーピッカーの色変更
    //頂点4つの矩形想定
    auto& dl = mWindow.getDrawList();
    dl.setVertexColor(Vector4(color, 1.f), info.colorPickerStartIndex + 1);
    dl.setVertexColor(Vector4(color, 1.f), info.colorPickerStartIndex + 2);

    //色相バーのカーソル位置を変更する
    auto prevPos = dl.getVertexBuffer()[info.hueBarCursorStartIndex].pos.y;
    dl.updateVertexPosition(
        Vector2(0.f, clampMousePosY - prevPos),
        info.hueBarCursorStartIndex,
        info.hueBarCursorNumPoints
    );

    //子呼び出し
    onUpdateHueBar(color);
}

void GuiWidgetColorBase::calcStartIndex(unsigned& startIndex) const {
    startIndex = mWindow.getDrawList().getVertexCount();
}

void GuiWidgetColorBase::calcNumPoints(unsigned& numPoints, unsigned startIndex) const {
    numPoints = mWindow.getDrawList().getVertexCount() - startIndex;
}
