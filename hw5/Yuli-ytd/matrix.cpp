#include <vector>
#include <stdexcept>
#include <algorithm>
#include <mkl.h>
#include "matrix.hpp"

Matrix multiply_naive(Matrix const & mat1, Matrix const & mat2){

    // multipliable check
    if(mat1.ncol() != mat2.nrow()){
        throw std::out_of_range(
            "Matrices aren't multipliable"
        );
    }

    // buffer allocation for matrix multiplication
    Matrix result(mat1.nrow(), mat2.ncol());
   
    for (size_t i = 0; i < result.nrow(); i++) {
        for (size_t j = 0; j < result.ncol(); j++) {
            double ele = result(i, j);
            for (size_t k = 0; k < mat1.ncol(); k++) {
                ele += mat1(i, k) * mat2(k, j);
            }
            result(i, j) = ele;
        }
    }

    return result;
}

Matrix multiply_tile(Matrix const & mat1, Matrix const & mat2, size_t tsize){
    
    // multipliable check
    if(mat1.ncol() != mat2.nrow()){
        throw std::out_of_range(
            "Matrices aren't multipliable"
        );
    }

    // buffer allocation for matrix multiplication
    Matrix result(mat1.nrow(), mat2.ncol());
    size_t const nrow_res = result.nrow();
    size_t const ncol_res = result.ncol();
    size_t const ncol_mat1 = mat1.ncol();

    size_t const tile_size = tsize;

    for (size_t jj = 0; jj < ncol_res; jj += tile_size) {
        for (size_t kk = 0; kk < ncol_mat1; kk += tile_size) {
            for (size_t ii = 0; ii < nrow_res; ii += tile_size) {
                
                for (size_t j = jj; j < std::min(jj + tile_size, ncol_res); ++j) {
                    for (size_t k = kk; k < std::min(kk + tile_size, ncol_mat1); ++k) {
                        double b_val = mat2(k, j);
                        for (size_t i = ii; i < std::min(ii + tile_size, nrow_res); ++i) {
                            result(i, j) += mat1(i, k) * b_val;
                        }
                    }
                }

            }
        }
    }
    return result;
}

Matrix multiply_mkl(Matrix const & mat1, Matrix const & mat2){

    // multipliable check
    if(mat1.ncol() != mat2.nrow()){
        throw std::out_of_range(
            "Matrices aren't multipliable"
        );
    }

    // buffer allocation for matrix multiplication
    Matrix result(mat1.nrow(), mat2.ncol());

    cblas_dgemm(CblasColMajor, //layout
        CblasNoTrans, CblasNoTrans, //transpose mat1, mat2
        mat1.nrow(), mat2.ncol(), mat1.ncol(), //dimension of matrices
        1.0, //alpha
        mat1.data(), mat1.nrow(), //pointer to first ele of mat1, leading dimension of mat1 (lda)
        mat2.data(), mat2.nrow(), //pointer to first ele of mat2, lab
        0.0, //result <- alpha(mat1 x mat2) + beta(result)
        result.data(), //pointer to first ele of result
        result.nrow() //lac
    );

    return result;
}