#pragma once

#include <array>
#include "Vector3.h"
#include "Vector2.h"

class Matrix4x4
{
private:
    std::array<std::array<float, 4>, 4> _values;

public:
    Matrix4x4() = default;
    Matrix4x4(std::array<std::array<float, 4>, 4> values);

    float const* Data() const;

    Matrix4x4 Scale(Vector3 const& scale) const;
    Matrix4x4 Translate(Vector3 const& translate) const;
    Matrix4x4 Rotate(Vector3 const& axis, float theta) const;

    Matrix4x4 operator*(Matrix4x4 const& matrix) const;
    Vector3 operator*(Vector3 const& vector) const;
    Vector3 operator*(Vector2 const& vector) const;

    static Matrix4x4 IdentityMatrix();
    static Matrix4x4 ScaleMatrix(Vector3 const& scale);
    static Matrix4x4 TranslateMatrix(Vector3 const& translate);
    static Matrix4x4 RotateMatrix(Vector3 const& axis, float theta);
    static Matrix4x4 OrthoProjectionMatrix(float left, float right, float bottom, float top, float near, float far);
};