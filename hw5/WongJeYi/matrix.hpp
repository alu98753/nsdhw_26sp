#include "mkl.h"
#include <pybind11/pybind11.h>
#include <Python.h>
#include <algorithm>
#include <vector>
#include <iostream>

namespace py = pybind11;

class ByteCounter
{
private:
    int64_t m_allocated = 0;
    int64_t m_deallocated = 0;

public:
    ByteCounter() = default;
    void increment(int64_t size)
    {
        m_allocated += size;
    }
    void decrement(int64_t size)
    {
        m_deallocated += size;
    }
    ~ByteCounter(){
        m_allocated=0;
        m_deallocated=0;
    };
    ByteCounter(ByteCounter const &other)
    {
        m_allocated = other.m_allocated;
        m_deallocated = other.m_deallocated;
    }
    int64_t allocated()
    {
        return (m_allocated ? m_allocated : 0);
    }
    int64_t deallocated()
    {
        return (m_deallocated ? m_deallocated : 0);
    }
};

ByteCounter b;
template <class T>
struct MyAllocator
{
    using value_type = T;

    
    int64_t byte()
    {
        return (b.allocated() - b.deallocated());
    }
    int64_t allocated()
    {
        return b.allocated();
    }
    int64_t deallocated()
    {
        return b.deallocated();
    }
    MyAllocator() = default;
    
    template <class U>
    constexpr MyAllocator(const MyAllocator<U> &other) {};

    T *allocate(size_t size)
    {
        int64_t byteSize = size * sizeof(T);
        if (T *memory = static_cast<T *>(malloc(byteSize)))
        {
            b.increment(byteSize);
            return memory;
        }
        else
        {
            throw std::bad_alloc();
        }
    }

    void deallocate(T *memory, size_t size)
    {
        std::free(memory);
        int64_t byteSize = size * sizeof(T);
        b.decrement(byteSize);
    }
};
template <class T, class U>
bool operator==(const MyAllocator<T> &t, const MyAllocator<U> &u)
{
    return t.b.allocated == u.b.allocated && t.b.deallocated == u.b.deallocated;
}
template <class T, class U>
bool operator!=(const MyAllocator<T> &t, const MyAllocator<U> &u)
{
    return !t.b.allocated == u.b.allocated && t.b.deallocated == u.b.deallocated;
}


