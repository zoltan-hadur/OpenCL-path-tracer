#pragma once

#include "Vector2.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        class BoundingBox
        {
        private:
            Vector2 _topLeft;
            Vector2 _bottomRight;

        public:
            BoundingBox();

            Vector2 const& GetTopLeft() const;
            Vector2 const& GetBottomRight() const;
            Vector2 const& GetPosition() const;
            Vector2 GetSize() const;

            void Inflate(Vector2 point);
        };
    }
}