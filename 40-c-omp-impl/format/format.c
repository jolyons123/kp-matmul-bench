#include "format.h"
#include <math.h>
#include <stdio.h>

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