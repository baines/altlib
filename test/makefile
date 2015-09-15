
.PHONY: all clean

all: benchmark test_str_mut

test_str_mut: test_str_mut.cpp ../alt_str.h
	g++ -g -Og -std=c++11 $< -o $@

benchmark: benchmark.cpp ../alt_str.h
	g++ -ffast-math -fno-stack-protector -U_FORTIFY_SOURCE -O2 -std=c++11 $< -o $@
	
clean:
	rm -f test_str_mut benchmark
