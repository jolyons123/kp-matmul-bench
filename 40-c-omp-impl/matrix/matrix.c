#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"

#define RAND09() ( (((float) rand()) / (float) RAND_MAX) * 9)

int prepare_matrix_mult(matrix* A, matrix* B, matrix* C, int row_split, int col_split, matrix_mult_operation* mult_op){
    // check if mul is compatible
    if(A->cols != B->rows || C->rows != A->rows || C->cols != B->cols) return EXIT_FAILURE;

    // because col count in A equals row count in B we can subdivide both matrices equally along row in A and along col in B
    int split_A_cols = A->cols / col_split + (A->cols % col_split);
    int split_A_rows = A->rows / row_split + (A->rows % row_split);
    int split_B_cols = B->cols / row_split + (B->cols % row_split);

    // create arrays which hold submatrices
    sub_matrix_meta* meta_A_list = malloc(sizeof(sub_matrix_meta) * (split_A_rows * split_A_cols));
    sub_matrix_meta* meta_B_list = malloc(sizeof(sub_matrix_meta) * (split_A_cols * split_B_cols));
    split_matrix split_A, split_B;
    split_A.rows = split_A_rows;
    split_A.cols = split_A_cols;
    split_A.data = meta_A_list;
    split_B.rows = split_A_cols;
    split_B.cols = split_B_cols;
    split_B.data = meta_B_list;

    // iterate over submatrix arrays and fill indices
    //for(int i = 0; i < )

    mult_op->mat_A = A;
    mult_op->mat_B = B;
    mult_op->mat_C = C;
    mult_op->split_A = split_A;
    mult_op->split_B = split_B;

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
void matrix_random_init(matrix* mat){
    for(int i = 0; i < mat->rows * mat->cols; i++){
        float rnd = RAND09();
        mat->data[i] = rnd;
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
void print_matrix(matrix* mat, long row_split, long col_split, int max_len){
    long max_row = fminl(mat->rows, max_len);
    long max_col = fminl(mat->cols, max_len);
    fprintf(stdout, "###\n#Printing matrix with \"rows: %d, cols: %d, row_split: %d, col_split: %d\"\n###\n", mat->rows, mat->cols, row_split, col_split);

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