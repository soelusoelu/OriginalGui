#include "GuiFontCreater.h"
#include "../../Math/Math.h"
#include <cassert>
#include <vector>

void GuiFontCreater::createDefaultFont(const GuiFontConfig* config) {
    auto cfg = (config) ? *config : GuiFontConfig();

    if (!config) {
        cfg.oversampleH = 1;
        cfg.oversampleV = 1;
        cfg.pixelSnapH = true;
    }
    //デフォルトサイズは13ピクセル
    if (cfg.pixelsSize <= 0.f) {
        cfg.pixelsSize = 13.f;
    }
    cfg.ellipsisChar = static_cast<wchar_t>(0x0085);
    cfg.glyphOffset.y = floorf(cfg.pixelsSize / 13.f); //13ユニットごとにオフセットを1追加

    const auto glyphRanges = (cfg.glyphRanges) ? cfg.glyphRanges : getGlyphRangesDefault();
    auto font = addFontFromMemoryCompressedBase85TTF(
        PROGGY_CLEAN_TTF_COMPRESSED_DATA_BASE85,
        cfg.pixelsSize,
        &cfg,
        glyphRanges
    );
    //return font;
}

const wchar_t* GuiFontCreater::getGlyphRangesDefault() {
    constexpr wchar_t ranges[] = {
        0x0020, 0x00FF, //基礎ラテン語＋ラテン語サプリメント
        0,
    };
    return &ranges[0];
}

std::shared_ptr<GuiFont> GuiFontCreater::addFontFromMemoryTTF(
    void* fontData,
    int fontSize,
    float pixelsSize,
    const GuiFontConfig* config,
    const wchar_t* glyphRanges
) {
    //assert(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    auto cfg = config ? *config : GuiFontConfig();
    assert(cfg.fontData == nullptr);

    cfg.fontData = fontData;
    cfg.fontDataSize = fontSize;
    cfg.pixelsSize = pixelsSize;
    if (glyphRanges) {
        cfg.glyphRanges = glyphRanges;
    }
    //return AddFont(&cfg);
    return nullptr;
}

std::shared_ptr<GuiFont> GuiFontCreater::addFontFromMemoryCompressedTTF(
    const void* compressedFontData,
    int compressedFontSize,
    float pixelsSize,
    const GuiFontConfig* config,
    const wchar_t* glyphRanges
) {
    unsigned bufDecompressedSize = stbDecompressLength(static_cast<const unsigned char*>(compressedFontData));
    std::vector<unsigned char> bufDecompressedData(bufDecompressedSize);
    stbDecompress(bufDecompressedData.data(), (const unsigned char*)compressedFontData, compressedFontSize);

    auto cfg = config ? *config : GuiFontConfig();
    assert(cfg.fontData == nullptr);

    cfg.fontDataOwnedByAtlas = true;
    return addFontFromMemoryTTF(bufDecompressedData.data(), bufDecompressedSize, pixelsSize, &cfg, glyphRanges);
}

std::shared_ptr<GuiFont> GuiFontCreater::addFontFromMemoryCompressedBase85TTF(
    const char* compressedFontDataBase85,
    float pixelsSize,
    const GuiFontConfig* config,
    const wchar_t* glyphRanges
) {
    constexpr int compressedTTFSize = ((PROGGY_CLEAN_TTF_COMPRESSED_DATA_BASE85_SIZE + 4) / 5) * 4;
    unsigned char compressedTTF[compressedTTFSize] = { 0 };
    decode85(compressedTTF, reinterpret_cast<const unsigned char*>(compressedFontDataBase85));
    auto font = addFontFromMemoryCompressedTTF(compressedTTF, compressedTTFSize, pixelsSize, config, glyphRanges);
    return std::shared_ptr<GuiFont>();
}

unsigned GuiFontCreater::stbDecompress(unsigned char* dst, const unsigned char* src, unsigned length) {
    if (stbIn4(src, 0) != 0x57bC0000) {
        return 0;
    }
    if (stbIn4(src, 4) != 0) { // error! stream is > 4GB
        return 0;
    }

    auto olen = stbDecompressLength(src);
    stb__barrier_in_b = src;
    stb__barrier_out_e = dst + olen;
    stb__barrier_out_b = dst;
    src += 16;

    stb__dout = dst;
    while (true) {
        const auto oldSrc = src;
        src = stbDecompressToken(src);

        if (src == oldSrc) {
            if (*src == 0x05 && src[1] == 0xfa) {
                assert(stb__dout == dst + olen);

                if (stb__dout != dst + olen) {
                    return 0;
                }
                if (stbAdler32(1, dst, olen) != stbIn4(src, 2)) {
                    return 0;
                }

                return olen;
            } else {
                assert(false);
                return 0;
            }
        }

        assert(stb__dout <= dst + olen);

        if (stb__dout > dst + olen) {
            return 0;
        }
    }
}

