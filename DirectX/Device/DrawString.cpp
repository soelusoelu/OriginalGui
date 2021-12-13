﻿#include "DrawString.h"
#include "../Sprite/Sprite.h"
#include "../Sprite/SpriteInstancingDrawer.h"
#include "../System/Texture/Texture.h"
#include "../Transform/Transform2D.h"
#include "../Utility/AsciiHelper.h"
#include "../Utility/JsonHelper.h"
#include "../Utility/StringUtil.h"

DrawString::DrawString()
    : FileOperator("DrawString")
    , mNumberSprite(nullptr)
    , mFontSprite(nullptr)
    , mNumberFileName("")
    , mFontFileName("")
    , mDrawer(std::make_unique<SpriteInstancingDrawer>())
{
}

DrawString::~DrawString() = default;

void DrawString::saveAndLoad(JsonObject& inObj, FileMode mode) {
    JsonHelper::getSet(mNumberFileName, "number", inObj, mode);
    JsonHelper::getSet(mFontFileName, "font", inObj, mode);
}

void DrawString::initialize() {
    mNumberSprite = std::make_unique<Sprite>(mNumberFileName);
    mFontSprite = std::make_unique<Sprite>(mFontFileName, "SpriteInstancing.hlsl");
    mDrawer->initialize();
}

void DrawString::drawAll(const Matrix4& proj) const {
    for (const auto& param : mParamsInt) {
        drawInt(param, proj);
    }
    for (const auto& param : mParamsFloat) {
        drawFloat(param, proj);
    }
    for (const auto& param : mParamsString) {
        drawString(param, proj);
    }
    mDrawer->instancingDraw(*mFontSprite);
}

void DrawString::clear() {
    mParamsInt.clear();
    mParamsFloat.clear();
    mParamsString.clear();
    mDrawer->clear();
}

void DrawString::drawNumber(
    int number,
    const Vector2& position,
    const Vector2& scale,
    const Vector3& color,
    float alpha,
    Pivot pivot
) {
    mParamsInt.emplace_back(ParamInt{ number, position, scale, color, alpha, pivot });
}

void DrawString::drawNumber(
    float number,
    const Vector2& position,
    const Vector2& scale,
    int decimalDigits,
    const Vector3& color,
    float alpha,
    Pivot pivot
) {
    mParamsFloat.emplace_back(ParamFloat{ number, position, scale, decimalDigits, color, alpha, pivot });
}

void DrawString::drawString(
    const std::string& alphabet,
    const Vector2& position,
    const Vector2& scale,
    const Vector3& color,
    float alpha,
    Pivot pivot
) {
    mParamsString.emplace_back(ParamString{ alphabet, position, scale, color, alpha, pivot });
}

void DrawString::drawInt(const ParamInt& param, const Matrix4& proj) const {
    //要素取り出し
    auto number = param.number;
    auto pos = param.position;
    const auto& scale = param.scale;
    const auto& color = param.color;
    auto alpha = param.alpha;
    auto pivot = param.pivot;

    //マイナスは扱わない
    if (number < 0) {
        number = 0;
    }

    //桁数計算
    int digit = 1;
    for (int i = number; i >= 10; i /= 10) {
        ++digit;
    }

    //描画する文字の横幅
    const float OFFSET_X = CHAR_WIDTH * scale.x;
    //ピボットから描画位置を調整
    pos = StringUtil::calcPositionFromPivot(pos, Vector2(OFFSET_X, CHAR_HEIGHT * scale.y), digit, pivot);

    //数字を文字列化し、1文字ずつ取り出す
    auto& t = mNumberSprite->transform();
    t.setScale(scale);
    mNumberSprite->setColor(color);
    mNumberSprite->setAlpha(alpha);
    for (const auto& n : std::to_string(number)) {
        t.setPosition(pos);
        //数字のテクスチャが数字1つにつき幅32高さ64
        //文字と文字を引き算し、整数値を取得している
        float num = (n - '0') * CHAR_WIDTH;
        num /= SPRITE_WIDTH;
        mNumberSprite->setUV(num, 0.f, num + CHAR_WIDTH_RATE, 1.f);

        //ワールド座標を更新し、描画
        t.computeWorldTransform();
        mNumberSprite->draw(proj);

        //1文字描画したら1桁分右にずらす
        pos.x += OFFSET_X;
    }
}

