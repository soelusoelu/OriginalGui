#pragma once

#include "FontInfo.h"
#include "GuiFontConfig.h"
#include "../../Math/Math.h"

struct PackedChar {
    //ビットマップ上のbboxの座標
    unsigned short x0 = 0;
    unsigned short y0 = 0;
    unsigned short x1 = 0;
    unsigned short y1 = 0;

    float xoff = 0.f;
    float yoff = 0.f;
    float xadvance = 0.f;
    float xoff2 = 0.f;
    float yoff2 = 0.f;
};

struct PackRange {
    float fontSize = 0.f;
    //0でない場合は、文字が連続していて、これが最初のコードポイントとなる
    int firstUnicodeCodepointInRange = 0;
    //nullptrでない場合、unicodeコードポイントの配列
    int* arrayOfUnicodeCodepoints = nullptr;
    int numChars = 0;
    PackedChar* chardataForRange = nullptr;
    unsigned char hOversample = 0;
    unsigned char vOversample = 0;
};

class TrueType {
public:
    TrueType() = delete;
    ~TrueType() = delete;

    //フォント読み込み構造の初期化、フォントデータの有効性チェック
    static bool initializeFont(FontInfo& info, const GuiFontConfig& config);
    static int findGlyphIndex(const FontInfo& info, int unicodeCodepoint);

    static float scaleForPixelHeight(const FontInfo& info, float height);
    static float scaleForMappingEmToPixels(const FontInfo& info, float pixels);

    static void getGlyphBitmapBoxSubpixel(
        const FontInfo& info,
        int glyph,
        const Vector2& scale,
        const Vector2& shift,
        int* ix0 = nullptr,
        int* iy0 = nullptr,
        int* ix1 = nullptr,
        int* iy1 = nullptr
    );
    static bool getGlyphBox(
        const FontInfo& info,
        int glyphIndex,
        int* x0 = nullptr,
        int* y0 = nullptr,
        int* x1 = nullptr,
        int* y1 = nullptr
    );

private:
    static unsigned findTable(const unsigned char* data, const char* tag);
    static int getGlyfOffset(const FontInfo& info, int glyphIndex);

    static unsigned char ttByte(const unsigned char* p);
    static short ttShort(const unsigned char* p);
    static unsigned short ttUshort(const unsigned char* p);
    static unsigned ttUlong(const unsigned char* p);
    static bool ttTag(const unsigned char* p, const char* str);
};
