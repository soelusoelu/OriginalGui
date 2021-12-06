#pragma once

#include "DrawCornerFlags.h"
#include "GuiDrawCommand.h"
#include "GuiDrawListSharedData.h"
#include "GuiVertex.h"
#include "../../Device/Flag.h"
#include "../../Math/Math.h"
#include <vector>

class GuiContext;

class GuiDrawList {
public:
    GuiDrawList(GuiContext& context);
    ~GuiDrawList();
    GuiDrawList(const GuiDrawList&) = delete;
    GuiDrawList& operator=(const GuiDrawList&) = delete;

    //描画レイヤーを増やす
    void addLayer();
    //レイヤーを指定する
    void setLayer(unsigned layer);

    //丸みを考慮した矩形を追加する
    void addRectFilled(
        const Vector2& min,
        const Vector2& max,
        const Vector4& color = Vector4(1.f, 1.f, 1.f, 1.f),
        float rounding = 0.f,
        DrawCornerFlags flag = DrawCornerFlags::NONE
    );

    //ウィンドウ位置を更新する
    void updateWindowPosition(const Vector2& amount);
    //ウィンドウサイズを更新する
    //未実装
    void updateWindowSize(const Vector2& amount);
    //指定された範囲の頂点位置を更新する
    void updateVertexPosition(const Vector2& amount, unsigned startIndex, unsigned stopIndex);

    //描画コマンドを取得する
    const std::vector<GuiDrawCommand>& getDrawCommands() const;
    //頂点配列を取得する
    const std::vector<GuiVertex>& getVertexBuffer() const;
    //インデックス配列を取得する
    const std::vector<unsigned short>& getIndexBuffer() const;

private:
    //頂点配列をもとにポリゴンを形成する
    void addConvexPolyFilled(
        const std::vector<Vector2>& points,
        const Vector4& color
    );

    //パス配列に頂点を追加する
    void pathLineTo(const Vector2& pos);
    //パス配列からポリゴンを形成し、終わったら空にする
    void pathFillConvex(const Vector4& color);
    //丸みを考慮した矩形をパス配列に追加する
    void pathRect(
        const Vector2& rectMin,
        const Vector2& rectMax,
        float rounding,
        const Flag& flag
    );
    //指定された分だけ丸め、パス配列に追加する
    //速いが雑
    void pathArcToFast(
        const Vector2& center,
        float radius,
        int minOf12,
        int maxOf12
    );

    //頂点・インデックス配列の容量を確保する
    void primReserve(int idxCount, int vtxCount);
    //単純な矩形を頂点・インデックス配列に追加する
    void primRect(const Vector2& a, const Vector2& b, const Vector4& color);

public:
    static constexpr int COMMAND_WINDOW_INDEX = 0;
    static constexpr int COMMAND_WIDGETS_INDEX = 1;

private:
    GuiContext& mContext;

    //描画に必要なもの
    std::vector<GuiDrawCommand> mCommandBuffer;
    std::vector<GuiVertex> mVertexBuffer;
    std::vector<unsigned short> mIndexBuffer;

    std::vector<Vector2> mPath;
    GuiDrawCommand mCommandHeader;
    unsigned mCurrentLayer;
};
