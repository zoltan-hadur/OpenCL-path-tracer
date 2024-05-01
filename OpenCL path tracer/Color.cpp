#include "Color.h"

#include <format>
#include <iostream>

Color::Color(float R, float G, float B, float A) : R(R), G(G), B(B), A(A)
{

}

std::ostream& operator<<(std::ostream& os, Color& color)
{
    return os << std::format("[{}, {}, {}, {}]", color.R, color.G, color.B, color.A);
}

std::istream& operator>>(std::istream& is, Color& color)
{
    std::string str(std::istreambuf_iterator<char>(is), {});
    sscanf_s(str.c_str(), "[%f,%f,%f,%f]", &color.R, &color.G, &color.B, &color.A);
    return is;
}