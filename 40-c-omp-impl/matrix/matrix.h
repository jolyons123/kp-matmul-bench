typedef struct matrix{
    long rows;
    long cols;
    float *data;
} matrix;

typedef struct sub_matrix_meta{
    long row_start;
    long row_end;
    long col_start;
    long col_end;
} sub_matrix_meta;

typedef struct sub_matrix_dimensions{
    long rows;
    long cols;
} sub_matrix_dimensions;

typedef struct matrix_operation{
    matrix* mat_A;
    matrix* mat_B;
    matrix* mat_C;
    sub_matrix_meta* mat_A_split;
    sub_matrix_meta* mat_B_split;
} matrix_operation;

void matrix_init(matrix* mat);
sub_matrix_dimensions get_submatrix_dimensions(matrix* mat, int row_split, int col_split);