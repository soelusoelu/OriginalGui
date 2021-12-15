#pragma once

#include "GuiWidgetColorBase.h"
#include "../../../Math/Math.h"
#include <memory>
#include <string>
#include <vector>

class GuiWidgetText;

struct GuiColorPicker {
    void* color = nullptr;
    bool isVec4 = true;
};

class GuiWidgetColorPicker
    : public GuiWidgetColorBase
{
public:
    GuiWidgetColorPicker(GuiWindow& window);
    ~GuiWidgetColorPicker();
    GuiWidgetColorPicker(const GuiWidgetColorPicker&) = delete;
    GuiWidgetColorPicker& operator=(const GuiWidgetColorPicker&) = delete;

    virtual void update() override;
    virtual void onUpdateColorPicker(const Vector4& newColor) override;

    //カラーピッカー
    void colorPicker3(const std::string& label, Vector3& color);
    void colorPicker4(const std::string& label, Vector4& color);

private:
    //すべての新規カラーピッカーの処理
    void colorPicker(const std::string& label, void* color, bool isVec4);
    //数値をクランプする
    void clamp(GuiColorPicker& colorPicker);

private:
    std::vector<GuiColorPicker> mColorPickers;
};
