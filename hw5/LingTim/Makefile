CXX = g++
CXXFLAGS = -O3 -Wall -shared -std=c++14 -fPIC $(shell python3-config --includes) -I/usr/include/mkl
# 連結 MKL 的函式庫 (包含之前的 no-as-needed 修正)
LDFLAGS = -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl

# 自動獲取 Python 對應的 C++ extension 後綴
TARGET = _matrix$(shell python3-config --extension-suffix)

.PHONY: all clean

all: $(TARGET)

# 注意這裡：把 mod.cpp 和 matrix.cpp 兩個檔案一起編譯 ($^ 代表所有的依賴檔案)
$(TARGET): mod.cpp matrix.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *.so *.o