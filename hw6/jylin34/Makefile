# Makefile

.PHONY: all clean

# The file that the C++ extension for python will generate.
PYEXT_TARGET := $(shell find build -name "_matrix*.so" 2>/dev/null)

all: _matrix.so

_matrix.so: mod.cpp mm.cpp matrix.hpp CMakeLists.txt
	@echo "Running cmake..."
	cmake -B build .
	@echo "Building with cmake..."
	cmake --build build
	@echo "Copying shared library..."
	cp build/_matrix*.so .

clean:
	@echo "Cleaning up..."
	-rm -f _matrix*.so
	-rm -rf build
	-rm -rf __pycache__ .pytest_cache
	-rm -f performance.txt