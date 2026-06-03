cmake_minimum_required(VERSION 3.10)
project(_matrix) 

find_package(PythonInterp REQUIRED)
find_package(pybind11 REQUIRED)

execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import numpy; print(numpy.get_include())"
    OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

pybind11_add_module(_matrix mod.cpp mm.cpp)

target_include_directories(_matrix PRIVATE ${NUMPY_INCLUDE_DIR})

target_compile_options(_matrix PRIVATE -O3)

target_include_directories(_matrix PRIVATE /usr/include/mkl)

target_link_libraries(_matrix PRIVATE mkl_rt)
