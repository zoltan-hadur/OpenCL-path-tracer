#pragma once

#include "Vector2.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        class Vertex
        {
        private:
            Vector2 _position;
            Vector2 _textureCoordinate;

        public:
            Vertex(Vector2 position);
            Vertex(Vector2 position, Vector2 textureCoordinate);

            Vector2 const& Position() const;
            Vector2 const& TextureCoordinate() const;
        };
    }
}