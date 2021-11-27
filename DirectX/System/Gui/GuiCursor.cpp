#include "GuiCursor.h"

GuiCursor::GuiCursor()
    : mCursorsRect(Vector2(CURSOR_TEX_WIDTH * 2 + 1, CURSOR_TEX_HEIGHT))
    , mThickLinesRect(Vector2(LINE_WIDTH_MAX + 2, LINE_WIDTH_MAX + 1))
{
}

GuiCursor::~GuiCursor() = default;
