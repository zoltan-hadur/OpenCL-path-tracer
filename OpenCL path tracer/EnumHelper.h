#pragma once

template<typename T>
class EnumHelper
{
public:
    static T Next(T current, T max)
    {
        return static_cast<T>((static_cast<int>(current) + 1) % static_cast<int>(max));
    }
};