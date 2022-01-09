package main

import (
	"fmt"
	"jolyons123/mm-benchmark/matrix"
	"math/rand"
	"time"
)

func main() {
	n := 1000
	block_size := 20
	maxFloat := 10000
	A := make([]float32, n*n)
	B := make([]float32, n*n)
	// should be init with 0
	C := make([]float32, n*n)

	rand.Seed(time.Now().UnixNano())
	for i := 0; i < n*n; i++ {
		A[i] = rand.Float32() * float32(maxFloat)
		B[i] = rand.Float32() * float32(maxFloat)
	}

	var ftime int64
	fmt.Println("Starting calc with vanilla algorithm:")
	ftime = time.Now().UnixMilli()
	matrix.Mat_mul(A, B, C, n)
	ftime = time.Now().UnixMilli() - ftime
	fmt.Printf("Took %d ms \n", ftime)

	matrix.Mat_zero(C)

	fmt.Println("Starting calc with vanilla parallel extern gofunc algorithm:")
	ftime = time.Now().UnixMilli()
	matrix.Mat_mul_par(A, B, C, n)
	ftime = time.Now().UnixMilli() - ftime
	fmt.Printf("Took %d ms \n", ftime)

	matrix.Mat_zero(C)

	fmt.Println("Starting calc with vanilla parallel inline gofunc algorithm:")
	ftime = time.Now().UnixMilli()
	matrix.Mat_mul_par_inline(A, B, C, n)
	ftime = time.Now().UnixMilli() - ftime
	fmt.Printf("Took %d ms \n", ftime)

	matrix.Mat_zero(C)

	fmt.Println("Starting calc with blocked algorithm:")
	ftime = time.Now().UnixMilli()
	matrix.Mat_mul_block(A, B, C, n, block_size)
	ftime = time.Now().UnixMilli() - ftime
	fmt.Printf("Took %d ms \n", ftime)

	matrix.Mat_zero(C)

	fmt.Println("Starting calc with blocked algorithm:")
	ftime = time.Now().UnixMilli()
	matrix.Mat_mul_block(A, B, C, n, block_size)
	ftime = time.Now().UnixMilli() - ftime
	fmt.Printf("Took %d ms \n", ftime)

	matrix.Mat_zero(C)

	fmt.Println("Starting calc with blocked parallel extern gofunc algorithm:")
	ftime = time.Now().UnixMilli()
	matrix.Mat_mul_block_par(A, B, C, n, block_size)
	ftime = time.Now().UnixMilli() - ftime
	fmt.Printf("Took %d ms \n", ftime)

}
