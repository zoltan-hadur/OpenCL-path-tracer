#include "Component.h"

#include "Texture.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vertex.h"
#include "ShaderProgram.h"

using namespace OpenCL_PathTracer;
using namespace OpenCL_PathTracer::GL_Stuff;

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
    Bind();

    _vao->LinkAttrib(*_vbo, 0, 4, GL_FLOAT, sizeof(float) * 4, (void*)0);
    _vao->LinkAttrib(*_vbo, 1, 4, GL_FLOAT, sizeof(float) * 4, (void*)(sizeof(float) * 2));

    Unbind();
}

Vector2 const& Component::GetPosition() const
{
    return _position;
}

void Component::SetPosition(Vector2 position)
{
    _position = std::move(position);
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
    shaderProgram.Mode(_mode);
    if (_mode == ShaderMode::Color || _mode == ShaderMode::Text)
    {
        shaderProgram.Color(_color);
    }
    shaderProgram.ModelMatrix(Matrix4x4::ScaleMatrix({ _scale, 1.0f }).Translate({ _position, 0.0f }));
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_ebo->GetSize()), GL_UNSIGNED_INT, 0);
}

void Component::Bind() const
{
    if (_mode == ShaderMode::Texture || _mode == ShaderMode::Text)
    {
        _texture->Bind();
    }
    _vao->Bind();
    _vbo->Bind();
    _ebo->Bind();
}

void Component::Unbind() const
{
    if (_mode == ShaderMode::Texture || _mode == ShaderMode::Text)
    {
        _texture->Unbind();
    }
    _vao->Unbind();
    _vbo->Unbind();
    _ebo->Unbind();
}
