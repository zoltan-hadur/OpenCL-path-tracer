#pragma once

#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <format>

#include "ConsoleVariableBase.h"
#include "ConsoleVariable.h"
#include <iosfwd>

namespace OpenCL_PathTracer
{
    // Manages CVars
    // Class is based on https://github.com/arpg/CVars
    class ConsoleVariableManager
    {
    private:
        std::map<std::string, std::unique_ptr<ConsoleVariableBase>> _variables;

    public:
        template<typename T>
        void Add(ConsoleVariable<T> variable)
        {
            if (Find(variable.GetName()))
            {
                throw std::runtime_error(std::format("Console variable \"{}\" already exists!", variable.GetName()));
            }

            _variables[variable.GetName()] = std::make_unique<ConsoleVariable<T>>(variable);
        }
        ConsoleVariableBase* Find(std::string name) const;
        void Print(std::ostream& os) const;
        std::string AutoComplete(std::string partialName) const;
    };
}