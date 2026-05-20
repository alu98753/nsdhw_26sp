#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>
#include <vector>

class Matrix
{
public:

    Matrix(std::size_t nrow, std::size_t ncol);

    std::size_t nrow() const;
    std::size_t ncol() const;

    double & operator()(std::size_t i, std::size_t j);
    double operator()(std::size_t i, std::size_t j) const;

    bool operator==(Matrix const & other) const;

    std::vector<double> const & data() const;
    std::vector<double> & data();

private:

    void check_index(std::size_t i, std::size_t j) const;

    std::size_t m_nrow;
    std::size_t m_ncol;
    std::vector<double> m_data;
};

Matrix multiply_naive(Matrix const & lhs, Matrix const & rhs);
Matrix multiply_tile(Matrix const & lhs, Matrix const & rhs, std::size_t tile_size);
Matrix multiply_mkl(Matrix const & lhs, Matrix const & rhs);

#endif
