#pragma once

#include "FontInfo.h"
#include "GuiBitVector.h"
#include "GuiFontConfig.h"
#include "GuiRect.h"
#include "TrueType.h"
#include <vector>

class GuiFontAtlas;

struct GuiFontBuildSrcData {
    FontInfo fontInfo;
    //パックするコードポイントのリストを保持する
    PackRange packRange;
    //パックする四角形
    GuiRect* rects = nullptr;
    //グリフ出力
    PackedChar* packedChars = nullptr;
    //ユーザーの要求に応じた範囲
    const wchar_t* srcRanges = nullptr;
    //最大のコードポイント
    int glyphsHighest = 0;
    //グリフ数
    int glyphsCount = 0;
    //グリフのビットマップ
    GuiBitVector glyphsSet;
    //グリフコードポイントリスト
    std::vector<int> glyphsList;
};

struct GuiFontBuildDstData {
    int glyphsHighest = 0;
    int glyphsCount = 0;
    GuiBitVector glyphsSet;
};

class GuiFontInitializer {
public:
    GuiFontInitializer() = delete;
    ~GuiFontInitializer() = delete;

    static void initialize(GuiFontAtlas& font);

private:
    //要求されたコードポイントごとに、フォントデータ内に存在するかどうかをチェック
    static bool checkRequestedCodepoint(
        GuiFontBuildSrcData& src,
        GuiFontBuildDstData& dst,
        const GuiFontConfig& config,
        int& totalGlyphsCount
    );

    //ビットマップをフラットリストに展開
    static void unpackBitMap(
        GuiFontBuildSrcData& src,
        GuiFontBuildDstData& dst
    );

    static void unpackBitVectorToFlatIndexList(
        std::vector<int>& dst,
        const GuiBitVector& src
    );

    //グリフのサイズを集めて、仮想のキャンバスに詰め込む
    static void packVirtualCanvas(
        GuiFontBuildSrcData& src,
        GuiFontAtlas& font,
        const GuiFontConfig& config,
        int totalGlyphsCount
    );
};
