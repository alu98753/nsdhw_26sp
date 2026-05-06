#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <stdexcept>
#include <cstddef>
#include <atomic>

namespace memtrack {

static std::atomic<std::size_t> current_bytes(0);
static std::atomic<std::size_t> total_allocated(0);
static std::atomic<std::size_t> total_deallocated(0);

inline void add_allocate(std::size_t nbytes)
{
    current_bytes += nbytes;
    total_allocated += nbytes;
}

inline void add_deallocate(std::size_t nbytes)
{
    current_bytes -= nbytes;
    total_deallocated += nbytes;
}

inline std::size_t bytes()
{
    return current_bytes.load();
}

inline std::size_t allocated()
{
    return total_allocated.load();
}

inline std::size_t deallocated()
{
    return total_deallocated.load();
}

} // namespace memtrack


template <typename T>
class CustomAllocator
{
public:
    using value_type = T;

    CustomAllocator() noexcept {}

    template <typename U>
    CustomAllocator(const CustomAllocator<U>&) noexcept {}

    T* allocate(std::size_t n)
    {
        std::size_t nbytes = n * sizeof(T);
        memtrack::add_allocate(nbytes);
        return static_cast<T*>(::operator new(nbytes));
    }

    void deallocate(T* ptr, std::size_t n) noexcept
    {
        std::size_t nbytes = n * sizeof(T);
        memtrack::add_deallocate(nbytes);
        ::operator delete(ptr);
    }

    template <typename U>
    struct rebind
    {
        using other = CustomAllocator<U>;
    };
};

template <typename T, typename U>
bool operator==(const CustomAllocator<T>&, const CustomAllocator<U>&)
{
    return true;
}

template <typename T, typename U>
bool operator!=(const CustomAllocator<T>&, const CustomAllocator<U>&)
{
    return false;
}


class Matrix
{
public:
    using Buffer = std::vector<double, CustomAllocator<double>>;

    Matrix(std::size_t nrow, std::size_t ncol)
        : m_nrow(nrow), m_ncol(ncol), m_buffer(nrow * ncol, 0.0)
    {
    }

    std::size_t nrow() const
    {
        return m_nrow;
    }

    std::size_t ncol() const
    {
        return m_ncol;
    }

    double get(std::size_t i, std::size_t j) const
    {
        check_index(i, j);
        return m_buffer[i * m_ncol + j];
    }

    void set(std::size_t i, std::size_t j, double value)
    {
        check_index(i, j);
        m_buffer[i * m_ncol + j] = value;
    }

    bool equals(const Matrix& other) const
    {
        return m_nrow == other.m_nrow &&
               m_ncol == other.m_ncol &&
               m_buffer == other.m_buffer;
    }

private:
    void check_index(std::size_t i, std::size_t j) const
    {
        if (i >= m_nrow || j >= m_ncol)
        {
            throw std::out_of_range("Matrix index out of range");
        }
    }

private:
    std::size_t m_nrow;
    std::size_t m_ncol;
    Buffer m_buffer;

    friend Matrix multiply_naive(const Matrix& lhs, const Matrix& rhs);
    friend Matrix multiply_mkl(const Matrix& lhs, const Matrix& rhs);
};


inline Matrix multiply_naive(const Matrix& lhs, const Matrix& rhs)
{
    if (lhs.m_ncol != rhs.m_nrow)
    {
        throw std::invalid_argument("Matrix shape mismatch");
    }

    Matrix ret(lhs.m_nrow, rhs.m_ncol);

    for (std::size_t i = 0; i < lhs.m_nrow; ++i)
    {
        for (std::size_t k = 0; k < lhs.m_ncol; ++k)
        {
            double lhs_value = lhs.m_buffer[i * lhs.m_ncol + k];

            for (std::size_t j = 0; j < rhs.m_ncol; ++j)
            {
                ret.m_buffer[i * ret.m_ncol + j] +=
                    lhs_value * rhs.m_buffer[k * rhs.m_ncol + j];
            }
        }
    }

    return ret;
}


inline Matrix multiply_mkl(const Matrix& lhs, const Matrix& rhs)
{
    return multiply_naive(lhs, rhs);
}

#endif
