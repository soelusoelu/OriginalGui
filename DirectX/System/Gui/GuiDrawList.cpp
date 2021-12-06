#include "GuiDrawList.h"
#include "GuiContext.h"
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

void GuiDrawList::addRectFilled(
    const Vector2& min,
    const Vector2& max,
    const Vector4& color,
    float rounding,
    DrawCornerFlags flag
) {
    //アルファ値0なら無視
    if (color.w == 0) {
        return;
    }

    if (rounding > 0.0f) {
        Flag f;
        f.set(static_cast<unsigned>(flag));
        pathRect(min, max, rounding, f);
        pathFillConvex(color);
    } else {
        primReserve(6, 4);
        primRect(min, max, color);
    }
}

void GuiDrawList::updateWindowPosition(const Vector2& amount) {
    for (auto&& v : mVertexBuffer) {
        v.pos += amount;
    }
}

void GuiDrawList::updateWindowSize(const Vector2& amount) {

}

void GuiDrawList::updateVertexPosition(const Vector2& amount, unsigned startIndex, unsigned stopIndex) {
    for (unsigned i = startIndex; i <= stopIndex; ++i) {
        mVertexBuffer[i].pos += amount;
    }
}

const std::vector<GuiDrawCommand>& GuiDrawList::getDrawCommands() const {
    return mCommandBuffer;
}

const std::vector<GuiVertex>& GuiDrawList::getVertexBuffer() const {
    return mVertexBuffer;
}

const std::vector<unsigned short>& GuiDrawList::getIndexBuffer() const {
    return mIndexBuffer;
}

void GuiDrawList::addConvexPolyFilled(
    const std::vector<Vector2>& points,
    const Vector4& color
) {
    if (points.size() < 3) {
        return;
    }

    auto uv = mContext.getDrawListSharedData().texUvWhitePixel;

    //アンチエイリアスなしの塗りつぶし
    auto pointCount = points.size();
    int idxCount = (pointCount - 2) * 3;
    primReserve(idxCount, pointCount);

    auto idx = static_cast<unsigned short>(mVertexBuffer.size());
    for (int i = 2; i < pointCount; ++i) {
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

void GuiDrawList::pathFillConvex(const Vector4& color) {
    addConvexPolyFilled(mPath, color);
    mPath.clear();
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

void GuiDrawList::primReserve(int idxCount, int vtxCount) {
    assert(idxCount >= 0 && vtxCount >= 0);

    auto& drawCmd = mCommandBuffer[mCurrentLayer];
    drawCmd.elemCount += idxCount;

    mVertexBuffer.reserve(mVertexBuffer.size() + vtxCount);
    mIndexBuffer.reserve(mIndexBuffer.size() + idxCount);
}

void GuiDrawList::primRect(const Vector2& a, const Vector2& c, const Vector4& color) {
    Vector2 b(c.x, a.y);
    Vector2 d(a.x, c.y);
    Vector2 uv(mContext.getDrawListSharedData().texUvWhitePixel);
    auto idx = static_cast<unsigned short>(mVertexBuffer.size());
    mIndexBuffer.emplace_back(idx);
    mIndexBuffer.emplace_back(idx + 1);
    mIndexBuffer.emplace_back(idx + 2);
    mIndexBuffer.emplace_back(idx);
    mIndexBuffer.emplace_back(idx + 2);
    mIndexBuffer.emplace_back(idx + 3);

    GuiVertex vertex = { a, uv, color };
    mVertexBuffer.emplace_back(vertex);
    vertex.pos = b;
    mVertexBuffer.emplace_back(vertex);
    vertex.pos = c;
    mVertexBuffer.emplace_back(vertex);
    vertex.pos = d;
    mVertexBuffer.emplace_back(vertex);
}
