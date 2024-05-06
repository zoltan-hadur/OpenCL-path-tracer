#include "Freezable.h"

#include <stdexcept>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        void Freezable::ThrowIfFrozen() const
        {
            if (_isFrozen)
            {
                throw std::runtime_error("Object is frozen!");
            }
        }

        void Freezable::Freeze()
        {
            _isFrozen = true;
        }

        bool Freezable::IsFrozen() const
        {
            return _isFrozen;
        }
    }
}