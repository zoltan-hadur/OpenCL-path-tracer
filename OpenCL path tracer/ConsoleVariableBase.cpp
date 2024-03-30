#include "ConsoleVariableBase.h"

std::ostream& operator<<(std::ostream& os, ConsoleVariableBase const& node)
{
    return node.Print(os);
}

std::istream& operator>>(std::istream& is, ConsoleVariableBase& node)
{
    return node.Read(is);
}