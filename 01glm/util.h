#include <sstream>

/*
Función de ayuda para convertir un vec* a string
*/

template<typename V>
std::string to_string(const V &v) {
	std::ostringstream os;
	os << "vec" << v.length() << "(" << v[0];
	for (int i = 1; i < v.length(); i++) {
		os << ", " << v[i];
	}
	os << ")";
	return os.str();
}
