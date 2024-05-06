#pragma once

#include <glad/glad.h>
#include <vector>

#include "IBindable.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        class Vertex;

        class VBO : public IBindable
        {
        private:
            GLuint _id;
            std::size_t _size;

        protected:
            virtual void OnBind() override;
            virtual void OnUnbind() override;

        public:
            VBO(std::vector<Vertex> const& vertices);
            ~VBO();

            GLuint GetId() const;

            void UpdateVertices(std::vector<Vertex> const& vertices);
            void ReplaceVertices(std::vector<Vertex> const& vertices);
        };
    }
}