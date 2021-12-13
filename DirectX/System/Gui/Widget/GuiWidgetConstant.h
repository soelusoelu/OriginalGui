#pragma once

#include "../../../Math/Math.h"

namespace GuiWidgetConstant {
//基本フレーム横幅
constexpr float FRAME_WIDTH = 192.f;
//基本フレーム縦幅
constexpr float FRAME_HEIGHT = 20.f;
//基本フレームサイズ
const Vector2 FRAME_SIZE = Vector2(FRAME_WIDTH, FRAME_HEIGHT);
//基本フレームカラー
const Vector4 FRAME_COLOR = Vector4(0.15f, 0.25f, 0.35f, 0.8f);

//基本文字の縦の余白
constexpr float TEXT_HEIGHT_PADDING = 2.f;
//基本文字縦幅
constexpr float TEXT_HEIGHT = FRAME_HEIGHT - (TEXT_HEIGHT_PADDING * 2);
}
