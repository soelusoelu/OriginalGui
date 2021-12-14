#pragma once

#include "GuiDataType.h"
#include "../../../Math/Math.h"
#include <memory>
#include <string>
#include <vector>

class GuiWindow;
class GuiWidgetText;

struct GuiFrameInfo {
    std::string label = "";
    unsigned startIndex = 0;
    unsigned numPoints = 0;

    GuiDataType type = GuiDataType::INT;
    void* data = nullptr;
    unsigned valueTextIndex = 0;
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
    virtual void onUpdateSelectFrame(const GuiFrameInfo& frame) {};

protected:
    //1本の基本的なフレームを作成する
    unsigned createSingleFrame(const std::string& label);
    void createFrameText(unsigned index, GuiDataType type, void* v);

    //フレームの数値文字列を更新する
    void updateNumberText();

    //保有している数値を文字列に変換する
    std::string numberToText(const GuiFrameInfo& frame);
    std::string numberToText(const void* data, GuiDataType type);
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
    std::unique_ptr<GuiWidgetText> mText;
    std::vector<GuiFrameInfo> mFrames;
    //選択中のフレームのインデックス
    //-1なら掴んでいない
    int mFrameIndex;
};
