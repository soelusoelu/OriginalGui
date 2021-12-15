#include "Vector4.h"
#include "MathUtility.h"
#include "Vector3.h"

Vector4::Vector4()
    : x(0.f)
    , y(0.f)
    , z(0.f)
    , w(0.f)
{
}

Vector4::Vector4(float inX, float inY, float inZ, float inW)
    : x(inX)
    , y(inY)
    , z(inZ)
    , w(inW)
{
}

Vector4::Vector4(const Vector3& vec3, float inW)
    : x(vec3.x)
    , y(vec3.y)
    , z(vec3.z)
    , w(inW)
{
}

Vector4& Vector4::operator=(const Vector4& vec) {
    x = vec.x;
    y = vec.y;
    z = vec.z;
    w = vec.w;
    return *this;
}

Vector4 Vector4::operator-() const {
    return Vector4(-x, -y, -z, -w);
}

Vector4 operator+(const Vector4& a, const Vector4& b) {
    return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

Vector4 operator-(const Vector4& a, const Vector4& b) {
    return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

Vector4 operator*(const Vector4& a, const Vector4& b) {
    return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

Vector4 operator*(const Vector4& vec, float scalar) {
    return Vector4(vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar);
}

Vector4 operator*(float scalar, const Vector4& vec) {
    return Vector4(vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar);
}

Vector4 operator/(const Vector4& vec, float scalar) {
    if (scalar == 0) {
        return vec;
    }
    return Vector4(vec.x / scalar, vec.y / scalar, vec.z / scalar, vec.w / scalar);
}

Vector4 Vector4::clamp(const Vector4& value, const Vector4& min, const Vector4& max) {
    return Vector4(
        Math::clamp<float>(value.x, min.x, max.x),
        Math::clamp<float>(value.y, min.y, max.y),
        Math::clamp<float>(value.z, min.z, max.z),
        Math::clamp<float>(value.w, min.w, max.w)
    );
}

Vector4 Vector4::lerp(const Vector4& a, const Vector4& b, float f) {
    return Vector4(a + f * (b - a));
}

const Vector4 Vector4::zero(0.f, 0.f, 0.f, 0.f);
const Vector4 Vector4::one(1.f, 1.f, 1.f, 1.f);
