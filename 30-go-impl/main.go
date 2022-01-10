package main

import (
	"flag"
	"fmt"
	"jolyons123/mm-benchmark/matrix"
	"math/rand"
	"time"
)

var (
	n          *int
	block_size *int
	max_float  *int
)

func main() {
	n = flag.Int("n", 3000, "number of cols/rows for A and B")
	block_size = flag.Int("b", 50, "block size for blocked matrix-matrix multiplication")
	max_float = flag.Int("m", 10000, "maximum value (must be an even number) for random initialization of A and B")

	flag.Parse()

	fmt.Printf("Creating matrices A and B with col/row count = %d "+
		"and max init value = %d\nUsing block size = %d for blocked mm algorithm\n", *n, *max_float, *block_size)

	A := make([]float32, (*n)*(*n))
	B := make([]float32, (*n)*(*n))
	// should be init with 0
	C := make([]float32, (*n)*(*n))

	rand.Seed(time.Now().UnixNano())
	for i := 0; i < (*n)*(*n); i++ {
		A[i] = rand.Float32() * float32(*max_float)
		B[i] = rand.Float32() * float32(*max_float)
	}

	var ftime int64
	fmt.Println("Starting calc with vanilla algorithm:")
	ftime = makeTimestamp()
	matrix.Mat_mul(A, B, C, *n)
	ftime = makeTimestamp() - ftime
	fmt.Printf("Took \"%d\" ms\n", ftime)

	matrix.Mat_zero(C)

	fmt.Println("Starting calc with parallel vanilla extern gofunc algorithm:")
	ftime = makeTimestamp()
	matrix.Mat_mul_par(A, B, C, *n)
	ftime = makeTimestamp() - ftime
	fmt.Printf("Took \"%d\" ms\n", ftime)

	matrix.Mat_zero(C)

	fmt.Println("Starting calc with blocked algorithm:")
	ftime = makeTimestamp()
	matrix.Mat_mul_block(A, B, C, *n, *block_size)
	ftime = makeTimestamp() - ftime
	fmt.Printf("Took \"%d\" ms\n", ftime)

	matrix.Mat_zero(C)

	fmt.Println("Starting calc with parallel blocked extern gofunc algorithm:")
	ftime = makeTimestamp()
	matrix.Mat_mul_block_par(A, B, C, *n, *block_size)
	ftime = makeTimestamp() - ftime
	fmt.Printf("Took \"%d\" ms\n", ftime)
}

func makeTimestamp() int64 {
	return time.Now().UnixNano() / int64(time.Millisecond)
}
