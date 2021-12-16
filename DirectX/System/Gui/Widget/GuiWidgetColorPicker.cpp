#include "GuiWidgetColorPicker.h"
#include "GuiWidgetSlider.h"
#include "GuiWidgetText.h"
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
        data.x = newColor.x;
        data.y = newColor.y;
        data.z = newColor.z;
    } else {
        auto& data = *static_cast<Vector3*>(cp.color);
        data = Vector3(newColor.x, newColor.y, newColor.z);
    }
}

void GuiWidgetColorPicker::colorPicker3(const std::string& label, Vector3& color) {
    colorPicker(label, &color, false);
    mWindow.getWidgets().getSlider().sliderVector3("", color, Vector3::zero, Vector3::one);
}

void GuiWidgetColorPicker::colorPicker4(const std::string& label, Vector4& color) {
    colorPicker(label, &color, true);
    mWindow.getWidgets().getSlider().sliderVector4("", color, Vector4::zero, Vector4::one);
}

void GuiWidgetColorPicker::colorPicker(const std::string& label, void* color, bool isVec4) {
    //カラーピッカーの描画
    createColorPicker(mWindow.getNextWidgetPosition(), color, isVec4);

    //色相バー
    createHueBar();

    //カラーピッカーのカーソル
    createColorPickerCursor();

    //色相バーのカーソル
    createHueBarCursor();

    //配列に追加
    mColorPickers.emplace_back(GuiColorPicker{ color, isVec4 });

    //ラベル
    mWindow.getWidgets().getText().label(label, getColorPickerPosition(mColorsInfo.back()));

    //次のウィジェットの描画位置を設定する
    const auto& prevPos = mWindow.getNextWidgetPosition();
    auto paddingY = mWindow.getContext().getFramePadding().y;
    mWindow.setNextWidgetPosition(prevPos + Vector2(0.f, COLOR_PICKER_HEIGHT + paddingY));
}
