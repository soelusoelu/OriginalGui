#include "GuiDrawList.h"
#include "GuiContext.h"
#include "../../Device/DrawString.h"
#include "../../Utility/AsciiHelper.h"
#include "../../Utility/StringUtil.h"
#include <cassert>

GuiDrawList::GuiDrawList(GuiContext& context)
    : mContext(context)
    , mCommandBuffer(2) //0: ウィンドウ, 1: ウィジェット
    , mCurrentLayer(0)
{
}

GuiDrawList::~GuiDrawList() = default;

void GuiDrawList::addLayer() {
    mCommandBuffer.emplace_back(GuiDrawCommand());
}

void GuiDrawList::setLayer(unsigned layer) {
    assert(layer < mCommandBuffer.size());
    mCurrentLayer = layer;
}

void GuiDrawList::addPolyline(
    const std::vector<Vector2>& points,
    const Vector4& color,
    float thickness,
    bool closed
) {
    auto pointsCount = points.size();
    if (pointsCount < 2) {
        return;
    }

    const auto& uv = mContext.getDrawListSharedData().texUvWhitePixel;
    int lineCount = closed ? pointsCount : pointsCount - 1;

    //ノンテクスチャベース、ノンアンチエイリアスのライン
    int idxCount = lineCount * 6;
    int vtxCount = lineCount * 4; //エッジを共有しない
    primReserve(idxCount, vtxCount);

    for (int i1 = 0; i1 < lineCount; ++i1) {
        int i2 = (i1 + 1) % pointsCount;
        const auto& p1 = points[i1];
        const auto& p2 = points[i2];

        //線の中心から太くするため
        auto d = p2 - p1;
        normalize2fOverZero(d);
        d *= (thickness * 0.5f);

        auto idx = static_cast<unsigned short>(mVertexBuffer.size());
        mIndexBuffer.emplace_back(idx);
        mIndexBuffer.emplace_back(idx + 1);
        mIndexBuffer.emplace_back(idx + 2);
        mIndexBuffer.emplace_back(idx);
        mIndexBuffer.emplace_back(idx + 2);
        mIndexBuffer.emplace_back(idx + 3);

        GuiVertex vertex = { Vector2(p1.x + d.y, p1.y - d.x), uv, color };
        mVertexBuffer.emplace_back(vertex);
        vertex.pos = Vector2(p2.x + d.y, p2.y - d.x);
        mVertexBuffer.emplace_back(vertex);
        vertex.pos = Vector2(p2.x - d.y, p2.y + d.x);
        mVertexBuffer.emplace_back(vertex);
        vertex.pos = Vector2(p1.x - d.y, p1.y + d.x);
        mVertexBuffer.emplace_back(vertex);
    }
}

void GuiDrawList::addTriangle(
    const Vector2& p1,
    const Vector2& p2,
    const Vector2& p3,
    const Vector4& color,
    float thickness
) {
    pathLineTo(p1);
    pathLineTo(p2);
    pathLineTo(p3);
    pathStroke(color, thickness, true);
}

void GuiDrawList::addTriangleFilled(
    const Vector2& p1,
    const Vector2& p2,
    const Vector2& p3,
    const Vector4& color
) {
    pathLineTo(p1);
    pathLineTo(p2);
    pathLineTo(p3);
    pathFillConvex(color);
}

void GuiDrawList::addRect(
    const Vector2& min,
    const Vector2& max,
    const Vector4& color,
    float rounding,
    DrawCornerFlags flag,
    float thickness
) {
    Flag f(static_cast<unsigned>(flag));
    pathRect(min + Vector2(0.5f, 0.5f), max - Vector2(0.49f, 0.49f), rounding, f);
    pathStroke(color, thickness, true);
}

void GuiDrawList::addRectFilled(
    const Vector2& min,
    const Vector2& max,
    const Vector4& color,
    float rounding,
    DrawCornerFlags flag
) {
    if (rounding > 0.0f) {
        Flag f(static_cast<unsigned>(flag));
        pathRect(min, max, rounding, f);
        pathFillConvex(color);
    } else {
        primReserve(6, 4);
        primRect(min, max, color);
    }
}

