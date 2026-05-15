#include "matrix.hpp"
#include <mkl.h>
#include <algorithm>

Matrix multiply_naive(const Matrix& mat1, const Matrix& mat2) {
    if (mat1.ncol() != mat2.nrow()) throw std::invalid_argument("Dimension mismatch");
    Matrix result(mat1.nrow(), mat2.ncol());
    
    for (size_t i = 0; i < mat1.nrow(); ++i) {
        for (size_t j = 0; j < mat2.ncol(); ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < mat1.ncol(); ++k) {
                sum += mat1(i, k) * mat2(k, j);
            }
            result(i, j) = sum;
        }
    }
    return result;
}

Matrix multiply_tile(const Matrix& mat1, const Matrix& mat2, size_t tsize) {
    if (mat1.ncol() != mat2.nrow()) throw std::invalid_argument("Dimension mismatch");
    Matrix result(mat1.nrow(), mat2.ncol());
    if (tsize == 0) return multiply_naive(mat1, mat2);

    for (size_t i0 = 0; i0 < mat1.nrow(); i0 += tsize) {
        size_t imax = std::min(i0 + tsize, mat1.nrow());
        for (size_t j0 = 0; j0 < mat2.ncol(); j0 += tsize) {
            size_t jmax = std::min(j0 + tsize, mat2.ncol());
            for (size_t k0 = 0; k0 < mat1.ncol(); k0 += tsize) {
                size_t kmax = std::min(k0 + tsize, mat1.ncol());

                for (size_t i = i0; i < imax; ++i) {
                    for (size_t k = k0; k < kmax; ++k) {
                        double v1 = mat1(i, k);
                        for (size_t j = j0; j < jmax; ++j) {
                            result(i, j) += v1 * mat2(k, j);
                        }
                    }
                }
            }
        }
    }
    return result;
}

Matrix multiply_mkl(const Matrix& mat1, const Matrix& mat2) {
    if (mat1.ncol() != mat2.nrow()) throw std::invalid_argument("Dimension mismatch");
    Matrix result(mat1.nrow(), mat2.ncol());

    cblas_dgemm(
        CblasRowMajor, CblasNoTrans, CblasNoTrans,
        mat1.nrow(), mat2.ncol(), mat1.ncol(),
        1.0, mat1.data(), mat1.ncol(),
        mat2.data(), mat2.ncol(),
        0.0, result.data(), result.ncol()
    );
    return result;
}