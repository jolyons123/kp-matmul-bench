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

int main(/*int argc, char* argv[]*/)
{
    // parse args
    mat_arg args = {1000, 1000, 1000, 250, 250};
    /*int res = parse_args(argc, argv, &args);
    if (res != EXIT_SUCCESS){
        print_usage();
        return EXIT_FAILURE;
    }*/

    // create matrices
    matrix mat_A = create_matrix(args.m, args.n);
    matrix mat_B = create_matrix(args.n, args.q);
    matrix mat_C = create_matrix(args.m, args.q);

    // init rng seed
    srand(DEV_SEED);

    // fill with random values
    matrix_random_init(&mat_A, 9.0);
    matrix_random_init(&mat_B, 9.0);
    // fill result matrix with zeroes
    memset(mat_C.data, 0.0, mat_C.cols * mat_C.rows * sizeof(float));

    // Print matrices
    print_matrix('A', &mat_A, args.col_split, args.row_split, 4);
    print_matrix('B', &mat_B, args.col_split, args.row_split, 4);

    double algorithm_time;

    fprintf(stdout, "Starting calc with parallelized prepared block-wise algorithm:\n");

    matrix_mult_operation mult_op;
    int res = prepare_matrix_block_mult(&mat_A, &mat_B, &mat_C, args.row_split, args.col_split, &mult_op);
    if(res != EXIT_SUCCESS){
        fprintf(stderr, "Cannot prepare matrix multiplication. Check that dimensions are matching.\n");
        return res;
    }

    //print_split_matrix('A', &mult_op.split_A, 20);
    //print_split_matrix('B', &mult_op.split_B, 20);
    algorithm_time = omp_get_wtime();
    matrix_block_mul(&mult_op);
    algorithm_time = omp_get_wtime() - algorithm_time;
    fprintf(stdout, "Time was \"%04.8f\".. ms\n", algorithm_time);

    fprintf(stdout, "Restarting calc with vanilla algorithm:\n");
    memset(mat_C.data, 0, mat_C.cols * mat_C.rows * sizeof(float));

    algorithm_time = omp_get_wtime();
    matrix_vanilla_mul(&mat_A, &mat_B, &mat_C);
    algorithm_time = omp_get_wtime() - algorithm_time;
    fprintf(stdout, "Time was \"%04.8f\".. ms\n", algorithm_time);

    fprintf(stdout, "Restarting calc with vanilla omp algorithm:\n");
    memset(mat_C.data, 0, mat_C.cols * mat_C.rows * sizeof(float));

    algorithm_time = omp_get_wtime();
    matrix_vanilla_mul_omp(&mat_A, &mat_B, &mat_C);
    algorithm_time = omp_get_wtime() - algorithm_time;
    fprintf(stdout, "Time was \"%04.8f\".. ms\n", algorithm_time);

    /*fprintf(stdout, "Restarting calc with all-in-one-function block-wise algorithm:\n");
    memset(mat_C.data, 0, mat_C.cols * mat_C.rows * sizeof(float));

    algorithm_time = omp_get_wtime();
    matrix_block_mul_2(&mat_A, &mat_B, &mat_C, args.row_split, args.col_split);
    algorithm_time = omp_get_wtime() - algorithm_time;
    fprintf(stdout, "Time was \"%04.8f\".. ms\n", algorithm_time);*/

    // Cleanup
    //close_matrix_mult(&mult_op);
    free_matrix(&mat_A);
    free_matrix(&mat_B);
    free_matrix(&mat_C);
}