void GuiDrawList::addRectFilledMultiColor(
    const Vector2& min,
    const Vector2& max,
    const Vector4& upperLeftColor,
    const Vector4& upperRightColor,
    const Vector4& bottomRightColor,
    const Vector4& bottomLeftColor
) {
    primReserve(6, 4);

    const auto& uv = mContext.getDrawListSharedData().texUvWhitePixel;

    auto idx = static_cast<unsigned short>(mVertexBuffer.size());
    mIndexBuffer.emplace_back(idx);
    mIndexBuffer.emplace_back(idx + 1);
    mIndexBuffer.emplace_back(idx + 2);
    mIndexBuffer.emplace_back(idx);
    mIndexBuffer.emplace_back(idx + 2);
    mIndexBuffer.emplace_back(idx + 3);

    primWriteVertex(min, uv, upperLeftColor);
    primWriteVertex(Vector2(max.x, min.y), uv, upperRightColor);
    primWriteVertex(max, uv, bottomRightColor);
    primWriteVertex(Vector2(min.x, max.y), uv, bottomLeftColor);
}

void GuiDrawList::addCircle(
    const Vector2& center,
    float radius,
    const Vector4& color,
    float thickness
) {
    const auto& drawListSharedData = mContext.getDrawListSharedData();
    const auto& circleSegmentCounts = drawListSharedData.circleSegmentCounts;

    //半径の大きさによって区分数を決める
    int numSegments = 0;
    int radiusIdx = static_cast<int>(radius) - 1;
    if (radiusIdx < circleSegmentCounts.size()) {
        //事前に計算された値を使用
        numSegments = circleSegmentCounts[radiusIdx];
    } else {
        //新たに計算して求める
        numSegments = drawListSharedData.calcCircleAutoSegment(radius, drawListSharedData.circleSegmentMaxError);
    }

    if (numSegments == 12) {
        pathArcToFast(center, radius - 0.5f, 0, 12 - 1);
    } else {
        float max = Math::TwoPI * (static_cast<float>(numSegments) - 1.f) / static_cast<float>(numSegments);
        pathArcTo(center, radius - 0.5f, 0.f, max, numSegments - 1);
    }

    pathStroke(color, thickness, true);
}

void GuiDrawList::addCircleFilled(
    const Vector2& center,
    float radius,
    const Vector4& color
) {
    const auto& drawListSharedData = mContext.getDrawListSharedData();
    const auto& circleSegmentCounts = drawListSharedData.circleSegmentCounts;

    //半径の大きさによって区分数を決める
    int numSegments = 0;
    int radiusIdx = static_cast<int>(radius) - 1;
    if (radiusIdx < circleSegmentCounts.size()) {
        //事前に計算された値を使用
        numSegments = circleSegmentCounts[radiusIdx];
    } else {
        //新たに計算して求める
        numSegments = drawListSharedData.calcCircleAutoSegment(radius, drawListSharedData.circleSegmentMaxError);
    }

    if (numSegments == 12) {
        pathArcToFast(center, radius, 0, 12 - 1);
    } else {
        float max = Math::TwoPI * (static_cast<float>(numSegments) - 1.f) / static_cast<float>(numSegments);
        pathArcTo(center, radius, 0.f, max, numSegments - 1);
    }

    pathFillConvex(color);
}

void GuiDrawList::addText(
    const std::string& text,
    const Vector2& pos,
    float pixelSizeY,
    const Vector4& color,
    Pivot pivot
) {
    //文字数分容量確保
    auto len = static_cast<unsigned>(text.length());
    primReserve(len * 6, len * 4);

    //指定のピクセルサイズから実際の描画サイズを求める
    auto scaleY = pixelSizeY / DrawString::CHAR_HEIGHT;
    auto pixelSizeX = DrawString::CHAR_WIDTH * scaleY;
    auto size = Vector2(pixelSizeX, pixelSizeY);

    //全文字を配列に追加していく
    auto drawPos = pos;
    for (const auto& c : text) {
        char t = AsciiHelper::clampCharToAscii(c);
        auto leftTop = AsciiHelper::calcPositionRateToAscii(
            t,
            DrawString::WIDTH_CHAR_COUNT,
            DrawString::HEIGHT_CHAR_COUNT
        );
        auto rightBottom = leftTop + AsciiHelper::calcSizeRateToAscii(
            DrawString::WIDTH_CHAR_COUNT,
            DrawString::HEIGHT_CHAR_COUNT
        );

        //文字追加
        primRectUV(
            drawPos,
            drawPos + size,
            leftTop,
            rightBottom,
            color
        );

        drawPos.x += size.x;
    }
}

