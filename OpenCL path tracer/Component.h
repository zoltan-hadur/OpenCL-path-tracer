#pragma once

#include <memory>
#include <glad/glad.h>
#include <vector>

#include "Vector2.h"
#include "ShaderMode.h"
#include "Color.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        class Texture;
        class VAO;
        class VBO;
        class EBO;
        class Vertex;
        class ShaderProgram;

        class Component
        {
        private:
            Vector2 _position;
            Vector2 _scale;
            ShaderMode _mode;
            Color _color;
            std::shared_ptr<Texture> _texture;
            std::unique_ptr<VAO> _vao;
            std::unique_ptr<VBO> _vbo;
            std::unique_ptr<EBO> _ebo;

        protected:
            Component(ShaderMode mode, Color color, std::shared_ptr<Texture> texture);

        public:
            Vector2 const& GetPosition() const;
            void SetPosition(Vector2 position);
            Vector2 GetSize() const;
            void SetSize(Vector2 size);
            Vector2 const& GetScale() const;
            void SetScale(Vector2 scale);
            Color const& GetColor() const;
            void SetColor(Color color);

            void UpdateData(std::vector<Vertex> const& vertices, std::vector<GLuint> const& indices);
            void ReplaceData(std::vector<Vertex> const& vertices, std::vector<GLuint> const& indices);
            void Draw(ShaderProgram& shaderProgram) const;
        };
    }
}