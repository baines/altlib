#ifndef ALT_ARRAY_H_
#define ALT_ARRAY_H_

#ifndef ALT_ARRAY_MEMCPY
	#include <string.h>
	#define ALT_ARRAY_MEMCPY memcpy
#endif

namespace alt {

template<class T, size_t N, class = void>
struct Array {

	constexpr T& operator[](size_t n){
		return data[n];
	}
	
	T* begin(){ return data; }
	T* end(){ return data + N; }
	
	const T* begin() const { return data; }
	const T* end() const { return data + N; }
	
	bool operator==(const Array& other) const {
		return memcmp(data, other.data, sizeof(data)) == 0;
	}
	
	bool operator!=(const Array& other) const {
		return memcmp(data, other.data, sizeof(data)) != 0;
	}
	
	bool operator<(const Array& other) const {
		return memcmp(data, other.data, sizeof(data)) < 0;
	}
	
	bool operator>(const Array& other) const {
		return memcmp(data, other.data, sizeof(data)) > 0;
	}
	
	constexpr size_t size(){ return N; }

	T data[N];
};

}

#endif
