#include "GuiWidgets.h"
#include "GuiWidgetSlider.h"

GuiWidgets::GuiWidgets(GuiWindow& window)
    : mSlider(std::make_unique<GuiWidgetSlider>(window))
{
}

GuiWidgets::~GuiWidgets() = default;

void GuiWidgets::update() {
    mSlider->update();
}

GuiWidgetSlider& GuiWidgets::getSlider() const {
    return *mSlider;
}
