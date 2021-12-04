#pragma once

#include <memory>
#include <string>

class GuiContext;
class GuiWindow;

class Gui {
public:
    Gui() = delete;
    ~Gui() = delete;

    //GUIウィンドウを新規作成する
    static std::shared_ptr<GuiWindow> createWindow(
        const std::string& name,
        bool endButton = false
    );

public:
    static inline GuiContext* mContext = nullptr;
};
