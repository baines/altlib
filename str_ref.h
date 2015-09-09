#ifndef ALTLIB_STR_REF_H_
#define ALTLIB_STR_REF_H_
#include <cstring>
#include <memory>
#include "str_common.h"

namespace alt {
namespace detail {
	template<class T>
	size_t StrLen(const T* ptr){
		const T* p = ptr;
		while(*p) ++p;
		return p - ptr;
	}

	static const size_t STR_REF_ALLOCA_LIMIT = 1024;
}

template<class T> struct TStrMut;

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
