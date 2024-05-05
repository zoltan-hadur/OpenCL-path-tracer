#version 330 core

in vec2 textureCoordinate;

out vec4 FragColor;

uniform vec4 color;
uniform sampler2D texture0;
uniform int mode;

const int Color = 0;
const int Texture = 1;
const int Text = 2;

void main()
{
  switch(mode)
  {
    case Color:
    {
      FragColor = color;
      break;
    }
    case Texture:
    {
      FragColor = texture(texture0, textureCoordinate);
      break; 
    }
    case Text:
    {
      FragColor = vec4(color.rgb, color.a * texture(texture0, textureCoordinate).r);
      break;
    }
  }
}