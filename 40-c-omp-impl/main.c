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

int main(int argc, char* argv[])
{
    // parse args
    mat_arg args = {3, 4, 3, 2, 2};
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
    //matrix_random_init(&mat_A);
    //matrix_random_init(&mat_B);
    matrix_random_init(&mat_A, 9.0);
    matrix_random_init(&mat_B, 9.0);
    // fill result matrix with zeroes
    memset(mat_C.data, 0.0, mat_C.cols * mat_C.rows * sizeof(float));

    // Print matrices
    print_matrix('A', &mat_A, args.col_split, args.row_split, 20);
    
    print_matrix('B', &mat_B, args.col_split, args.row_split, 20);

    matrix_mult_operation mult_op;
    int res = prepare_matrix_block_mult(&mat_A, &mat_B, &mat_C, args.row_split, args.col_split, &mult_op);
    if(res != EXIT_SUCCESS){
        fprintf(stderr, "Cannot prepare matrix multiplication. Check that dimensions are matching.\n");
        return res;
    }

    //print_split_matrix('A', &mult_op.split_A, 20);
    //print_split_matrix('B', &mult_op.split_B, 20);

    matrix_block_mul(&mult_op);

    print_matrix('C', &mat_C, args.col_split, args.row_split, 20);

    // Cleanup
    close_matrix_mult(&mult_op);
    free_matrix(&mat_A);
    free_matrix(&mat_B);
    free_matrix(&mat_C);
}