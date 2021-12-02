#pragma once

#include <memory>
#include <string>
#include <vector>

class GuiFontAtlas;
class GuiWindow;

class GuiContext {
public:
    GuiContext();
    ~GuiContext();
    GuiContext(const GuiContext&) = delete;
    GuiContext& operator=(const GuiContext&) = delete;

    //ウィンドウの作成
    std::shared_ptr<GuiWindow> createWindow(const std::string& name, bool endButton = false);
    void render();

private:
    std::unique_ptr<GuiFontAtlas> mFontAtlas;
    std::vector<std::shared_ptr<GuiWindow>> mWindows;
};
