#include "TextureType.h"

#include <type_traits>

TextureType operator|(TextureType left, TextureType right)
{
    return static_cast<TextureType>(static_cast<std::underlying_type<TextureType>::type>(left) | static_cast<std::underlying_type<TextureType>::type>(right));
}