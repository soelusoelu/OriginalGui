#pragma once

#include "GuiDataType.h"
#include "GuiWidgetConstant.h"
#include "../../../Math/Math.h"
#include <any>
#include <string>
#include <vector>

class GuiWindow;

struct GuiSlider {
    std::string label = "";
    GuiDataType type = GuiDataType::INT;
    void* data = nullptr;
    std::any min;
    std::any max;
    unsigned frameVerticesStartIndex = 0;
    unsigned grabVerticesStartIndex = 0;
    unsigned grabVerticesNumPoints = 0;
};

class GuiWidgetSlider {
public:
    GuiWidgetSlider(GuiWindow& window);
    ~GuiWidgetSlider();
    GuiWidgetSlider(const GuiWidgetSlider&) = delete;
    GuiWidgetSlider& operator=(const GuiWidgetSlider&) = delete;

    //毎フレーム更新
    void update();

    //各種スライダー
    void sliderInt(const std::string& label, int& v, int min, int max);
    void sliderFloat(const std::string& label, float& v, float min, float max);

private:
    //すべての新規スライダーの処理
    void sliderScalar(
        const std::string& label,
        GuiDataType type,
        void* v,
        const std::any& min,
        const std::any& max
    );
    //マウス位置からスライダーを選択する
    void selectSlider();
    //選択中のスライダーの数値を更新する
    void updateNumber();
    //選択中のスライダーのグラブ位置を更新する
    void updateGrabPosition(float f);

    //sliderのフレーム位置を取得する
    const Vector2& getFramePosition(const GuiSlider& slider) const;
    //掴んでいるグラブを取得する
    const GuiSlider& getGrabbingSlider() const;
    //掴んでいるか
    bool isGrabbing() const;
    //グラブの最低位置を求める
    float calcGrabbingPosXMin(const GuiSlider& slider) const;
    //グラブの最大位置を求める
    float calcGrabbingPosXMax(const GuiSlider& slider) const;

private:
    GuiWindow& mWindow;
    std::vector<GuiSlider> mSliders;
    //マウスで掴んでいるグラブのインデックス
    //-1なら掴んでいない
    int mGrabbingIndex;

    static constexpr float GRAB_WIDTH = 16.f;
    static constexpr float GRAB_WIDTH_HALF = GRAB_WIDTH / 2;
    static constexpr float GRAB_PADDING = 2.f;
    static constexpr float GRAB_HEIGHT = GuiWidgetConstant::FRAME_HEIGHT - GRAB_PADDING * 2;
    static const inline Vector2 GRAB_SIZE = Vector2(GRAB_WIDTH, GRAB_HEIGHT);
};
