#ifndef ALT_STR_H_
#define ALT_STR_H_
#include <algorithm>
#include <functional>
#include <cstring>
#include <cstdint>
#include <memory>
#include <numeric>
#include <type_traits>

namespace alt {

/********************************************************
   Common base class containing find() functionality
********************************************************/

template<class T> struct TStrRef;

namespace detail {

template<class T, class S>
class TStrCommon {
protected:
	using ref_t = TStrRef<typename std::remove_const<T>::type>;

	T* find(const ref_t& needle, T* b, T* e) const {
		return std::search(b, e, needle.begin(), needle.end());
	}	
	T* find(const ref_t& needle, T* b) const { return find(needle, b, end()); }
	T* find(const ref_t& needle) const { return find(needle, begin(), end()); }
	
	T* rfind(const ref_t& needle, T* b, T* e) const {
		return std::search(
			std::reverse_iterator<T*>(b),
			std::reverse_iterator<T*>(e),
			needle.rbegin(),
			needle.rend()
		).base() - needle.size();
	}
	T* rfind(const ref_t& needle, T* b) const { return rfind(needle, b, begin()); }
	T* rfind(const ref_t& needle) const { return rfind(needle, end(), begin()); }
	
	T* find_any(const ref_t& chars, T* b, T* e) const {
		return std::find_first_of(b, e, chars.begin(), chars.end());
	}
	T* find_any(const ref_t& needle, T* b) const { return find_any(needle, b, end()); }
	T* find_any(const ref_t& needle) const { return find_any(needle, begin(), end()); }

	T* find_not(const ref_t& chars, T* b, T* e) const {
		return std::find_first_of(b, e, chars.begin(), chars.end(), std::not_equal_to<T>());
	}
	T* find_not(const ref_t& needle, T* b) const { return find_not(needle, b, end()); }
	T* find_not(const ref_t& needle) const { return find_not(needle, begin(), end()); }

	T* rfind_any(const ref_t& chars, T* b, T* e) const {
		return std::find_first_of(
			std::reverse_iterator<T*>(b),
			std::reverse_iterator<T*>(e),
			chars.begin(),
			chars.end()
		).base() - 1;
	}
	T* rfind_any(const ref_t& needle, T* b) const { return rfind_any(needle, b, begin()); }
	T* rfind_any(const ref_t& needle) const { return rfind_any(needle, end(), begin()); }

	T* rfind_not(const ref_t& chars, T* b, T* e) const {
		return std::find_first_of(
			std::reverse_iterator<T*>(b),
			std::reverse_iterator<T*>(e),
			chars.begin(),
			chars.end(),
			std::not_equal_to<T>()
		).base() - 1;
	}
	T* rfind_not(const ref_t& needle, T* b) const { return rfind_not(needle, b, begin()); }
	T* rfind_not(const ref_t& needle) const { return rfind_not(needle, end(), begin()); }
private:
	T* begin() const { return const_cast<T*>(reinterpret_cast<const S*>(this)->begin()); }
	T* end() const { return const_cast<T*>(reinterpret_cast<const S*>(this)->end()); }
};

}

/********************************************************
   StrRef, StrRef16, StrRef32 - Immutable string types
********************************************************/

namespace detail {
	template<class T>
	size_t StrLen(const T* ptr){
		const T* p = ptr;
		while(*p) ++p;
		return p - ptr;
	}

	static const size_t STR_REF_ALLOCA_LIMIT = 1024;
}

template<class T> class TStrMut;

template<class T>
struct TStrRef : public detail::TStrCommon<const T, TStrRef<T>> {

	typedef T* iterator;
	typedef const T* const_iterator;

	TStrRef() = default;
	
	constexpr TStrRef(const T* p, size_t sz) : p(p), sz(sz){}
	constexpr TStrRef(const T* a, const T* b) : p(a), sz(b - a){}
	constexpr TStrRef(const T& c) : p(&c), sz(1){}

	constexpr TStrRef(const TStrMut<T>& str);

	template<size_t N>
	constexpr TStrRef(const T (&p)[N]) : p(p), sz(N-1){}

	template<class U>
	constexpr TStrRef(const U* const& p) : p(p), sz(detail::StrLen<T>(p)){}

	template<class F>
	typename std::result_of<F(T*)>::type pass_c_str(F&& func) const {
		std::unique_ptr<T[]> heap_buf;
		T* buf;
		
		if((sz * sizeof(T)) >= detail::STR_REF_ALLOCA_LIMIT){
			heap_buf = std::unique_ptr<T[]>(new T[sz+1]);
			buf = heap_buf.get();
		} else {
			buf = reinterpret_cast<T*>(alloca((sz+1)*sizeof(T)));
		}

		memcpy(buf, p, sz * sizeof(T));
		buf[sz] = 0;

		return func(buf);
	}

	const T& front() const { return p[0]; }
	const T& back() const { return p[sz-1]; }
	
	using detail::TStrCommon<const T, TStrRef<T>>::find;	
	using detail::TStrCommon<const T, TStrRef<T>>::rfind;
	using detail::TStrCommon<const T, TStrRef<T>>::find_any;
	using detail::TStrCommon<const T, TStrRef<T>>::rfind_any;
	using detail::TStrCommon<const T, TStrRef<T>>::find_not;
	using detail::TStrCommon<const T, TStrRef<T>>::rfind_not;
	
	void remove_prefix(size_t n){
		size_t min_sz = std::min(sz, n);
		p += min_sz;
		sz -= min_sz;	
	}

	void remove_suffix(size_t n){
		sz = std::max<ssize_t>(0, sz - n);
	}
	
	bool cmp(const TStrRef<T>& other) const {
		return size() == other.size() && memcmp(p, other.data(), size() * sizeof(T)) == 0;
	}
	
	bool operator==(const TStrRef<T>& other) const { return cmp(other); }
	bool operator!=(const TStrRef<T>& other) const { return !cmp(other); }
	
	constexpr const T* data() const { return p; }
	constexpr const T* begin() const { return p; }
	constexpr const T* end() const { return p + sz; }
	constexpr std::reverse_iterator<const T*> rbegin() const { return std::reverse_iterator<const T*>(end()); }
	constexpr std::reverse_iterator<const T*> rend() const { return std::reverse_iterator<const T*>(begin()); }
	constexpr const T& operator[](size_t i) const { return p[i]; }
	
	constexpr size_t size() const { return sz; }
	constexpr int sizei() const { return static_cast<int>(sz); }
private:
	const T* p;
	size_t sz;
};

using StrRef   = TStrRef<char>;
using StrRef16 = TStrRef<char16_t>;
using StrRef32 = TStrRef<char32_t>;

/********************************************************
   StrMut, StrMut16, StrMut32 - Mutable string types
********************************************************/

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

/********************************************************************
   Iostream << compat stuff, define ALT_STR_IOSTREAM if you want it
********************************************************************/

#ifdef ALT_STR_IOSTREAM
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

#endif

