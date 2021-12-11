#include "GuiWidgetColorPicker.h"
#include "../GuiContext.h"
#include "../GuiWindow.h"

GuiWidgetColorPicker::GuiWidgetColorPicker(GuiWindow& window)
    : mWindow(window)
{
}

GuiWidgetColorPicker::~GuiWidgetColorPicker() = default;

void GuiWidgetColorPicker::colorPicker3(const std::string& label, Vector3& color) {
    colorPicker(label, &color, false);
}

void GuiWidgetColorPicker::colorPicker4(const std::string& label, Vector4& color) {
    colorPicker(label, &color, true);
}

void GuiWidgetColorPicker::colorPicker(const std::string& label, void* color, bool isVec4) {
    //フレームの描画
    auto nextPos = mWindow.getNextWidgetPosition();
    auto& drawList = mWindow.getDrawList();
    auto frameStart = static_cast<unsigned>(drawList.getVertexBuffer().size());
    //初期は白と赤のピッカー
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
    const auto& framePadding = mWindow.getContext().getFramePadding();

    constexpr int HUE_BAR_COLORS_SIZE = HUE_BAR_COLORS.size();
    constexpr float SY = COLOR_PICKER_HEIGHT / HUE_BAR_COLORS_SIZE;
    const float HUE_BAR_WIDTH = GuiWidgetConstant::FRAME_WIDTH - COLOR_PICKER_WIDTH - framePadding.x;
    auto huePos = nextPos;
    huePos.x += COLOR_PICKER_WIDTH + framePadding.x;
    //全色グラデーションするために6回に分ける
    for (int i = 0; i < HUE_BAR_COLORS_SIZE; ++i) {
        drawList.addRectFilledMultiColor(
            huePos,
            huePos + Vector2(HUE_BAR_WIDTH, SY),
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
    mColorPickers.emplace_back(GuiColorPicker{ label, color, isVec4, frameStart });
}
