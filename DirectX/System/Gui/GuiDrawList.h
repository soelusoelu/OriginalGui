﻿#pragma once

#include "DrawCornerFlags.h"
#include "GuiDrawCommand.h"
#include "GuiDrawListSharedData.h"
#include "../../Math/Math.h"
#include <vector>

struct GuiDrawVertex {
    Vector2 pos;
    Vector2 uv;
    Vector4 color;
};

class GuiDrawList {
public:
    GuiDrawList();
    ~GuiDrawList();
    GuiDrawList(const GuiDrawList&) = delete;
    GuiDrawList& operator=(const GuiDrawList&) = delete;

    void addRectFilled(
        const Vector2& min,
        const Vector2& max,
        const Vector4& color = Vector4(1.f, 1.f, 1.f, 1.f),
        float rounding = 0.f,
        DrawCornerFlags flag = DrawCornerFlags::NONE
    );
    void addConvexPolyFilled(
        const std::vector<Vector2>& points,
        const Vector4& color
    );

private:
    void pathLineTo(const Vector2& pos);
    void pathFillConvex(const Vector4& color);
    void pathRect(
        const Vector2& rectMin,
        const Vector2& rectMax,
        float rounding = 0.f,
        DrawCornerFlags flag = DrawCornerFlags::NONE
    );
    //12段の円にあらかじめ計算された角度を使用する
    void pathArcToFast(
        const Vector2& center,
        float radius,
        int minOf12,
        int maxOf12
    );

    void primReserve(int idxCount, int vtxCount);
    void primRect(const Vector2& a, const Vector2& b, const Vector4& color);

private:
    //描画に必要なもの
    std::vector<GuiDrawCommand> mCommandBuffer;
    std::vector<GuiDrawVertex> mVertexBuffer;
    std::vector<unsigned short> mIndexBuffer;

    const GuiDrawListSharedData* mData;
    unsigned mVertexCurrentIndex;
    GuiDrawVertex* mVertexWritePtr;
    unsigned short* mIndexWritePtr;
    std::vector<Vector2> mPath;
    GuiDrawCommand mCommandHeader;
};