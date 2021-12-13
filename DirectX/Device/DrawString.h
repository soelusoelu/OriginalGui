#pragma once

#include "../Device/FileOperator.h"
#include "../Math/Math.h"
#include "../Transform/Pivot.h"
#include <list>
#include <memory>
#include <string>

class Sprite;
class SpriteInstancingDrawer;

class DrawString
    : public FileOperator
{
public:
    DrawString();
    ~DrawString();
    DrawString(const DrawString&) = delete;
    DrawString& operator=(const DrawString&) = delete;

    virtual void saveAndLoad(JsonObject& inObj, FileMode mode) override;

    void initialize();
    //保持してる文字列を一括描画
    void drawAll(const Matrix4& proj) const;
    //保持してる文字列をすべて削除
    void clear();

    void drawNumber(
        int number,
        const Vector2& position,
        const Vector2& scale = Vector2::one,
        const Vector3& color = ColorPalette::white,
        float alpha = 1.f,
        Pivot pivot = Pivot::LEFT_TOP
    );
    void drawNumber(
        float number,
        const Vector2& position,
        const Vector2& scale = Vector2::one,
        int decimalDigits = 1,
        const Vector3& color = ColorPalette::white,
        float alpha = 1.f,
        Pivot pivot = Pivot::LEFT_TOP
    );
    void drawString(
        const std::string& alphabet,
        const Vector2& position,
        const Vector2& scale = Vector2::one,
        const Vector3& color = ColorPalette::white,
        float alpha = 1.f,
        Pivot pivot = Pivot::LEFT_TOP
    );

private:
    struct ParamInt {
        int number;
        Vector2 position;
        Vector2 scale;
        Vector3 color;
        float alpha;
        Pivot pivot;
    };
    struct ParamFloat {
        float number;
        Vector2 position;
        Vector2 scale;
        int decimalDigits;
        Vector3 color;
        float alpha;
        Pivot pivot;
    };
    struct ParamString {
        std::string alphabet;
        Vector2 position;
        Vector2 scale;
        Vector3 color;
        float alpha;
        Pivot pivot;
    };

    void drawInt(const ParamInt& param, const Matrix4& proj) const;
    void drawFloat(const ParamFloat& param, const Matrix4& proj) const;
    void drawString(const ParamString& param, const Matrix4& proj) const;

public:
    static constexpr int CHAR_WIDTH = 32; //画像1文字の横幅
    static constexpr int CHAR_HEIGHT = 64; //画像1文字の縦幅
    static const inline Vector2 CHAR_SIZE = Vector2(CHAR_WIDTH, CHAR_HEIGHT);

    static constexpr int SPRITE_WIDTH = 512; //画像横幅
    static constexpr int SPRITE_HEIGHT = 512; //画像の縦幅

    static constexpr int WIDTH_CHAR_COUNT = SPRITE_WIDTH / CHAR_WIDTH; //フォント画像の画像横の文字数
    static constexpr int HEIGHT_CHAR_COUNT = SPRITE_HEIGHT / CHAR_HEIGHT;

    static constexpr float CHAR_WIDTH_RATE = 1.f / static_cast<float>(WIDTH_CHAR_COUNT); //画像横幅に対する1文字の比率
    static constexpr float CHAR_HEIGHT_RATE = 1.f / static_cast<float>(HEIGHT_CHAR_COUNT); //画像縦幅に対する1文字の比率
    static const inline Vector2 CHAR_RATE = Vector2(CHAR_WIDTH_RATE, CHAR_HEIGHT_RATE);

    static constexpr int PERIOD_WIDTH = CHAR_WIDTH; //ピリオドの横幅
    static constexpr float PERIOD_RATE = static_cast<float>(PERIOD_WIDTH) / static_cast<float>(SPRITE_WIDTH); //画像横幅に対する1文字の比率

private:
    std::unique_ptr<Sprite> mNumberSprite;
    std::unique_ptr<Sprite> mFontSprite;
    std::list<ParamInt> mParamsInt;
    std::list<ParamFloat> mParamsFloat;
    std::list<ParamString> mParamsString;
    std::string mNumberFileName;
    std::string mFontFileName;
    std::unique_ptr<SpriteInstancingDrawer> mDrawer;
};
