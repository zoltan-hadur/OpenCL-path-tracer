#pragma once

enum class TextureType
{
    Nothing = 0,
    Texture = 1 << 0,
    BumpMap = 1 << 1
};

TextureType operator|(TextureType left, TextureType right);