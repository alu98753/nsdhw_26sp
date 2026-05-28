
SRC:=$(wildcard	*.cpp)
TARGET=_matrix$(PY_EXT)
_INSTALL_DUMMY	:=	$(shell	python3	-m	pip	install	pybind11)
NUMPY_INC := $(shell python3 -c "import numpy; print(numpy.get_include())")
PY_INCLUDES:=$(shell	python3	-m	pybind11	--includes)	
PY_EXT	:=$(shell	python3-config	--extension-suffix)	

all:$(TARGET)

$(TARGET):	$(SRC)
	g++	-O3	-Wall	-shared	-std=c++17	-fPIC	$(PY_INCLUDES)	-I/usr/include/mkl	-I$(NUMPY_INC)	$(SRC)	-o	$(TARGET)	-L/usr/lib/x86_64-linux-gnu -lmkl_rt

.PHONY:	test	clean

test:	all
	python3	-m	unittest	test_matrix.py
clean:
	rm	-rf	*$(PY_EXT)	__pycache__	.pytest_cache

