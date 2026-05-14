CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17 -fPIC
LDFLAGS = -lmkl_rt

PYTHON_INCLUDES := $(shell python3-config --includes)
PYBIND11_INCLUDES := $(shell python3 -m pybind11 --includes 2>/dev/null)
NUMPY_INCLUDES := $(shell python3 -c "import numpy; print('-I' + numpy.get_include())")
MKL_INCLUDES := -I/usr/include/mkl
INCLUDES := $(PYTHON_INCLUDES) $(PYBIND11_INCLUDES) $(NUMPY_INCLUDES) $(MKL_INCLUDES)


TARGET = _matrix.so
OBJS = matrix.o mod.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -shared $(OBJS) -o $@ $(LDFLAGS)

matrix.o: matrix.cpp matrix.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

mod.o: mod.cpp matrix.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf *.o *.so __pycache__ .pytest_cache