#include "AsciiHelper.h"
#include <cassert>

char AsciiHelper::clampCharToAscii(char c) {
    return Math::clamp<char>(c, 32, 127);
}

Vector2 AsciiHelper::calcPositionRateToAscii(
    char asciiNumber,
    int asciiWidthCharCount,
    int asciiHeightCharCount
) {
    assert(asciiNumber >= 32);
    asciiNumber -= 32;

    float left = static_cast<float>(asciiNumber % asciiWidthCharCount);
    left /= asciiWidthCharCount;
    float top = static_cast<float>(asciiNumber / asciiWidthCharCount);
    top /= asciiHeightCharCount;
    return Vector2(left, top);
}

Vector2 AsciiHelper::calcSizeRateToAscii(
    int asciiWidthCharCount,
    int asciiHeightCharCount
) {
    float w = 1.f / asciiWidthCharCount;
    float h = 1.f / asciiHeightCharCount;
    return Vector2(w, h);
}
