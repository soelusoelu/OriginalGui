﻿#pragma once

#include "GuiDrawList.h"
#include "../../Math/Math.h"
#include <memory>
#include <string>

class GuiWindow {
public:
    GuiWindow(const std::string& name, bool endButton = false);
    ~GuiWindow();
    GuiDrawList& getDrawList() const;

private:
    std::string mName;
    std::unique_ptr<GuiDrawList> mDrawList;
    //ウィンドウの右上にウィンドウを閉じるボタンが表示され、
    //クリックするとウィンドウが閉じる
    bool mEndButton;
    //ウィンドウ位置
    Vector2 mPosition;
    //ウィンドウサイズ
    Vector2 mSize;
    //このウィンドウが選択されているか
    bool mActive;
};