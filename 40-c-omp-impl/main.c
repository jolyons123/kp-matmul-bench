#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <float.h>
#include <math.h>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "args/args.h"
#include "matrix/matrix.h"

#define DEV_SEED 11
#define RAND09() ( (((float) rand()) / (float) RAND_MAX) * 9)

// matrix operations
#define MIDX(r, c, w) (w * r + c)

void print_matrix(const float* M, long rows, long cols, long row_split, long col_split, int max_len);

int main(int argc, char* argv[])
{
    // parse args
    mat_arg args = {8, 9, 5, 2, 2};
    /*int res = parse_args(argc, argv, &args);
    if (res != EXIT_SUCCESS){
        print_usage();
        return EXIT_FAILURE;
    }*/
    
    // input matrices
    long a_len = args.m * args.n;
    long b_len = args.n * args.q;
    float A[a_len];
    float B[b_len];
    matrix mat_A = {args.m, args.n, &A};
    matrix mat_B = {args.n, args.q, &B};
    // result matrix
    long c_len = args.m * args.q;
    float C[c_len];
    matrix mat_C = {args.m, args.q, &C};

    // init rng seed
    srand(DEV_SEED);
    // init all matrices
    matrix_init(&mat_A);
    matrix_init(&mat_B);
    memset(mat_C.data, 0, c_len);

    // We need to know how many submatrices we can have in each matrix. This depends on row_split, col_split and the size of the matrix
    // ??? find out split
    sub_matrix_dimensions dim_A = get_submatrix_dimensions(&mat_A, args.a, args.b);
    sub_matrix_dimensions dim_B = get_submatrix_dimensions(&mat_B, args.a, args.b);
    // The array holds the sub_matrix_meta structues which at the end hold the indexes of the respetive submatrix
    sub_matrix_meta mat_A_split[dim_A.rows * dim_A.cols];
    sub_matrix_meta mat_B_split[dim_B.rows * dim_B.cols];

    print_matrix(A, args.m, args.n, args.a, args.b, 6);

    /*int nthreads, tid;
    {
        tid = omp_get_thread_num();
        printf("welcome to GFG from thread = %d\n", tid);
        if (tid == 0){
            nthreads = omp_get_num_threads();
            printf("number of threads = %d\n", nthreads);
        }
    }*/
}

void print_matrix(const float* M, long rows, long cols, long row_split, long col_split, int max_len){
    long max_row = fminl(rows, max_len);
    long max_col = fminl(cols, max_len);
    fprintf(stdout, "Printing matrix with \"rows: %d, cols: %d, row_split: %d, col_split: %d\"\n", rows, cols, row_split, col_split);

    // For each row
    for(int i = 0; i < max_row; i++){
        // Print horizontal split line
        if(i != 0 && i % row_split == 0){
            for(int k = 0; k < max_col; k++){
                // compensate for the "| " pattern (two characters)
                if(k != 0 && k % col_split == 0){
                    fprintf(stdout, "- ");
                }
                // deal with the float output which is "x.y" (three characters)
                //else{
                    fprintf(stdout, "--- ");
                //}
            }
            fprintf(stdout, "\n");
        }
        
        // For each col
        for(int j = 0; j < max_col; j++){
            // Print vertical split line
            if(j != 0 && j % col_split == 0){
                fprintf(stdout, "| ");
            }

            fprintf(stdout, "%1.1f ", M[MIDX(i, j, rows)]);
        }
        fprintf(stdout, "\n");
    }
}