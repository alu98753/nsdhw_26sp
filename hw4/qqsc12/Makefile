CXX = g++
# 使用 python3-config 抓取路徑，這在 GitHub Actions 環境比較穩
PYTHON_INCLUDES = $(shell python3 -m pybind11 --includes || python3-config --includes)
MKL_INCLUDES = -I/usr/include/mkl

CXXFLAGS = -O3 -Wall -shared -std=c++14 -fPIC $(PYTHON_INCLUDES) $(MKL_INCLUDES)
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lmkl_rt

TARGET = _matrix$(shell python3-config --extension-suffix)

all: $(TARGET)

$(TARGET): _matrix.cpp
	$(CXX) $(CXXFLAGS) _matrix.cpp -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf $(TARGET) __pycache__ .pytest_cache