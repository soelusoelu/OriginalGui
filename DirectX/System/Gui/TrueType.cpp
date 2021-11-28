#include "TrueType.h"
#include "GuiPlatform.h"
#include "GuiRect.h"
#include <cassert>

bool TrueType::initializeFont(FontInfo& info, const GuiFontConfig& config) {
    info.data = config.fontData;

    const auto data = config.fontData.data();
    info.cmap = findTable(data, "cmap");
    info.loca = findTable(data, "loca");
    info.head = findTable(data, "head");
    info.glyf = findTable(data, "glyf");
    info.hhea = findTable(data, "hhea");
    info.hmtx = findTable(data, "hmtx");
    info.kern = findTable(data, "kern");
    info.gpos = findTable(data, "GPOS");

    if (!info.cmap || !info.head || !info.hhea || !info.hmtx) {
        return false;
    }
    if (info.glyf) {
        //truetypeフォントに必要
        if (!info.loca) {
            return false;
        }
    } else {
        //CFF/Type2フォント(OTF)の初期化
        //TO DO
        assert(false);
    }

    if (auto t = findTable(data, "maxp")) {
        info.numGlyphs = ttUshort(data + t + 4);
    } else {
        info.numGlyphs = 0xffff;
    }

    //後で楽するために先に検索
    int numTables = ttUshort(data + info.cmap + 2);
    for (int i = 0; i < numTables; ++i) {
        unsigned encodingRecord = info.cmap + 4 + i * 8;

        auto platform = static_cast<GuiPlatform>(ttUshort(data + encodingRecord));
        if (platform == GuiPlatform::GUI_PLATFORM_MICROSOFT) {
            auto platformMS = static_cast<GuiPlatformMicrosoft>(ttUshort(data + encodingRecord + 2));
            if (platformMS == GuiPlatformMicrosoft::GUI_PLATFORM_MS_UNICODE_BMP
                || platformMS == GuiPlatformMicrosoft::GUI_PLATFORM_MS_UNICODE_FULL) {
                info.indexMap = info.cmap + ttUlong(data + encodingRecord + 4);
                break;
            }
        } else if (platform == GuiPlatform::GUI_PLATFORM_UNICODE) {
            info.indexMap = info.cmap + ttUlong(data + encodingRecord + 4);
            break;
        }
    }

    if (info.indexMap == 0) {
        return false;
    }

    info.indexToLocFormat = ttUshort(data + info.head + 50);

    return true;
}

int TrueType::findGlyphIndex(const FontInfo& info, int unicodeCodepoint) {
    const auto data = info.data.data();
    unsigned indexMap = info.indexMap;

    unsigned short format = ttUshort(data + indexMap + 0);
    if (format == 0) { //appleバイトエンコーディング
        int bytes = ttUshort(data + indexMap + 2);
        if (unicodeCodepoint < bytes - 6) {
            return ttByte(data + indexMap + 6 + unicodeCodepoint);
        }
        return 0;
    } else if (format == 6) {
        unsigned first = ttUshort(data + indexMap + 6);
        unsigned count = ttUshort(data + indexMap + 8);
        if (static_cast<unsigned>(unicodeCodepoint) >= first && static_cast<unsigned>(unicodeCodepoint) < first + count) {
            return ttUshort(data + indexMap + 10 + (unicodeCodepoint - first) * 2);
        }
        return 0;
    } else if (format == 2) { //日本語
        assert(false);
        return 0;
    } else if (format == 4) { //Windowsフォントの標準マッピング
        unsigned short segcount = ttUshort(data + indexMap + 6) >> 1;
        unsigned short searchRange = ttUshort(data + indexMap + 8) >> 1;
        unsigned short entrySelector = ttUshort(data + indexMap + 10);
        unsigned short rangeShift = ttUshort(data + indexMap + 12) >> 1;

        //セグメントのバイナリ検索を行う
        unsigned endCount = indexMap + 14;
        unsigned search = endCount;

        if (unicodeCodepoint > 0xffff) {
            return 0;
        }

        if (unicodeCodepoint >= ttUshort(data + search + rangeShift * 2)) {
            search += rangeShift * 2;
        }

        search -= 2;
        while (entrySelector) {
            unsigned short end;
            searchRange >>= 1;
            end = ttUshort(data + search + searchRange * 2);
            if (unicodeCodepoint > end) {
                search += searchRange * 2;
            }
            --entrySelector;
        }
        search += 2;

        {
            unsigned short offset, start;
            unsigned short item = static_cast<unsigned short>(((search - endCount) >> 1));

            assert(unicodeCodepoint <= ttUshort(data + endCount + 2 * item));
            start = ttUshort(data + indexMap + 14 + segcount * 2 + 2 + 2 * item);
            if (unicodeCodepoint < start) {
                return 0;
            }

            offset = ttUshort(data + indexMap + 14 + segcount * 6 + 2 + 2 * item);
            if (offset == 0) {
                return static_cast<unsigned short>((unicodeCodepoint + ttShort(data + indexMap + 14 + segcount * 4 + 2 + 2 * item)));
            }

            return ttUshort(data + offset + (unicodeCodepoint - start) * 2 + indexMap + 14 + segcount * 6 + 2 + 2 * item);
        }
    } else if (format == 12 || format == 13) {
        unsigned ngroups = ttUlong(data + indexMap + 12);
        int low, high;
        low = 0; high = static_cast<int>(ngroups);
        //バイナリーサーチの正しいグループ
        while (low < high) {
            int mid = low + ((high - low) >> 1); //ラウンドダウンするので、low <= mid < high
            unsigned start_char = ttUlong(data + indexMap + 16 + mid * 12);
            unsigned end_char = ttUlong(data + indexMap + 16 + mid * 12 + 4);
            if (static_cast<unsigned>(unicodeCodepoint) < start_char) {
                high = mid;
            } else if (static_cast<unsigned>(unicodeCodepoint) > end_char) {
                low = mid + 1;
            } else {
                unsigned start_glyph = ttUlong(data + indexMap + 16 + mid * 12 + 8);
                if (format == 12) {
                    return start_glyph + unicodeCodepoint - start_char;
                }
                else { // format == 13
                    return start_glyph;
                }
            }
        }
        return 0; // not found
    }

    assert(false);
    return 0;
}

