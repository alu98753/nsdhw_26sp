# 1. 偵測 Python & NumPy 標頭檔路徑
PYTHON_INC    := $(shell python3-config --includes)
PY_EXT_SUFFIX := $(shell python3-config --extension-suffix)
NUMPY_INC     := $(shell python3 -c "import numpy; print(numpy.get_include())")

# 2. 指定 pybind11 包含路徑 (嘗試使用 python 模組獲取，若無則回退至常見系統路徑)
PYBIND11_INC  := $(shell python3 -m pybind11 --includes)
ifeq ($(PYBIND11_INC),)
PYBIND11_INC := -I/usr/include -I/usr/local/include -I/usr/lib/python3/dist-packages/pybind11/include
endif

# 3. 編譯參數 (優化等級 O3，使用原生 CPU 指令優化，並引入 MKL 和 NumPy 標頭檔目錄)
CXX = g++
CXXFLAGS = -O3 -march=native -Wall -shared -std=c++14 -fPIC $(PYTHON_INC) $(PYBIND11_INC) -I$(NUMPY_INC) -I/usr/include/mkl

# 4. 強制連結 mkl_rt 並將動態鏈結路徑 RPATH 寫入 .so 二進位檔
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lmkl_rt -lpthread -lm -ldl -Wl,-rpath,/usr/lib/x86_64-linux-gnu

# 5. 目標檔名
TARGET = _matrix$(PY_EXT_SUFFIX)

all: $(TARGET)

$(TARGET): _matrix.cpp matrix.hpp
	$(CXX) $(CXXFLAGS) _matrix.cpp -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf $(TARGET) __pycache__ .pytest_cache

.PHONY: all clean
