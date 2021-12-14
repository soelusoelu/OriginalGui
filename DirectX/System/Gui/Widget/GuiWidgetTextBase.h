#pragma once

#include "../../../Math/Math.h"
#include "../../../Transform/Pivot.h"
#include <string>
#include <vector>

class GuiWindow;

struct GuiTextBaseVertexInfo {
    unsigned textStartIndex = 0;
    unsigned textNumPoints = 0;
};

class GuiWidgetTextBase {
public:
    GuiWidgetTextBase(GuiWindow& window);
    virtual ~GuiWidgetTextBase();
    GuiWidgetTextBase(const GuiWidgetTextBase&) = delete;
    GuiWidgetTextBase& operator=(const GuiWidgetTextBase&) = delete;

    void updateBase();
    virtual void update() {};

protected:
    //テキストを作成する
    void createText(
        const std::string& text,
        const Vector2& pos,
        float pixelSizeY,
        int capacity = -1,
        const Vector4& color = Vector4(1.f, 1.f, 1.f, 1.f),
        Pivot pivot = Pivot::LEFT_TOP
    );

protected:
    GuiWindow& mWindow;
    std::vector<std::string> mTexts;
    std::vector<GuiTextBaseVertexInfo> mTextsVertexInfo;
};
