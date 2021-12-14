#include "GuiWidgetText.h"
#include "GuiWidgetConstant.h"
#include "../GuiContext.h"
#include "../GuiWindow.h"

GuiWidgetText::GuiWidgetText(GuiWindow& window)
    : GuiWidgetTextBase(window)
{
}

GuiWidgetText::~GuiWidgetText() = default;

void GuiWidgetText::text(const std::string& text) {
    const auto& nextPos = mWindow.getNextWidgetPosition();

    //親クラスのテキスト表示関数を呼び出す
    createText(
        text,
        nextPos + Vector2(0.f, GuiWidgetConstant::TEXT_HEIGHT_PADDING),
        GuiWidgetConstant::TEXT_HEIGHT
    );

    //ウィジェット位置を更新
    mWindow.setNextWidgetPosition(
        nextPos + Vector2(0.f, GuiWidgetConstant::TEXT_HEIGHT + GuiWidgetConstant::TEXT_HEIGHT_PADDING * 2.f)
    );
}
