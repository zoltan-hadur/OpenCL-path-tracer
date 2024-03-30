#include "ConsoleVariableBase.h"

void ConsoleVariableBase::print(std::ostream& os, bool only_var)
{
    //To have the ability to use operator<< overload on derived classes
}

void ConsoleVariableBase::read(std::istream& is)
{
    //To have the ability to use operator>> overload on derived classes
}

std::ostream& operator<<(std::ostream& os, ConsoleVariableBase& node)
{
    node.print(os);
    return os;
}

std::istream& operator>>(std::istream& is, ConsoleVariableBase& node)
{
    node.read(is);
    return is;
}