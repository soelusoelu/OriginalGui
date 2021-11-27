#pragma once

#include "FontInfo.h"
#include "GuiFontConfig.h"

class TrueType {
public:
    TrueType() = delete;
    ~TrueType() = delete;

    //フォント読み込み構造の初期化、フォントデータの有効性チェック
    static bool initializeFont(FontInfo& info, const GuiFontConfig& config);
    static int findGlyphIndex(const FontInfo& info, int unicodeCodepoint);

private:
    static unsigned findTable(const unsigned char* data, const char* tag);

private:
    static unsigned char byte(const unsigned char* p);
    static unsigned short ushort(const unsigned char* p);
    static unsigned ulong(const unsigned char* p);
    static bool stbttTag(const unsigned char* p, const char* str);
};
