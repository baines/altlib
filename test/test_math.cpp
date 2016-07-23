#include "../alt_math.h"
#include <cstdio>

int main(void){

	using namespace alt;
	
	// vectors

	vec2 a = { 1.0f, 2.0f };
	vec2 b = { 3.0f, 4.0f };

	// vector ops

	a += b;
	a -= b;

	// scalar ops

	a += 1.0f;
	a -= 2.0f;
	a *= 3.0f;
	a /= 4.0f;

	a = a + 1.0f;
	a = a - 2.0f;
	a = a * 3.0f;
	a = a / 3.0f;

	// funcs

	a = { 1.0f, 2.0f };

	printf("a: ");
	for(size_t i = 0; i < vec2::size; ++i){
		printf("%.2f ", a[i]);
	}
	puts("");
	
	printf("b: ");
	for(size_t i = 0; i < vec2::size; ++i){
		printf("%.2f ", b[i]);
	}
	puts("");

	printf("a length: %.2f, b length: %.2f, dot: %.2f,\n", length(a), length(b), dot(a, b));

	a = normalize(a);
	b = normalize(b);

	printf("normalized a: ");
	for(size_t i = 0; i < vec2::size; ++i){
		printf("%.2f ", a[i]);
	}
	puts("");
	
	printf("normalized b: ");
	for(size_t i = 0; i < vec2::size; ++i){
		printf("%.2f ", b[i]);
	}
	puts("");

	printf("a length: %.2f, b length: %.2f, dot: %.2f,\n", length(a), length(b), dot(a, b));
	
	// matrices

	mat2 ma = { 1.0f, 2.0f, 3.0f, 4.0f };

	mat4 mb = ortho(0.f, 100.f, 0.f, 100.f, 1.f, 100.f);
	printf("ortho: ");
	for(size_t i = 0; i < mb.len(); ++i){
		printf("%.2f ", mb.data[i]);
	}
	puts("");

	mat4 mc = frustum(0.f, 100.f, 0.f, 100.f, 1.f, 100.f);
	printf("frustum: ");
	for(size_t i = 0; i < mc.len(); ++i){
		printf("%.2f ", mc.data[i]);
	}
	puts("");

}
