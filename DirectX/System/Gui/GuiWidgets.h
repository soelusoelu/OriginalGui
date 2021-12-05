#pragma once

#include <memory>

class GuiWindow;
class GuiWidgetSlider;

class GuiWidgets {
public:
    GuiWidgets(GuiWindow& window);
    ~GuiWidgets();
    GuiWidgets(const GuiWidgets&) = delete;
    GuiWidgets& operator=(const GuiWidgets&) = delete;

    void update();
    GuiWidgetSlider& getSlider() const;

private:
    std::unique_ptr<GuiWidgetSlider> mSlider;
};
