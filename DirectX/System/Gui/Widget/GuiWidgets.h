#pragma once

#include <memory>

class GuiWindow;
class GuiWidgetSlider;
class GuiWidgetColorPicker;

class GuiWidgets {
public:
    GuiWidgets(GuiWindow& window);
    ~GuiWidgets();
    GuiWidgets(const GuiWidgets&) = delete;
    GuiWidgets& operator=(const GuiWidgets&) = delete;

    void update();
    GuiWidgetSlider& getSlider() const;
    GuiWidgetColorPicker& getColorPicker() const;

private:
    std::unique_ptr<GuiWidgetSlider> mSlider;
    std::unique_ptr<GuiWidgetColorPicker> mColorPicker;
};
