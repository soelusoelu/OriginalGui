#pragma once

#include "GuiDrawListSharedData.h"
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

    //ウィンドウの作成
    std::shared_ptr<GuiWindow> createWindow(const std::string& name, bool endButton = false);
    //すべてのGuiを描画する
    void draw();
    //DrawListSharedDataを取得する
    const GuiDrawListSharedData& getDrawListSharedData() const;
    //ウィンドウを取得する
    const GuiWindow& getWindow(unsigned index) const;
    //ウィンドウ数を取得する
    unsigned getWindowCount() const;

private:
    std::unique_ptr<GuiFontAtlas> mFontAtlas;
    std::unique_ptr<GuiRenderer> mGuiRenderer;
    std::vector<std::shared_ptr<GuiWindow>> mWindows;
    GuiDrawListSharedData mDrawListSharedData;
};
