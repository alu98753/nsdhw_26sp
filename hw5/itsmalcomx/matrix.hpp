#pragma once

#include <vector>
#include <stdexcept>
#include <cstddef>

class Matrix
{
public:
    Matrix(size_t nrow, size_t ncol)
      : m_nrow(nrow), m_ncol(ncol), m_buffer(nrow * ncol, 0.0)
    {}

    // Copy
    Matrix(Matrix const &)             = default;
    Matrix & operator=(Matrix const &) = default;

    // Move
    Matrix(Matrix &&) noexcept             = default;
    Matrix & operator=(Matrix &&) noexcept = default;

    ~Matrix() = default;

    // Element access (row-major)
    double   operator()(size_t row, size_t col) const { return m_buffer[row * m_ncol + col]; }
    double & operator()(size_t row, size_t col)       { return m_buffer[row * m_ncol + col]; }

    size_t nrow() const { return m_nrow; }
    size_t ncol() const { return m_ncol; }

    bool operator==(Matrix const & other) const
    {
        if (m_nrow != other.m_nrow || m_ncol != other.m_ncol) return false;
        return m_buffer == other.m_buffer;
    }

    double * data()       { return m_buffer.data(); }
    double const * data() const { return m_buffer.data(); }

private:
    size_t              m_nrow;
    size_t              m_ncol;
    std::vector<double> m_buffer;
};

// Free function declarations
Matrix multiply_naive(Matrix const & A, Matrix const & B);
Matrix multiply_tile (Matrix const & A, Matrix const & B, size_t tsize);
Matrix multiply_mkl  (Matrix const & A, Matrix const & B);
