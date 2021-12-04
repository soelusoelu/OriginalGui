#include "Gui.h"
#include "GuiContext.h"
#include "GuiWindow.h"
#include <cassert>

std::shared_ptr<GuiWindow> Gui::createWindow(
    const std::string& name,
    bool endButton
) {
    assert(mContext);

    auto w = std::make_shared<GuiWindow>(*mContext, name, endButton);
    mContext->addWindow(w);
    return w;
}
