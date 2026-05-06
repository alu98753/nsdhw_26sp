#include <pybind11/pybind11.h>
#include <utility>

#include "matrix.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_matrix, m)
{
    m.def("bytes", &memtrack::bytes);
    m.def("allocated", &memtrack::allocated);
    m.def("deallocated", &memtrack::deallocated);

    py::class_<Matrix>(m, "Matrix")
        .def(py::init<std::size_t, std::size_t>())
        .def_property_readonly("nrow", &Matrix::nrow)
        .def_property_readonly("ncol", &Matrix::ncol)
        .def("__getitem__",
            [](const Matrix& mat, std::pair<std::size_t, std::size_t> index)
            {
                return mat.get(index.first, index.second);
            })
        .def("__setitem__",
            [](Matrix& mat,
               std::pair<std::size_t, std::size_t> index,
               double value)
            {
                mat.set(index.first, index.second, value);
            })
        .def("__eq__", &Matrix::equals);

    m.def("multiply_naive", &multiply_naive);
    m.def("multiply_mkl", &multiply_mkl);
}
