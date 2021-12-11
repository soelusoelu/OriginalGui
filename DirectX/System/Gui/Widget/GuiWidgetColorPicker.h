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
    float hueBarWidth = 0.f;
    unsigned colorPickerVerticesStartIndex = 0;
    unsigned hueBarVerticesStartIndex = 0;
};

class GuiWidgetColorPicker {
public:
    GuiWidgetColorPicker(GuiWindow& window);
    ~GuiWidgetColorPicker();
    GuiWidgetColorPicker(const GuiWidgetColorPicker&) = delete;
    GuiWidgetColorPicker& operator=(const GuiWidgetColorPicker&) = delete;

    //毎フレーム更新
    void update();

    //カラーピッカー
    void colorPicker3(const std::string& label, Vector3& color);
    void colorPicker4(const std::string& label, Vector4& color);

private:
    //すべての新規カラーピッカーの処理
    void colorPicker(const std::string& label, void* color, bool isVec4);

    //マウス位置からカラーピッカーを選択する
    void selectColorPicker();
    //カラーピッカーの選択色を更新する
    void updateColorPicker();
    //マウス位置から色相バーを選択する
    void selectHueBar();
    //色相バーの選択色を更新する
    void updateHueBar();

    //カラーピッカーの位置を取得する
    const Vector2& getColorPickerPosition(const GuiColorPicker& colorPicker) const;
    //色相バーの位置を取得する
    const Vector2& getHueBarPosition(const GuiColorPicker& colorPicker) const;
    //選択中のカラーピッカーを取得する
    const GuiColorPicker& getSelectingColorPicker() const;
    //カラーピッカーを選択中か
    bool isSelectingColorPicker() const;
    //色相バーを選択中か
    bool isSelectingHueBar() const;

private:
    GuiWindow& mWindow;
    std::vector<GuiColorPicker> mColorPickers;
    //選択中のカラーピッカー/色相バーのインデックス
    //-1なら掴んでいない
    int mColorPickerIndex;
    int mHueBarIndex;

    static constexpr float COLOR_PICKER_WIDTH = GuiWidgetConstant::FRAME_WIDTH - 32.f;
    static constexpr float COLOR_PICKER_HEIGHT = COLOR_PICKER_WIDTH;
    static const inline Vector2 COLOR_PICKER_SIZE = Vector2(COLOR_PICKER_WIDTH, COLOR_PICKER_HEIGHT);

    static constexpr float HUE_BAR_HEIGHT = COLOR_PICKER_HEIGHT;

    static const inline std::array<Vector3, 6> HUE_BAR_COLORS = {
        Vector3(1.f, 0.f, 0.f),
        Vector3(1.f, 1.f, 0.f),
        Vector3(0.f, 1.f, 0.f),
        Vector3(0.f, 1.f, 1.f),
        Vector3(0.f, 0.f, 1.f),
        Vector3(1.f, 0.f, 1.f),
    };
};
