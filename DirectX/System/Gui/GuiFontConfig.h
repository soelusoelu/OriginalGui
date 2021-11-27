#pragma once

#include "../../Math/Math.h"

//struct GuiFontConfig {
//    //フォントデータ
//    void* fontData = nullptr;
//    //フォントデータサイズ
//    int fontDataSize = 0;
//    //FontAtlasがデータの所有権を保持するか
//    bool fontDataOwnedByAtlas = true;
//    //フォントファイル内のインデックス
//    int fontNo = 0;
//    //ピクセル単位でのサイズ
//    float pixelsSize = 0.f;
//    //サブピクセルの位置決めのために、より高い品質でラスタライズする
//    int oversampleH = 3;
//    //サブピクセルの位置決めのために、より高い品質でラスタライズする
//    //Y軸のサブピクセル位置は使わない
//    int oversampleV = 1;
//    //すべてのグリフをピクセル境界に合わせる
//    //ピクセルアライメントされていないフォントをデフォルトフォントに統合する場合などに便利
//    //有効にすると、OversampleH/Vを1に設定できる
//    bool pixelSnapH = false;
//    //グリフ間の余分な間隔(ピクセル単位)
//    //現在、X軸のみサポート
//    Vector2 glyphExtraSpacing = Vector2::zero;
//    //このフォント入力からすべてのグリフをオフセットする
//    Vector2 glyphOffset = Vector2::zero;
//    //ユーザーが提供するUnicode範囲のリストへのポインタ(1つの範囲につき2つの値、値は包括的、ゼロ終端のリスト)
//    //この配列データは、フォントが生きている限り持続する必要がある
//    const wchar_t* glyphRanges = nullptr;
//    //グリフの最小AdvanceX、Minを設定してフォントアイコンを揃える
//    //Min/Maxの両方を設定してモノスペースフォントを強制する
//    float glyphMinAdvanceX = 0.f;
//    //グリフの最大AdvanceX
//    float glyphMaxAdvanceX = FLT_MAX;
//    //前のImFontにマージすることで、複数の入力フォントを1つのImFontにまとめることができる
//    //(例：ASCIIフォント＋アイコン＋日本語グリフ)
//    //異なる高さのフォントをマージするときは、GlyphOffset.yを使用する
//    bool mergeMode = false;
//    //カスタムフォントラスタライザ(例：ImGuiFreeType)の設定
//    unsigned rasterizerFlags = 0x00;
//    //フォントの明暗の調整(1.fより大きいと明るく、低いと暗い)
//    float rasterizerMultiply = 1.f;
//    //エリプシス文字のunicodeコードポイントを明示的に指定
//    wchar_t ellipsisChar = USHRT_MAX;
//    //デバッグ用ネーム
//    char name[40] = { 0 };
//};

struct GuiFontConfig {
    //フォントデータ
    void* fontData = nullptr;
    //フォントデータサイズ
    int fontDataSize = 0;
    //ピクセル単位でのサイズ
    float pixelsSize = 0.f;
    //サブピクセルの位置決めのために、より高い品質でラスタライズする
    int oversampleH = 3;
    //サブピクセルの位置決めのために、より高い品質でラスタライズする
    //Y軸のサブピクセル位置は使わない
    int oversampleV = 1;
    //すべてのグリフをピクセル境界に合わせる
    //ピクセルアライメントされていないフォントをデフォルトフォントに統合する場合などに便利
    //有効にすると、OversampleH/Vを1に設定できる
    bool pixelSnapH = false;
    //グリフ間の余分な間隔(ピクセル単位)
    //現在、X軸のみサポート
    Vector2 glyphExtraSpacing = Vector2::zero;
    //このフォント入力からすべてのグリフをオフセットする
    Vector2 glyphOffset = Vector2::zero;
    //ユーザーが提供するUnicode範囲のリストへのポインタ(1つの範囲につき2つの値、値は包括的、ゼロ終端のリスト)
    //この配列データは、フォントが生きている限り持続する必要がある
    const wchar_t* glyphRanges = nullptr;
    //グリフの最小AdvanceX、Minを設定してフォントアイコンを揃える
    //Min/Maxの両方を設定してモノスペースフォントを強制する
    float glyphMinAdvanceX = 0.f;
    //グリフの最大AdvanceX
    float glyphMaxAdvanceX = FLT_MAX;
    //カスタムフォントラスタライザ(例：ImGuiFreeType)の設定
    unsigned rasterizerFlags = 0x00;
    //フォントの明暗の調整(1.fより大きいと明るく、低いと暗い)
    float rasterizerMultiply = 1.f;
};
