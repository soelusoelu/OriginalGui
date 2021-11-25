#pragma once

#include <memory>

class GuiFontAtlas;

class GuiContext {
public:
    GuiContext();
    ~GuiContext();
    GuiContext(const GuiContext&) = delete;
    GuiContext& operator=(const GuiContext&) = delete;

private:
    std::unique_ptr<GuiFontAtlas> mFontAtlas;
};
