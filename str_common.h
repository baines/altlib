#ifndef ALTLIB_STR_COMMON_H_
#define ALTLIB_STR_COMMON_H_
#include "str_ref.h"

namespace alt {
namespace detail {

template<class T>
T* str_find(const TStrRef<T>& needle, T* begin, T* end, int inc){
	const size_t _sz = needle.size();
	const T* _end = begin + _sz;
	
	while(_end != (end + inc)){
		if(TStrRef<T>(begin, _end) == needle) return begin;
		begin += inc;
		_end += inc;
	}
	
	return nullptr;
}

template<class T>
T* str_find_any(const TStrRef<T>& chars, T* begin, T* end, int inc){
	for(char* p = begin; p != (end + inc); p += inc){
		for(char c : chars){
			if(*p == c) return p;
		}
	}
	return nullptr;
}

}

template<class T>
char* TStrRef<T>::find(const TStrRef<T>& needle, size_t offset){
	return detail::str_find(needle, begin() + offset, end(), +1);
}

template<class T>
char* TStrRef<T>::rfind(const TStrRef<T>& needle, size_t offset){
	const size_t _sz = needle.size();
	return detail::str_find(needle, end() - _sz - offset, begin() + _sz, -1);
}

template<class T>
char* TStrRef<T>::find_any(const TStrRef<T>& chars, size_t offset){
	return detail::str_find_any(chars, begin() + offset, end(), +1);
}

template<class T>
char* TStrRef<T>::rfind_any(const TStrRef<T>& chars, size_t offset){
	return detail::str_find_any(chars, end() - offset, begin(), -1);
}

}

#endif
