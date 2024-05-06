#include "Component.h"

#include "Texture.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vertex.h"
#include "ShaderProgram.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        Component::Component(ShaderMode mode, Color color, std::shared_ptr<Texture> texture)
        {
            _position = Vector2();
            _scale = Vector2(1, 1);
            _mode = mode;
            _color = color;
            _texture = texture;

            _vao = std::make_unique<VAO>();
            _vbo = std::make_unique<VBO>(std::vector<Vertex>());
            _ebo = std::make_unique<EBO>(std::vector<GLuint>());
            // Vertex memory layout
            // 0x0000 Vector2 _position          float x
            // 0x0004                            float y
            // 0x0008 Vector2 _textureCoordinate float x
            // 0x000C                            float y
            // 
            // Vertex shader layout
            // layout (location = 0) in vec2 aPosition;
            // layout (location = 1) in vec2 aTextureCoordinate;
            //
            // For layout 0, there are 2 components, each with type float. Overall, a vertex has 4 floats. Position starts at 0x0000.
            // For layout 1, there are 2 components, each with type float. Overall, a vertex has 4 floats. TextureCoordinate starts at 0x0008 (2 * sizeof(float)).
            _vao->LinkAttribute(*_vbo, 0, 2, GL_FLOAT, sizeof(float) * 4, (void*)0);
            _vao->LinkAttribute(*_vbo, 1, 2, GL_FLOAT, sizeof(float) * 4, (void*)(sizeof(float) * 2));
            
            _vao->Bind();
            _vbo->Bind();
            _ebo->Bind();
            _vao->Unbind();
            _vbo->Unbind();
            _ebo->Unbind();
        }

        Vector2 const& Component::GetPosition() const
        {
            return _position;
        }

        void Component::SetPosition(Vector2 position)
        {
            _position = std::move(position);
        }

        Vector2 Component::GetSize() const
        {
            return _vbo->GetBoundingBox().GetSize() * _scale;
        }

        void Component::SetSize(Vector2 size)
        {
            _scale = size / _vbo->GetBoundingBox().GetSize();
        }

        Vector2 const& Component::GetScale() const
        {
            return _scale;
        }

        void Component::SetScale(Vector2 scale)
        {
            _scale = std::move(scale);
        }

        Color const& Component::GetColor() const
        {
            return _color;
        }

        void Component::SetColor(Color color)
        {
            _color = color;
        }

        void Component::UpdateData(std::vector<Vertex> const& vertices, std::vector<GLuint> const& indices)
        {
            _vbo->UpdateVertices(vertices);
            _ebo->UpdateIndices(indices);
        }

        void Component::ReplaceData(std::vector<Vertex> const& vertices, std::vector<GLuint> const& indices)
        {
            _vbo->ReplaceVertices(vertices);
            _ebo->ReplaceIndices(indices);
        }

        void Component::Draw(ShaderProgram& shaderProgram) const
        {
            shaderProgram.SetMode(_mode);
            if (_mode == ShaderMode::Color || _mode == ShaderMode::Text)
            {
                shaderProgram.SetColor(_color);
            }
            shaderProgram.SetModelMatrix(Matrix4x4::ScaleMatrix({ _scale, 1.0f }).Translate({ _position, 0.0f }));

            _vao->Bind();
            if (_mode == ShaderMode::Texture || _mode == ShaderMode::Text)
            {
                _texture->Bind();
            }
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_ebo->GetSize()), GL_UNSIGNED_INT, 0);
            _vao->Unbind();
            if (_mode == ShaderMode::Texture || _mode == ShaderMode::Text)
            {
                _texture->Unbind();
            }
        }
    }
}