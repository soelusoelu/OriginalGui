#pragma once

#include "DrawCornerFlags.h"
#include "GuiDrawCommand.h"
#include "GuiDrawListSharedData.h"
#include "GuiVertex.h"
#include "../../Math/Math.h"
#include <vector>

class GuiContext;

class GuiDrawList {
public:
    GuiDrawList(GuiContext& context);
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

    void updateWindowPosition(const Vector2& amount);
    void updateWindowSize(const Vector2& amount);
    const std::vector<GuiDrawCommand>& getDrawCommands() const;
    const std::vector<GuiVertex>& getVertexBuffer() const;
    const std::vector<unsigned short>& getIndexBuffer() const;

private:
    void addConvexPolyFilled(
        const std::vector<Vector2>& points,
        const Vector4& color
    );

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
    GuiContext& mContext;

    //描画に必要なもの
    std::vector<GuiDrawCommand> mCommandBuffer;
    std::vector<GuiVertex> mVertexBuffer;
    std::vector<unsigned short> mIndexBuffer;

    std::vector<Vector2> mPath;
    GuiDrawCommand mCommandHeader;

    static constexpr int COMMAND_WINDOW_INDEX = 0;
};
