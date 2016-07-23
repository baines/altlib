#include "../alt_array.h"
#include <stdio.h>

struct pod {
	int a;
	float b;
	char c[8];
};

struct non_pod {

	non_pod() : a(0), b(0), c(){}

	virtual void func(){
		a = 1;
	}
private:
	int a;
	float b;
	char c[8];
};

int main(void){
	using alt::Array;

	Array<int, 5> array = { 1, 2, 3, 4, 5 };
	
	for(auto& i : array){
		printf("%d\n", i);
	}

	return 0;
}

