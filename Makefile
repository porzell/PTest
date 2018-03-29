all: my_first_test unit_test

%: examples/%.cpp
	g++ -std=c++11 $< -o $@

