#pragma once

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        class Freezable
        {
        private:
            bool _isFrozen = false;

        protected:
            void ThrowIfFrozen() const;

        public:
            void Freeze();
            bool IsFrozen() const;
        };
    }
}