#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include "matrix.h"

#define RAND09() ( (((float) rand()) / (float) RAND_MAX) * 9)
#define RAND(x) ( (((float) rand()) / (float) RAND_MAX) * x)

/**
 * @brief Prepare a block-wise matrix multiplication by partitioning the input matrices into blocks of
 * size row_split and col_split and further calculating the index ranges for each submatrix
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

void matrix_block_mul(matrix_mult_operation* mult_op){
    //fprintf(stdout, "***\n*Starting compute. Num max threads: %d\n", omp_get_max_threads());
    for(int u = 0; u < mult_op->split_A.rows; u++){
        #pragma omp parallel for
        for(int v = 0; v < mult_op->split_B.cols; v++){
            for(int c = 0; c < mult_op->split_A.cols; c++){
                sub_matrix_mul(mult_op, &mult_op->split_A.data[MIDX(u, c, mult_op->split_A.cols)], &mult_op->split_B.data[MIDX(c, v, mult_op->split_B.cols)]);
            }
        }
    }
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
            for(int k = 0; k < A->cols; k++){
                C->data[MIDX(i, j, C->cols)] += A->data[MIDX(i, k, A->cols)] * B->data[MIDX(k, j, B->cols)];
            }
        }
    }

    return EXIT_SUCCESS;
}

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

int matrix_block_mul_2(matrix* A, matrix* B, matrix* C, int row_split, int col_split){
    if(A->cols != B->rows) return EXIT_FAILURE;

    // The following three loops are iterating over the block matrices
    for(int i_ = 0; i_ < A->rows; i_ += row_split){
        // Note: we are going in row_split steps along the columns of B because the split along rows of A has to be equal to the split along columns of B
        //#pragma omp parallel for
        for(int j_ = 0; j_ < B->cols; j_ += row_split){
            for(int k_ = 0; k_ < A->cols; k_ += col_split){
                // The remaining loops are for the regular matrix multiplication with the exception to minor changes due to block matrix multiplication
                for(int i = i_; i < fminl(i_ + row_split, A->rows); i++){
                    for(int j = j_; j < fminl(j_ + row_split, B->cols); j++){
                        for(int k = k_; k < fminl(k_ + col_split, A->cols); k++){
                            C->data[MIDX(i, j, C->cols)] += A->data[MIDX(i, k, A->cols)] * B->data[MIDX(k, j, B->cols)];
                        }
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}


void sub_matrix_mul(matrix_mult_operation* mul_op, sub_matrix_meta* A, sub_matrix_meta* B){
    for(int i = A->row_start; i < A->row_end; i++){
        for(int j = B->col_start; j < B->col_end; j++){
            for(int k = A->col_start; k < A->col_end; k++){
                float val_left = mul_op->mat_A->data[MIDX(i, k, mul_op->mat_A->cols)];
                float val_right = mul_op->mat_B->data[MIDX(k, j, mul_op->mat_B->cols)]; 
                mul_op->mat_C->data[MIDX(i, j, mul_op->mat_C->cols)] += val_left * val_right;
                //fprintf(stdout, "mul %1.2f with %1.2f equals %1.2f\n", val_left, val_right, mul_op->mat_C->data[MIDX(i, j, mul_op->mat_C->cols)]);
            }
        }
    }
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
}

/**
 * @brief 
 * 
 * @param rows 
 * @param cols 
 * @return long 
 */
long get_matrix_size(long rows, long cols){
    return rows * cols;
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

/**
 * @brief Print a matrix struct including horizontal and vertical lines which show how
 * the given matrix can be split into submatrices with specified row_split and col_split
 * 
 * @param mat 
 * @param row_split can be negative to disable splitting along columns
 * @param col_split can be negative to disable splitting along rows
 * @param max_len maximum number of columns or rows to print
 */
void print_matrix(char name, matrix* mat, long row_split, long col_split, long max_len){
    long max_row = fminl(mat->rows, max_len);
    long max_col = fminl(mat->cols, max_len);
    fprintf(stdout, "###\n#Printing matrix \"%c\" with \"rows: %ld, cols: %ld, row_split: %ld, col_split: %ld\"\n###\n", name, mat->rows, mat->cols, row_split, col_split);

    // For each row
    for(int i = 0; i < max_row; i++){
        // Print horizontal split line
        if(row_split > 0 && i != 0 && i % row_split == 0){
            for(int k = 0; k < max_col; k++){
                // compensate for the "| " pattern (two characters)
                if(k != 0 && k % col_split == 0){
                    fprintf(stdout, "- ");
                }
                fprintf(stdout, "--- ");
            }
            fprintf(stdout, "\n");
        }
        
        // For each col
        for(int j = 0; j < max_col; j++){
            // Print vertical split line
            if(col_split > 0 && j != 0 && j % col_split == 0){
                fprintf(stdout, "| ");
            }

            fprintf(stdout, "%1.1f ", mat->data[MIDX(i, j, mat->cols)]);
        }
        fprintf(stdout, "\n");
    }
}

/**
 * @brief Print the given split_matrix
 * 
 * @param name 
 * @param mat 
 * @param max_len 
 */
void print_split_matrix(char name, split_matrix* mat, long max_len){
    long max_row = fminl(mat->rows, max_len);
    long max_col = fminl(mat->cols, max_len);
    fprintf(stdout, "###\n#Printing split-matrix \"%c\" with \"rows: %ld, cols: %ld\"\n###\n", name, mat->rows, mat->cols);

    // For each row
    for(int i = 0; i < max_row; i++){
        // For each col
        for(int j = 0; j < max_col; j++){
            sub_matrix_meta* p = &mat->data[MIDX(i, j, mat->cols)];
            fprintf(stdout, "%c%d%d[[%ld,%ld],[%ld,%ld]] ", name, i, j, p->col_start, p->col_end, p->row_start, p->row_end);
        }
        fprintf(stdout, "\n");
    }
}