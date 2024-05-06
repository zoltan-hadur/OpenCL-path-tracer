#pragma once

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        class IBindable
        {
        private:
            bool _isBound = false;

        protected:
            virtual void OnBind() = 0;
            virtual void OnUnbind() = 0;

        public:
            bool IsBound() const;

            void Bind();
            void Unbind();
        };
    }
}