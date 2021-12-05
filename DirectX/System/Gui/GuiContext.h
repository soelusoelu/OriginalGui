#pragma once

#include "GuiDrawListSharedData.h"
#include "../../Math/Math.h"
#include <memory>
#include <string>
#include <vector>

class GuiFontAtlas;
class GuiWindow;
class GuiRenderer;

class GuiContext {
public:
    GuiContext();
    ~GuiContext();
    GuiContext(const GuiContext&) = delete;
    GuiContext& operator=(const GuiContext&) = delete;

    //すべてのGUIを更新する
    void update();
    //すべてのGUIを描画する
    void draw();

    //ウィンドウの追加
    void addWindow(const std::shared_ptr<GuiWindow>& window);
    //ウィンドウを取得する
    const GuiWindow& getWindow(unsigned index) const;
    //ウィンドウ数を取得する
    unsigned getWindowCount() const;

    //DrawListSharedDataを取得する
    const GuiDrawListSharedData& getDrawListSharedData() const;

    void setFramePadding(const Vector2& padding);
    const Vector2& getFramePadding() const;

private:
    std::unique_ptr<GuiFontAtlas> mFontAtlas;
    std::unique_ptr<GuiRenderer> mGuiRenderer;
    std::vector<std::shared_ptr<GuiWindow>> mWindows;
    GuiDrawListSharedData mDrawListSharedData;
    Vector2 mFramePadding;
};
