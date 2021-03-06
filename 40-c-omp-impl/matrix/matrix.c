#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include "matrix.h"

#define RAND09() ( (((float) rand()) / (float) RAND_MAX) * 9)
#define RAND(x) ( (((float) rand()) / (float) RAND_MAX) * x)

/**
 * @brief Prepare a block-wise matrix multiplication by partitioning the input matrices into blocks of
 * size row_split and col_split and further calculating the index ranges for each submatrix.
 * This implementation of calculating the indices is not supposed to be efficient.
 * 
 * @param A 
 * @param B 
 * @param C 
 * @param row_split 
 * @param col_split 
 * @param mult_op 
 * @return int 
 */
int prepare_matrix_block_mult(matrix* A, matrix* B, matrix* C, int row_split, int col_split, matrix_mult_operation* mult_op){
    // check if mul is compatible
    if(A->cols != B->rows || C->rows != A->rows || C->cols != B->cols) return EXIT_FAILURE;

    // because col count in A equals row count in B we can subdivide both matrices equally along row in A and along col in B
    int split_A_cols = A->cols / col_split + (A->cols % col_split);
    int split_A_rows = A->rows / row_split + (A->rows % row_split);
    int split_B_cols = B->cols / row_split + (B->cols % row_split);
    int split_B_rows = split_A_cols;

    // create arrays which hold submatrices
    sub_matrix_meta* meta_A_list = malloc(sizeof(sub_matrix_meta) * (split_A_rows * split_A_cols));
    sub_matrix_meta* meta_B_list = malloc(sizeof(sub_matrix_meta) * (split_B_rows * split_B_cols));
    split_matrix split_A, split_B;
    split_A.rows = split_A_rows;
    split_A.cols = split_A_cols;
    split_A.data = meta_A_list;
    split_B.rows = split_B_rows;
    split_B.cols = split_B_cols;
    split_B.data = meta_B_list;

    // iterate over submatrix arrays and fill indices
    // A
    for(int i = 0; i < split_A_rows; i++){
        for(int j = 0; j < split_A_cols; j++){
            sub_matrix_meta* p = &meta_A_list[MIDX(i, j, split_A_cols)];
            p->col_start = j * col_split;
            p->col_end = fminl(p->col_start + col_split,  A->cols);
            p->row_start = i * row_split;
            p->row_end = fminl(p->row_start + row_split, A->rows);
        }
    }
    // B
    for(int i = 0; i < split_B_rows; i++){
        for(int j = 0; j < split_B_cols; j++){
            sub_matrix_meta* p = &meta_B_list[MIDX(i, j, split_B_cols)];
            p->col_start = j * row_split;
            p->col_end = fminl(p->col_start + row_split,  B->cols);
            // B uses col_split for its rows because it has to match A's columns
            p->row_start = i * col_split;
            p->row_end = fminl(p->row_start + col_split, B->rows);
        }
    }

    mult_op->mat_A = A;
    mult_op->mat_B = B;
    mult_op->mat_C = C;
    mult_op->split_A = split_A;
    mult_op->split_B = split_B;

    return EXIT_SUCCESS;
}

/**
 * @brief Perform block-wise matrix-matrix multiplication with OpenMP parallelism which uses precomputed
 * indices of submatrices.
 * In contrast to the Go implementation the OpenMP implementation does perform poorly on a 
 * ryzen 2600x with hyperthreading. Reducing the thread count to the number of physical cores,
 * using C89 syntax, declaring inner loop variables as private, outsourcing the inner loops, etc. 
 * did NOT help.
 * 
 * @param mult_op 
 */
void matrix_block_mul_omp(matrix_mult_operation* mult_op){
    #pragma omp parallel for
    for(int u = 0; u < mult_op->split_A.rows; u++){
        for(int v = 0; v < mult_op->split_B.cols; v++){
            for(int c = 0; c < mult_op->split_A.cols; c++){
                sub_matrix_mul(mult_op, &mult_op->split_A.data[MIDX(u, c, mult_op->split_A.cols)], &mult_op->split_B.data[MIDX(c, v, mult_op->split_B.cols)]);
            }
        }
    }
}

/**
 * @brief Perform block-wise matrix-matrix multiplication by using precomputed
 * indices of submatrices.
 * 
 * @param mult_op 
 */
void matrix_block_mul(matrix_mult_operation* mult_op){
    for(int u = 0; u < mult_op->split_A.rows; u++){
        for(int v = 0; v < mult_op->split_B.cols; v++){
            for(int c = 0; c < mult_op->split_A.cols; c++){
                sub_matrix_mul(mult_op, &mult_op->split_A.data[MIDX(u, c, mult_op->split_A.cols)], &mult_op->split_B.data[MIDX(c, v, mult_op->split_B.cols)]);
            }
        }
    }
}

/**
 * @brief Multiplies two submatrices during a block-wise matrix-matrix multiplication using
 * precomputed indices.
 * 
 * @param mul_op 
 * @param A 
 * @param B 
 */
