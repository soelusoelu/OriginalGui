#pragma once

#include "../../../Math/Math.h"
#include "../../../Transform/Pivot.h"
#include <string>
#include <vector>

class GuiWindow;

struct GuiTextInfo {
    std::string text = "";
    Vector2 origin = Vector2::zero;
    Pivot pivot = Pivot::LEFT_TOP;
    unsigned capacity = 0;
    unsigned startIndex = 0;
    unsigned numPoints = 0;
};

class GuiWidgetText {
public:
    GuiWidgetText(GuiWindow& window);
    ~GuiWidgetText();
    GuiWidgetText(const GuiWidgetText&) = delete;
    GuiWidgetText& operator=(const GuiWidgetText&) = delete;

    void update();

    //テキストを追加する
    void text(const std::string& text);
    //テキストを追加する
    unsigned text(
        const std::string& text,
        const Vector2& pos,
        float pixelSizeY,
        int capacity = -1,
        const Vector4& color = Vector4(1.f, 1.f, 1.f, 1.f),
        Pivot pivot = Pivot::LEFT_TOP
    );
    //テキストを書き換える
    void changeText(
        unsigned index,
        const std::string& text
    );
    //テキストを削除する
    void clearText(GuiTextInfo& info);
    //テキストの位置を取得する
    Vector2 getPosition(const GuiTextInfo& info) const;
    //文字のサイズを取得する
    Vector2 getSize(const GuiTextInfo& info) const;

private:
    GuiWindow& mWindow;
    std::vector<GuiTextInfo> mTexts;
};
