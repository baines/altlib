#ifndef ALTLIB_STR_COMMON_H_
#define ALTLIB_STR_COMMON_H_
#include "str_ref.h"

namespace alt {
namespace detail {

template<class T>
T* str_find(const TStrRef<typename std::remove_const<T>::type>& needle, T* begin, T* end, int inc){
	using U = typename std::remove_const<T>::type;

	const size_t _sz = needle.size();
	const T* _end = begin + _sz;
	
	while(_end != (end + inc)){
		if(TStrRef<U>(begin, _end) == needle) return begin;
		begin += inc;
		_end += inc;
	}
	
	return nullptr;
}

template<class T>
T* str_find_any(const TStrRef<typename std::remove_const<T>::type>& chars, T* begin, T* end, int inc){
	for(T* p = begin; p != (end + inc); p += inc){
		for(T c : chars){
			if(*p == c) return p;
		}
	}
	return nullptr;
}

}

template<class T>
const T* TStrRef<T>::find(const TStrRef<T>& needle, size_t offset) const {
	return detail::str_find<const T>(needle, begin() + offset, end(), +1);
}

template<class T>
const T* TStrRef<T>::rfind(const TStrRef<T>& needle, size_t offset) const {
	const size_t _sz = needle.size();
	return detail::str_find<const T>(needle, end() - _sz - offset, begin() + _sz, -1);
}

template<class T>
const T* TStrRef<T>::find_any(const TStrRef<T>& chars, size_t offset) const {
	return detail::str_find_any<const T>(chars, begin() + offset, end(), +1);
}

template<class T>
const T* TStrRef<T>::rfind_any(const TStrRef<T>& chars, size_t offset) const {
	return detail::str_find_any<const T>(chars, end() - offset, begin(), -1);
}

}

#endif
