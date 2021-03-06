#include "GuiWidgets.h"
#include "GuiWidgetColorPicker.h"
#include "GuiWidgetSlider.h"
#include "GuiWidgetText.h"

GuiWidgets::GuiWidgets(GuiWindow& window)
    : mSlider(std::make_unique<GuiWidgetSlider>(window))
    , mColorPicker(std::make_unique<GuiWidgetColorPicker>(window))
    , mText(std::make_unique<GuiWidgetText>(window))
{
}

GuiWidgets::~GuiWidgets() = default;

void GuiWidgets::update() {
    mSlider->baseUpdate();
    mColorPicker->baseUpdate();
    mText->update();
}

GuiWidgetSlider& GuiWidgets::getSlider() const {
    return *mSlider;
}

GuiWidgetColorPicker& GuiWidgets::getColorPicker() const {
    return *mColorPicker;
}

GuiWidgetText& GuiWidgets::getText() const {
    return *mText;
}
