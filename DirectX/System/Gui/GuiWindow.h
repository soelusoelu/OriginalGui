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
    //背景色を設定する
    void setBackgroundColor(const Vector4& color);
    //背景色を取得する
    const Vector4& getBackgroundColor() const;
    //タイトルバー色を設定する
    void setTitleBarColor(const Vector4& color);
    //タイトルバー色を取得する
    const Vector4& getTitleBarColor() const;
    //アクティブ状態にする
    void setActive();
    //非アクティブ状態にする
    void setInactive();

    void sliderInt(const std::string& label, int& v, int min, int max);
    void sliderFloat(const std::string& label, float& v, float min, float max);

    GuiContext& getContext() const;
    GuiDrawList& getDrawList() const;

    void setNextWidgetPosition(const Vector2& pos);
    const Vector2& getNextWidgetPosition() const;

private:
    void setTitleBarVertexColor(const Vector4& color);

private:
    GuiContext& mContext;
    GuiWidgets mWidgets;
    std::string mName;
    std::unique_ptr<GuiDrawList> mDrawList;
    //ウィンドウの右上にウィンドウを閉じるボタンが表示され、
    //クリックするとウィンドウが閉じる
    bool mEndButton;
    //アクティブか
    bool mIsActive;
    //ウィンドウ位置
    Vector2 mPosition;
    //ウィンドウサイズ
    Vector2 mSize;
    //背景
    Vector4 mBackgroundColor;
    unsigned mBackgroundVerticesStartIndex;
    unsigned mBackgroundVerticesNumPoints;
    //タイトルバー
    float mTitleBarSizeY;
    Vector4 mTitleBarColor;
    Vector4 mInactiveTitleBarColor;
    unsigned mTitleBarVerticesStartIndex;
    unsigned mTitleBarVerticesNumPoints;
    //次のウィジェット軸表示位置
    Vector2 mNextWidgetPosition;
    //ウィンドウ移動中か
    bool mIsWindowMoving;

    static constexpr float ROUNDING = 7.f;
    static constexpr float OUTER_BORDRES_THICKNESS = 1.f;
};
