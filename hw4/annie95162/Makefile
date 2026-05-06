PYTHON := python3
CXX := g++

EXT_SUFFIX := $(shell $(PYTHON)-config --extension-suffix)
PYBIND11_INCLUDES := $(shell $(PYTHON) -m pybind11 --includes)

CXXFLAGS := -O3 -Wall -Wextra -shared -std=c++11 -fPIC $(PYBIND11_INCLUDES)

TARGET := _matrix$(EXT_SUFFIX)
SRC := _matrix.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC) matrix.hpp
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f _matrix*.so *.o
