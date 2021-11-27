#include "GuiFontInitializer.h"
#include "GuiFontAtlas.h"
#include "GuiFontConfig.h"
#include "TrueType.h"
#include <algorithm>
#include <cassert>

void GuiFontInitializer::initialize(GuiFontAtlas& font) {
    GuiFontBuildSrcData srcData{};
    GuiFontBuildDstData dstData{};

    const auto& config = font.getFont().config;
    TrueType::initializeFont(srcData.fontInfo, config);
    srcData.srcRanges = config.glyphRanges.data();

    auto maxElement = std::max_element(config.glyphRanges.begin(), config.glyphRanges.end());
    srcData.glyphsHighest = static_cast<int>(*maxElement);
    dstData.glyphsHighest = srcData.glyphsHighest;

    int totalGlyphsCount = 0;
    if (!checkRequestedCodepoint(srcData, dstData, config, totalGlyphsCount)) {
        return;
    }
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
        for (unsigned codepoint = glyphRanges[i * 2]; codepoint <= glyphRanges[i * 2 + 1]; ++codepoint) {
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
