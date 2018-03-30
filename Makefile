EXAMPLES_SOURCE=$(wildcard examples/*.cpp)
EXAMPLES_EXECUTABLES = $(patsubst examples/%.cpp, %, $(EXAMPLES_SOURCE))

all: $(EXAMPLES_EXECUTABLES)

.PHONY : clean
clean :
	-rm -f $(EXAMPLES_EXECUTABLES)
%: examples/%.cpp
	g++-5 -std=c++14 $< -o $@

