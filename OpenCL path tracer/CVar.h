#pragma once

#include <functional>
#include <string>
#include <sstream>

// Stores any type of data, that can be printed to the console, and can be set from the console
// Class is based on https://github.com/arpg/CVars
template<typename T> class CVar : public ConsoleVariableBase {
private:
	std::string name;	// Name of the variable
	T* var;				// Pointer to the variable, so they are attached (modifying the cvar also modifies tha variable and vice versa)
	std::string help;	// Description of the variable
	std::function<void(void)> callback;
public:
    // Creates a CVar
    CVar(std::string name, T& var, std::string help, std::function<void(void)> callback = nullptr)
    {
        this->name = name;
        this->var = (&var);
        this->help = help;
        this->callback = callback;
    }
	T& get_var();																			// Get the variable of a CVar
	void set_var_p(T* var);																	// Attach a variable to the CVar
	void set_callback(std::function<void(void)> callback);
    std::ostream& Print(std::ostream& os, bool onlyValue) const override;									// Prints any type of variable, just need to overload << for your own class
    std::istream& Read(std::istream& is) override;													// Reads any type of variable, just need to overload >> for your own class
	template<typename T> friend std::ostream& operator<<(std::ostream& os, CVar<T>& cvar);
	template<typename T> friend std::istream& operator>>(std::istream& is, CVar<T>& cvar);
};

template<typename T> T& CVar<T>::get_var() {
	return *(this->var);
}

template<typename T> void CVar<T>::set_var_p(T* var) {
	this->var = var;
}

template<typename T> void CVar<T>::set_callback(std::function<void(void)> callback) {
	this->callback = callback;
}

template<typename T> std::ostream& CVar<T>::Print(std::ostream& os, bool onlyValue) const {
	if (onlyValue) {
		os << *var;
	} else {
		os << name << " = " << *var;
		if (!help.empty()) {
			os << "   |" << this->help << "|";
		}
	}
    return os;
}

template<typename T> std::istream& CVar<T>::Read(std::istream& is) {
	is >> *(this->var);
	if (callback) {
		callback();
	}
    return is;
}

template<typename T> std::ostream& operator<<(std::ostream& os, CVar<T>& cvar) {
	cvar.Print(os);
	return os;
}

template<typename T> std::istream& operator>>(std::istream& is, CVar<T>& cvar) {
    cvar.Read(is);
    return is;
}