void GuiDrawList::updateWindowPosition(const Vector2& amount) {
    for (auto&& v : mVertexBuffer) {
        v.pos += amount;
    }
}

void GuiDrawList::updateWindowSize(const Vector2& amount) {

}

void GuiDrawList::updateVertexPosition(const Vector2& amount, unsigned startIndex, unsigned numPoints) {
    for (unsigned i = 0; i < numPoints; ++i) {
        mVertexBuffer[i + startIndex].pos += amount;
    }
}

void GuiDrawList::setVertexColor(const Vector4& color, unsigned index) {
    mVertexBuffer[index].color = color;
}

void GuiDrawList::setVertexColors(const Vector4& color, unsigned startIndex, unsigned numPoints) {
    for (unsigned i = 0; i < numPoints; ++i) {
        mVertexBuffer[i + startIndex].color = color;
    }
}

const std::vector<GuiDrawCommand>& GuiDrawList::getDrawCommands() const {
    return mCommandBuffer;
}

const std::vector<GuiVertex>& GuiDrawList::getVertexBuffer() const {
    return mVertexBuffer;
}

unsigned GuiDrawList::getVertexCount() const {
    return static_cast<unsigned>(mVertexBuffer.size());
}

const std::vector<unsigned short>& GuiDrawList::getIndexBuffer() const {
    return mIndexBuffer;
}

void GuiDrawList::addConvexPolyFilled(
    const std::vector<Vector2>& points,
    const Vector4& color
) {
    auto pointsCount = points.size();
    if (pointsCount < 3) {
        return;
    }

    const auto& uv = mContext.getDrawListSharedData().texUvWhitePixel;

    //アンチエイリアスなしの塗りつぶし
    int idxCount = (pointsCount - 2) * 3;
    primReserve(idxCount, pointsCount);

    auto idx = static_cast<unsigned short>(mVertexBuffer.size());
    for (int i = 2; i < pointsCount; ++i) {
        mIndexBuffer.emplace_back(idx);
        mIndexBuffer.emplace_back(idx + i - 1);
        mIndexBuffer.emplace_back(idx + i);
    }

    GuiVertex vertex = { Vector2::zero, uv, color };
    for (const auto& p : points) {
        vertex.pos = p;
        mVertexBuffer.emplace_back(vertex);
    }
}

void GuiDrawList::pathLineTo(const Vector2& pos) {
    mPath.emplace_back(pos);
}

void GuiDrawList::pathRect(
    const Vector2& rectMin,
    const Vector2& rectMax,
    float rounding,
    const Flag& flag
) {
    if (rounding <= 0.f || flag.check(static_cast<unsigned>(DrawCornerFlags::NONE))) {
        pathLineTo(rectMin);
        pathLineTo(Vector2(rectMax.x, rectMin.y));
        pathLineTo(rectMax);
        pathLineTo(Vector2(rectMin.x, rectMax.y));
    } else {
        float roundingTL = 0.f;
        float roundingTR = 0.f;
        float roundingBR = 0.f;
        float roundingBL = 0.f;
        if (flag.check(static_cast<unsigned>(DrawCornerFlags::TOP_LEFT))) {
            roundingTL = rounding;
        }
        if (flag.check(static_cast<unsigned>(DrawCornerFlags::TOP_RIGHT))) {
            roundingTR = rounding;
        }
        if (flag.check(static_cast<unsigned>(DrawCornerFlags::BOT_RIGHT))) {
            roundingBR = rounding;
        }
        if (flag.check(static_cast<unsigned>(DrawCornerFlags::BOT_LEFT))) {
            roundingBL = rounding;
        }

        //指定された位置だけ丸める
        pathArcToFast(Vector2(rectMin.x + roundingTL, rectMin.y + roundingTL), roundingTL, 6, 9);
        pathArcToFast(Vector2(rectMax.x - roundingTR, rectMin.y + roundingTR), roundingTR, 9, 12);
        pathArcToFast(Vector2(rectMax.x - roundingBR, rectMax.y - roundingBR), roundingBR, 0, 3);
        pathArcToFast(Vector2(rectMin.x + roundingBL, rectMax.y - roundingBL), roundingBL, 3, 6);
    }
}

