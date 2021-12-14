#pragma once

#include <memory>

class GuiWindow;
class GuiWidgetSlider;
class GuiWidgetColorPicker;
class GuiWidgetText;

class GuiWidgets {
public:
    GuiWidgets(GuiWindow& window);
    ~GuiWidgets();
    GuiWidgets(const GuiWidgets&) = delete;
    GuiWidgets& operator=(const GuiWidgets&) = delete;

    void update();
    GuiWidgetSlider& getSlider() const;
    GuiWidgetColorPicker& getColorPicker() const;
    GuiWidgetText& getText() const;

private:
    std::unique_ptr<GuiWidgetSlider> mSlider;
    std::unique_ptr<GuiWidgetColorPicker> mColorPicker;
    std::unique_ptr<GuiWidgetText> mText;
};
