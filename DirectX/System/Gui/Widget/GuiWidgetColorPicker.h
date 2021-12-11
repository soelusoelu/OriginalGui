#pragma once

#include "GuiDataType.h"
#include "GuiWidgetConstant.h"
#include "../../../Math/Math.h"
#include <array>
#include <string>
#include <vector>

class GuiWindow;

struct GuiColorPicker {
    std::string label = "";
    void* color = nullptr;
    bool isVec4 = true;
    unsigned frameVerticesStartIndex = 0;
};

class GuiWidgetColorPicker {
public:
    GuiWidgetColorPicker(GuiWindow& window);
    ~GuiWidgetColorPicker();
    GuiWidgetColorPicker(const GuiWidgetColorPicker&) = delete;
    GuiWidgetColorPicker& operator=(const GuiWidgetColorPicker&) = delete;

    void colorPicker3(const std::string& label, Vector3& color);
    void colorPicker4(const std::string& label, Vector4& color);

private:
    void colorPicker(const std::string& label, void* color, bool isVec4);

private:
    GuiWindow& mWindow;
    std::vector<GuiColorPicker> mColorPickers;

    static constexpr float COLOR_PICKER_WIDTH = GuiWidgetConstant::FRAME_WIDTH - 32.f;
    static constexpr float COLOR_PICKER_HEIGHT = COLOR_PICKER_WIDTH;
    static const inline Vector2 COLOR_PICKER_SIZE = Vector2(COLOR_PICKER_WIDTH, COLOR_PICKER_HEIGHT);

    static constexpr float HUE_BAR_WIDTH = GuiWidgetConstant::FRAME_WIDTH - COLOR_PICKER_WIDTH - GuiWidgetConstant::PADDING;
    static constexpr float HUE_BAR_HEIGHT = COLOR_PICKER_HEIGHT;
    static const inline Vector2 HUE_BAR_SIZE = Vector2(HUE_BAR_WIDTH, HUE_BAR_HEIGHT);

    static const inline std::array<Vector3, 6> HUE_BAR_COLORS = {
        Vector3(1.f, 0.f, 0.f),
        Vector3(1.f, 1.f, 0.f),
        Vector3(0.f, 1.f, 0.f),
        Vector3(0.f, 1.f, 1.f),
        Vector3(0.f, 0.f, 1.f),
        Vector3(1.f, 0.f, 1.f),
    };
};
