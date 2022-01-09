#include <matrix/matrix.h>
#include <catch2/catch_test_macros.hpp>


TEST_CASE( "Create a matrix", "[matrix]" ) {
    matrix mat = create_matrix(4, 5);

    REQUIRE( mat.rows == 4 );
    REQUIRE( mat.cols == 5 );

    free_matrix(&mat);
}

/* TEST_CASE( "Free a matrix", "[matrix]" ) {
    matrix mat = create_matrix(4, 5);
    free_matrix(&mat);

    REQUIRE( mat.data == NULL );
} */

/* TEST_CASE( "Prepare a matrix block multiplication", "[matrix]" ) {
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
        REQUIRE( mult_op.split_A.cols * mult_op.split_A.rows == sizeof(submatrix_list_A) );
        REQUIRE( mult_op.split_B.cols * mult_op.split_B.rows == sizeof(submatrix_list_B) );
        for(size_t i = 0; i < sizeof(submatrix_list_A); i++){
            REQUIRE( submatrix_list_A[i].row_start == mult_op.split_A.data[i].row_start );
            REQUIRE( submatrix_list_A[i].row_end == mult_op.split_A.data[i].row_end );
            REQUIRE( submatrix_list_A[i].col_start == mult_op.split_A.data[i].col_start );
            REQUIRE( submatrix_list_A[i].col_end == mult_op.split_A.data[i].col_end );
        }
    }

    free_matrix(&mat_A);
    free_matrix(&mat_B);
    free_matrix(&mat_C);
} */

/*TEST_CASE( "Matrix-matrix multiplication", "[matrix]" ) {
    int n = 4;
    float a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    float b[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    float res[] = {56, 62, 68, 74, 152, 174, 196, 218, 248, 286, 324, 362, 344, 398, 452, 506};

    SECTION( "Vanilla matrix-matrix multiplication" ) {

    }
}*/