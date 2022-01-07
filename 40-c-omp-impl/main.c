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
    for(int i = 0; i < mat_A.cols * mat_A.rows; i++){
        mat_A.data[i] = (float)i;
    }
    for(int i = 0; i < mat_B.cols * mat_B.rows; i++){
        mat_B.data[i] = (float)i;
    }
    // fill result matrix with zeroes
    memset(mat_C.data, 0.0, mat_C.cols * mat_C.rows * sizeof(float));

    // Print matrices
    print_matrix('A', &mat_A, args.col_split, args.row_split, 20);
    print_matrix('B', &mat_B, args.col_split, args.row_split, 20);

    matrix_mult_operation mult_op;
    int res = prepare_matrix_mult(&mat_A, &mat_B, &mat_C, args.row_split, args.col_split, &mult_op);
    if(res != EXIT_SUCCESS){
        fprintf(stderr, "Cannot prepare matrix multiplication. Check that dimensions are matching.\n");
        return res;
    }

    //print_split_matrix('A', &mult_op.split_A, 20);
    //print_split_matrix('B', &mult_op.split_B, 20);
    omp_set_num_threads(4);
    fprintf(stdout, "***\n*Starting compute. Num all threads: %d", omp_get_num_threads());


    for(int u = 0; u < mult_op.split_A.rows; u++){
        #pragma omp parallel for
        for(int v = 0; v < mult_op.split_B.cols; v++){
            fprintf(stdout, "Hello from Thread %d", omp_get_thread_num());
            for(int c = 0; c < mult_op.split_A.cols; c++){
                sub_matrix_mul(&mult_op, &mult_op.split_A.data[MIDX(u, c, mult_op.split_A.cols)], &mult_op.split_B.data[MIDX(c, v, mult_op.split_B.cols)]);
            }
        }
    }
    /*sub_matrix_meta one = mult_op.split_A.data[0];
    sub_matrix_meta two = mult_op.split_B.data[0];

    fprintf(stdout, "\n### Calc first ###\n");
    for(int i = one.row_start; i < one.row_end; i++){
        for(int j = two.col_start; j < two.col_end; j++){
            for(int k = one.col_start; k < one.col_end; k++){
                float val_left = mat_A.data[MIDX(i, k, mat_A.cols)];
                float val_right = mat_B.data[MIDX(k, j, mat_B.cols)]; 
                mat_C.data[MIDX(i, j, mat_C.cols)] += val_left * val_right;
                fprintf(stdout, "mul %1.2f with %1.2f equals %1.2f\n", val_left, val_right, mat_C.data[MIDX(i, j, mat_C.cols)]);
            }
        }
    }
    fprintf(stdout, "\n###");*/

    /*sub_matrix_meta second_one = mult_op.split_A.data[MIDX(0, 1, mult_op.split_A.cols)];
    sub_matrix_meta second_two = mult_op.split_B.data[MIDX(1, 0, mult_op.split_B.cols)];

    fprintf(stdout, "\n### Calc second ###\n");
    for(int i = second_one.row_start; i < second_one.row_end; i++){
        for(int j = second_two.col_start; j < second_two.col_end; j++){
            for(int k = second_one.col_start; k < second_one.col_end; k++){
                float val_left = mat_A.data[MIDX(i, k, mat_A.cols)];
                float val_right = mat_B.data[MIDX(k, j, mat_B.cols)]; 
                mat_C.data[MIDX(i, j, mat_C.cols)] += val_left * val_right;
                fprintf(stdout, "mul %1.2f with %1.2f equals %1.2f\n", val_left, val_right, mat_C.data[MIDX(i, j, mat_C.cols)]);
            }
        }
    }
    fprintf(stdout, "\n###");*/

    print_matrix('C', &mat_C, args.col_split, args.row_split, 20);

    // Cleanup
    close_matrix_mult(&mult_op);
    free_matrix(&mat_A);
    free_matrix(&mat_B);
    free_matrix(&mat_C);
}


/**
 * @brief Snippet section
 * 
 */

/*int nthreads, tid;
    {
        tid = omp_get_thread_num();
        printf("welcome to GFG from thread = %d\n", tid);
        if (tid == 0){
            nthreads = omp_get_num_threads();
            printf("number of threads = %d\n", nthreads);
        }
}*/