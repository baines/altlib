#ifndef ALTLIB_STR_IOSTREAM_COMPAT_H_
#define ALTLIB_STR_IOSTREAM_COMPAT_H_
#include <iostream>

namespace alt {

template<class T>
std::ostream& operator<<(std::ostream& stream, const TStrRef<T>& str){
	str.pass_c_str([&](T* cstr){
		stream << cstr;
	});
}

template<class T>
std::ostream& operator<<(std::ostream& stream, const TStrMut<T>& str){
	stream << str.c_str();
}

}
#endif
