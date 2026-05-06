#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <mkl.h>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

namespace py = pybind11;

// 1. 全域變數追蹤記憶體
static size_t g_bytes = 0;
static size_t g_allocated = 0;
static size_t g_deallocated = 0;

// 2. 自定義分配器 CustomAllocator
template <class T>
struct CustomAllocator {
    using value_type = T;
    CustomAllocator() = default;

    template <class U>
    CustomAllocator(const CustomAllocator<U>&) {}

    T* allocate(std::size_t n) {
        size_t bytes = n * sizeof(T);
        T* p = static_cast<T*>(std::malloc(bytes));
        if (!p) throw std::bad_alloc();
        
        g_bytes += bytes;
        g_allocated += bytes;
        return p;
    }

    void deallocate(T* p, std::size_t n) noexcept {
        size_t bytes = n * sizeof(T);
        g_bytes -= bytes;
        g_deallocated += bytes;
        std::free(p);
    }
};

template <class T, class U>
bool operator==(const CustomAllocator<T>&, const CustomAllocator<U>&) { return true; }
template <class T, class U>
bool operator!=(const CustomAllocator<T>&, const CustomAllocator<U>&) { return false; }

// 3. 修改 Matrix 類別
class Matrix {
public:
    Matrix(size_t nrow, size_t ncol) : m_nrow(nrow), m_ncol(ncol), m_data(nrow * ncol) {
        // 初始化為 0
        std::fill(m_data.begin(), m_data.end(), 0);
    }

    double  operator()(size_t row, size_t col) const { return m_data[row * m_ncol + col]; }
    double& operator()(size_t row, size_t col)       { return m_data[row * m_ncol + col]; }

    bool operator==(const Matrix& other) const {
        return m_nrow == other.m_nrow && m_ncol == other.m_ncol && m_data == other.m_data;
    }

    size_t nrow() const { return m_nrow; }
    size_t ncol() const { return m_ncol; }
    
    double* data() { return m_data.data(); }
    const double* data() const { return m_data.data(); }

private:
    size_t m_nrow, m_ncol;
    // 使用自定義分配器
    std::vector<double, CustomAllocator<double>> m_data;
};

// --- 以下矩陣運算逻辑保持不變 ---

Matrix multiply_naive(const Matrix& m1, const Matrix& m2) {
    if (m1.ncol() != m2.nrow()) throw std::invalid_argument("Dimension mismatch");
    Matrix res(m1.nrow(), m2.ncol());
    for (size_t i = 0; i < m1.nrow(); ++i) {
        for (size_t j = 0; j < m2.ncol(); ++j) {
            double sum = 0;
            for (size_t k = 0; k < m1.ncol(); ++k) {
                sum += m1(i, k) * m2(k, j);
            }
            res(i, j) = sum;
        }
    }
    return res;
}

Matrix multiply_mkl(const Matrix& m1, const Matrix& m2) {
    if (m1.ncol() != m2.nrow()) throw std::invalid_argument("Dimension mismatch");
    Matrix res(m1.nrow(), m2.ncol());
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                m1.nrow(), m2.ncol(), m1.ncol(),
                1.0, m1.data(), m1.ncol(),
                m2.data(), m2.ncol(),
                0.0, res.data(), res.ncol());
    return res;
}

// 4. 定義 Python 介面
PYBIND11_MODULE(_matrix, m) {
    m.def("bytes", []() { return g_bytes; });
    m.def("allocated", []() { return g_allocated; });
    m.def("deallocated", []() { return g_deallocated; });

    py::class_<Matrix>(m, "Matrix")
        .def(py::init<size_t, size_t>())
        .def_property_readonly("nrow", &Matrix::nrow)
        .def_property_readonly("ncol", &Matrix::ncol)
        .def(py::self == py::self)
        .def("__getitem__", [](const Matrix &mat, std::pair<size_t, size_t> idx) {
            if (idx.first >= mat.nrow() || idx.second >= mat.ncol())
                throw py::index_error("Index out of range");
            return mat(idx.first, idx.second);
        })
        .def("__setitem__", [](Matrix &mat, std::pair<size_t, size_t> idx, double val) {
            if (idx.first >= mat.nrow() || idx.second >= mat.ncol())
                throw py::index_error("Index out of range");
            mat(idx.first, idx.second) = val;
        });

    m.def("multiply_naive", &multiply_naive);
    m.def("multiply_mkl", &multiply_mkl);
}