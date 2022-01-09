#include <catch2/catch_test_macros.hpp>
#include "matrix/matrix.h"

TEST_CASE( "Create a matrix", "[matrix]" ) {
    matrix mat = create_matrix(4, 5);

    REQUIRE( mat.rows == 4 );
    REQUIRE( mat.cols == 5);

    free_matrix(&mat);
}

TEST_CASE( "Free a matrix", "[matrix]" ) {
    matrix mat = create_matrix(4, 5);
    free_matrix(&mat);

    REQUIRE( mat.data == NULL );
}

TEST_CASE( "Prepare a matrix block multiplication", "[matrix]" ) {
    matrix mat_A = create_matrix(4, 3);
    matrix mat_B = create_matrix(3, 4);
    matrix mat_C = create_matrix(4, 4);

    SECTION( "Prepare should return blocked submatrices" ) {
        //TODO:...

        matrix_mult_operation mult_op;
        int res = prepare_matrix_block_mult(&mat_A, &mat_B, &mat_C, 2, 2, &mult_op);

        REQUIRE( res == EXIT_SUCCESS );
        
    }
}

TEST_CASE( "Matrix-matrix multiplication", "[matrix]" ) {
    int n = 4;
    float a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    float b[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    float res[] = {56, 62, 68, 74, 152, 174, 196, 218, 248, 286, 324, 362, 344, 398, 452, 506};

    SECTION( "Vanilla matrix-matrix multiplication" ) {

    }
}