float TrueType::scaleForPixelHeight(const FontInfo& info, float height) {
    int fheight = ttShort(info.data.data() + info.hhea + 4) - ttShort(info.data.data() + info.hhea + 6);
    return (height / static_cast<float>(fheight));
}

float TrueType::scaleForMappingEmToPixels(const FontInfo& info, float pixels) {
    int unitsPerEm = ttUshort(info.data.data() + info.head + 18);
    return (pixels / static_cast<float>(unitsPerEm));
}

void TrueType::getGlyphBitmapBoxSubpixel(
    const FontInfo& info,
    int glyph,
    const Vector2& scale,
    const Vector2& shift,
    int* ix0,
    int* iy0,
    int* ix1,
    int* iy1
) {
    int x0 = 0;
    int y0 = 0;
    int x1 = 0;
    int y1 = 0;

    if (!getGlyphBox(info, glyph, &x0, &y0, &x1, &y1)) {
        //スペース文字
        if (ix0) {
            *ix0 = 0;
        }
        if (iy0) {
            *iy0 = 0;
        }
        if (ix1) {
            *ix1 = 0;
        }
        if (iy1) {
            *iy1 = 0;
        }
    } else {
        //インテグラルBボックスへの移動
        if (ix0) {
            *ix0 = floorf(x0 * scale.x + shift.x);
        }
        if (iy0) {
            *iy0 = floorf(-y1 * scale.y + shift.y);
        }
        if (ix1) {
            *ix1 = ceilf(x1 * scale.x + shift.x);
        }
        if (iy1) {
            *iy1 = ceilf(-y0 * scale.y + shift.y);
        }
    }
}

bool TrueType::getGlyphBox(
    const FontInfo& info,
    int glyphIndex,
    int* x0,
    int* y0,
    int* x1,
    int* y1
) {
    int g = getGlyfOffset(info, glyphIndex);
    if (g < 0) {
        return false;
    }

    if (x0) {
        *x0 = ttShort(info.data.data() + g + 2);
    }
    if (y0) {
        *y0 = ttShort(info.data.data() + g + 4);
    }
    if (x1) {
        *x1 = ttShort(info.data.data() + g + 6);
    }
    if (y1) {
        *y1 = ttShort(info.data.data() + g + 8);
    }

    return true;
}

void TrueType::packBegin(
    PackContext& packContext,
    unsigned char* pixels,
    int pw,
    int ph,
    int strideInBytes,
    int padding,
    void* allocContext
) {
    rpContext* context = new rpContext();
    int numNodes = pw - padding;
    rpNode* nodes = new rpNode[numNodes];

    packContext.userAllocatorContext = allocContext;
    packContext.width = pw;
    packContext.height = ph;
    packContext.pixels = pixels;
    packContext.packInfo = context;
    packContext.nodes = nodes;
    packContext.padding = padding;
    packContext.strideInBytes = (strideInBytes != 0) ? strideInBytes : pw;
    packContext.hOversample = 1;
    packContext.vOversample = 1;
    packContext.skipMissing = 0;

    context->initialize(pw - padding, ph - padding, nodes, numNodes);

    if (pixels) {
        memset(pixels, 0, pw * ph);
    }
}

unsigned TrueType::findTable(const unsigned char* data, const char* tag) {
    int numTables = ttUshort(data + 4);
    constexpr unsigned tabledir = 12;

    for (int i = 0; i < numTables; ++i) {
        unsigned loc = tabledir + 16 * i;
        if (ttTag(data + loc, tag)) {
            return ttUlong(data + loc + 8);
        }
    }

    return 0;
}

int TrueType::getGlyfOffset(const FontInfo& info, int glyphIndex) {
    int g1 = 0;
    int g2 = 0;

    //範囲外
    if (glyphIndex >= info.numGlyphs) {
        return -1;
    }
    //不明なインデックス→グリフマップフォーマット
    if (info.indexToLocFormat >= 2) {
        return -1;
    }

    if (info.indexToLocFormat == 0) {
        g1 = info.glyf + ttUshort(info.data.data() + info.loca + glyphIndex * 2) * 2;
        g2 = info.glyf + ttUshort(info.data.data() + info.loca + glyphIndex * 2 + 2) * 2;
    } else {
        g1 = info.glyf + ttUlong(info.data.data() + info.loca + glyphIndex * 4);
        g2 = info.glyf + ttUlong(info.data.data() + info.loca + glyphIndex * 4 + 4);
    }

    //もし長さが0なら-1を返す
    return ((g1 == g2) ? -1 : g1);
}

unsigned char TrueType::ttByte(const unsigned char* p) {
    return *p;
}

short TrueType::ttShort(const unsigned char* p) {
    return p[0] * 256 + p[1];
}

unsigned short TrueType::ttUshort(const unsigned char* p) {
    return p[0] * 256 + p[1];
}

unsigned TrueType::ttUlong(const unsigned char* p) {
    return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3];
}

bool TrueType::ttTag(const unsigned char* p, const char* str) {
    if (p[0] != str[0]) {
        return false;
    }
    if (p[1] != str[1]) {
        return false;
    }
    if (p[2] != str[2]) {
        return false;
    }
    if (p[3] != str[3]) {
        return false;
    }

    return true;
}
