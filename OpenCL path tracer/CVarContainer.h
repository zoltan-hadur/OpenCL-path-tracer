#pragma once

#include <functional>
#include <map>
#include <string>
#include <sstream>
#include <ostream>
#include <algorithm>
#include "ConsoleVariableBase.h"
#include "CVar.h"

// Manages CVars
// Class is based on https://github.com/arpg/CVars
class CVarContainer {
private:
	std::map<std::string, ConsoleVariableBase*> variables;																								// Easy search for CVars using their names
public:
	ConsoleVariableBase* find(std::string name, bool throw_exception = true);																			// Tries to find the corresponding CVar
	template<typename T> T& create_cvar(std::string name, T& var, std::string help = "", std::function<void(void)> callback = nullptr);		// Creates a CVar and stores it, while returns a reference to the variable
	template<typename T> T& get_cvar(std::string name);																						// Returns the reference of the corresponding CVar's variable
	template<typename T> void set_cvar(std::string name, T& var);																			// Sets the corresponding CVar's variable's value
	void set_cvar(std::string name, std::string value);																						// Sets the corresponding CVar's variable's value that was read from the console
	template<typename T> void attach_cvar(std::string name, T* var, std::string help = "", std::function<void(void)> callback = nullptr);	// Attaches a variable to a CVar
	void print_tree(std::ostringstream& cout);																									// Prints all CVar to the given ostream
	std::string complete(std::string name);																									// Tries to complete a CVar's name whose was partially given
};

// Tries to find the corresponding CVar
ConsoleVariableBase* CVarContainer::find(std::string name, bool throw_exception) {
	if (variables.find(name) == variables.end()) {				// If not found
		if (throw_exception) {
			throw "CVar does not exist!";						// Throw exception if flag is true
		} else {
			return NULL;										// Return with NULL if flag is false
		}
	} else {
		return variables.at(name);								// Else return with the CVar
	}
}

// Creates a CVar and stores it, while returns a reference to the variable
template<typename T> T& CVarContainer::create_cvar(std::string name, T& var, std::string help, std::function<void(void)> callback) {
	if (find(name, false) == NULL) {							// If CVar does not exist
		CVar<T>* cvar = new CVar<T>(name, var, help, callback);	// Create it
		variables.insert({ name, cvar });						// Store it
		return cvar->get_var();									// And return with it's value
	} else {
		throw "CVar already exist!";							// Else throw exception
	}
}

// Returns the reference of the corresponding CVar's variable
template<typename T> T& CVarContainer::get_cvar(std::string name) {
	ConsoleVariableBase* cvarnode = find(name, false);						// Try to find CVar
	if (cvarnode != NULL) {										// If found
		CVar<T>* cvar = (CVar<T>*)cvarnode;						// Get it's value
		return cvar->get_var();									// And return with it
	} else {
		throw "CVar does not exist!";							// Else throw exception
	}
}

// Sets the corresponding CVar's variable's value
template<typename T> void CVarContainer::set_cvar(std::string name, T& var) {
	ConsoleVariableBase* cvarnode = find(name, false);						// Try to find CVar
	if (cvarnode != NULL) {										// If found
		CVar<T>* cvar = (CVar<T>*)cvarnode;						// Get it's value
		cvar->get_var() = var;									// And set it to the new value
	} else {
		throw "CVar does not exist!";							// Else throw exception
	}
}

// Sets the corresponding CVar's variable's value that was read from the console
void CVarContainer::set_cvar(std::string name, std::string value) {
	ConsoleVariableBase* cvarnode = find(name, false);						// Try to find CVar
	if (cvarnode != NULL) {										// If found
		std::istringstream is(value);
		is >> *cvarnode;										// Set it's value that was read from console
	} else {
		throw "CVar does not exist!";							// Else throw exception
	}
}

// Attaches a variable to a CVar
template<typename T> void CVarContainer::attach_cvar(std::string name, T* var, std::string help, std::function<void(void)> callback) {
	ConsoleVariableBase* cvarnode = find(name, false);						// Try to find CVar
	if (cvarnode != NULL) {										// If found
		CVar<T>* cvar = (CVar<T>*)cvarnode;						// Get it's value
		cvar->set_var_p(var);									// Set the pointer that points to the new data
		cvar->set_callback(callback);
	} else {
		this->create_cvar(name, *var, help, callback);			// Else create a CVar
	}
}

// Prints all CVar to the given ostream
void CVarContainer::print_tree(std::ostringstream& cout) {
	int max_length = 0;
	for (auto node : variables) {								// Find the longest CVar name's length's
		max_length = std::max(max_length, (int)node.first.length());
	}

	// Bullshit algorithm to not print redundant information
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
		node.second->print(cout, true);
		cout << "\n";
		previous = variable.substr(0, variable.find_last_of('.'));
	}
}

// Tries to complete a CVar's name whose was partially given
std::string CVarContainer::complete(std::string name) {
	for (auto node : variables) {								// Iterates through all the CVar
		//auto found = node.first.substr(0, name.length()).find(name, 0);					// Tries to find the partial name in the CVar's name's
		//if (found != std::string::npos) {						// 
		if (node.first == name) {
			return "";
		}
		if (node.first.substr(0, name.length()) == name) {		// Found it
			std::string str = node.first.substr(name.length());	// Other part of the CVar's name
			auto found = str.find('.');							// Do not want to complete the full name, just until the first dot
			if (found != std::string::npos) {
				return str.substr(0, found);
			} else {
				return str;
			}
		}
	}
	return "";													// Nothing was found
}