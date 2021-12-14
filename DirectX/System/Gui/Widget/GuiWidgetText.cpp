#include "GuiWidgetText.h"
#include "GuiWidgetConstant.h"
#include "../GuiContext.h"
#include "../GuiWindow.h"
#include "../../../Device/DrawString.h"
#include "../../../Utility/AsciiHelper.h"
#include "../../../Utility/StringUtil.h"

GuiWidgetText::GuiWidgetText(GuiWindow& window)
    : mWindow(window)
{
}

GuiWidgetText::~GuiWidgetText() = default;

void GuiWidgetText::update() {
}

void GuiWidgetText::text(const std::string& text) {
    auto& dl = mWindow.getDrawList();
    const auto& nextPos = mWindow.getNextWidgetPosition();

    //親クラスのテキスト表示関数を呼び出す
    auto start = dl.getVertexCount();
    auto pos = nextPos + Vector2(0.f, GuiWidgetConstant::TEXT_HEIGHT_PADDING);
    dl.addText(text, pos, GuiWidgetConstant::TEXT_HEIGHT);
    auto numPoints = dl.getVertexCount() - start;

    //ウィジェット位置を更新
    mWindow.setNextWidgetPosition(
        nextPos + Vector2(0.f, GuiWidgetConstant::TEXT_HEIGHT + GuiWidgetConstant::TEXT_HEIGHT_PADDING * 2.f)
    );

    mTexts.emplace_back(GuiTextInfo{ text, pos, Pivot::LEFT_TOP, start, numPoints });
}

unsigned GuiWidgetText::text(
    const std::string& text,
    const Vector2& pos,
    float pixelSizeY,
    int capacity,
    const Vector4& color,
    Pivot pivot
) {
    auto& dl = mWindow.getDrawList();

    //親クラスのテキスト表示関数を呼び出す
    auto start = dl.getVertexCount();
    dl.addText(
        text,
        pos,
        pixelSizeY,
        capacity,
        color,
        pivot
    );
    auto numPoints = dl.getVertexCount() - start;

    mTexts.emplace_back(GuiTextInfo{ text, pos - mWindow.getPosition(), pivot, numPoints / 4, start, numPoints});

    return (mTexts.size() - 1);
}

void GuiWidgetText::changeText(
    unsigned index,
    const std::string& text
) {
    auto& info = mTexts[index];

    //まず全文字空白にする
    clearText(info);

    //文字を変更していく
    auto& dl = mWindow.getDrawList();
    auto size = getSize(info);
    auto pos = StringUtil::calcPositionFromPivot(getPosition(info), text, size, info.pivot);
    auto len = static_cast<unsigned>(text.length());
    len = Math::Min(len, info.capacity);
    for (unsigned i = 0; i < len; ++i) {
        auto leftTop = AsciiHelper::calcPositionRateToAscii(
            text[i],
            DrawString::WIDTH_CHAR_COUNT,
            DrawString::HEIGHT_CHAR_COUNT
        );

        //文字更新
        int idx = info.startIndex + i * 4;
        dl.setVertexPosition(pos, idx);
        dl.setVertexPosition(pos + Vector2(size.x, 0.f), idx + 1);
        dl.setVertexPosition(pos + size, idx + 2);
        dl.setVertexPosition(pos + Vector2(0.f, size.y), idx + 3);

        dl.setVertexUV(leftTop, idx);
        dl.setVertexUV(leftTop + Vector2(DrawString::CHAR_WIDTH_RATE, 0.f), idx + 1);
        dl.setVertexUV(leftTop + DrawString::CHAR_RATE, idx + 2);
        dl.setVertexUV(leftTop + Vector2(0.f, DrawString::CHAR_HEIGHT_RATE), idx + 3);

        pos.x += size.x;
    }

    info.text = text;
}

void GuiWidgetText::clearText(GuiTextInfo& info) {
    mWindow.getDrawList().setVertexUVs(
        mWindow.getContext().getDrawListSharedData().texUvTransparentPixel,
        info.startIndex,
        info.numPoints
    );
}

Vector2 GuiWidgetText::getPosition(const GuiTextInfo& info) const {
    return info.origin + mWindow.getPosition();
}

Vector2 GuiWidgetText::getSize(const GuiTextInfo& info) const {
    const auto& vb = mWindow.getDrawList().getVertexBuffer();
    return (vb[info.startIndex + 2].pos - vb[info.startIndex].pos);
}
