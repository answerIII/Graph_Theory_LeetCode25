package main

import (
	"fmt"
	"math"
)

func main() {
	fmt.Println(countOfPairs(3, 1, 3))
}

func countOfPairs(n int, x int, y int) []int {
	res := make([]int, n+1)
	if y < x {
		x, y = y, x
	}
	x--
	y--

	for i := 0; i < n; i++ {
		for j := i + 1; j < n; j++ {
			minPath := min(j-i, Abs(i-x)+1+Abs(j-y))
			res[minPath] += 2
		}
	}

	return res[1:]
}

func Abs(num int) int {
	return int(math.Abs(float64(num)))
}
