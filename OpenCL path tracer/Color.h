#pragma once

#pragma pack(1)
__declspec(align(16))
class Color
{
public:
    float R;
    float G;
    float B;
    float A;

    Color(float R, float G, float B, float A = 1);
};
#pragma pack()