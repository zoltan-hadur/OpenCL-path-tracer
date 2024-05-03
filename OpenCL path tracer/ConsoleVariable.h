#pragma once

#include <functional>
#include <string>
#include <iostream>

#include "ConsoleVariableBase.h"

namespace OpenCL_PathTracer
{
    // Stores any type of data, that can be printed to the console, and can be set from the console
    // Class is based on https://github.com/arpg/CVars
    template<typename T> class ConsoleVariable : public ConsoleVariableBase
    {
    private:
        T* _value;

    public:
        ConsoleVariable(T* value, std::string name, std::string description = "", std::function<void(void)> callback = nullptr) :
            ConsoleVariableBase(name, description, callback)
        {
            _value = value;
        }

        T& Value()
        {
            return *_value;
        }

        void Attach(T* value)
        {
            _value = value;
        }

        void SetCallback(std::function<void(void)> callback)
        {
            _callback = callback;
        }

        std::ostream& Print(std::ostream& os, bool onlyValue) const override
        {
            if (onlyValue)
            {
                os << *_value;
            }
            else
            {
                os << _name << " = " << *_value;
                if (!_description.empty())
                {
                    os << "   |" << _description << "|";
                }
            }
            return os;
        }

        std::istream& Read(std::istream& is) override
        {
            is >> *_value;
            if (_callback)
            {
                _callback();
            }
            return is;
        }
    };
}