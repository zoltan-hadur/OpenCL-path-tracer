#pragma once

#include <array>

namespace OpenCL_PathTracer
{
    class Vector2;
    class Vector3;

    class Matrix4x4
    {
    private:
        float _values[4][4];

    public:
        Matrix4x4();
        Matrix4x4(std::array<std::array<float, 4>, 4> values);

        float const* GetData() const;

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
}