#include "ConsoleVariableManager.h"

namespace OpenCL_PathTracer
{
    ConsoleVariableBase* ConsoleVariableManager::Find(std::string name) const
    {
        return _variables.contains(name) ? _variables.at(name).get() : nullptr;
    }

    void ConsoleVariableManager::Print(std::ostream& os) const
    {
        size_t maxLength = 0;
        for (auto const& [name, _] : _variables)
        {
            maxLength = std::max(maxLength, name.length());
        }

        // Omit redundant information
        std::string previous = "";
        for (auto const& [name, variable] : _variables)
        {
            auto found = name.find(previous);
            while (found == std::string::npos)
            {
                auto dotFound = previous.find_last_of('.');
                previous = dotFound == std::string::npos ? "" : previous.substr(0, dotFound);
                found = name.find(previous);
            }
            os << std::string(previous.length(), ' ') << name.substr(previous.length()) << std::string(maxLength - name.length(), ' ') << " = ";
            variable->Print(os, true);
            os << "\n";
            previous = name.substr(0, name.find_last_of('.'));
        }
    }

    std::string ConsoleVariableManager::AutoComplete(std::string partialName) const
    {
        for (auto const& [name, _] : _variables)
        {
            if (name == partialName)
            {
                return "";
            }
            if (name.substr(0, partialName.length()) == partialName)
            {
                std::string str = name.substr(partialName.length());
                auto found = str.find('.');
                return found == std::string::npos ? str : str.substr(0, found);
            }
        }
        return "";
    }
}