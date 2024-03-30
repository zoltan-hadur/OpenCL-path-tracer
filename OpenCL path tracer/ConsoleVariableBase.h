#pragma once

#include <string>
#include <functional>
#include <iostream>

// Base class for heterogeneous collection
// Class is based on https://github.com/arpg/CVars
class ConsoleVariableBase
{
protected:
    std::string _name;
    std::string _description;
    std::function<void(void)> _callback;

    ConsoleVariableBase(std::string name, std::string description, std::function<void(void)> callback);

public:
    virtual std::ostream& Print(std::ostream& os, bool onlyValue = false) const = 0;
    virtual std::istream& Read(std::istream& is) = 0;
    friend std::ostream& operator<<(std::ostream& os, ConsoleVariableBase const& node);
    friend std::istream& operator>>(std::istream& is, ConsoleVariableBase& node);
};