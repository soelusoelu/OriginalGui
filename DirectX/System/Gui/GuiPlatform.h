#pragma once

enum class GuiPlatform {
    GUI_PLATFORM_UNICODE = 0,
    GUI_PLATFORM_MAC = 1,
    GUI_PLATFORM_ISO = 2,
    GUI_PLATFORM_MICROSOFT = 3
};

enum class GuiPlatformMicrosoft {
    GUI_PLATFORM_MS_SYMBOL = 0,
    GUI_PLATFORM_MS_UNICODE_BMP = 1,
    GUI_PLATFORM_MS_SHIFTJIS = 2,
    GUI_PLATFORM_MS_UNICODE_FULL = 10
};
