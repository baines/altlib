#include "../alt_str.h"
#include <string>
#include <chrono>

using namespace std::chrono;

struct Test {
	const char* name;
	long int std_result, alt_result;
};

template<class F>
microseconds time_func(size_t rounds, F&& func){
	auto start = steady_clock::now();
	for(size_t i = 0; i < rounds; ++i){
		func();
	}
	auto end = steady_clock::now();
	
	return duration_cast<microseconds>((end - start) / rounds);
}

template<class Fstd, class Falt>
void benchmark(const char* name, Fstd&& std, Falt&& alt){
	constexpr size_t num_rounds = 300;
	auto t = Test{
		name,
		time_func(num_rounds, std).count(),
		time_func(num_rounds, alt).count()
	};
	printf(
		"| %12s | %9ldµs | %9ldµs | %4ld |\n", 
		t.name, t.std_result, t.alt_result, t.alt_result - t.std_result
	);
}

int main(void){

	puts("+--------------+-------------+-------------+------+");
	puts("|     Test     | std::string | alt::StrMut | Diff |");
	puts("|--------------|-------------|-------------|------|");
		
	std::string long_std = "Test";
	alt::StrMut long_alt = "Test";

	benchmark("Append small",
		[&](){
			for(int i = 0; i < 50000; ++i){
				long_std.append("a");
			}
		},
		[&](){
			for(int i = 0; i < 50000; ++i){
				long_alt.append("a");
			}
		}
	);
	
	benchmark("Append large",
		[](){
			std::string s = "Test";
			for(int i = 0; i < 50000; ++i){
				s.append("longer piece of text.");
			}
		},
		[](){
			alt::StrMut s = "Test";
			for(int i = 0; i < 50000; ++i){
				s.append("longer piece of text.");
			}
		}
	);
	
	// copying backwards takes a long time, apparently...
	benchmark("Erase",
		[&](){
			long_std.erase(2, 2);
		},
		[&](){
			long_alt.erase(2, 2);
		}
	);
		
	benchmark("Replace",
		[](){
			std::string s = "Test";
			for(int i = 0; i < 10000; ++i){
				s.replace(2, 2, "blah");
			}
		},
		[](){
			alt::StrMut s = "Test";
			for(int i = 0; i < 10000; ++i){
				s.replace(2, 2, "blah");
			}
		}
	);
		
	benchmark("Insert",
		[](){
			std::string s = "Test";
			for(int i = 0; i < 10000; ++i){
				s.insert(2, "blah");
			}
		},
		[](){
			alt::StrMut s = "Test";
			for(int i = 0; i < 10000; ++i){
				s.insert(2, "blah");
			}
		}
	);
	
	puts("+--------------+-------------+-------------+------+");
	
	return 0;
}
