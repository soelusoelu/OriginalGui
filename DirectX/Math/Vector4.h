#pragma once

class Vector3;

class Vector4 {
public:
    float x;
    float y;
    float z;
    float w;

public:
    Vector4();
    explicit Vector4(float inX, float inY, float inZ, float inW);
    explicit Vector4(const Vector3& vec3, float inW);

    Vector4& operator=(const Vector4& vec);
    Vector4 operator-() const;
    friend Vector4 operator+(const Vector4& a, const Vector4& b);
    friend Vector4 operator-(const Vector4& a, const Vector4& b);
    friend Vector4 operator*(const Vector4& a, const Vector4& b);
    friend Vector4 operator*(const Vector4& vec, float scalar);
    friend Vector4 operator*(float scalar, const Vector4& vec);
    friend Vector4 operator/(const Vector4& vec, float scalar);

    //2つのベクトルを線形補間する
    static Vector4 lerp(const Vector4& a, const Vector4& b, float f);
};
