#include "Panel.h"

#include "Vector2.h"
#include "Vertex.h"
#include "EBO.h"
#include "VBO.h"
#include "VAO.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        Panel::Panel(Vector2 position, Vector2 size, Color color) : Component(color)
        {
            ReplaceData(
                {
                    { { 0, 0 }, { } },
                    { { 0, 1 }, { } },
                    { { 1, 1 }, { } },
                    { { 1, 0 }, { } }
                },
                {
                    0, 1, 2,
                    2, 3, 0
                }
            );
            SetPosition(position);
            SetSize(size);
        }
    }
}