#include "Gui.h"
#include "GuiContext.h"

Gui::Gui()
    : mContext(std::make_unique<GuiContext>())
{
}

Gui::~Gui() = default;
