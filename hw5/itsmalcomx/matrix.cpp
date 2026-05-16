#include "matrix.hpp"
#include <algorithm>
#include <stdexcept>
#include <mkl.h>


Matrix multiply_naive(const Matrix & A, const Matrix & B)
{
    if (A.ncol() != B.nrow())
        throw std::invalid_argument("multiply_naive: dimension mismatch");

    const size_t M = A.nrow();
    const size_t K = A.ncol();
    const size_t N = B.ncol();

    Matrix C(M, N);
    for (size_t i = 0; i < M; ++i)
        for (size_t j = 0; j < N; ++j)
            for (size_t k = 0; k < K; ++k)
                C(i, j) += A(i, k) * B(k, j);
    return C;
}

// Tiled uses i-k-j order inside tiles: cache-FRIENDLY
Matrix multiply_tile(const Matrix & A, const Matrix & B, size_t tsize)
{
    if (A.ncol() != B.nrow())
        throw std::invalid_argument("multiply_tile: dimension mismatch");

    if (tsize == 0) return multiply_naive(A, B);

    const size_t M = A.nrow();
    const size_t K = A.ncol();
    const size_t N = B.ncol();

    Matrix C(M, N);

    for (size_t i0 = 0; i0 < M; i0 += tsize)
    {
        size_t imax = std::min(i0 + tsize, M);
        for (size_t j0 = 0; j0 < N; j0 += tsize)
        {
            size_t jmax = std::min(j0 + tsize, N);
            for (size_t k0 = 0; k0 < K; k0 += tsize)
            {
                size_t kmax = std::min(k0 + tsize, K);
                // i-k-j order inside tile: sequential access on both A row and B row
                for (size_t i = i0; i < imax; ++i)
                    for (size_t k = k0; k < kmax; ++k)
                    {
                        double a_ik = A(i, k);
                        for (size_t j = j0; j < jmax; ++j)
                            C(i, j) += a_ik * B(k, j);
                    }
            }
        }
    }
    return C;
}

Matrix multiply_mkl(const Matrix & A, const Matrix & B)
{
    if (A.ncol() != B.nrow())
        throw std::invalid_argument("multiply_mkl: dimension mismatch");

    Matrix C(A.nrow(), B.ncol());
    cblas_dgemm(
        CblasRowMajor, CblasNoTrans, CblasNoTrans,
        A.nrow(), B.ncol(), A.ncol(),
        1.0, A.data(), A.ncol(),
        B.data(), B.ncol(),
        0.0, C.data(), C.ncol()
    );
    return C;
}