unsigned GuiFontCreater::stbDecompressLength(const unsigned char* input) {
    return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

const unsigned char* GuiFontCreater::stbDecompressToken(const unsigned char* i) {
    if (*i >= 0x20) {
        if (*i >= 0x80) {
            stbMatch(stb__dout - i[1] - 1, i[0] - 0x80 + 1);
            i += 2;
        } else if (*i >= 0x40) {
            stbMatch(stb__dout - (stbIn2(i, 0) - 0x4000 + 1), i[2] + 1);
            i += 3;
        } else /* *i >= 0x20 */ {
            stbLit(i + 1, i[0] - 0x20 + 1);
            i += 1 + (i[0] - 0x20 + 1);
        }
    } else {
        if (*i >= 0x18) {
            stbMatch(stb__dout - (stbIn3(i, 0) - 0x180000 + 1), i[3] + 1);
            i += 4;
        } else if (*i >= 0x10) {
            stbMatch(stb__dout - (stbIn3(i, 0) - 0x100000 + 1), stbIn2(i, 3) + 1);
            i += 5;
        } else if (*i >= 0x08) {
            stbLit(i + 2, stbIn2(i, 0) - 0x0800 + 1);
            i += 2 + (stbIn2(i, 0) - 0x0800 + 1);
        } else if (*i == 0x07) {
            stbLit(i + 3, stbIn2(i, 1) + 1);
            i += 3 + (stbIn2(i, 1) + 1);
        } else if (*i == 0x06) {
            stbMatch(stb__dout - (stbIn3(i, 1) + 1), i[4] + 1);
            i += 5;
        } else if (*i == 0x04) {
            stbMatch(stb__dout - (stbIn3(i, 1) + 1), stbIn2(i, 4) + 1);
            i += 6;
        }
    }

    return i;
}

void GuiFontCreater::decode85(unsigned char* dst, const unsigned char* src) {
    while (*src) {
        unsigned tmp = 
            decode85Byte(src[0])
            + 85 * (decode85Byte(src[1])
            + 85 * (decode85Byte(src[2])
            + 85 * (decode85Byte(src[3])
            + 85 * decode85Byte(src[4])
        )));
        dst[0] = ((tmp >> 0) & 0xFF);
        dst[1] = ((tmp >> 8) & 0xFF);
        dst[2] = ((tmp >> 16) & 0xFF);
        dst[3] = ((tmp >> 24) & 0xFF);
        src += 5;
        dst += 4;
    }
}

unsigned GuiFontCreater::decode85Byte(char c) {
    return (c >= '\\') ? c - 36 : c - 35;
}

int GuiFontCreater::stbIn2(const unsigned char* src, int x) {
    return (src[x] << 8) + src[x + 1];
}

int GuiFontCreater::stbIn3(const unsigned char* src, int x) {
    return (src[x] << 16) + stbIn2(src, x + 1);
}

int GuiFontCreater::stbIn4(const unsigned char* src, int x) {
    return (src[x] << 24) + stbIn3(src, x + 1);
}

void GuiFontCreater::stbMatch(const unsigned char* data, unsigned int length) {
    assert(stb__dout + length <= stb__barrier_out_e);

    if (stb__dout + length > stb__barrier_out_e) {
        stb__dout += length;
        return;
    }
    if (data < stb__barrier_out_b) {
        stb__dout = stb__barrier_out_e + 1;
        return;
    }
    while (length--) {
        *stb__dout++ = *data++;
    }
}

void GuiFontCreater::stbLit(const unsigned char* data, unsigned int length) {
    assert(stb__dout + length <= stb__barrier_out_e);

    if (stb__dout + length > stb__barrier_out_e) {
        stb__dout += length;
        return;
    }
    if (data < stb__barrier_in_b) {
        stb__dout = stb__barrier_out_e + 1;
        return;
    }

    memcpy(stb__dout, data, length);
    stb__dout += length;
}

unsigned GuiFontCreater::stbAdler32(unsigned adler32, unsigned char* buffer, unsigned buflen) {
    constexpr unsigned long ADLER_MOD = 65521;
    unsigned long s1 = adler32 & 0xffff;
    unsigned long s2 = adler32 >> 16;
    unsigned long blocklen = buflen % 5552;

    unsigned long i;
    while (buflen) {
        for (i = 0; i + 7 < blocklen; i += 8) {
            s1 += buffer[0], s2 += s1;
            s1 += buffer[1], s2 += s1;
            s1 += buffer[2], s2 += s1;
            s1 += buffer[3], s2 += s1;
            s1 += buffer[4], s2 += s1;
            s1 += buffer[5], s2 += s1;
            s1 += buffer[6], s2 += s1;
            s1 += buffer[7], s2 += s1;

            buffer += 8;
        }

        for (; i < blocklen; ++i) {
            s1 += *buffer++;
            s2 += s1;
        }

        s1 %= ADLER_MOD;
        s2 %= ADLER_MOD;
        buflen -= blocklen;
        blocklen = 5552;
    }

    return static_cast<unsigned>((s2 << 16)) + static_cast<unsigned>(s1);
}
