#include <catch2/catch_test_macros.hpp>
extern "C" {
    #include <matrix/matrix.h>
}
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <string.h>
#endif


TEST_CASE( "Create a matrix", "[matrix]" ) {
    matrix mat = create_matrix(4, 5);

    REQUIRE( mat.rows == 4 );
    REQUIRE( mat.cols == 5 );

    free_matrix(&mat);
}

TEST_CASE( "Free a matrix", "[matrix]" ) {
    matrix mat = create_matrix(4, 5);
    free_matrix(&mat);

    REQUIRE( mat.data == (float*)NULL );
}

TEST_CASE( "Prepare a matrix block multiplication", "[matrix]" ) {
    matrix mat_A = create_matrix(4, 3);
    matrix mat_B = create_matrix(3, 4);
    matrix mat_C = create_matrix(4, 4);

    SECTION( "Prepare should return blocked submatrices" ) {
        const int block_size = 2;
        sub_matrix_meta 
        submatrix_list_A[] = {
            {0, 2, 0, 2}, {0, 2, 2, 3}, 
            {2, 4, 0, 2}, {2, 4, 2, 3}
        },
        submatrix_list_B[] = {
            {0, 2, 0, 2}, {0, 2, 2, 4}, 
            {2, 3, 0, 2}, {2, 3, 2, 4}
        };

        matrix_mult_operation mult_op;
        int res = prepare_matrix_block_mult(&mat_A, &mat_B, &mat_C, block_size, block_size, &mult_op);

        REQUIRE( res == EXIT_SUCCESS );
        REQUIRE( mult_op.split_A.cols * mult_op.split_A.rows == sizeof(submatrix_list_A) / sizeof(sub_matrix_meta) );
        REQUIRE( mult_op.split_B.cols * mult_op.split_B.rows == sizeof(submatrix_list_B) / sizeof(sub_matrix_meta) );
        for(size_t i = 0; i < sizeof(submatrix_list_A) / sizeof(sub_matrix_meta); i++){
            REQUIRE( submatrix_list_A[i].row_start == mult_op.split_A.data[i].row_start );
            REQUIRE( submatrix_list_A[i].row_end == mult_op.split_A.data[i].row_end );
            REQUIRE( submatrix_list_A[i].col_start == mult_op.split_A.data[i].col_start );
            REQUIRE( submatrix_list_A[i].col_end == mult_op.split_A.data[i].col_end );
        }

        close_matrix_mult(&mult_op);
    }

    free_matrix(&mat_A);
    free_matrix(&mat_B);
    free_matrix(&mat_C);
}

TEST_CASE( "Matrix-matrix multiplication", "[matrix]" ) {
    int n = 4;
    int block_size = 2;
    float a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    float b[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    float res[] = {56, 62, 68, 74, 152, 174, 196, 218, 248, 286, 324, 362, 344, 398, 452, 506};

    matrix mat_A = create_matrix(n, n);
    matrix mat_B = create_matrix(n, n);
    matrix mat_C = create_matrix(n, n);

    memcpy(mat_A.data, a, sizeof(a));
    memcpy(mat_B.data, b, sizeof(b));

    SECTION( "Vanilla matrix-matrix multiplication" ) {
        memset(mat_C.data, 0, sizeof(res));
        matrix_vanilla_mul(&mat_A, &mat_B, &mat_C);

        for(int i = 0; i < n*n; i++){
            REQUIRE( mat_C.data[i] == res[i] );
        }
    }

    SECTION( "Parallel vanilla omp matrix-matrix multiplication" ) {
        memset(mat_C.data, 0, sizeof(res));
        matrix_vanilla_mul_omp(&mat_A, &mat_B, &mat_C);

        for(int i = 0; i < n*n; i++){
            REQUIRE( mat_C.data[i] == res[i] );
        }
    }

    SECTION( "Prepared blocked matrix-matrix multiplication" ) {
        matrix_mult_operation mult_op;
        prepare_matrix_block_mult(&mat_A, &mat_B, &mat_C, block_size, block_size, &mult_op);

        memset(mat_C.data, 0, sizeof(res));
        matrix_block_mul(&mult_op);

        for(int i = 0; i < n*n; i++){
            REQUIRE( mult_op.mat_C->data[i] == res[i] );
        }

        close_matrix_mult(&mult_op);
    }

    SECTION( "Parallel prepared blocked omp matrix-matrix multiplication" ) {
        matrix_mult_operation mult_op;
        prepare_matrix_block_mult(&mat_A, &mat_B, &mat_C, block_size, block_size, &mult_op);

        memset(mat_C.data, 0, sizeof(res));
        matrix_block_mul_omp(&mult_op);

        for(int i = 0; i < n*n; i++){
            REQUIRE( mult_op.mat_C->data[i] == res[i] );
        }

        close_matrix_mult(&mult_op);
    }

    SECTION( "Parallel inline blocked omp matrix-matrix multiplication" ) {
        memset(mat_C.data, 0, sizeof(res));
        matrix_block_mul_inline_omp(&mat_A, &mat_B, &mat_C, block_size, block_size);

        for(int i = 0; i < n*n; i++){
            REQUIRE( mat_C.data[i] == res[i] );
        }
    }

    free_matrix(&mat_A);
    free_matrix(&mat_B);
    free_matrix(&mat_C);
}

TEST_CASE( "Matrix initialization", "[matrix]" ) {
    int n = 4;
    matrix mat_A = create_matrix(n, n);

    SECTION( "Random initialization with maximum value" ) {
        matrix_random_init(&mat_A, 10.0f);

        for(int i = 0; i < n*n; i++){
            REQUIRE( mat_A.data[i] < 10.0f );
        }
    }

    SECTION( "Initialize matrix by using the index as the value" ) {
        matrix_simple_init(&mat_A);

        for(int i = 0; i < n*n; i++){
            REQUIRE( mat_A.data[i] == (float)i );
        }
    }

    free_matrix(&mat_A);
}