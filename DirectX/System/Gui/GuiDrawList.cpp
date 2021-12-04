#include "GuiDrawList.h"
#include "GuiContext.h"
#include <cassert>

GuiDrawList::GuiDrawList(GuiContext& context)
    : mContext(context)
{
    mCommandBuffer.emplace_back(GuiDrawCommand());
}

GuiDrawList::~GuiDrawList() = default;

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
        pathRect(min, max, rounding, flag);
        pathFillConvex(color);
    } else {
        primReserve(6, 4);
        primRect(min, max, color);
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
    int vtxCount = pointCount;
    primReserve(idxCount, vtxCount);
    GuiVertex vertex = { Vector2::zero, uv, color };
    for (int i = 0; i < vtxCount; ++i) {
        vertex.pos = points[i];
        mVertexBuffer.emplace_back(vertex);
    }

    auto idx = static_cast<unsigned short>(mVertexBuffer.size());
    for (int i = 2; i < pointCount; ++i) {
        mIndexBuffer.emplace_back(idx);
        mIndexBuffer.emplace_back(idx + i - 1);
        mIndexBuffer.emplace_back(idx + i);
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
    DrawCornerFlags flag
) {
    rounding = Math::Min(rounding, Math::abs(rectMax.x - rectMin.x) * ((flag == DrawCornerFlags::TOP) || (flag == DrawCornerFlags::BOT) ? 0.5f : 1.f) - 1.f);
    rounding = Math::Min(rounding, Math::abs(rectMax.y - rectMin.y) * ((flag == DrawCornerFlags::LEFT) || (flag == DrawCornerFlags::RIGHT) ? 0.5f : 1.f) - 1.f);

    if (rounding <= 0.f || flag == DrawCornerFlags::NONE) {
        pathLineTo(rectMin);
        pathLineTo(Vector2(rectMax.x, rectMin.y));
        pathLineTo(rectMax);
        pathLineTo(Vector2(rectMin.x, rectMax.y));
    } else {
        float rounding_tl = (flag == DrawCornerFlags::TOP_LEFT) ? rounding : 0.f;
        float rounding_tr = (flag == DrawCornerFlags::TOP_RIGHT) ? rounding : 0.f;
        float rounding_br = (flag == DrawCornerFlags::BOT_RIGHT) ? rounding : 0.f;
        float rounding_bl = (flag == DrawCornerFlags::BOT_LEFT) ? rounding : 0.f;
        pathArcToFast(Vector2(rectMin.x + rounding_tl, rectMin.y + rounding_tl), rounding_tl, 6, 9);
        pathArcToFast(Vector2(rectMax.x - rounding_tr, rectMin.y + rounding_tr), rounding_tr, 9, 12);
        pathArcToFast(Vector2(rectMax.x - rounding_br, rectMax.y - rounding_br), rounding_br, 0, 3);
        pathArcToFast(Vector2(rectMin.x + rounding_bl, rectMax.y - rounding_bl), rounding_bl, 3, 6);
    }
}

void GuiDrawList::pathArcToFast(
    const Vector2& center,
    float radius,
    int minOf12,
    int maxOf12
) {
    if (radius == 0.f || minOf12 > maxOf12) {
        mPath.emplace_back(center);
        return;
    }

    mPath.reserve(mPath.size() + (maxOf12 - minOf12 + 1));
    for (int a = minOf12; a <= maxOf12; ++a) {
        //const auto& c = data->ArcFastVtx[a % IM_ARRAYSIZE(_Data->ArcFastVtx)];
        //path.emplace_back(Vector2(center.x + c.x * radius, center.y + c.y * radius));
    }
}

void GuiDrawList::primReserve(int idxCount, int vtxCount) {
    assert(idxCount >= 0 && vtxCount >= 0);

    auto& drawCmd = mCommandBuffer[mCommandBuffer.size() - 1];
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
