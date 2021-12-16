#pragma once

#include "GuiWidgetConstant.h"
#include "../../../Math/Math.h"
#include <vector>

class GuiWindow;

struct GuiColorBaseInfo {
    unsigned colorPickerStartIndex = 0;
    unsigned hueBarStartIndex = 0;
    unsigned colorPickerCursorStartIndex = 0;
    unsigned colorPickerCursorNumPoints = 0;
    unsigned hueBarCursorStartIndex = 0;
    unsigned hueBarCursorNumPoints = 0;

    void* color = nullptr;
    bool isVec4 = true;
};

class GuiWidgetColorBase {
public:
    GuiWidgetColorBase(GuiWindow& window);
    virtual ~GuiWidgetColorBase();
    GuiWidgetColorBase(const GuiWidgetColorBase&) = delete;
    GuiWidgetColorBase& operator=(const GuiWidgetColorBase&) = delete;

    void baseUpdate();
    virtual void update() {};
    virtual void onSelectColorPicker() {};
    virtual void onUpdateColorPicker(const Vector4& newColor) {};
    virtual void onSelectHueBar() {};
    virtual void onUpdateHueBar(const Vector3& newColor) {};

protected:
    //カラーピッカーを作成する
    void createColorPicker(const Vector2& pos, void* color, bool isVec4);
    //カラーピッカーのカーソルを作成する
    void createColorPickerCursor();
    //色相バーを作成する
    void createHueBar();
    //色相バーのカーソルを作成する
    void createHueBarCursor();

    //数値をクランプする
    void clamp(GuiColorBaseInfo& colorBase);
    //選択中のカラーピッカーを取得する
    const GuiColorBaseInfo& getSelectingVertexInfo() const;
    //選択中のインデックスを取得する
    int getSelectingIndex() const;
    //カラーピッカーを選択中か
    bool isSelectingColorPicker() const;
    //色相バーを選択中か
    bool isSelectingHueBar() const;
    //Vector3, 4に関わらずVector3型で取得する
    Vector3 getVector3(const GuiColorBaseInfo& colorBase) const;

    const GuiColorBaseInfo& getVertexInfo(unsigned index) const;
    const Vector2& getColorPickerPosition(const GuiColorBaseInfo& info) const;
    const Vector2& getColorPickerCursorPosition(const GuiColorBaseInfo& info) const;
    const Vector2& getHueBarPosition(const GuiColorBaseInfo& info) const;
    float getHueBarWidth(const GuiColorBaseInfo& info) const;

private:
    //マウス位置からカラーピッカーを選択する
    void selectColorPicker();
    //カラーピッカーの選択色を更新する
    void updateColorPicker();
    //マウス位置から色相バーを選択する
    void selectHueBar();
    //色相バーの選択色を更新する
    void updateHueBar();
    //管理している値をもとに更新する
    void updateColorPickerValueBased(const GuiColorBaseInfo& colorBase);

    //カラーピッカーの頂点色を設定する
    void setColorPickerVertexColor(const GuiColorBaseInfo& colorBase, const Vector3& color);
    //色相バーのカーソル位置を設定する
    void setHueBarCursorPosition(const GuiColorBaseInfo& colorBase, float posY);
    //次のプリミティブの始まりのインデックスを求める
    void calcStartIndex(unsigned& startIndex) const;
    //現在のプリミティブの頂点数を求める
    void calcNumPoints(unsigned& numPoints, unsigned startIndex) const;

protected:
    GuiWindow& mWindow;
    std::vector<GuiColorBaseInfo> mColorsInfo;
    //選択中のカラーピッカー/色相バーのインデックス
    //-1なら掴んでいない
    int mColorPickerIndex;
    int mHueBarIndex;

    static constexpr float COLOR_PICKER_WIDTH = GuiWidgetConstant::FRAME_WIDTH / 1.15f;
    static constexpr float COLOR_PICKER_HEIGHT = COLOR_PICKER_WIDTH;
    static const inline Vector2 COLOR_PICKER_SIZE = Vector2(COLOR_PICKER_WIDTH, COLOR_PICKER_HEIGHT);

    static constexpr float COLOR_PICKER_CURSOR_RADIUS = COLOR_PICKER_WIDTH / 32.f;

    static constexpr float HUE_BAR_HEIGHT = COLOR_PICKER_HEIGHT;

    static constexpr float HUE_BAR_CURSOR_HALF_SIZE = 3.f;

    static constexpr int HUE_BAR_COLORS_SIZE = 6;
    static const inline Vector3 HUE_BAR_COLORS[HUE_BAR_COLORS_SIZE] = {
        Vector3(1.f, 0.f, 0.f),
        Vector3(1.f, 1.f, 0.f),
        Vector3(0.f, 1.f, 0.f),
        Vector3(0.f, 1.f, 1.f),
        Vector3(0.f, 0.f, 1.f),
        Vector3(1.f, 0.f, 1.f),
    };
};
