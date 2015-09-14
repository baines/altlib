#ifndef ALTLIB_STR_COMMON_H_
#define ALTLIB_STR_COMMON_H_
#include <algorithm>
#include <functional>

namespace alt {

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

}

#endif