class Matrix
{

public:
    int m_nrow;
    int m_ncol;
    std::vector<double, MyAllocator<double>> vec;
    Matrix() = default;
    ~Matrix() = default;
    // Constructor
    Matrix(int nrow, int ncol) : m_nrow(nrow), m_ncol(ncol)
    {
        vec.resize(nrow * ncol);
        std::fill(vec.begin(), vec.end(), 0.0);
    }
    int64_t byte()
    {
        return (vec.get_allocator().byte());
    }
    int64_t allocated()
    {
        return vec.get_allocator().allocated();
    }
    int64_t deallocated()
    {
        return vec.get_allocator().deallocated();
    }
    int const nrow()const
    {
        return m_nrow;
    }
    int const ncol()const
    {
        return m_ncol;
    }
    // Copy Constructor
    Matrix(const Matrix &other) : m_nrow(other.m_nrow), m_ncol(other.m_ncol)
    {
        vec.resize(m_nrow * m_ncol);
        if (!other.vec.empty())
        {
            std::copy(other.vec.begin(), other.vec.end(), vec.begin());
        }
    }
    Matrix(Matrix &&other) noexcept 
        : m_nrow(other.m_nrow), m_ncol(other.m_ncol), vec(std::move(other.vec)) {
        other.m_nrow = 0;
        other.m_ncol = 0;
    }
    double & operator()(size_t i,size_t j){
        if(i>=m_nrow || j>=m_ncol){
            throw std::out_of_range("Matrix index out of range");
        }
        else{
            return vec[i*m_nrow+j];
        }
    }
    const double & operator()(size_t i,size_t j)const{
        if(i>=m_nrow || j>=m_ncol){
            throw std::out_of_range("Matrix index out of range");
        }
        else{
            return vec[i*m_nrow+j];
        }
    }
    Matrix& operator=(Matrix &&other) noexcept {
        if (this != &other) {
            m_nrow = other.m_nrow;
            m_ncol = other.m_ncol;
            vec = std::move(other.vec);
            other.m_nrow = 0;
            other.m_ncol = 0;
        }
        return *this;
    }
    bool operator==(Matrix const &other)const noexcept{
        return is_equal(other);
    }
    bool is_equal(const Matrix & other) const {
        if (m_nrow != other.m_nrow || m_ncol != other.m_ncol) return false;
        return vec == other.vec;
    }
    static int findLCF(int s)
    {
        if (s <= 1)
            return s;
        for (int i = 2; i < s; i++)
        {
            if (s % i == 0)
            {
                return i;
                break;
            }
        }
        return s;
    }
};
    static Matrix multiply_naive(const Matrix & mat1, const Matrix & mat2)
    {
        if (mat1.m_ncol != mat2.m_nrow)
        {
            throw std::runtime_error("Row and column mismatch");
        }
        if (mat1.m_nrow == 0)
        {
            throw std::runtime_error("empty mat1");
        }

        const std::vector<double, MyAllocator<double>>& m1 = mat1.vec;
        const std::vector<double, MyAllocator<double>> & m2 = mat2.vec;
        Matrix result = Matrix(mat1.m_nrow, mat2.m_ncol);
        // i =row, j=col
        // mat[i][j]
        int M = mat1.m_nrow;
        int N = mat2.m_ncol;
        int K = mat1.m_ncol;

        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                for (int k = 0; k < K; k++)
                {
                    result.vec[i * N + j] += m1[i * K + k] * m2[k * N + j];
                }
            }
        }
        return result;
    }

    static Matrix multiply_tile(const Matrix & mat1, const Matrix & mat2, int tsize)
    {
        if (mat1.m_ncol != mat2.m_nrow)
        {
            throw std::runtime_error("Row and column mismatch");
        }
        if (mat1.m_nrow == 0)
        {
            throw std::runtime_error("empty mat1");
        }
        const std::vector<double, MyAllocator<double>> & m1 = mat1.vec;
        const std::vector<double, MyAllocator<double>> & m2 = mat2.vec;
        Matrix result = Matrix(mat1.m_nrow, mat2.m_ncol);
        // i =row, j=col
        // mat[i][j]
        // Stride = LCF
        if (tsize == 0)
        {
            tsize = 1;
        }

        int M = mat1.m_nrow;
        int N = mat2.m_ncol;
        int K = mat1.m_ncol;
        for (int i = 0; i < M; i += tsize)
        {
            for (int k = 0; k < K; k += tsize)
            {
                for (int j = 0; j < N; j += tsize)
                {
                    for (int ii = i; ii < std::min(i + tsize, M); ii++)
                    {
                        for (int kk = k; kk < std::min(k + tsize, K); kk++)
                        {
                            for (int jj = j; jj < std::min(j + tsize, N); jj++)
                            {
                                result.vec[ii * N + jj] += m1[(kk + K * ii)] * m2[kk * N + jj];
                            }
                        }
                    }
                }
            }
        }
        return result;
    }

    static Matrix multiply_mkl(const Matrix & mat1,const Matrix & mat2)
    {
        if (mat1.m_ncol != mat2.m_nrow)
            throw std::runtime_error("Dimension mismatch");
        Matrix result = Matrix(mat1.m_nrow, mat2.m_ncol);

        if (mat1.vec.empty() || mat2.vec.empty()) return result;
        int m = mat1.m_nrow;
        int n = mat2.m_ncol;
        int k = mat1.m_ncol;

        double alpha = 1.0;
        double beta = 0.0;

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                    m, n, k, alpha, mat1.vec.data(), k, mat2.vec.data(), n, beta, result.vec.data(), n);

        return result;
    }
