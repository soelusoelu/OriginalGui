#include "TrueType.h"
#include "GuiPlatform.h"
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
        info.numGlyphs = ushort(data + t + 4);
    } else {
        info.numGlyphs = 0xffff;
    }

    //後で楽するために先に検索
    int numTables = ushort(data + info.cmap + 2);
    for (int i = 0; i < numTables; ++i) {
        unsigned encodingRecord = info.cmap + 4 + i * 8;

        auto platform = static_cast<GuiPlatform>(ushort(data + encodingRecord));
        if (platform == GuiPlatform::GUI_PLATFORM_MICROSOFT) {
            auto platformMS = static_cast<GuiPlatformMicrosoft>(ushort(data + encodingRecord + 2));
            if (platformMS == GuiPlatformMicrosoft::GUI_PLATFORM_MS_UNICODE_BMP
                || platformMS == GuiPlatformMicrosoft::GUI_PLATFORM_MS_UNICODE_FULL) {
                info.indexMap = info.cmap + ulong(data + encodingRecord + 4);
                break;
            }
        } else if (platform == GuiPlatform::GUI_PLATFORM_UNICODE) {
            info.indexMap = info.cmap + ulong(data + encodingRecord + 4);
            break;
        }
    }

    if (info.indexMap == 0) {
        return false;
    }

    info.indexToLocFormat = ushort(data + info.head + 50);

    return true;
}

int TrueType::findGlyphIndex(const FontInfo& info, int unicodeCodepoint) {
    const auto data = info.data.data();
    unsigned indexMap = info.indexMap;

    unsigned short format = ushort(data + indexMap + 0);
    if (format == 0) { //appleバイトエンコーディング
        int bytes = ushort(data + indexMap + 2);
        if (unicodeCodepoint < bytes - 6) {
            return byte(data + indexMap + 6 + unicodeCodepoint);
        }
        return 0;
    } else if (format == 6) {
        unsigned first = ushort(data + indexMap + 6);
        unsigned count = ushort(data + indexMap + 8);
        if (static_cast<unsigned>(unicodeCodepoint) >= first && static_cast<unsigned>(unicodeCodepoint) < first + count) {
            return ushort(data + indexMap + 10 + (unicodeCodepoint - first) * 2);
        }
        return 0;
    } else if (format == 2) { //日本語
        assert(false);
        return 0;
    } else if (format == 4) { //Windowsフォントの標準マッピング
        unsigned short segcount = ushort(data + indexMap + 6) >> 1;
        unsigned short searchRange = ushort(data + indexMap + 8) >> 1;
        unsigned short entrySelector = ushort(data + indexMap + 10);
        unsigned short rangeShift = ushort(data + indexMap + 12) >> 1;

        //セグメントのバイナリ検索を行う
        unsigned endCount = indexMap + 14;
        unsigned search = endCount;

        if (unicodeCodepoint > 0xffff) {
            return 0;
        }

        if (unicodeCodepoint >= ushort(data + search + rangeShift * 2)) {
            search += rangeShift * 2;
        }

        search -= 2;
        while (entrySelector) {
            unsigned short end;
            searchRange >>= 1;
            end = ushort(data + search + searchRange * 2);
            if (unicodeCodepoint > end) {
                search += searchRange * 2;
            }
            --entrySelector;
        }
        search += 2;

        {
            unsigned short offset, start;
            unsigned short item = static_cast<unsigned short>(((search - endCount) >> 1));

            assert(unicodeCodepoint <= ushort(data + endCount + 2 * item));
            start = ushort(data + indexMap + 14 + segcount * 2 + 2 + 2 * item);
            if (unicodeCodepoint < start) {
                return 0;
            }

            offset = ushort(data + indexMap + 14 + segcount * 6 + 2 + 2 * item);
            if (offset == 0) {
                return static_cast<unsigned short>((unicodeCodepoint + ushort(data + indexMap + 14 + segcount * 4 + 2 + 2 * item)));
            }

            return ushort(data + offset + (unicodeCodepoint - start) * 2 + indexMap + 14 + segcount * 6 + 2 + 2 * item);
        }
    } else if (format == 12 || format == 13) {
        unsigned ngroups = ulong(data + indexMap + 12);
        int low, high;
        low = 0; high = static_cast<int>(ngroups);
        //バイナリーサーチの正しいグループ
        while (low < high) {
            int mid = low + ((high - low) >> 1); //ラウンドダウンするので、low <= mid < high
            unsigned start_char = ulong(data + indexMap + 16 + mid * 12);
            unsigned end_char = ulong(data + indexMap + 16 + mid * 12 + 4);
            if (static_cast<unsigned>(unicodeCodepoint) < start_char) {
                high = mid;
            } else if (static_cast<unsigned>(unicodeCodepoint) > end_char) {
                low = mid + 1;
            } else {
                unsigned start_glyph = ulong(data + indexMap + 16 + mid * 12 + 8);
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

unsigned TrueType::findTable(const unsigned char* data, const char* tag) {
    int numTables = ushort(data + 4);
    constexpr unsigned tabledir = 12;

    for (int i = 0; i < numTables; ++i) {
        unsigned loc = tabledir + 16 * i;
        if (stbttTag(data + loc, tag)) {
            return ulong(data + loc + 8);
        }
    }

    return 0;
}

unsigned char TrueType::byte(const unsigned char* p) {
    return *p;
}

unsigned short TrueType::ushort(const unsigned char* p) {
    return p[0] * 256 + p[1];
}

unsigned TrueType::ulong(const unsigned char* p) {
    return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3];
}

bool TrueType::stbttTag(const unsigned char* p, const char* str) {
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
