#ifndef ALTLIB_STR_MUT_H_
#define ALTLIB_STR_MUT_H_
#include <cstring>
#include <type_traits>
#include <numeric>
#include <algorithm>
#include <cstdint>
#include "str_common.h"
#include "str_ref.h"

namespace alt {

namespace detail {

	inline size_t get_next_size(size_t current, size_t required){
		current = std::max<size_t>(current, 32);
		while(current < required){
			current <<= 2;
		}
		return current;
	}
}

template<class T>
class TStrMut : public detail::TStrCommon<T, TStrMut<T>> {
	static constexpr size_t LOCAL_SZ = (28 - sizeof(T*)) / sizeof(T);
public:
	TStrMut() : ptr(local_str), used_size(0) {}
		
	template<size_t N>
	TStrMut(const T (&ref)[N], typename std::enable_if<!(N <= LOCAL_SZ)>::type* = 0)
	: ptr(new T[detail::get_next_size(0, N)])
	, used_size(N-1)
	, allocated_size(detail::get_next_size(0, N)) {
		memcpy(ptr, &ref, N * sizeof(T));
	}

	template<size_t N>
	TStrMut(const T (&ref)[N], typename std::enable_if<N <= LOCAL_SZ>::type* = 0)
	: ptr(local_str)
	, used_size(N-1) {
		memcpy(local_str, &ref, N * sizeof(T));
	}
	
	TStrMut(const T* t, size_t sz)
	: ptr(sz >= LOCAL_SZ ? new T[detail::get_next_size(0, sz+1)] : local_str)
	, used_size(sz){
		memcpy(ptr, t, sz * sizeof(T));
		ptr[used_size] = 0;
		if(ptr != local_str){
			allocated_size = detail::get_next_size(0, sz+1);
		}
	}
	
	TStrMut(const T* a, const T* b) : TStrMut(a, b - a){}
	TStrMut(const TStrRef<T>& str) : TStrMut(str.data(), str.size()){}
	TStrMut(const TStrMut<T>& str) : TStrMut(str.c_str(), str.size()){}
		
	TStrMut<T>& operator=(TStrMut<T>&& other){
		used_size = other.size();
		
		if(other.ptr != other.local_str){
			if(ptr != local_str) delete [] ptr;
			ptr = other.ptr;
			allocated_size = other.allocated_size;
			
			other.ptr = other.local_str;
		} else {
			reserve(used_size);
			memcpy(ptr, other.ptr, other.used_size * sizeof(T));
			ptr[used_size] = 0;
		}

		other.clear();

		return *this;
	}
	
	TStrMut<T>& operator=(const TStrMut<T>& other){
		used_size = other.size();
		reserve(used_size);
		memcpy(ptr, other.c_str(), used_size * sizeof(T));
		ptr[used_size] = 0;

		return *this;
	}

	TStrMut<T>& assign(const TStrRef<T>& other){
		return (*this) = other;
	}
	
	template<size_t N>
	TStrMut<T>& operator=(const char (&other)[N]){
		used_size = N-1;
		reserve(used_size);
		memcpy(ptr, &other, used_size * sizeof(T));
		ptr[used_size] = 0;

		return *this;
	}
		
	TStrMut<T>& clear(){
		used_size = 0;
		*ptr = 0;
		return *this;
	}
	
	void reserve(size_t amount){
		const bool using_local = ptr == local_str;
		const size_t limit = using_local ? LOCAL_SZ : allocated_size;

		if(amount >= limit){
			const size_t new_size = detail::get_next_size(limit, amount + 1);
			T* new_ptr = new T[new_size];
			memcpy(new_ptr, ptr, (1 + used_size) * sizeof(T));
			if(!using_local) delete [] ptr;
			ptr = new_ptr;
			allocated_size = new_size;
		}
	}
		
	TStrMut<T>& insert(size_t index, const TStrRef<T>& str){
		const size_t _sz = str.size();
		reserve(used_size + _sz);
		memmove(ptr + index + _sz, ptr + index, (used_size - index) * sizeof(T));
		memcpy(ptr + index, str.data(), _sz * sizeof(T));
		used_size += _sz;
		ptr[used_size] = 0;

		return *this;
	}
	
	TStrMut<T>& erase(size_t index, size_t count){
		erase(ptr + index, ptr + index + count);

		return *this;
	}
	
