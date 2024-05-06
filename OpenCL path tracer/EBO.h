#pragma once

#include <glad/glad.h>
#include <vector>

#include "IBindable.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        class EBO : public IBindable
        {
        private:
            GLuint _id;
            std::size_t _size;

        protected:
            virtual void OnBind() override;
            virtual void OnUnbind() override;

        public:
            EBO(std::vector<GLuint> const& indices);
            ~EBO();

            GLuint GetId() const;
            std::size_t GetSize() const;

            void UpdateIndices(std::vector<GLuint> const& indices);
            void ReplaceIndices(std::vector<GLuint> const& indices);
        };
    }
}