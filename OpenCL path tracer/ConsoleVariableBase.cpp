#include "ConsoleVariableBase.h"

using namespace OpenCL_PathTracer;

ConsoleVariableBase::ConsoleVariableBase(std::string name, std::string description, std::function<void(void)> callback)
{
    _name = name;
    _description = description;
    _callback = callback;
}

std::string ConsoleVariableBase::Name() const
{
    return _name;
}

std::ostream& operator<<(std::ostream& os, ConsoleVariableBase const& node)
{
    return node.Print(os);
}

std::istream& operator>>(std::istream& is, ConsoleVariableBase& node)
{
    return node.Read(is);
}