void DrawString::drawFloat(const ParamFloat& param, const Matrix4& proj) const {
    //要素取り出し
    auto number = param.number;
    auto pos = param.position;
    const auto& scale = param.scale;
    auto decimalDigits = param.decimalDigits;
    const auto& color = param.color;
    auto alpha = param.alpha;
    auto pivot = param.pivot;

    //マイナスは扱わない
    if (number < 0) {
        number = 0;
    }

    //小数部分の桁数指定
    auto num = StringUtil::floatToString(number, decimalDigits);

    //描画する文字の横幅
    const float OFFSET_X = CHAR_WIDTH * scale.x;
    const float OFFSET_PERIOD_X = PERIOD_WIDTH * scale.x;
    //ピボットから描画位置を調整
    pos = StringUtil::calcPositionFromPivot(pos, Vector2(OFFSET_X, CHAR_HEIGHT * scale.y), num.length(), pivot);

    //数字を文字列化し、1文字ずつ取り出す
    auto& t = mNumberSprite->transform();
    t.setScale(scale);
    mNumberSprite->setColor(color);
    mNumberSprite->setAlpha(alpha);
    for (const auto& n : num) {
        t.setPosition(pos);
        //数字のテクスチャが数字1つにつき幅32高さ64
        //文字と文字を引き算し、整数値を取得している
        float offsetX = 0.f;
        if (n == '.') {
            constexpr float num = 10 * CHAR_WIDTH_RATE; //ピリオドは画像の10番目
            mNumberSprite->setUV(num, 0.f, num + PERIOD_RATE, 1.f);

            //「.」のときは1文字の半分ずらす
            offsetX = PERIOD_WIDTH;
        } else {
            float num = (n - '0') * CHAR_WIDTH;
            num /= SPRITE_WIDTH;
            mNumberSprite->setUV(num, 0.f, num + CHAR_WIDTH_RATE, 1.f);

            //1文字描画したら1桁分右にずらす
            offsetX = OFFSET_X;
        }

        //ワールド座標を更新し、描画
        t.computeWorldTransform();
        mNumberSprite->draw(proj);

        pos.x += offsetX;
    }
}

void DrawString::drawString(const ParamString& param, const Matrix4& proj) const {
    //要素取り出し
    const auto& alphabet = param.alphabet;
    auto pos = param.position;
    const auto& scale = param.scale;
    const auto& color = param.color;
    auto alpha = param.alpha;
    auto pivot = param.pivot;

    //描画する文字の横幅
    const float OFFSET_X = CHAR_WIDTH * scale.x;
    //ピボットから描画位置を調整
    pos = StringUtil::calcPositionFromPivot(pos, alphabet, Vector2(OFFSET_X, CHAR_HEIGHT * scale.y), pivot);

    auto& trans = mFontSprite->transform();
    trans.setScale(scale);
    mFontSprite->setColor(color);
    mFontSprite->setAlpha(alpha);
    for (const auto& c : alphabet) {
        trans.setPosition(pos);

        char t = AsciiHelper::clampCharToAscii(c);
        auto leftTop = AsciiHelper::calcPositionRateToAscii(t, WIDTH_CHAR_COUNT, HEIGHT_CHAR_COUNT);

        mFontSprite->setUV(leftTop.x, leftTop.y, leftTop.x + CHAR_WIDTH_RATE, leftTop.y + CHAR_HEIGHT_RATE);

        //ワールド座標を更新し、描画
        trans.computeWorldTransform();
        mDrawer->add(*mFontSprite, proj);

        //描画位置を1文字分ずらす
        pos.x += OFFSET_X;
    }
}
