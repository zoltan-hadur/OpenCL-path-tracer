#pragma once

#include "Component.h"

namespace OpenCL_PathTracer
{
    class Vector2;

    namespace GL_Stuff
    {
        class Panel : public Component
        {
        public:
            Panel(Vector2 position, Vector2 size, Color color);
        };
    }
}