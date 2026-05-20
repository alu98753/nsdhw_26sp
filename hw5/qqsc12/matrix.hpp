#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <mkl.h>

class Matrix {
public:
    // 建構子：初始化指定大小的矩陣，並將所有元素設為 0.0
    Matrix(size_t nrow, size_t ncol) 
        : m_nrow(nrow), m_ncol(ncol), m_data(nrow * ncol, 0.0) {}

    // 重載括號運算子 () 方便 C++ 內部進行 (row, col) 的 2D 存取
    double  operator()(size_t row, size_t col) const { return m_data[row * m_ncol + col]; }
    double& operator()(size_t row, size_t col)       { return m_data[row * m_ncol + col]; }

    // 判斷兩個矩陣是否相等
    bool operator==(const Matrix& other) const {
        return m_nrow == other.m_nrow && m_ncol == other.m_ncol && m_data == other.m_data;
    }

    size_t nrow() const { return m_nrow; }
    size_t ncol() const { return m_ncol; }
    
    // 獲取底層一維陣列的指標，提供給 MKL 使用
    double* data() { return m_data.data(); }
    const double* data() const { return m_data.data(); }

private:
    size_t m_nrow, m_ncol;
    std::vector<double> m_data; // 底層連續記憶體儲存
};

// 1. Naive 慢速乘法 (最内層對 m2 造成非連續的跳躍存取，藉此拉大與優化版的效能差距)
inline Matrix multiply_naive(const Matrix& m1, const Matrix& m2) {
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

// 2. Tiling 區塊優化乘法 (從你 HW3 的優良寫法繼承，快取局部性高，且對編譯器友善)
inline Matrix multiply_tile(const Matrix& m1, const Matrix& m2, size_t tsize) {
    if (m1.ncol() != m2.nrow()) throw std::invalid_argument("Dimension mismatch");
    if (tsize == 0) return multiply_naive(m1, m2);

    const size_t nrow1 = m1.nrow();
    const size_t ncol1 = m1.ncol();
    const size_t ncol2 = m2.ncol();
    Matrix res(nrow1, ncol2);

    const double* m1_ptr = m1.data();
    const double* m2_ptr = m2.data();
    double* res_ptr = res.data();

    // 採用外部區塊分割：i0 -> j0 -> k0
    for (size_t i0 = 0; i0 < nrow1; i0 += tsize) {
        size_t i_end = std::min(i0 + tsize, nrow1);
        for (size_t j0 = 0; j0 < ncol2; j0 += tsize) {
            size_t j_end = std::min(j0 + tsize, ncol2);
            for (size_t k0 = 0; k0 < ncol1; k0 += tsize) {
                size_t k_end = std::min(k0 + tsize, ncol1);

                // 內部核心運算
                for (size_t i = i0; i < i_end; ++i) {
                    size_t i_off = i * ncol1;
                    size_t res_off = i * ncol2;
                    for (size_t k = k0; k < k_end; ++k) {
                        double r = m1_ptr[i_off + k];
                        size_t k_off = k * ncol2;
                        
                        // 確保最內層是連續記憶體存取 res_ptr 與 m2_ptr
                        for (size_t j = j0; j < j_end; ++j) {
                            res_ptr[res_off + j] += r * m2_ptr[k_off + j];
                        }
                    }
                }
            }
        }
    }
    return res;
}

// 3. Intel MKL CBLAS 矩陣相乘
inline Matrix multiply_mkl(const Matrix& m1, const Matrix& m2) {
    if (m1.ncol() != m2.nrow()) throw std::invalid_argument("Dimension mismatch");
    Matrix res(m1.nrow(), m2.ncol());

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                m1.nrow(), m2.ncol(), m1.ncol(),
                1.0, m1.data(), m1.ncol(),
                m2.data(), m2.ncol(),
                0.0, res.data(), res.ncol());
    return res;
}

#endif // MATRIX_HPP
