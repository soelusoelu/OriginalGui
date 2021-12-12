#include "GuiWidgetColorPicker.h"
#include "../GuiContext.h"
#include "../GuiWindow.h"

GuiWidgetColorPicker::GuiWidgetColorPicker(GuiWindow& window)
    : GuiWidgetColorBase(window)
{
}

GuiWidgetColorPicker::~GuiWidgetColorPicker() = default;

void GuiWidgetColorPicker::update() {
    if (mColorPickers.empty()) {
        return;
    }


}

void GuiWidgetColorPicker::onUpdateColorPicker(const Vector4& newColor) {
    auto& cp = mColorPickers[getSelectingIndex()];
    if (cp.isVec4) {
        auto& data = *static_cast<Vector4*>(cp.color);
        data = newColor;
    } else {
        auto& data = *static_cast<Vector3*>(cp.color);
        data = Vector3(newColor.x, newColor.y, newColor.z);
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
    createColorPicker(mWindow.getNextWidgetPosition());

    //カラーピッカーのカーソル
    createColorPickerCursor();

    //色相バー
    createHueBar();

    //色相バーのカーソル
    createHueBarCursor();

    //配列に追加
    mColorPickers.emplace_back(GuiColorPicker{ label, color, isVec4 });

    //次のウィジェットの描画位置を設定する
    const auto& prevPos = mWindow.getNextWidgetPosition();
    auto paddingY = mWindow.getContext().getFramePadding().y;
    mWindow.setNextWidgetPosition(prevPos + Vector2(0.f, COLOR_PICKER_HEIGHT + paddingY));
}
