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

typedef struct split_matrix{
    long rows;
    long cols;
    sub_matrix_meta* data;
} split_matrix;

typedef struct sub_matrix_dimensions{
    long rows;
    long cols;
} sub_matrix_dimensions;

typedef struct matrix_mult_operation{
    matrix* mat_A;
    matrix* mat_B;
    matrix* mat_C;
    split_matrix split_A;
    split_matrix split_B;
} matrix_mult_operation;

matrix create_matrix(long rows, long cols);
void free_matrix(matrix* mat);
void matrix_random_init(matrix* mat);
int prepare_matrix_mult(matrix* A, matrix* B, matrix* C, int row_split, int col_split, matrix_mult_operation* mult_op);
void close_matrix_mult(matrix_mult_operation* mult_op);
void print_matrix(matrix* mat, long row_split, long col_split, long max_len);
void print_split_matrix(split_matrix* mat, char character, long max_len);

// matrix operations
#define MIDX(r, c, w) (w * r + c)