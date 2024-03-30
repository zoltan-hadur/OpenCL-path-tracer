#pragma once

#include <sstream>
#include <iostream>
#include <ostream>
#include <istream>

// Base class for heterogeneous collection
// Class is based on https://github.com/arpg/CVars
class ConsoleVariableBase {
public:
	virtual void print(std::ostream& os, bool only_var = false);
	virtual void read(std::istream& is);
	friend std::ostream& operator<<(std::ostream& os, ConsoleVariableBase& node);
	friend std::istream& operator>>(std::istream& is, ConsoleVariableBase& node);
};

void ConsoleVariableBase::print(std::ostream& os, bool only_var) {
	//To have the ability to use operator<< overload on derived classes
}

void ConsoleVariableBase::read(std::istream& is) {
	//To have the ability to use operator>> overload on derived classes
}

std::ostream& operator<<(std::ostream& os, ConsoleVariableBase& node) {
	node.print(os);
	return os;
}

std::istream& operator>>(std::istream& is, ConsoleVariableBase& node) {
	node.read(is);
	return is;
}