	TStrMut<T>& erase(T* _begin, T* _end){
		memmove(_begin, _end, (end() - _end) * sizeof(T));
		used_size -= _end - _begin;
		ptr[used_size] = 0;

		return *this;
	}
	
	TStrMut<T>& append(const TStrRef<T>& str){
		const size_t _sz = str.size();
		reserve(used_size + _sz);
		memcpy(ptr + used_size, str.data(), _sz * sizeof(T));
		used_size += _sz;
		ptr[used_size] = 0;

		return *this;
	}
	
	TStrMut<T>& append(std::initializer_list<TStrRef<T>> strs){
		const size_t total = std::accumulate(strs.begin(), strs.end(), used_size,
		[](size_t a, const TStrRef<T>& b){
			return a + b.size();
		});

		reserve(total);
		
		for(auto& s : strs){
			const size_t _sz = s.size();
			memcpy(ptr + used_size, s.data(), _sz * sizeof(T));
			used_size += _sz;
		}
		
		ptr[used_size] = 0;

		return *this;
	}
	
	TStrMut<T>& append(size_t num, const TStrRef<T>& str){
		const size_t _sz = str.size();
		const T* _data = str.data();
		
		reserve(used_size + (_sz * num));
		while(num--){
			memcpy(ptr + used_size, _data, _sz * sizeof(T));
			used_size += _sz;
		}
		ptr[used_size] = 0;

		return *this;
	}
		
	TStrMut<T>& replace(size_t index, size_t count, const TStrRef<T>& str){
		const size_t _sz = str.size();
		const int diff = _sz - count;

		reserve(used_size + diff);
		T *_begin = ptr + index, *_end = ptr + index + count;
		if(diff != 0){
			memmove(_begin + _sz, _end, (end() - _end) * sizeof(T));
			used_size += diff;
			ptr[used_size] = 0;
		}
		memcpy(_begin, str.data(), _sz * sizeof(T));

		return *this;
	}
	
	TStrMut<T>& replace(T* _begin, T* _end, const TStrRef<T>& str){
		replace(_begin - ptr, _end - _begin, str);

		return *this;
	}
	
	using detail::TStrCommon<T, TStrMut<T>>::find;	
	using detail::TStrCommon<T, TStrMut<T>>::rfind;
	using detail::TStrCommon<T, TStrMut<T>>::find_any;
	using detail::TStrCommon<T, TStrMut<T>>::rfind_any;
	using detail::TStrCommon<T, TStrMut<T>>::find_not;
	using detail::TStrCommon<T, TStrMut<T>>::rfind_not;
	
	bool cmp(const TStrRef<T>& other) const {
		return size() == other.size() && memcmp(ptr, other.data(), size() * sizeof(T)) == 0;
	}
	
	bool operator==(const TStrRef<T>& other) const { return cmp(other); }
	bool operator!=(const TStrRef<T>& other) const { return !cmp(other); }
	
	TStrRef<T> substr(size_t index, size_t count) const {
		return substr(ptr + index, ptr + index + count);
	}
	TStrRef<T> substr(const T* begin, const T* end) const {
		return TStrRef<T>(begin, end);
	}

	bool empty() const { return used_size == 0; }

	T* c_str() const { return ptr; }

	T* data(){ return ptr; }
	const T* data() const { return ptr; }

	T* begin(){ return ptr; }
	const T* begin() const { return ptr; }

	T* end(){ return ptr + used_size; }
	const T* end() const { return ptr + used_size; }

	std::reverse_iterator<T*> rbegin(){ return std::reverse_iterator<T*>(end()); }
	std::reverse_iterator<T*> rend(){ return std::reverse_iterator<T*>(begin()); }
	
	constexpr size_t size() const { return used_size; }
	
	T& operator[](size_t index){ return ptr[index]; }
	const T& operator[](size_t index) const { return ptr[index]; }

	T& front(){ return *ptr; }
	T& back(){ return ptr[used_size-1]; }
	
	~TStrMut(){
		if(ptr != local_str) delete [] ptr;
	}
private:
	T* ptr;
	uint32_t used_size;
	union {
		uint32_t allocated_size;
		T local_str[LOCAL_SZ];
	};
};


using StrMut   = TStrMut<char>;
using StrMut16 = TStrMut<char16_t>;
using StrMut32 = TStrMut<char32_t>;

template<class T>
constexpr TStrRef<T>::TStrRef(const TStrMut<T>& str) : p(str.data()), sz(str.size()){}

}

#endif
