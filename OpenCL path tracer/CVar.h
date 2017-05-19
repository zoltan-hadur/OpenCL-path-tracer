#pragma once

#include <map>
#include <sstream>

template<typename T>
class CVar : public CVarBase {
private:
	std::string name;
	T* var;
	std::string help;
public:
	CVar<T>(std::string name, T& var, std::string help = "");
	T& get_var();
	void set_var_p(T* var);
	void print(std::ostream& os, bool with_name = true) override;
	void read(std::istream& is) override;
	template<typename T> friend std::ostream& operator<<(std::ostream& os, CVar<T>& cvar);
	template<typename T> friend std::istream& operator>>(std::istream& is, CVar<T>& cvar);
};

template<typename T> CVar<T>::CVar(std::string name, T& var, std::string help) {
	this->name = name;
	this->var = (&var);
	this->help = help;
}

template<typename T> T& CVar<T>::get_var() {
	return *(this->var);
}

template<typename T> void CVar<T>::set_var_p(T* var) {
	this->var = var;
}

template<typename T> void CVar<T>::print(std::ostream& os, bool with_name) {
	if (with_name) {
		os << this->name << " = " << *(this->var) << "   |" << this->help << "|";
	} else {
		os << *(this->var);
	}
}

template<typename T> void CVar<T>::read(std::istream& is) {
	is >> *(this->var);
}

template<typename T> std::ostream& operator<<(std::ostream& os, CVar<T>& cvar) {
	cvar.print(os);
	return os;
}

template<typename T> std::istream& operator>>(std::istream& is, CVar<T>& cvar) {
	cvar.read(os);
	return os;
}