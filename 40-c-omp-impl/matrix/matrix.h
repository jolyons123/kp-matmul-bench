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
void matrix_random_init(matrix* mat, float max);
int prepare_matrix_block_mult(matrix* A, matrix* B, matrix* C, int row_split, int col_split, matrix_mult_operation* mult_op);
void close_matrix_mult(matrix_mult_operation* mult_op);
void print_matrix(char name, matrix* mat, long row_split, long col_split, long max_len);
void print_split_matrix(char name, split_matrix* mat, long max_len);
void sub_matrix_mul(matrix_mult_operation* mul_op, sub_matrix_meta* A, sub_matrix_meta* B);
void matrix_simple_init(matrix* mat);
void matrix_block_mul_omp(matrix_mult_operation* mult_op);
int matrix_vanilla_mul(matrix* A, matrix* B, matrix* C);
int matrix_block_mul_2(matrix* A, matrix* B, matrix* C, int row_split, int col_split);
int matrix_vanilla_mul_omp(matrix* A, matrix* B, matrix* C);
void matrix_block_mul(matrix_mult_operation* mult_op);

// matrix operations
#define MIDX(r, c, w) (w * r + c)