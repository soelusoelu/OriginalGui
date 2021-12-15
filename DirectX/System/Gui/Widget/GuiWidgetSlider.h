#pragma once

#include "GuiWidgetFrameBase.h"
#include "GuiDataType.h"
#include "GuiWidgetConstant.h"
#include "../../../Math/Math.h"
#include <any>
#include <memory>
#include <string>
#include <vector>

struct GuiSlider {
    GuiDataType type = GuiDataType::INT;
    void* data = nullptr;
    std::any min;
    std::any max;
    unsigned grabStartIndex = 0;
    unsigned grabNumPoints = 0;
};

class GuiWidgetSlider
    : public GuiWidgetFrameBase
{
public:
    GuiWidgetSlider(GuiWindow& window);
    ~GuiWidgetSlider();
    GuiWidgetSlider(const GuiWidgetSlider&) = delete;
    GuiWidgetSlider& operator=(const GuiWidgetSlider&) = delete;

    virtual void update() override;
    virtual void onUpdateSelectFrame(const GuiFrameInfo& frame) override;

    //各種スライダー
    void sliderInt(
        const std::string& label,
        int& v,
        int min,
        int max
    );
    void sliderFloat(
        const std::string& label,
        float& v,
        float min,
        float max
    );
    void sliderVector2(
        const std::string& label,
        Vector2& v,
        const Vector2& min,
        const Vector2& max
    );
    void sliderVector3(
        const std::string& label,
        Vector3& v,
        const Vector3& min,
        const Vector3& max
    );
    void sliderVector4(
        const std::string& label,
        Vector4& v,
        const Vector4& min,
        const Vector4& max
    );

private:
    //すべての新規スライダーの処理
    void sliderScalar(
        GuiFrameInfo& frame,
        GuiDataType type,
        void* v,
        const std::any& min,
        const std::any& max
    );
    //選択中のスライダーの数値を更新する
    void updateNumber(const GuiSlider& slider, float t);
    //スライダーのグラブ位置を更新する
    void updateGrabPosition(const GuiSlider& slider, const GuiFrameInfo& frame);
    //選択中のスライダーのグラブ位置を更新する
    void updateGrabPosition(const GuiSlider& slider, const GuiFrameInfo& frame, float t);
    //数値をクランプする
    void clamp(void* data, const std::any& min, const std::any& max, GuiDataType type);

    //グラブの最低位置を求める
    float calcGrabbingPosXMin(const GuiFrameInfo& frame) const;
    //グラブの最大位置を求める
    float calcGrabbingPosXMax(const GuiFrameInfo& frame) const;

private:
    std::vector<GuiSlider> mSliders;

    static constexpr float GRAB_WIDTH = GuiWidgetConstant::FRAME_WIDTH / 16.f;
    static constexpr float GRAB_WIDTH_HALF = GRAB_WIDTH / 2;
    static constexpr float GRAB_PADDING = 2.f;
    static constexpr float GRAB_HEIGHT = GuiWidgetConstant::FRAME_HEIGHT - GRAB_PADDING * 2;
    static const inline Vector2 GRAB_SIZE = Vector2(GRAB_WIDTH, GRAB_HEIGHT);
    static const inline Vector4 GRAB_COLOR = Vector4(0.15f, 0.3f, 0.75f, 0.9f);
};
