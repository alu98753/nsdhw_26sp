CXX      = g++
CXXFLAGS = -std=c++17 -O3 -march=native -funroll-loops -ffast-math -Wall -fPIC -shared

PYTHON      ?= python3
PY_SUFFIX   := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))")

TARGET = _matrix$(PY_SUFFIX)

# Python headers (works even without pip pybind11)
PY_INCLUDE  := $(shell $(PYTHON) -c "import sysconfig; print('-I' + sysconfig.get_path('include'))")

# pybind11 headers: try pip first, fall back to system
PYBIND11_INCLUDE := $(shell $(PYTHON) -m pybind11 --includes 2>/dev/null || echo "-I/usr/include/pybind11")

# numpy headers
NUMPY_INCLUDE := $(shell $(PYTHON) -c "import numpy; print('-I' + numpy.get_include())" 2>/dev/null || \
                 python3 -c "import numpy; print('-I' + numpy.get_include())")

ifneq ("$(wildcard PRELOAD_MKL)","")
    BLAS_CFLAGS = -DUSE_MKL -I/usr/include/mkl
    BLAS_LFLAGS = -L/usr/lib/x86_64-linux-gnu \
                  -lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl
    $(info INFO: building with MKL)
else
    BLAS_CFLAGS =
    BLAS_LFLAGS = -L/usr/lib/x86_64-linux-gnu/openblas-pthread -lopenblas
    $(info INFO: building with OpenBLAS)
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): matrix.cpp mod.cpp
	$(CXX) $(CXXFLAGS) \
		$(PY_INCLUDE) \
		$(PYBIND11_INCLUDE) \
		$(NUMPY_INCLUDE) \
		$(BLAS_CFLAGS) \
		-o $@ $^ \
		$(BLAS_LFLAGS)

clean:
	rm -f _matrix*.so _matrix*.pyd
