package matrix

import (
	"fmt"
	"sync"
)

func PrintMatrix(mat []float32, n int) {
	for i := 0; i < n; i++ {
		for j := 0; j < n; j++ {
			fmt.Printf("%f ", mat[i*n+j])
		}
		fmt.Println("")
	}
}

func Mat_zero(mat []float32) {
	for i := range mat {
		mat[i] = 0
	}
}

func Mat_mul(A []float32, B []float32, C []float32, n int) {
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

func Mat_mul_par(A []float32, B []float32, C []float32, n int) {
	var wg sync.WaitGroup
	for i := 0; i < n; i++ {
		wg.Add(1)
		go mat_mul_par_kernel(&wg, A, B, C, n, i)
	}
	wg.Wait()
}

func mat_mul_par_kernel(wg *sync.WaitGroup, A []float32, B []float32, C []float32, n int, i int) {
	defer wg.Done()
	for j := 0; j < n; j++ {
		var acc float32 = 0
		for k := 0; k < n; k++ {
			acc += A[i*n+k] * B[k*n+j]
		}
		C[i*n+j] = acc
	}
}

func Mat_mul_block(A []float32, B []float32, C []float32, n int, block_size int) {
	for i_ := 0; i_ < n; i_ += block_size {
		for j_ := 0; j_ < n; j_ += block_size {
			for k_ := 0; k_ < n; k_ += block_size {
				for i := i_; i < minInt(i_+block_size, n); i++ {
					for j := j_; j < minInt(j_+block_size, n); j++ {
						acc := C[i*n+j]
						for k := k_; k < minInt(k_+block_size, n); k++ {
							acc += A[i*n+k] * B[k*n+j]
						}
						C[i*n+j] = acc
					}
				}
			}
		}
	}
}

func Mat_mul_block_par(A []float32, B []float32, C []float32, n int, block_size int) {
	var wg sync.WaitGroup
	for i_ := 0; i_ < n; i_ += block_size {
		wg.Add(1)
		go mat_mul_block_par_kernel(&wg, A, B, C, n, block_size, i_)
	}
	wg.Wait()
}

func mat_mul_block_par_kernel(wg *sync.WaitGroup, A []float32, B []float32, C []float32, n int, block_size int, i_ int) {
	defer wg.Done()
	for j_ := 0; j_ < n; j_ += block_size {
		for k_ := 0; k_ < n; k_ += block_size {
			for i := i_; i < minInt(i_+block_size, n); i++ {
				for j := j_; j < minInt(j_+block_size, n); j++ {
					acc := C[i*n+j]
					for k := k_; k < minInt(k_+block_size, n); k++ {
						acc += A[i*n+k] * B[k*n+j]
					}
					C[i*n+j] = acc
				}
			}
		}
	}
}

func minInt(a, b int) int {
	if a <= b {
		return a
	}
	return b
}
