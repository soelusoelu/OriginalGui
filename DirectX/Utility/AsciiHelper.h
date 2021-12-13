#pragma once

#include "../Math/Math.h"

namespace AsciiHelper {

//文字をASCII範囲に収める
char clampCharToAscii(char c);

//任意のASCII矩形における指定文字の位置割合を求める
Vector2 calcPositionRateToAscii(
    char asciiNumber,
    int asciiWidthCharCount,
    int asciiHeightCharCount
);

//任意のASCII矩形における1文字のサイズ割合を求める
Vector2 calcSizeRateToAscii(
    int asciiWidthCharCount,
    int asciiHeightCharCount
);

};
