#pragma once

#include <vector>
#include <cstddef>
#include <stdexcept>

class Matrix {
public:
    Matrix(size_t nrow, size_t ncol) : m_nrow(nrow), m_ncol(ncol), m_buffer(nrow * ncol, 0.0) {}

    size_t nrow() const { return m_nrow; }
    size_t ncol() const { return m_ncol; }

    double operator()(size_t row, size_t col) const {
        return m_buffer[row * m_ncol + col];
    }
    double& operator()(size_t row, size_t col) {
        return m_buffer[row * m_ncol + col];
    }

    bool operator==(const Matrix& other) const {
        if (m_nrow != other.m_nrow || m_ncol != other.m_ncol) return false;
        return m_buffer == other.m_buffer;
    }

    const double* data() const { return m_buffer.data(); }
    double* data() { return m_buffer.data(); }

private:
    size_t m_nrow;
    size_t m_ncol;
    std::vector<double> m_buffer;
};

// 宣告矩陣相乘函式
Matrix multiply_naive(const Matrix& mat1, const Matrix& mat2);
Matrix multiply_tile(const Matrix& mat1, const Matrix& mat2, size_t tsize);
Matrix multiply_mkl(const Matrix& mat1, const Matrix& mat2);