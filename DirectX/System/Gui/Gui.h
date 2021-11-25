#pragma once

#include <memory>

class GuiContext;

class Gui {
public:
    Gui();
    ~Gui();
    Gui(const Gui&) = delete;
    Gui& operator=(const Gui&) = delete;

private:
    std::unique_ptr<GuiContext> mContext;
};
