#include "GuiWidgetTextBase.h"
#include "../GuiContext.h"
#include "../GuiWindow.h"

GuiWidgetTextBase::GuiWidgetTextBase(GuiWindow& window)
    : mWindow(window)
{
}

GuiWidgetTextBase::~GuiWidgetTextBase() = default;

void GuiWidgetTextBase::updateBase() {
    update();
}

void GuiWidgetTextBase::createText(
    const std::string& text,
    const Vector2& pos,
    float pixelSizeY,
    int capacity,
    const Vector4& color,
    Pivot pivot
) {
    auto& dl = mWindow.getDrawList();

    auto start = dl.getVertexCount();
    mWindow.getDrawList().addText(
        text,
        pos,
        pixelSizeY,
        capacity,
        color,
        pivot
    );
    auto numPoints = dl.getVertexCount() - start;

    mTexts.emplace_back(text);
    mTextsVertexInfo.emplace_back(GuiTextBaseVertexInfo{ start, numPoints });
}
