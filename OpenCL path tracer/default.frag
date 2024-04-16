#version 330 core

in vec2 textureCoordinate;

out vec4 FragColor;

uniform vec4 color;
uniform sampler2D texture0;

void main()
{
  FragColor = color + texture(texture0, textureCoordinate);
}