CXX = g++
# 1. 增加 MKL 的標頭檔路徑 (-I)
CXXFLAGS = -O3 -Wall -shared -std=c++14 -fPIC \
           $(shell python3 -m pybind11 --includes) \
           -I/usr/include/mkl

# 2. 增加 MKL 的函式庫路徑 (-L) 確保連結器找得到
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lmkl_rt

# 找到 extension suffix
TARGET = _matrix$(shell python3-config --extension-suffix)

all: $(TARGET)

$(TARGET): _matrix.cpp
	$(CXX) $(CXXFLAGS) _matrix.cpp -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf $(TARGET) __pycache__ .pytest_cache