void sub_matrix_mul(matrix_mult_operation* mul_op, sub_matrix_meta* A, sub_matrix_meta* B){
    for(int i = A->row_start; i < A->row_end; i++){
        for(int j = B->col_start; j < B->col_end; j++){
            float acc = mul_op->mat_C->data[MIDX(i, j, mul_op->mat_C->cols)];
            for(int k = A->col_start; k < A->col_end; k++){
                float val_left = mul_op->mat_A->data[MIDX(i, k, mul_op->mat_A->cols)];
                float val_right = mul_op->mat_B->data[MIDX(k, j, mul_op->mat_B->cols)]; 
                acc += val_left * val_right;
            }
            mul_op->mat_C->data[MIDX(i, j, mul_op->mat_C->cols)] = acc;
        }
    }
}

/**
 * @brief Perform block-wise matrix-matrix multiplication with OpenMP parallelism without
 * precomputed indices.
 * Peformance is almost equal to precomputed indices version. See @matrix_block_mul_omp
 * 
 * @param A 
 * @param B 
 * @param C 
 * @param row_split 
 * @param col_split 
 * @return int 
 */
int matrix_block_mul_inline_omp(matrix* A, matrix* B, matrix* C, int row_split, int col_split){
    if(A->cols != B->rows) return EXIT_FAILURE;

    // The following three loops are iterating over the block matrices
    #pragma omp parallel for
    for(int i_ = 0; i_ < A->rows; i_ += row_split){
        // Note: we are going in row_split steps along the columns of B because the split along rows of A has to be equal to the split along columns of B
        for(int j_ = 0; j_ < B->cols; j_ += row_split){
            for(int k_ = 0; k_ < A->cols; k_ += col_split){
                // The remaining loops are for the regular matrix multiplication with the exception to minor changes due to block matrix multiplication
                for(int i = i_; i < fminl(i_ + row_split, A->rows); i++){
                    for(int j = j_; j < fminl(j_ + row_split, B->cols); j++){
                        float acc = C->data[MIDX(i, j, C->cols)];
                        for(int k = k_; k < fminl(k_ + col_split, A->cols); k++){
                            acc += A->data[MIDX(i, k, A->cols)] * B->data[MIDX(k, j, B->cols)];
                        }
                        C->data[MIDX(i, j, C->cols)] = acc;
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Multiply two given matrices A and B in vanilla style and store the result in C
 * 
 * @param A 
 * @param B 
 * @param C 
 * @return int 
 */
int matrix_vanilla_mul(matrix* A, matrix* B, matrix* C){
    if(A->cols != B->rows) return EXIT_FAILURE;

    for(int i = 0; i < A->rows; i++){
        for(int j = 0; j < B->cols; j++){
            float acc = C->data[MIDX(i, j, C->cols)];
            for(int k = 0; k < A->cols; k++){
                acc += A->data[MIDX(i, k, A->cols)] * B->data[MIDX(k, j, B->cols)];
            }
            C->data[MIDX(i, j, C->cols)] = acc;
        }
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Multiply two given matrices A and B in vanilla style and store the result in C.
 * In contrast to the Go implementation the OpenMP implementation does perform poorly on a 
 * ryzen 2600x with hyperthreading. Reducing the thread count to the number of physical cores,
 * using C89 syntax, declaring inner loop variables as private, outsourcing the inner loops, etc. 
 * did NOT help.
 * 
 * @param A 
 * @param B 
 * @param C 
 * @return int 
 */
int matrix_vanilla_mul_omp(matrix* A, matrix* B, matrix* C){
    if(A->cols != B->rows) return EXIT_FAILURE;

    #pragma omp parallel for
    for(int i = 0; i < A->rows; i++){
        for(int j = 0; j < B->cols; j++){
            float dot = 0;
            for(int k = 0; k < A->cols; k++){
                dot += A->data[MIDX(i, k, A->cols)] * B->data[MIDX(k, j, B->cols)];
            }
            C->data[MIDX(i, j, C->cols)] = dot;
        }
    }
    
    return EXIT_SUCCESS;
}

/**
 * @brief Create a matrix object and allocate memory for the float array
 * 
 * @param rows 
 * @param cols 
 * @return matrix 
 */
matrix create_matrix(long rows, long cols){
    matrix mat;
    mat.cols = cols;
    mat.rows = rows;
    mat.data = malloc(sizeof(float) * (rows * cols));

    return mat;
}

/**
 * @brief Free the data used for storing the submatrix indices
 * 
 * @param mult_op 
 */
void close_matrix_mult(matrix_mult_operation* mult_op){
    free(mult_op->split_A.data);
    free(mult_op->split_B.data);
}

/**
 * @brief Free the matrix float array
 * 
 * @param mat 
 */
void free_matrix(matrix* mat){
    free(mat->data);
    mat->data = NULL;
}

/**
 * @brief Initialize a given matrix with random float values ranging from 0.0 to 9.0
 * 
 * @param mat 
 */
void matrix_random_init(matrix* mat, float max){
    for(int i = 0; i < mat->rows * mat->cols; i++){
        float rnd = RAND(max);
        //fprintf(stdout, "Generating random number: %1.2f\n", rnd);
        mat->data[i] = rnd;
    }
}

/**
 * @brief Initialize a give matrix by using indices as values in increasing order.
 * 
 * @param mat 
 */
void matrix_simple_init(matrix* mat){
    for(int i = 0; i < mat->rows * mat->cols; i++){
        mat->data[i] = (float)i;
    }
}