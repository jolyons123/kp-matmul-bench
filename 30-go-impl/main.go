package main

import (
	"fmt"
	"math/rand"
	"time"
)

func main() {
	n := 2000
	maxFloat := 10000
	A := make([]float32, n*n)
	B := make([]float32, n*n)
	// should be init with 0
	C := make([]float32, n*n)

	rand.Seed(time.Now().UnixNano())
	for i := 0; i < n; i++ {
		A[i] = rand.Float32() * float32(maxFloat)
		B[i] = rand.Float32() * float32(maxFloat)
	}

	var ftime int64
	fmt.Println("Starting calc with vanilla algorithm:")
	ftime = time.Now().UnixMilli()
	mat_mul(A, B, C, n)
	ftime = time.Now().UnixMilli() - ftime
	fmt.Printf("Took %d ms \n", ftime)

	mat_zero(C)

	fmt.Println("Starting calc with vanilla parallel extern gofunc algorithm:")
	ftime = time.Now().UnixMilli()
	mat_mul_par(A, B, C, n)
	ftime = time.Now().UnixMilli() - ftime
	fmt.Printf("Took %d ms \n", ftime)

	mat_zero(C)

	fmt.Println("Starting calc with vanilla parallel inline gofunc algorithm:")
	ftime = time.Now().UnixMilli()
	mat_mul_par_inline(A, B, C, n)
	ftime = time.Now().UnixMilli() - ftime
	fmt.Printf("Took %d ms \n", ftime)

}

func mat_zero(mat []float32) {
	for i := range mat {
		mat[i] = 0
	}
}

func mat_mul(A []float32, B []float32, C []float32, n int) {
	for i := 0; i < n; i++ {
		for j := 0; j < n; j++ {
			var acc float32 = 0
			for k := 0; k < n; k++ {
				acc += A[i*n+k] * B[k*n+j]
			}
			C[i*n+j] = acc
		}
	}
}

func mat_mul_par_inline(A []float32, B []float32, C []float32, n int) {
	for i := 0; i < n; i++ {
		go func(i int) {
			for j := 0; j < n; j++ {
				var acc float32 = 0
				for k := 0; k < n; k++ {
					acc += A[i*n+k] * B[k*n+j]
				}
				C[i*n+j] = acc
			}
		}(i)
	}
}

func mat_mul_par(A []float32, B []float32, C []float32, n int) {
	for i := 0; i < n; i++ {
		go mat_mul_par_kernel(A, B, C, n, i)
	}
}

func mat_mul_par_kernel(A []float32, B []float32, C []float32, n int, i int) {
	for j := 0; j < n; j++ {
		var acc float32 = 0
		for k := 0; k < n; k++ {
			acc += A[i*n+k] * B[k*n+j]
		}
		C[i*n+j] = acc
	}
}
