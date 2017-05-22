#pragma once

#include <sstream>
#include <iostream>
#include <ostream>
#include <istream>

// Base class for heterogeneous collection
// Class is based on https://github.com/arpg/CVars
class CVarBase {
public:
	virtual void print(std::ostream& os, bool with_name = true);
	virtual void read(std::istream& is);
	friend std::ostream& operator<<(std::ostream& os, CVarBase& node);
	friend std::istream& operator>>(std::istream& is, CVarBase& node);
};

void CVarBase::print(std::ostream& os, bool with_name) {
	//To have the ability to use operator<< overload on derived classes
}

void CVarBase::read(std::istream& is) {
	//To have the ability to use operator>> overload on derived classes
}

std::ostream& operator<<(std::ostream& os, CVarBase& node) {
	node.print(os);
	return os;
}

std::istream& operator>>(std::istream& is, CVarBase& node) {
	node.read(is);
	return is;
}