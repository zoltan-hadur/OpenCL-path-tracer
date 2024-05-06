#include "IBindable.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        bool IBindable::IsBound() const
        {
            return _isBound;
        }

        void IBindable::Bind()
        {
            OnBind();
            _isBound = true;
        }

        void IBindable::Unbind()
        {
            OnUnbind();
            _isBound = false;
        }
    }
}