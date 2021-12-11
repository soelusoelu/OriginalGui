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

    //四角ポリゴンで形成したラインを追加する
    void addPolyline(
        const std::vector<Vector2>& points,
        const Vector4& color,
        float thickness,
        bool closed
    );
    //丸みを考慮した矩形(枠のみ)を追加する
    void addRect(
        const Vector2& min,
        const Vector2& max,
        const Vector4& color = Vector4(1.f, 1.f, 1.f, 1.f),
        float rounding = 0.f,
        DrawCornerFlags flag = DrawCornerFlags::NONE,
        float thickness = 1.f
    );
    //丸みを考慮した矩形を追加し、塗りつぶす
    void addRectFilled(
        const Vector2& min,
        const Vector2& max,
        const Vector4& color = Vector4(1.f, 1.f, 1.f, 1.f),
        float rounding = 0.f,
        DrawCornerFlags flag = DrawCornerFlags::NONE
    );
    //それぞれの頂点の色を指定した矩形を追加し塗りつぶす
    void addRectFilledMultiColor(
        const Vector2& min,
        const Vector2& max,
        const Vector4& upperLeftColor,
        const Vector4& upperRightColor,
        const Vector4& bottomRightColor,
        const Vector4& bottomLeftColor
    );
    //円をを追加し塗りつぶす
    void addCircleFilled(
        const Vector2& center,
        float radius,
        const Vector4& color = Vector4(1.f, 1.f, 1.f, 1.f)
    );

    //ウィンドウ位置を更新する
    void updateWindowPosition(const Vector2& amount);
    //ウィンドウサイズを更新する
    //未実装
    void updateWindowSize(const Vector2& amount);
    //指定された範囲の頂点位置を更新する
    void updateVertexPosition(const Vector2& amount, unsigned startIndex, unsigned numPoints);
    //指定された頂点の頂点色を変更する
    void setVertexColor(const Vector4& color, unsigned index);
    //指定された範囲の頂点色を変更する
    void setVertexColors(const Vector4& color, unsigned startIndex, unsigned numPoints);

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
    //丸みを考慮した矩形をパス配列に追加する
    void pathRect(
        const Vector2& rectMin,
        const Vector2& rectMax,
        float rounding,
        const Flag& flag
    );
    void pathArcTo(
        const Vector2& center,
        float radius,
        float min,
        float max,
        int numSegments
    );
    //指定された分だけ丸め、パス配列に追加する
    //速いが雑
    void pathArcToFast(
        const Vector2& center,
        float radius,
        int minOf12,
        int maxOf12
    );
    //パス配列からラインを形成し、終わったら空にする
    void pathStroke(const Vector4& color, float thickness, bool closed);
    //パス配列からポリゴンを形成し、終わったら空にする
    void pathFillConvex(const Vector4& color);

    //頂点・インデックス配列の容量を確保する
    void primReserve(int idxCount, int vtxCount);
    //頂点配列に追加する
    void primWriteVertex(const Vector2& pos, const Vector2& uv, const Vector4& color);
    //単純な矩形を頂点・インデックス配列に追加する
    void primRect(const Vector2& min, const Vector2& max, const Vector4& color);

    void normalize2fOverZero(Vector2& v) const;

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
