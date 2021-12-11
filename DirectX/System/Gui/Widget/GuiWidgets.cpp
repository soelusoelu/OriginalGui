#include "GuiWidgets.h"
#include "GuiWidgetColorPicker.h"
#include "GuiWidgetSlider.h"

GuiWidgets::GuiWidgets(GuiWindow& window)
    : mSlider(std::make_unique<GuiWidgetSlider>(window))
    , mColorPicker(std::make_unique<GuiWidgetColorPicker>(window))
{
}

GuiWidgets::~GuiWidgets() = default;

void GuiWidgets::update() {
    mSlider->update();
    mColorPicker->update();
}

GuiWidgetSlider& GuiWidgets::getSlider() const {
    return *mSlider;
}

GuiWidgetColorPicker& GuiWidgets::getColorPicker() const {
    return *mColorPicker;
}
