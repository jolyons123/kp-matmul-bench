#include <stdlib.h>
#include "matrix.h"

#define RAND09() ( (((float) rand()) / (float) RAND_MAX) * 9)

int prepare_matrix_operation(matrix_operation *mat_op, int row_split, int col_split){
    
}

//TODO:???
sub_matrix_dimensions get_submatrix_dimensions(matrix* mat, int row_split, int col_split){
    sub_matrix_dimensions dim = {0, 0};
    dim.cols = mat->cols / col_split;
    dim.rows = mat->rows / row_split;

    return dim;
}

long get_matrix_size(long rows, long cols){
    return rows * cols;
}

void matrix_init(matrix* mat){
    for(int i = 0; i < mat->rows * mat->cols; i++){
        float rnd = RAND09();
        mat->data[i] = rnd;
    }
}