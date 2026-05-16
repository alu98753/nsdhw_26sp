#include "matrix.hpp"

#include <stdexcept>
#include <algorithm>
#include <cstring>

// ── Naive O(n³) multiplication ────────────────────────────────────────────────
Matrix multiply_naive(Matrix const & A, Matrix const & B)
{
    if (A.ncol() != B.nrow())
        throw std::out_of_range("multiply_naive: incompatible dimensions");

    const size_t M = A.nrow();
    const size_t K = A.ncol();
    const size_t N = B.ncol();

    Matrix C(M, N);
    for (size_t i = 0; i < M; ++i)
        for (size_t k = 0; k < K; ++k)
        {
            const double a_ik = A(i, k);
            for (size_t j = 0; j < N; ++j)
                C(i, j) += a_ik * B(k, j);
        }
    return C;
}

// ── Tiled multiplication ──────────────────────────────────────────────────────
// Uses a local micro-tile (stack buffer) to ensure cache locality.
// Works for any matrix and tile size — handles boundary tiles cleanly.
Matrix multiply_tile(Matrix const & A, Matrix const & B, size_t tsize)
{
    if (A.ncol() != B.nrow())
        throw std::out_of_range("multiply_tile: incompatible dimensions");

    if (tsize == 0)
        return multiply_naive(A, B);

    const size_t M = A.nrow();
    const size_t K = A.ncol();
    const size_t N = B.ncol();

    Matrix C(M, N);

    // Tile over all three dimensions
    for (size_t i0 = 0; i0 < M; i0 += tsize)
    {
        const size_t i_end = std::min(i0 + tsize, M);
        for (size_t k0 = 0; k0 < K; k0 += tsize)
        {
            const size_t k_end = std::min(k0 + tsize, K);
            for (size_t j0 = 0; j0 < N; j0 += tsize)
            {
                const size_t j_end = std::min(j0 + tsize, N);

                // Inner kernel: i-k-j order keeps B access sequential
                for (size_t i = i0; i < i_end; ++i)
                {
                    const double * __restrict__ a_row = A.data() + i * K;
                    double       * __restrict__ c_row = C.data() + i * N;
                    for (size_t k = k0; k < k_end; ++k)
                    {
                        const double a_ik = a_row[k];
                        const double * __restrict__ b_row = B.data() + k * N;
                        for (size_t j = j0; j < j_end; ++j)
                            c_row[j] += a_ik * b_row[j];
                    }
                }
            }
        }
    }
    return C;
}

// ── MKL / cblas multiplication ────────────────────────────────────────────────
#ifdef USE_MKL
#include <mkl.h>
#else
#include <cblas.h>
#endif

Matrix multiply_mkl(Matrix const & A, Matrix const & B)
{
    if (A.ncol() != B.nrow())
        throw std::out_of_range("multiply_mkl: incompatible dimensions");

    const size_t M = A.nrow();
    const size_t K = A.ncol();
    const size_t N = B.ncol();

    Matrix C(M, N);

    cblas_dgemm(
        CblasRowMajor, CblasNoTrans, CblasNoTrans,
        (int)M, (int)N, (int)K,
        1.0,
        A.data(), (int)K,
        B.data(), (int)N,
        0.0,
        C.data(), (int)N
    );
    return C;
}
