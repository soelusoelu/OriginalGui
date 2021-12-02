#include "GuiDrawList.h"
#include <cassert>

GuiDrawList::GuiDrawList()
    : mData(nullptr)
    , mVertexCurrentIndex(0)
    , mVertexWritePtr(nullptr)
    , mIndexWritePtr(nullptr)
{
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

void GuiDrawList::addConvexPolyFilled(
    const std::vector<Vector2>& points,
    const Vector4& color
) {
    if (points.size() < 3) {
        return;
    }

    auto uv = mData->texUvWhitePixel;

    //アンチエイリアスなしの塗りつぶし
    int idxCount = (points.size() - 2) * 3;
    int vtxCount = points.size();
    primReserve(idxCount, vtxCount);
    for (int i = 0; i < vtxCount; ++i) {
        mVertexWritePtr[0].pos = points[i];
        mVertexWritePtr[0].uv = uv;
        mVertexWritePtr[0].color = color;
        ++mVertexWritePtr;
    }
    for (int i = 2; i < points.size(); ++i) {
        mIndexWritePtr[0] = static_cast<unsigned short>(mVertexCurrentIndex);
        mIndexWritePtr[1] = static_cast<unsigned short>(mVertexCurrentIndex + i - 1);
        mIndexWritePtr[2] = static_cast<unsigned short>(mVertexCurrentIndex + i);
        mIndexWritePtr += 3;
    }
    mVertexCurrentIndex += vtxCount;
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

    auto vtxBufferOldSize = mVertexBuffer.size();
    mVertexBuffer.resize(vtxBufferOldSize + vtxCount);
    mVertexWritePtr = mVertexBuffer.data() + vtxBufferOldSize;

    auto idxBufferOldSize = mIndexBuffer.size();
    mIndexBuffer.resize(idxBufferOldSize + idxCount);
    mIndexWritePtr = mIndexBuffer.data() + idxBufferOldSize;
}

void GuiDrawList::primRect(const Vector2& a, const Vector2& c, const Vector4& color) {
    Vector2 b(c.x, a.y);
    Vector2 d(a.x, c.y);
    Vector2 uv(mData->texUvWhitePixel);
    unsigned short idx = static_cast<unsigned>(mVertexCurrentIndex);
    mIndexWritePtr[0] = idx;
    mIndexWritePtr[1] = idx + 1;
    mIndexWritePtr[2] = idx + 2;
    mIndexWritePtr[3] = idx;
    mIndexWritePtr[4] = idx + 2;
    mIndexWritePtr[5] = idx + 3;
    mIndexWritePtr += 6;

    mVertexWritePtr[0].pos = a; mVertexWritePtr[0].uv = uv; mVertexWritePtr[0].color = color;
    mVertexWritePtr[1].pos = b; mVertexWritePtr[1].uv = uv; mVertexWritePtr[1].color = color;
    mVertexWritePtr[2].pos = c; mVertexWritePtr[2].uv = uv; mVertexWritePtr[2].color = color;
    mVertexWritePtr[3].pos = d; mVertexWritePtr[3].uv = uv; mVertexWritePtr[3].color = color;
    mVertexWritePtr += 4;
    mVertexCurrentIndex += 4;
}
