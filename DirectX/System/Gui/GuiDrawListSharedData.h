#pragma once

#include "../../Math/Math.h"
#include <array>

struct GuiDrawListSharedData {
    Vector2 texUvWhitePixel = Vector2(0.09375f, 0.03125f);
    float circleSegmentMaxError = 1.6f;
    std::array<Vector2, 12> arcFastVertex;
    std::array<unsigned char, 64> circleSegmentCounts = { 0 };

    static constexpr int CIRCLE_AUTO_SEGMENT_MIN = 12;
    static constexpr int CIRCLE_AUTO_SEGMENT_MAX = 512;

    GuiDrawListSharedData() {
        int size = arcFastVertex.size();
        for (int i = 0; i < size; ++i) {
            float a = (i * Math::TwoPI) / static_cast<float>(size);
            arcFastVertex[i] = Vector2(cosf(a), sinf(a));
        }

        size = circleSegmentCounts.size();
        for (int i = 0; i < size; ++i) {
            float radius = i + 1.f;
            int segmentCount = calcCircleAutoSegment(radius, circleSegmentMaxError);
            circleSegmentCounts[i] = static_cast<unsigned char>(Math::Min(segmentCount, UCHAR_MAX));
        }
    }

    int calcCircleAutoSegment(float radius, float maxError) const {
        return Math::clamp(
            static_cast<int>(Math::TwoPI / acosf((radius - maxError) / radius)),
            CIRCLE_AUTO_SEGMENT_MIN,
            CIRCLE_AUTO_SEGMENT_MAX
        );
    }
};
