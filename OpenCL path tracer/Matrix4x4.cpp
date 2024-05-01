#include "Matrix4x4.h"

#include "Vector2.h"
#include "Vector3.h"

#include <immintrin.h>
#include <cmath>

Matrix4x4::Matrix4x4()
{
    memset(_values, 0, sizeof(float) * 16);
}

Matrix4x4::Matrix4x4(std::array<std::array<float, 4>, 4> values)
{
    memcpy(_values, values.data(), sizeof(float) * 16);
}

float const* Matrix4x4::Data() const
{
    return &_values[0][0];
}

Matrix4x4 Matrix4x4::Scale(Vector3 const& scale) const
{
    return ScaleMatrix(scale) * (*this);
}

Matrix4x4 Matrix4x4::Translate(Vector3 const& translate) const
{
    return TranslateMatrix(translate) * (*this);
}

Matrix4x4 Matrix4x4::Rotate(Vector3 const& axis, float theta) const
{
    return RotateMatrix(axis, theta) * (*this);
}

Matrix4x4 Matrix4x4::operator*(Matrix4x4 const& matrix) const
{
    auto result = Matrix4x4();
    for (int i = 0; i < 4; ++i)
    {
        for (int k = 0; k < 4; ++k)
        {
            _mm_store_ps(
                &result._values[i][0],
                _mm_add_ps(
                    _mm_load_ps(&result._values[i][0]),
                    _mm_mul_ps(
                        _mm_load_ps1(&_values[i][k]),
                        _mm_load_ps(&matrix._values[k][0])
                    )
                )
            );
        }
    }
    return result;
}

Vector3 Matrix4x4::operator*(Vector3 const& vector) const
{
    auto result = Vector3();
    for (int i = 0; i < 3; ++i)
    {
        float sum = 0;
        for (int j = 0; j < 4; ++j)
        {
            sum = sum + _values[i][j] * (j == 3 ? 1.0f : vector[j]);
        }
        result[i] = sum;
    }
    return result;
}

Vector3 Matrix4x4::operator*(Vector2 const& vector) const
{
    return (*this) * Vector3(vector.x, vector.y, 0);
}

Matrix4x4 Matrix4x4::IdentityMatrix()
{
    return Matrix4x4({ {
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
    } });
}

Matrix4x4 Matrix4x4::ScaleMatrix(Vector3 const& scale)
{
    return Matrix4x4({ {
        { scale.x,       0,       0, 0 },
        {       0, scale.y,       0, 0 },
        {       0,       0, scale.z, 0 },
        {       0,       0,       0, 1 }
    } });
}

Matrix4x4 Matrix4x4::TranslateMatrix(Vector3 const& translate)
{
    return Matrix4x4({ {
        { 1, 0, 0, translate.x },
        { 0, 1, 0, translate.y },
        { 0, 0, 1, translate.z },
        { 0, 0, 0,           1 }
    } });
}

Matrix4x4 Matrix4x4::RotateMatrix(Vector3 const& axis, float theta)
{
    auto sint = std::sinf(theta);
    auto cost = std::cosf(theta);
    auto x = axis.x;
    auto y = axis.y;
    auto z = axis.z;
    return Matrix4x4({ {
        { cost + x * x * (1 - cost)           ,        x * y * (1 - cost) - z * sint,        x * z * (1 - cost) + y * sint, 0 },
        {        y * x * (1 - cost) + z * sint, cost + y * y * (1 - cost)           ,        y * z * (1 - cost) - x * sint, 0 },
        {        z * x * (1 - cost) - y * sint,        z * y * (1 - cost) + x * sint, cost + z * z * (1 - cost)           , 0 },
        {                                    0,                                    0,                                    0, 1 },
    } });
}

Matrix4x4 Matrix4x4::OrthoProjectionMatrix(float left, float right, float bottom, float top, float near, float far)
{
    auto s = ScaleMatrix(Vector3(2.0f / (right - left), 2.0f / (top - bottom), 2.0f / (far - near)));
    auto t = TranslateMatrix(Vector3(-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near)));
    return t * s;
}
