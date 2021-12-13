#pragma once

#include "../../../Math/Math.h"

//基本的に使用される定数
namespace GuiWidgetConstant {
//フレーム横幅
constexpr float FRAME_WIDTH = 192.f;
//フレーム縦幅
constexpr float FRAME_HEIGHT = 20.f;
//フレームサイズ
const Vector2 FRAME_SIZE = Vector2(FRAME_WIDTH, FRAME_HEIGHT);
//フレームカラー
const Vector4 FRAME_COLOR = Vector4(0.15f, 0.25f, 0.35f, 0.8f);

//文字の縦の余白
constexpr float TEXT_HEIGHT_PADDING = 2.f;
//文字縦幅
constexpr float TEXT_HEIGHT = FRAME_HEIGHT - (TEXT_HEIGHT_PADDING * 2);

//有効桁数
constexpr int DIGITS = 9;
}
