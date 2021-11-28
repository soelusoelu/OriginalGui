#pragma once

#include <vector>

struct GuiBuf {
    unsigned char* data = nullptr;
    int cursor = 0;
    int size = 0;
};

struct FontInfo {
    //フォントデータへのポインタ
    std::vector<unsigned char> data;

    //範囲チェックに必要なグリフの数
    int numGlyphs = 0;

    //テーブルの位置を.ttfの開始位置からのオフセットで表示
    unsigned cmap = 0;
    unsigned loca = 0;
    unsigned head = 0;
    unsigned glyf = 0;
    unsigned hhea = 0;
    unsigned hmtx = 0;
    unsigned kern = 0;
    unsigned gpos = 0;

    //選択した文字エンコーディングのcmapマッピング
    int indexMap = 0;
    //グリフインデックスからグリフへのマッピングに必要なフォーマット
    int indexToLocFormat = 0;
};
