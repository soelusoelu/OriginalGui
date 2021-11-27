#pragma once

#include "FontInfo.h"
#include "GuiBitVector.h"
#include "GuiFontConfig.h"
#include <vector>

class GuiFontAtlas;

struct GuiFontBuildSrcData {
    FontInfo fontInfo;
    //ユーザーの要求に応じた範囲
    const wchar_t* srcRanges = nullptr;
    //最大のコードポイント
    int glyphsHighest = 0;
    //グリフ数
    int glyphsCount = 0;
    //グリフのビットマップ
    GuiBitVector glyphsSet;
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
};
