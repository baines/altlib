#ifndef ALTLIB_STR_REF_H_
#define ALTLIB_STR_REF_H_
#include <cstring>
#include <memory>

namespace alt {

template<class T>
size_t StrLen(const T* ptr){
	const T* p = ptr;
	while(*p) ++p;
	return p - ptr;
}

template<class T>
struct TStrRef {
	TStrRef() = default;
	
	template<size_t N>
	constexpr TStrRef(const T (&p)[N]) : p(p), sz(N-1){}
	constexpr TStrRef(const T* p, size_t sz) : p(p), sz(sz){}
	constexpr TStrRef(char* p) : p(p), sz(StrLen<T>(p)){}
	constexpr TStrRef(const T* a, const T* b) : p(a), sz(b - a){}
	constexpr TStrRef(const char& c) : p(&c), sz(1){}

	template<class F>
	void pass_c_str(F&& func) const {
		const bool use_heap = sz >= 1024;
		T* buf = use_heap ? new T[sz+1] : reinterpret_cast<T*>(alloca(sz+1));
		memcpy(buf, p, sz * sizeof(T));
		buf[sz] = 0;

		func(buf);
		
		if(use_heap) delete [] buf;		
	}
	
	char* find(const TStrRef<T>& needle, size_t offset = 0);	
	char* rfind(const TStrRef<T>& needle, size_t offset = 0);
	char* find_any(const TStrRef<T>& chars, size_t offset = 0);
	char* rfind_any(const TStrRef<T>& chars, size_t offset = 0);
	
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
