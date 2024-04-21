#version 330 core

in vec2 textureCoordinate;

out vec4 FragColor;

uniform vec4 color;
uniform sampler2D texture0;
uniform int mode;

void main()
{
  switch(mode)
  {
    // Color
    case 0:
    {
      FragColor = color;
      break;
    }
    // Texture
    case 1:
    {
      FragColor = texture(texture0, textureCoordinate);
      break; 
    }
    // Text
    case 2:
    {
      FragColor = vec4(color.rgb, color.a * texture(texture0, textureCoordinate).r);
      break;
    }
  }
}