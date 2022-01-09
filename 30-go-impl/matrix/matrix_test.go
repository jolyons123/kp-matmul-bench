package matrix_test

import (
	"jolyons123/mm-benchmark/matrix"
	"testing"
)

// quadratic matrices with row and column count being 4
var n = 4
var a = [16]float32{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
var b = [16]float32{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}

// result of a * b
var res = [16]float32{56, 62, 68, 74, 152, 174, 196, 218, 248, 286, 324, 362, 344, 398, 452, 506}

func TestMatZero(t *testing.T) {
	a_copy := a

	matrix.Mat_zero(a_copy[:])

	for i := range a_copy {
		if a_copy[i] != 0 {
			t.Errorf("Matrix is not zero at index %d", i)
			break
		}
	}
}

func TestMatMul(t *testing.T) {
	a_copy := a
	b_copy := b
	c := [16]float32{}

	matrix.Mat_mul(a_copy[:], b_copy[:], c[:], n)

	for i := range c {
		if c[i] != res[i] {
			t.Errorf("Matrix does not match at index %d. Should be %f but is %f", i, res[i], c[i])
		}
	}
}

func TestMatMulPar(t *testing.T) {
	a_copy := a
	b_copy := b
	c := [16]float32{}

	matrix.Mat_mul_par(a_copy[:], b_copy[:], c[:], n)

	for i := range c {
		if c[i] != res[i] {
			t.Errorf("Matrix does not match at index %d. Should be %f but is %f", i, res[i], c[i])
		}
	}
}

func TestMatMulParInline(t *testing.T) {
	a_copy := a
	b_copy := b
	c := [16]float32{}

	matrix.Mat_mul_par_inline(a_copy[:], b_copy[:], c[:], n)

	for i := range c {
		if c[i] != res[i] {
			t.Errorf("Matrix does not match at index %d. Should be %f but is %f", i, res[i], c[i])
		}
	}
}

func TestMatMulBlock(t *testing.T) {
	a_copy := a
	b_copy := b
	c := [16]float32{}

	matrix.Mat_mul_block(a_copy[:], b_copy[:], c[:], n, n/2)

	for i := range c {
		if c[i] != res[i] {
			t.Errorf("Matrix does not match at index %d. Should be %f but is %f", i, res[i], c[i])
		}
	}
}

func TestMatMulBlockPar(t *testing.T) {
	a_copy := a
	b_copy := b
	c := [16]float32{}

	matrix.Mat_mul_block_par(a_copy[:], b_copy[:], c[:], n, n/2)

	for i := range c {
		if c[i] != res[i] {
			t.Errorf("Matrix does not match at index %d. Should be %f but is %f", i, res[i], c[i])
		}
	}
}
