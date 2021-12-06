#pragma once

#include "GuiDrawList.h"
#include "GuiWidgets.h"
#include "../../Math/Math.h"
#include <memory>
#include <string>

class GuiContext;

class GuiWindow {
public:
    GuiWindow(
        GuiContext& context,
        const std::string& name,
        bool endButton
    );
    ~GuiWindow();

    void update();

    //ウィンドウ位置を設定する
    void setPosition(const Vector2& pos);
    //ウィンドウ位置を取得する
    const Vector2& getPosition() const;
    //ウィンドウサイズを設定する
    void setSize(const Vector2& size);
    //ウィンドウサイズを取得する
    const Vector2& getSize() const;

    void sliderInt(const std::string& label, int& v, int min, int max);
    void sliderFloat(const std::string& label, float& v, float min, float max);

    GuiContext& getContext() const;
    GuiDrawList& getDrawList() const;

    void setNextWidgetPosition(const Vector2& pos);
    const Vector2& getNextWidgetPosition() const;

private:
    GuiContext& mContext;
    GuiWidgets mWidgets;
    std::string mName;
    std::unique_ptr<GuiDrawList> mDrawList;
    //ウィンドウの右上にウィンドウを閉じるボタンが表示され、
    //クリックするとウィンドウが閉じる
    bool mEndButton;
    //ウィンドウ位置
    Vector2 mPosition;
    //ウィンドウサイズ
    Vector2 mSize;
    //背景色
    Vector4 mBackgroundColor;
    //タイトルバー色
    Vector4 mTitleBarColor;
    //次のウィジェット軸表示位置
    Vector2 mNextWidgetPosition;

    static constexpr float ROUNDING = 7.f;
};
