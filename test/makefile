
.PHONY: all clean

all: test_str_mut test_array test_math benchmark

test_str_mut: test_str_mut.cpp ../alt_str.h
	g++ -g -Og -std=c++1y $< -o $@

test_array: test_array.cpp ../alt_array.h
	g++ -g -Og -std=c++1y $< -o $@

test_math: test_math.cpp ../alt_math.h ../alt_math_vec.inc ../alt_math_mat.inc
	g++ -g -Og -std=c++1y $< -o $@

benchmark: benchmark.cpp ../alt_str.h
	g++ -ffast-math -fno-stack-protector -U_FORTIFY_SOURCE -O2 -std=c++1y $< -o $@

clean:
	rm -f test_str_mut test_array test_math benchmark
