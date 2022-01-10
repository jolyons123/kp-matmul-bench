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
#include <string.h>
#endif

#include "args/args.h"
#include "matrix/matrix.h"

#define DEV_SEED 11

int main(int argc, char* argv[])
{
    // parse args
    mat_arg args = {3000, 3000, 3000, 50, 50, 10000};
    int res = parse_args(argc, argv, &args);
    if (res != EXIT_SUCCESS){
        return EXIT_FAILURE;
    }

    fprintf(stdout, "Creating matrix A with rows = %d, cols = %d and B with rows = %d, cols = %d and max init value = %d" \
        "\nUsing block size = (%d, %d) for blocked mm algorithm\n", args.m, args.n, args.n, args.q, args.max_float, args.row_split, args.col_split);

    // create matrices
    matrix mat_A = create_matrix(args.m, args.n);
    matrix mat_B = create_matrix(args.n, args.q);
    matrix mat_C = create_matrix(args.m, args.q);

    // init rng seed
    srand(DEV_SEED);

    // fill with random values
    matrix_random_init(&mat_A, 9.0);
    matrix_random_init(&mat_B, 9.0);

    // Print matrices
    /* print_matrix('A', &mat_A, args.col_split, args.row_split, 4);
    print_matrix('B', &mat_B, args.col_split, args.row_split, 4); */

    matrix_mult_operation mult_op;
    res = prepare_matrix_block_mult(&mat_A, &mat_B, &mat_C, args.row_split, args.col_split, &mult_op);
    if(res != EXIT_SUCCESS){
        fprintf(stderr, "Cannot prepare matrix multiplication. Check that dimensions are matching.\n");
        return res;
    }

    double algorithm_time;

    fprintf(stdout, "Starting calc with vanilla algorithm:\n");
    memset(mat_C.data, 0, mat_C.cols * mat_C.rows * sizeof(float));

    algorithm_time = omp_get_wtime();
    matrix_vanilla_mul(&mat_A, &mat_B, &mat_C);
    algorithm_time = omp_get_wtime() - algorithm_time;
    fprintf(stdout, "Took \"%04.2f\" ms\n", algorithm_time);

    fprintf(stdout, "Starting calc with parallel vanilla omp algorithm:\n");
    memset(mat_C.data, 0, mat_C.cols * mat_C.rows * sizeof(float));

    algorithm_time = omp_get_wtime();
    matrix_vanilla_mul_omp(&mat_A, &mat_B, &mat_C);
    algorithm_time = omp_get_wtime() - algorithm_time;
    fprintf(stdout, "Took \"%04.2f\" ms\n", algorithm_time);

    fprintf(stdout, "Starting calc with prepared blocked algorithm:\n");

    algorithm_time = omp_get_wtime();
    matrix_block_mul(&mult_op);
    algorithm_time = omp_get_wtime() - algorithm_time;
    fprintf(stdout, "Took \"%04.2f\" ms\n", algorithm_time);

    fprintf(stdout, "Starting calc with parallel prepared blocked omp algorithm:\n");
    memset(mat_C.data, 0, mat_C.cols * mat_C.rows * sizeof(float));

    algorithm_time = omp_get_wtime();
    matrix_block_mul_omp(&mult_op);
    algorithm_time = omp_get_wtime() - algorithm_time;
    fprintf(stdout, "Took \"%04.2f\" ms\n", algorithm_time);

    fprintf(stdout, "Starting calc with parallel inline blocked omp algorithm:\n");
    memset(mat_C.data, 0, mat_C.cols * mat_C.rows * sizeof(float));

    algorithm_time = omp_get_wtime();
    matrix_block_mul_inline_omp(&mat_A, &mat_B, &mat_C, args.row_split, args.col_split);
    algorithm_time = omp_get_wtime() - algorithm_time;
    fprintf(stdout, "Took \"%04.2f\" ms\n", algorithm_time);

    // Cleanup
    close_matrix_mult(&mult_op);
    free_matrix(&mat_A);
    free_matrix(&mat_B);
    free_matrix(&mat_C);
}