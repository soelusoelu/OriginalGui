#include "GuiFontInitializer.h"
#include "GuiFontAtlas.h"
#include "GuiFontAtlasCustomRect.h"
#include "GuiFontConfig.h"
#include "GuiRect.h"
#include "PackContext.h"
#include "TrueType.h"
#include <algorithm>
#include <cassert>

void GuiFontInitializer::initialize(GuiFontAtlas& font) {
    GuiFontBuildSrcData srcData{};
    GuiFontBuildDstData dstData{};

    const auto& config = font.getFont().config;
    if (!TrueType::initializeFont(srcData.fontInfo, config)) {
        return;
    }
    srcData.srcRanges = config.glyphRanges.data();

    auto maxElement = std::max_element(config.glyphRanges.begin(), config.glyphRanges.end());
    srcData.glyphsHighest = static_cast<int>(*maxElement);
    dstData.glyphsHighest = srcData.glyphsHighest;

    int totalGlyphsCount = 0;
    if (!checkRequestedCodepoint(srcData, dstData, config, totalGlyphsCount)) {
        return;
    }

    unpackBitMap(srcData, dstData);
    packVirtualCanvas(srcData, font, config, totalGlyphsCount);

    PackContext packContext = { 0 };
    startPacking(packContext, font);
}

bool GuiFontInitializer::checkRequestedCodepoint(
    GuiFontBuildSrcData& src,
    GuiFontBuildDstData& dst,
    const GuiFontConfig& config,
    int& totalGlyphsCount
) {
    src.glyphsSet.create(src.glyphsHighest + 1);
    if (dst.glyphsSet.storage.empty()) {
        dst.glyphsSet.create(dst.glyphsHighest + 1);
    }

    const auto& glyphRanges = config.glyphRanges;
    for (size_t i = 0; i < glyphRanges.size(); i += 2) {
        for (unsigned codepoint = glyphRanges[i]; codepoint <= glyphRanges[i + 1]; ++codepoint) {
            //既存のグリフを上書きしない
            if (dst.glyphsSet.testBit(codepoint)) {
                continue;
            }
            if (!TrueType::findGlyphIndex(src.fontInfo, codepoint)) {
                continue;
            }

            ++src.glyphsCount;
            ++dst.glyphsCount;
            src.glyphsSet.setBit(codepoint);
            dst.glyphsSet.setBit(codepoint);
            ++totalGlyphsCount;
        }
    }

    return true;
}

void GuiFontInitializer::unpackBitMap(
    GuiFontBuildSrcData& src,
    GuiFontBuildDstData& dst
) {
    src.glyphsList.reserve(src.glyphsCount);
    unpackBitVectorToFlatIndexList(src.glyphsList, src.glyphsSet);
    src.glyphsSet.clear();
    assert(src.glyphsList.size() == src.glyphsCount);

    //もうGuiFontBuildDstDataは使わないので削除
    dst.glyphsSet.clear();
}

void GuiFontInitializer::unpackBitVectorToFlatIndexList(
    std::vector<int>& dst,
    const GuiBitVector& src
) {
    const auto& storage = src.storage;
    static_assert(sizeof(storage.data()[0]) == sizeof(int));

    const auto begin = storage.begin();
    for (auto itr = begin; itr < storage.end(); ++itr) {
        if (unsigned entries = *itr) {
            for (unsigned n = 0; n < 32; ++n) {
                if (entries & (static_cast<unsigned>(1) << n)) {
                    dst.emplace_back(static_cast<int>((((itr - begin) << 5) + n)));
                }
            }
        }
    }
}

