#pragma once

#include "../../../Math/Math.h"
#include <vector>

class GuiWindow;

struct GuiFrameInfo {
    unsigned startIndex = 0;
    unsigned numPoints = 0;
};

class GuiWidgetFrameBase {
public:
    GuiWidgetFrameBase(GuiWindow& window);
    virtual ~GuiWidgetFrameBase();
    GuiWidgetFrameBase(const GuiWidgetFrameBase&) = delete;
    GuiWidgetFrameBase& operator=(const GuiWidgetFrameBase&) = delete;

    void baseUpdate();
    virtual void update() {};
    virtual void onSelectFrame(const GuiFrameInfo& frame) {};
    virtual void onUpdateFrame(const GuiFrameInfo& frame) {};

protected:
    //1本の基本的なフレームを作成する
    unsigned createSingleFrame();

    //指定のフレームの位置を取得する
    const Vector2& getFramePosition(unsigned index) const;
    const Vector2& getFramePosition(const GuiFrameInfo& frame) const;
    //選択中のフレームを取得する
    const GuiFrameInfo& getSelectingFrame() const;
    //フレームを選択中か
    bool isSelectingFrame() const;

private:
    //マウス位置からフレームを選択する
    void selectFrame();

protected:
    GuiWindow& mWindow;
    std::vector<GuiFrameInfo> mFrames;
    //選択中のフレームのインデックス
    //-1なら掴んでいない
    int mFrameIndex;
};
