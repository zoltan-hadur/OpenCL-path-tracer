#include "BoundingBox.h"

#include <limits>
#include <algorithm>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        BoundingBox::BoundingBox()
        {
            _topLeft = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
            _bottomRight = { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };
        }

        Vector2 const& BoundingBox::GetTopLeft() const
        {
            return _topLeft;
        }

        Vector2 const& BoundingBox::GetBottomRight() const
        {
            return _bottomRight;
        }

        Vector2 const& BoundingBox::GetPosition() const
        {
            return _topLeft;
        }

        Vector2 BoundingBox::GetSize() const
        {
            return _bottomRight - _topLeft;
        }

        void BoundingBox::Inflate(Vector2 point)
        {
            _topLeft = { std::min(point.x, _topLeft.x), std::min(point.y, _topLeft.y) };
            _bottomRight = { std::max(point.x, _bottomRight.x), std::max(point.y, _bottomRight.y) };
        }
    }
}