#pragma once

#include "GuiWidgetTextBase.h"
#include <vector>

class GuiWidgetText
    : public GuiWidgetTextBase
{
public:
    GuiWidgetText(GuiWindow& window);
    ~GuiWidgetText();
    GuiWidgetText(const GuiWidgetText&) = delete;
    GuiWidgetText& operator=(const GuiWidgetText&) = delete;

    void text(const std::string& text);
};