void GuiDrawList::pathArcTo(
    const Vector2& center,
    float radius,
    float min,
    float max,
    int numSegments
) {
    if (radius == 0.f) {
        pathLineTo(center);
        return;
    }

    mPath.reserve(mPath.size() + (numSegments + 1));
    for (int i = 0; i <= numSegments; ++i) {
        float a = min + (static_cast<float>(i) / static_cast<float>(numSegments)) * (max - min);
        pathLineTo(Vector2(center.x + cosf(a) * radius, center.y + sinf(a) * radius));
    }
}

void GuiDrawList::pathArcToFast(
    const Vector2& center,
    float radius,
    int minOf12,
    int maxOf12
) {
    assert(0 <= minOf12 && minOf12 < 13);
    assert(0 <= maxOf12 && maxOf12 < 13);

    if (radius == 0.f || minOf12 > maxOf12) {
        pathLineTo(center);
        return;
    }

    mPath.reserve(mPath.size() + (maxOf12 - minOf12 + 1));
    const auto& arcFastVertex = mContext.getDrawListSharedData().arcFastVertex;
    for (int i = minOf12; i <= maxOf12; ++i) {
        const auto& c = arcFastVertex[i % arcFastVertex.size()];
        pathLineTo(Vector2(center.x + c.x * radius, center.y + c.y * radius));
    }
}

void GuiDrawList::pathStroke(
    const Vector4& color,
    float thickness,
    bool closed
) {
    addPolyline(mPath, color, thickness, closed);
    mPath.clear();
}

void GuiDrawList::pathFillConvex(
    const Vector4& color
) {
    addConvexPolyFilled(mPath, color);
    mPath.clear();
}

void GuiDrawList::primReserve(
    int idxCount,
    int vtxCount
) {
    assert(idxCount >= 0 && vtxCount >= 0);

    auto& drawCmd = mCommandBuffer[mCurrentLayer];
    drawCmd.elemCount += idxCount;

    mVertexBuffer.reserve(mVertexBuffer.size() + vtxCount);
    mIndexBuffer.reserve(mIndexBuffer.size() + idxCount);
}

void GuiDrawList::primWriteVertex(
    const Vector2& pos,
    const Vector2& uv,
    const Vector4& color
) {
    mVertexBuffer.emplace_back(GuiVertex{ pos, uv, color });
}

void GuiDrawList::primRect(
    const Vector2& min,
    const Vector2& max,
    const Vector4& color
) {
    const auto& uv = mContext.getDrawListSharedData().texUvWhitePixel;

    auto idx = static_cast<unsigned short>(mVertexBuffer.size());
    mIndexBuffer.emplace_back(idx);
    mIndexBuffer.emplace_back(idx + 1);
    mIndexBuffer.emplace_back(idx + 2);
    mIndexBuffer.emplace_back(idx);
    mIndexBuffer.emplace_back(idx + 2);
    mIndexBuffer.emplace_back(idx + 3);

    primWriteVertex(min, uv, color);
    primWriteVertex(Vector2(max.x, min.y), uv, color);
    primWriteVertex(max, uv, color);
    primWriteVertex(Vector2(min.x, max.y), uv, color);
}

void GuiDrawList::primRectUV(
    const Vector2& posMin,
    const Vector2& posMax,
    const Vector2& uvLeftTop,
    const Vector2& uvRightBottom,
    const Vector4& color
) {
    auto idx = static_cast<unsigned short>(mVertexBuffer.size());
    mIndexBuffer.emplace_back(idx);
    mIndexBuffer.emplace_back(idx + 1);
    mIndexBuffer.emplace_back(idx + 2);
    mIndexBuffer.emplace_back(idx);
    mIndexBuffer.emplace_back(idx + 2);
    mIndexBuffer.emplace_back(idx + 3);

    primWriteVertex(posMin, uvLeftTop, color);
    primWriteVertex(Vector2(posMax.x, posMin.y), Vector2(uvRightBottom.x, uvLeftTop.y), color);
    primWriteVertex(posMax, uvRightBottom, color);
    primWriteVertex(Vector2(posMin.x, posMax.y), Vector2(uvLeftTop.x, uvRightBottom.y), color);
}

void GuiDrawList::normalize2fOverZero(Vector2& v) const {
    float invLen = 1.f / v.length();
    v *= invLen;
}
