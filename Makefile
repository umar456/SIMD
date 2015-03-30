

main: main.cpp simd.hpp
	g++-4.9 -g -O3 -Wa,-q -march=haswell -std=c++11 -fpermissive $< -o $@

test: test.cpp
	g++-4.9 -g -O3 -Wa,-q -march=haswell -std=c++11 -fpermissive $< -o $@

asm: test.cpp
	g++-4.9 -g -O3 -Wa,-q -S -march=haswell -std=c++11 -fpermissive $< -o $@
clean:
	rm test
