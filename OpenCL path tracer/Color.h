#pragma once

#include <iostream>

#pragma pack(1)
__declspec(align(16))
class Color
{
public:
    float R;
    float G;
    float B;
    float A;

    Color() = default;
    Color(float R, float G, float B, float A = 1);

    friend std::ostream& operator<<(std::ostream& os, Color& color);
    friend std::istream& operator>>(std::istream& is, Color& color);
};
#pragma pack()