#pragma once

enum class DrawCornerFlags {
    NONE = 0,
    TOP_LEFT = 1,
    TOP_RIGHT = (1 << 1),
    BOT_LEFT = (1 << 2),
    BOT_RIGHT = (1 << 3),
    TOP = (TOP_LEFT | TOP_RIGHT),
    BOT = (BOT_LEFT | BOT_RIGHT),
    LEFT = (TOP_LEFT | BOT_LEFT),
    RIGHT = (TOP_RIGHT | BOT_RIGHT),
    ALL = (TOP | BOT)
};
