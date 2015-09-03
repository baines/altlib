#ifndef ALTLIB_STR_REF_H_
#define ALTLIB_STR_REF_H_
#include <cstring>
#include <memory>

namespace alt {
namespace detail {
template<class T>
size_t StrLen(const T* ptr){
	const T* p = ptr;
	while(*p) ++p;
	return p - ptr;
}

template<class T>
using enable_if_result_void = typename std::enable_if<
	std::is_same<void, typename std::result_of<T>::type>::value,
	typename std::result_of<T>::type
>;

template<class T>
using enable_if_result_not_void = typename std::enable_if<
	!std::is_same<void, typename std::result_of<T>::type>::value,
	typename std::result_of<T>::type
>;

}

template<class T>
struct TStrRef {

	typedef T* iterator;
	typedef const T* const_iterator;

	TStrRef() = default;
	
	constexpr TStrRef(const T* p, size_t sz) : p(p), sz(sz){}
	constexpr TStrRef(const T* a, const T* b) : p(a), sz(b - a){}
	constexpr TStrRef(const T& c) : p(&c), sz(1){}

	template<size_t N>
	constexpr TStrRef(const T (&p)[N]) : p(p), sz(N-1){}

	template<class U>
	constexpr TStrRef(const U* const& p) : p(p), sz(detail::StrLen<T>(p)){}

	template<class F>
	typename detail::enable_if_result_void<F(T*)>::type pass_c_str(F&& func) const {
		const bool use_heap = sz >= 1024;
		T* buf = use_heap ? new T[sz+1] : reinterpret_cast<T*>(alloca(sz+1));
		memcpy(buf, p, sz * sizeof(T));
		buf[sz] = 0;

		func(buf);
		
		if(use_heap) delete [] buf;
	}
	
	template<class F>
	typename detail::enable_if_result_not_void<F(T*)>::type pass_c_str(F&& func) const {
		const bool use_heap = sz >= 1024;
		T* buf = use_heap ? new T[sz+1] : reinterpret_cast<T*>(alloca(sz+1));
		memcpy(buf, p, sz * sizeof(T));
		buf[sz] = 0;

		typename std::result_of<F(T*)>::type result = func(buf);
		
		if(use_heap) delete [] buf;
		
		return result;
	}

	const T& front() const { return p[0]; }
	const T& back() const { return p[sz-1]; }
	
	const T* find(const TStrRef<T>& needle, size_t offset = 0) const;	
	const T* rfind(const TStrRef<T>& needle, size_t offset = 0) const;
	const T* find_any(const TStrRef<T>& chars, size_t offset = 0) const;
	const T* rfind_any(const TStrRef<T>& chars, size_t offset = 0) const;

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
	constexpr const T& operator[](size_t i){ return p[i]; }
	
	constexpr size_t size() const { return sz; }
	constexpr int sizei() const { return static_cast<int>(sz); }
private:
	const T* p;
	size_t sz;
};

using StrRef   = TStrRef<char>;
using StrRef16 = TStrRef<char16_t>;
using StrRef32 = TStrRef<char32_t>;

}

#endif
