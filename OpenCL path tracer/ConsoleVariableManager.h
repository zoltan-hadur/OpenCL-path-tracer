#pragma once

#include <format>
#include <functional>
#include <map>
#include <string>
#include <sstream>
#include <ostream>
#include <algorithm>
#include "ConsoleVariableBase.h"
#include "ConsoleVariable.h"

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
        if (Find(variable.Name()))
        {
            throw std::runtime_error(std::format("Console variable \"{}\" already exists!", variable.Name()));
        }

        _variables[variable.Name()] = std::make_unique<ConsoleVariable<T>>(variable);
    }
    ConsoleVariableBase* Find(std::string name) const;
    void Print(std::ostream& os) const;
    std::string AutoComplete(std::string partialName) const;
};