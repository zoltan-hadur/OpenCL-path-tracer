#pragma once

#include <map>
#include <sstream>
#include "CVar.h"

class CVarBase {
private:
	std::map<std::string, CVarBase*> variables;
public:
	virtual void print(std::ostream& os, bool with_name = true);
	virtual void read(std::istream& is);
	CVarBase* find(std::string name, bool throw_exception = true);
	template<typename T> T& create_cvar(std::string name, T& var, std::string help = "");
	template<typename T> T& get_cvar(std::string name);
	template<typename T> void set_cvar(std::string name, T& var);
	void set_cvar(std::string name, std::string value);
	template<typename T> void attach_cvar(std::string name, T* var, std::string help = "");
	void print_tree(std::ostream& cout, int depth = 0);
	std::string complete(std::string name);
	friend std::ostream& operator<<(std::ostream& os, CVarBase& node);
	friend std::istream& operator>>(std::istream& is, CVarBase& node);
};

void CVarBase::print(std::ostream& os, bool with_name) {
	//To have the ability to use operator<< overload on derived classes
}

void CVarBase::read(std::istream& is) {
	//To have the ability to use operator>> overload on derived classes
}

CVarBase* CVarBase::find(std::string name, bool throw_exception) {
	if (variables.find(name) == variables.end()) {
		if (throw_exception) {
			throw "CVar does not exist!";
		} else {
			return NULL;
		}
	} else {
		return variables.at(name);
	}
}

template<typename T> T& CVarBase::create_cvar(std::string name, T& var, std::string help) {
	if (find(name, false) == NULL) {
		CVar<T>* cvar = new CVar<T>(name, var, help);
		variables.insert({ name, cvar });
		return cvar->get_var();
	} else {
		throw "CVar already exist!";
	}
}

template<typename T> T& CVarBase::get_cvar(std::string name) {
	CVarBase* cvarnode = find(name, false);
	if (cvarnode != NULL) {
		CVar<T>* cvar = (CVar<T>*)cvarnode;
		return cvar->get_var();
	} else {
		throw "CVar does not exist!";
	}
}

template<typename T> void CVarBase::set_cvar(std::string name, T& var) {
	CVarBase* cvarnode = find(name, false);
	if (cvarnode != NULL) {
		CVar<T>* cvar = (CVar<T>*)cvarnode;
		cvar->get_var() = var;
	} else {
		throw "CVar does not exist!";
	}
}

void CVarBase::set_cvar(std::string name, std::string value) {
	CVarBase* cvarnode = find(name, false);
	if (cvarnode != NULL) {
		std::istringstream is(value);
		is >> *cvarnode;
	} else {
		throw "CVar does not exist!";
	}
}

template<typename T> void CVarBase::attach_cvar(std::string name, T* var, std::string help) {
	CVarBase* cvarnode = find(name, false);
	if (cvarnode != NULL) {
		CVar<T>* cvar = (CVar<T>*)cvarnode;
		cvar->set_var_p(var);
	} else {
		this->create_cvar(name, *var, help);
	}
}

void CVarBase::print_tree(std::ostream& cout, int depth) {
	//for (auto node : variables) {
	//	//std::cout << std::string(depth, ' ') << *node.second << "\n";
	//	cout << std::string(depth, ' ') << *node.second << "\n";
	//	node.second->print_tree(cout, depth + 1);
	//}
	////std::cout.flush();

	int max_length = 0;
	for (auto node : variables) {
		max_length = std::max(max_length, (int)node.first.length());
	}

	std::string previous = "";
	for (auto node : variables) {
		std::string variable = node.first;
		auto found = variable.find(previous);
		while (found == std::string::npos) {
			auto found_dot = previous.find_last_of('.');
			if (found_dot != std::string::npos) {
				previous = previous.substr(0, found_dot);
			} else {
				previous = "";
			}
			found = variable.find(previous);
		}
		cout << std::string(previous.length(), ' ') << variable.substr(previous.length()) << std::string(max_length - variable.length(), ' ') << " = ";
		node.second->print(cout, false);
		cout << "\n";
		previous = variable.substr(0, variable.find_last_of('.'));
	}
}

std::string CVarBase::complete(std::string name) {
	for (auto node : variables) {
		auto found = node.first.find(name, 0);
		if (found != std::string::npos) {
			std::string str = node.first.substr(name.length());
			found = str.find('.');
			if (found != std::string::npos) {
				return name + str.substr(0, found);
			} else {
				return name + str;
			}
		}
	}
	return name;
}

std::ostream& operator<<(std::ostream& os, CVarBase& node) {
	node.print(os);
	return os;
}

std::istream& operator>>(std::istream& is, CVarBase& node) {
	node.read(is);
	return is;
}