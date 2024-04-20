#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTextureCoordinate;

out vec2 textureCoordinate;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;

void main()
{
  gl_Position = projectionMatrix * modelMatrix * vec4(aPosition.xy, 0, 1.0);
  textureCoordinate = aTextureCoordinate;
}