void GuiFontInitializer::packVirtualCanvas(
    GuiFontBuildSrcData& src,
    GuiFontAtlas& font,
    const GuiFontConfig& config,
    int totalGlyphsCount
) {
    //パック文字データを割り当て、パック文字バッファを非パック文字としてフラグを立てる(x0=y0=x1=y1=0)
    std::vector<GuiRect> bufRects(totalGlyphsCount);
    std::vector<PackedChar> bufPackedchars(totalGlyphsCount);

    int totalSurface = 0;

    if (src.glyphsCount == 0) {
        return;
    }

    src.rects = bufRects.data();
    src.packedChars = bufPackedchars.data();

    //フォーマット変換
    src.packRange.fontSize = config.pixelsSize;
    src.packRange.firstUnicodeCodepointInRange = 0;
    src.packRange.arrayOfUnicodeCodepoints = src.glyphsList.data();
    src.packRange.numChars = src.glyphsList.size();
    src.packRange.chardataForRange = src.packedChars;
    src.packRange.hOversample = static_cast<unsigned char>(config.oversampleH);
    src.packRange.vOversample = static_cast<unsigned char>(config.oversampleV);

    //パックする必要のあるすべての長方形のサイズを集める
    float scale = 0.f;
    if (config.pixelsSize > 0.f) {
        scale = TrueType::scaleForPixelHeight(src.fontInfo, config.pixelsSize);
    } else {
        scale = TrueType::scaleForMappingEmToPixels(src.fontInfo, -config.pixelsSize);
    }

    int padding = font.getTexGlyphPadding();
    for (int i = 0; i < src.glyphsList.size(); ++i) {
        int glyphIndexInFont = TrueType::findGlyphIndex(src.fontInfo, src.glyphsList[i]);
        assert(glyphIndexInFont != 0);

        int x0 = 0;
        int y0 = 0;
        int x1 = 0;
        int y1 = 0;
        Vector2 scl(scale * config.oversampleH, scale * config.oversampleV);
        TrueType::getGlyphBitmapBoxSubpixel(src.fontInfo, glyphIndexInFont, scl, Vector2::zero, &x0, &y0, &x1, &y1);
        src.rects[i].w = static_cast<unsigned short>(x1 - x0 + padding + config.oversampleH - 1);
        src.rects[i].h = static_cast<unsigned short>(y1 - y0 + padding + config.oversampleV - 1);
        totalSurface += src.rects[i].w * src.rects[i].h;
    }

    //テクスチャの横幅決定
    int surfaceSqrt = static_cast<int>(Math::sqrt(static_cast<float>(totalSurface))) + 1;
    auto& w = font.getFont().texWidth;
    if (surfaceSqrt >= 4096 * 0.7f) {
        w = 4096;
    } else if (surfaceSqrt >= 2048 * 0.7f) {
        w = 2048;
    } else if (surfaceSqrt >= 1024 * 0.7f) {
        w = 1024;
    } else {
        w = 512;
    }
}

void GuiFontInitializer::startPacking(PackContext& packContext, const GuiFontAtlas& font) {
    constexpr int TEX_HEIGHT_MAX = 1024 * 32;

    TrueType::packBegin(
        packContext,
        nullptr,
        font.getFont().texWidth,
        TEX_HEIGHT_MAX,
        0,
        font.getTexGlyphPadding(),
        nullptr
    );

    //ImFontAtlasBuildPackCustomRects(atlas, spc.pack_info);
}

void GuiFontInitializer::fontAtlasBuildPackCustomRects(GuiFontAtlas& font, void* contextOpaque) {
    rpContext* packContext = static_cast<rpContext*>(contextOpaque);
    assert(packContext);

    //std::vector<GuiFontAtlasCustomRect>& user_rects = font.CustomRects;
    //IM_ASSERT(user_rects.Size >= 1); // We expect at least the default custom rects to be registered, else something went wrong.

    //ImVector<stbrp_rect> pack_rects;
    //pack_rects.resize(user_rects.Size);
    //memset(pack_rects.Data, 0, (size_t)pack_rects.size_in_bytes());
    //for (int i = 0; i < user_rects.Size; i++) {
    //    pack_rects[i].w = user_rects[i].Width;
    //    pack_rects[i].h = user_rects[i].Height;
    //}
    //stbrp_pack_rects(packContext, &pack_rects[0], pack_rects.Size);
    //for (int i = 0; i < pack_rects.Size; i++)
    //    if (pack_rects[i].was_packed) {
    //        user_rects[i].X = pack_rects[i].x;
    //        user_rects[i].Y = pack_rects[i].y;
    //        IM_ASSERT(pack_rects[i].w == user_rects[i].Width && pack_rects[i].h == user_rects[i].Height);
    //        atlas->TexHeight = ImMax(atlas->TexHeight, pack_rects[i].y + pack_rects[i].h);
    //    }
}
